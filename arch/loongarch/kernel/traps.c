// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Loongson Technology Corporation Limited
 */
#include <linux/bitops.h>
#include <linux/bug.h>
#include <linux/compiler.h>
#include <linux/context_tracking.h>
#include <linux/cpu_pm.h>
#include <linux/kexec.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/extable.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/sched/debug.h>
#include <linux/smp.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/bootmem.h>
#include <linux/memblock.h>
#include <linux/interrupt.h>
#include <linux/ptrace.h>
#include <linux/kgdb.h>
#include <linux/kdebug.h>
#include <linux/kprobes.h>
#include <linux/notifier.h>
#include <linux/kdb.h>
#include <linux/irq.h>
#include <linux/perf_event.h>
#include <linux/uaccess.h>

#include <asm/addrspace.h>
#include <asm/bootinfo.h>
#include <asm/branch.h>
#include <asm/break.h>
#include <asm/cpu.h>
#include <asm/fpu.h>
#include <asm/idle.h>
#include <asm/loongarchregs.h>
#include <asm/module.h>
#include <asm/pgtable.h>
#include <asm/ptrace.h>
#include <asm/sections.h>
#include <asm/siginfo.h>
#include <asm/tlb.h>
#include <asm/watch.h>
#include <asm/mmu_context.h>
#include <asm/types.h>
#include <asm/stacktrace.h>
#include <asm/unwind.h>
#include <asm/lbt.h>

extern asmlinkage void handle_ade(void);
extern asmlinkage void handle_ale(void);
extern asmlinkage void handle_sys(void);
extern asmlinkage void handle_sys_wrap(void);
extern asmlinkage void handle_bp(void);
extern asmlinkage void handle_ri(void);
extern asmlinkage void handle_fpu(void);
extern asmlinkage void handle_fpe(void);
extern asmlinkage void handle_lbt(void);
extern asmlinkage void handle_lsx(void);
extern asmlinkage void handle_lasx(void);
extern asmlinkage void handle_reserved(void);
extern asmlinkage void handle_watch(void);
extern void tlb_do_page_fault_protect(void);
extern void *vector_table[];

void *exception_table[EXCCODE_INT_START] = {
	[0 ... EXCCODE_INT_START - 1] = handle_reserved,

	[EXCCODE_TLBL] = handle_tlb_load,
	[EXCCODE_TLBS] = handle_tlb_store,
	[EXCCODE_TLBI] = handle_tlb_load,
	[EXCCODE_TLBM] = handle_tlb_modify,
	[EXCCODE_TLBRI ... EXCCODE_TLBPE] = tlb_do_page_fault_protect,
	[EXCCODE_ADE] = handle_ade,
	[EXCCODE_ALE] = handle_ale,
	[EXCCODE_SYS] = handle_sys_wrap,
	[EXCCODE_BP] = handle_bp,
	[EXCCODE_INE] = handle_ri,
	[EXCCODE_IPE] = handle_ri,
	[EXCCODE_FPDIS] = handle_fpu,
	[EXCCODE_LSXDIS] = handle_lsx,
	[EXCCODE_LASXDIS] = handle_lasx,
	[EXCCODE_FPE] = handle_fpe,
	[EXCCODE_WATCH] = handle_watch,
	[EXCCODE_BTDIS] = handle_lbt,
};
EXPORT_SYMBOL_GPL(exception_table);

static void show_backtrace(struct task_struct *task, const struct pt_regs *regs)
{
	struct unwind_state state;
	struct pt_regs *pregs = (struct pt_regs *)regs;
	unsigned long pc;

	if (!task)
		task = current;

	unwind_start(&state, task, pregs);

#ifdef CONFIG_UNWINDER_PROLOGUE
	if (user_mode(regs))
		state.enable = false;
#endif

	printk("Call Trace:\n");
	for (; !unwind_done(&state); unwind_next_frame(&state)) {
		pc = unwind_get_return_address(&state);
		print_ip_sym(pc);
	}

	pr_cont("\n");
}

/*
 * This routine abuses get_user()/put_user() to reference pointers
 * with at least a bit of error checking ...
 */
static void show_stacktrace(struct task_struct *task,
	const struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	long stackdata;
	int i;
	unsigned long __user *sp = (unsigned long __user *)regs->regs[3];

	printk("Stack :");
	i = 0;
	while ((unsigned long) sp & (PAGE_SIZE - 1)) {
		if (i && ((i % (64 / field)) == 0)) {
			pr_cont("\n");
			printk("       ");
		}
		if (i > 39) {
			pr_cont(" ...");
			break;
		}

		if (__get_user(stackdata, sp++)) {
			pr_cont(" (Bad stack address)");
			break;
		}

		pr_cont(" %0*lx", field, stackdata);
		i++;
	}
	pr_cont("\n");
	show_backtrace(task, regs);
}

void show_stack(struct task_struct *task, unsigned long *sp)
{
	struct pt_regs regs;
	mm_segment_t old_fs = get_fs();

	regs.csr_crmd = 0;
	if (sp) {
		regs.regs[3] = (unsigned long)sp;
		regs.regs[1] = 0;
		regs.csr_era = 0;
	} else {
		if (task && task != current) {
			regs.regs[22] = task->thread.reg22;
			regs.regs[3] = task->thread.reg03;
			regs.regs[1] = 0;
			regs.csr_era = task->thread.reg01;
#ifdef CONFIG_KGDB_KDB
		} else if (atomic_read(&kgdb_active) != -1 &&
			   kdb_current_regs) {
			memcpy(&regs, kdb_current_regs, sizeof(regs));
#endif /* CONFIG_KGDB_KDB */
		} else {
			prepare_frametrace(&regs);
		}
	}
	/*
	 * show_stack() deals exclusively with kernel mode, so be sure to access
	 * the stack in the kernel (not user) address space.
	 */
	set_fs(KERNEL_DS);
	show_stacktrace(task, &regs);
	set_fs(old_fs);
}

static void show_code(unsigned int __user *pc)
{
	long i;
	unsigned int insn;

	printk("Code:");

	for(i = -3 ; i < 6 ; i++) {
		if (__get_user(insn, pc + i)) {
			pr_cont(" (Bad address in era)\n");
			break;
		}
		pr_cont("%c%08x%c", (i?' ':'<'), insn, (i?' ':'>'));
	}
	pr_cont("\n");
}

static void __show_regs(const struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	unsigned int excsubcode;
	unsigned int exccode;
	int i;

	show_regs_print_info(KERN_DEFAULT);

	/*
	 * Saved main processor registers
	 */
	for (i = 0; i < 32; ) {
		if ((i % 4) == 0)
			printk("$%2d   :", i);
		pr_cont(" %0*lx", field, regs->regs[i]);

		i++;
		if ((i % 4) == 0)
			pr_cont("\n");
	}

	/*
	 * Saved csr registers
	 */
	printk("era   : %0*lx %pS\n", field, regs->csr_era,
	       (void *) regs->csr_era);
	printk("ra    : %0*lx %pS\n", field, regs->regs[1],
	       (void *) regs->regs[1]);

	printk("CSR crmd: %08lx	", regs->csr_crmd);
	printk("CSR prmd: %08lx	", regs->csr_prmd);
	printk("CSR ecfg: %08llx	", csr_read64(LOONGARCH_CSR_ECFG));
	printk("CSR estat: %08lx	", regs->csr_estat);
	printk("CSR euen: %08lx	", regs->csr_euen);

	pr_cont("\n");

	exccode = ((regs->csr_estat) & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;
	excsubcode = ((regs->csr_estat) & CSR_ESTAT_ESUBCODE) >> CSR_ESTAT_ESUBCODE_SHIFT;
	printk("ExcCode : %x (SubCode %x)\n", exccode, excsubcode);

	if (exccode >= EXCCODE_TLBL && exccode <= EXCCODE_ALE)
		printk("BadVA : %0*lx\n", field, regs->csr_badv);

	printk("PrId  : %08x (%s)\n", read_cpucfg(LOONGARCH_CPUCFG0),
	       cpu_family_string());
}

/*
 * FIXME: really the generic show_regs should take a const pointer argument.
 */
void show_regs(struct pt_regs *regs)
{
	__show_regs((struct pt_regs *)regs);
	dump_stack();
}

void show_registers(struct pt_regs *regs)
{
	mm_segment_t old_fs = get_fs();

	__show_regs(regs);
	print_modules();
	printk("Process %s (pid: %d, threadinfo=%p, task=%p)\n",
	       current->comm, current->pid, current_thread_info(), current);

	if (!user_mode(regs))
		/* Necessary for getting the correct stack content */
		set_fs(KERNEL_DS);
	show_stacktrace(current, regs);
	show_code((unsigned int __user *) regs->csr_era);
	printk("\n");
	set_fs(old_fs);
}

static DEFINE_RAW_SPINLOCK(die_lock);

void __noreturn die(const char *str, struct pt_regs *regs)
{
	static int die_counter;
	int sig = SIGSEGV;

	oops_enter();

	if (notify_die(DIE_OOPS, str, regs, 0, current->thread.trap_nr,
		       SIGSEGV) == NOTIFY_STOP)
		sig = 0;

	console_verbose();
	raw_spin_lock_irq(&die_lock);
	bust_spinlocks(1);

	printk("%s[#%d]:\n", str, ++die_counter);
	show_registers(regs);
	add_taint(TAINT_DIE, LOCKDEP_NOW_UNRELIABLE);
	raw_spin_unlock_irq(&die_lock);

	oops_exit();

	if (regs && kexec_should_crash(current))
		crash_kexec(regs);

	if (in_interrupt())
		panic("Fatal exception in interrupt");

	if (panic_on_oops)
		panic("Fatal exception");

	do_exit(sig);
}

static inline void setup_vint_size(unsigned int size)
{
	unsigned int vs;

	vs = ilog2(size/4);

	if(vs == 0 || vs > 7)
		panic("vint_size %d Not support yet", vs);

	csr_xchg32(vs<<CSR_ECFG_VS_SHIFT, CSR_ECFG_VS, LOONGARCH_CSR_ECFG);
}

/*
 * Send SIGFPE according to FCSR Cause bits, which must have already
 * been masked against Enable bits.  This is impotant as Inexact can
 * happen together with Overflow or Underflow, and `ptrace' can set
 * any bits.
 */
void force_fcsr_sig(unsigned long fcsr, void __user *fault_addr,
		     struct task_struct *tsk)
{
	int si_code = FPE_FLTUNK;

	if (fcsr & FPU_CSR_INV_X)
		si_code = FPE_FLTINV;
	else if (fcsr & FPU_CSR_DIV_X)
		si_code = FPE_FLTDIV;
	else if (fcsr & FPU_CSR_OVF_X)
		si_code = FPE_FLTOVF;
	else if (fcsr & FPU_CSR_UDF_X)
		si_code = FPE_FLTUND;
	else if (fcsr & FPU_CSR_INE_X)
		si_code = FPE_FLTRES;

	force_sig_fault(SIGFPE, si_code, fault_addr, tsk);
}

int process_fpemu_return(int sig, void __user *fault_addr, unsigned long fcsr)
{
	int si_code;
	struct vm_area_struct *vma;

	switch (sig) {
	case 0:
		return 0;

	case SIGFPE:
		force_fcsr_sig(fcsr, fault_addr, current);
		return 1;

	case SIGBUS:
		force_sig_fault(SIGBUS, BUS_ADRERR, fault_addr, current);
		return 1;

	case SIGSEGV:
		down_read(&current->mm->mmap_sem);
		vma = find_vma(current->mm, (unsigned long)fault_addr);
		if (vma && (vma->vm_start <= (unsigned long)fault_addr))
			si_code = SEGV_ACCERR;
		else
			si_code = SEGV_MAPERR;
		up_read(&current->mm->mmap_sem);
		force_sig_fault(SIGSEGV, si_code, fault_addr, current);
		return 1;

	default:
		force_sig(sig, current);
		return 1;
	}
}

/*
 * XXX Delayed fp exceptions when doing a lazy ctx switch XXX
 */
asmlinkage void do_fpe(struct pt_regs *regs, unsigned long fcsr)
{
	enum ctx_state prev_state;
	void __user *fault_addr;
	int sig;

	prev_state = exception_enter();
	if (notify_die(DIE_FP, "FP exception", regs, 0, current->thread.trap_nr,
		       SIGFPE) == NOTIFY_STOP)
		goto out;

	/* Clear FCSR.Cause before enabling interrupts */
	write_fcsr(LOONGARCH_FCSR0, fcsr & ~mask_fcsr_x(fcsr));
	local_irq_enable();

	die_if_kernel("FP exception in kernel code", regs);

	sig = SIGFPE;
	fault_addr = (void __user *) regs->csr_era;

	/* Send a signal if required.  */
	process_fpemu_return(sig, fault_addr, fcsr);

out:
	exception_exit(prev_state);
}

/* GENERIC_BUG traps */

int is_valid_bugaddr(unsigned long addr)
{
	/*
	 * bug_handler() only called for BREAK #BRK_BUG.
	 * So the answer is trivial -- any spurious instances with no
	 * bug table entry will be rejected by report_bug().
	 */
	return 1;
}

static void bug_handler(struct pt_regs *regs)
{
	switch (report_bug(regs->csr_era, regs)) {
	case BUG_TRAP_TYPE_NONE:
	case BUG_TRAP_TYPE_BUG:
		die_if_kernel("Oops - BUG", regs);
		force_sig(SIGTRAP, current);
		break;

	case BUG_TRAP_TYPE_WARN:
		/* If thread survives, skip over the BUG instruction and continue: */
		regs->csr_era += LOONGARCH_INSN_SIZE;
		break;
	}
}

asmlinkage void do_bp(struct pt_regs *regs)
{
	unsigned int opcode, bcode;
	unsigned long era = exception_era(regs);
	enum ctx_state prev_state;
	mm_segment_t seg;

	seg = get_fs();
	if (!user_mode(regs))
		set_fs(KERNEL_DS);

	prev_state = exception_enter();
	current->thread.trap_nr = read_csr_excode();
	if (__get_user(opcode, (unsigned int __user *)era))
		goto out_sigsegv;

	bcode = (opcode & 0x7fff);

#ifdef CONFIG_KGDB_LOW_LEVEL_TRAP
	if (kgdb_ll_trap(DIE_TRAP, "Break", regs, code, current->thread.trap_nr,
			 SIGTRAP) == NOTIFY_STOP)
		return;
#endif /* CONFIG_KGDB_LOW_LEVEL_TRAP */

	/*
	 * notify the kprobe handlers, if instruction is likely to
	 * pertain to them.
	 */
	switch (bcode) {
	case BRK_KPROBE_BP:
		if (notify_die(DIE_BREAK, "Kprobe", regs, bcode,
			       current->thread.trap_nr, SIGTRAP) == NOTIFY_STOP)
			goto out;
		else
			break;
	case BRK_KPROBE_SSTEPBP:
		if (notify_die(DIE_SSTEPBP, "Kprobe_SingleStep", regs, bcode,
			       current->thread.trap_nr, SIGTRAP) == NOTIFY_STOP)
			goto out;
		else
			break;
	case BRK_UPROBE_BP:
		if (notify_die(DIE_UPROBE, "Uprobe", regs, bcode,
			       current->thread.trap_nr, SIGTRAP) == NOTIFY_STOP)
			goto out;
		else
			break;
	case BRK_UPROBE_XOLBP:
		if (notify_die(DIE_UPROBE_XOL, "Uprobe_XOL", regs, bcode,
			       current->thread.trap_nr, SIGTRAP) == NOTIFY_STOP)
			goto out;
		else
			break;
	default:
		if (notify_die(DIE_TRAP, "Break", regs, bcode,
			       current->thread.trap_nr, SIGTRAP) == NOTIFY_STOP)
			goto out;
		else
			break;
	}

	switch (bcode) {
	case BRK_BUG:
		bug_handler(regs);
		break;
	case BRK_DIVZERO:
		die_if_kernel("Break instruction in kernel code", regs);
		force_sig_fault(SIGFPE, FPE_INTDIV, (void __user *)regs->csr_era, current);
		break;
	case BRK_OVERFLOW:
		die_if_kernel("Break instruction in kernel code", regs);
		force_sig_fault(SIGFPE, FPE_INTOVF, (void __user *)regs->csr_era, current);
		break;
	default:
		die_if_kernel("Break instruction in kernel code", regs);
		force_sig_fault(SIGTRAP, TRAP_BRKPT, (void __user *)regs->csr_era, current);
		break;
	}

out:
	set_fs(seg);
	exception_exit(prev_state);
	return;

out_sigsegv:
	force_sig(SIGSEGV, current);
	goto out;
}

asmlinkage void do_watch(struct pt_regs *regs)
{
	enum ctx_state prev_state;
	struct loongarch3264_watch_reg_state *watches =
		&current->thread.watch.loongarch3264;
	siginfo_t info;
	int i;

	prev_state = exception_enter();

	if (test_tsk_thread_flag(current, TIF_LOAD_WATCH)) {
		loongarch_read_watch_registers(regs);
		for (i = 0; i < boot_cpu_data.watch_reg_use_cnt; i++) {
			if ((watch_csrrd(LOONGARCH_CSR_MWPS) & (0x1 << i))) {
				info.si_addr = (void __user *)watches->addr[i];
				watch_csrwr(0x1 << i, LOONGARCH_CSR_MWPS);
			}
		}
		force_sig_fault(SIGTRAP, TRAP_HWBKPT, info.si_addr, current);
	} else if (test_tsk_thread_flag(current, TIF_SINGLESTEP)) {
		int llbit = (csr_read32(LOONGARCH_CSR_LLBCTL) & 0x1);
		unsigned long pc = regs->csr_era;

		if (llbit) {
			csr_write32(0x10000, LOONGARCH_CSR_FWPS);
			csr_write32(0x4, LOONGARCH_CSR_LLBCTL);
		} else if (pc == current->thread.single_step) {
			csr_write32(0x10000, LOONGARCH_CSR_FWPS);
		} else {
			loongarch_read_watch_registers(regs);
			force_sig(SIGTRAP, current);
		}
	} else {
		if (notify_die(DIE_TRAP, "Break", regs, 0,
			       current->thread.trap_nr, SIGTRAP) != NOTIFY_STOP)
		loongarch_clear_watch_registers();
	}

	exception_exit(prev_state);
	return;
}

asmlinkage void do_ri(struct pt_regs *regs)
{
	unsigned int __user *era = (unsigned int __user *)exception_era(regs);
	enum ctx_state prev_state;
	unsigned int opcode = 0;
	int status = SIGILL;

	prev_state = exception_enter();
	current->thread.trap_nr = read_csr_excode();

	if (notify_die(DIE_RI, "RI Fault", regs, 0, current->thread.trap_nr,
		       SIGILL) == NOTIFY_STOP)
		goto out;

	die_if_kernel("Reserved instruction in kernel code", regs);

	if (unlikely(get_user(opcode, era) < 0)) {
		/*1:read access;2:write access*/
		current->thread.error_code = 1;
		status = SIGSEGV;
	}

	force_sig(status, current);

out:
	exception_exit(prev_state);
}

static void init_restore_fp(void)
{
	if (!used_math()) {
		/* First time FP context user. */
		init_fpu();
	} else {
		/* This task has formerly used the FP context */
		if (!is_fpu_owner())
			own_fpu_inatomic(1);
	}
}

static void init_restore_lsx(void)
{
	enable_lsx();

	if (!thread_lsx_context_live()) {
		/* First time LSX context user */
		init_restore_fp();
		init_lsx_upper();
		set_thread_flag(TIF_LSX_CTX_LIVE);
	} else {
		if (!is_simd_owner()) {
			if (is_fpu_owner()) {
				restore_lsx_upper(current);
			} else {
				__own_fpu();
				restore_lsx(current);
			}
		}
	}

	set_thread_flag(TIF_USEDSIMD);
}

static void init_restore_lasx(void)
{
	enable_lasx();

	if (!thread_lasx_context_live()) {
		/* First time LASX context user */
		init_restore_lsx();
		init_lasx_upper();
		set_thread_flag(TIF_LASX_CTX_LIVE);
	} else {
		if (is_fpu_owner() || is_simd_owner()) {
			init_restore_lsx();
			restore_lasx_upper(current);
		} else {
			__own_fpu();
			enable_lsx();
			restore_lasx(current);
		}
	}

	set_thread_flag(TIF_USEDSIMD);

	BUG_ON(!is_fp_enabled());
	BUG_ON(!is_lsx_enabled());
	BUG_ON(!test_thread_flag(TIF_USEDFPU));
}

asmlinkage void do_fpu(struct pt_regs *regs)
{
	enum ctx_state prev_state;

	prev_state = exception_enter();

	die_if_kernel("do_fpu invoked from kernel context!", regs);
	BUG_ON(is_lsx_enabled());
	BUG_ON(is_lasx_enabled());

	preempt_disable();
	init_restore_fp();
	preempt_enable();

	exception_exit(prev_state);
}

asmlinkage void do_lsx(struct pt_regs *regs)
{
	enum ctx_state prev_state;

	prev_state = exception_enter();

	if (!cpu_has_lsx) {
		force_sig(SIGILL, current);
		goto out;
	}

	die_if_kernel("do_lsx invoked from kernel context!", regs);
	BUG_ON(is_lasx_enabled());

	preempt_disable();
	init_restore_lsx();
	preempt_enable();

out:
	exception_exit(prev_state);
}

asmlinkage void do_lasx(struct pt_regs *regs)
{
	enum ctx_state prev_state;

	prev_state = exception_enter();

	if (!cpu_has_lasx) {
		force_sig(SIGILL, current);
		goto out;
	}
	die_if_kernel("lasx disable invoked from kernel context!",
			regs);

	preempt_disable();
	init_restore_lasx();
	preempt_enable();

out:
	exception_exit(prev_state);
}

#ifdef CONFIG_CPU_HAS_LBT
static void init_restore_lbt(void)
{
	if (!thread_lbt_context_live()) {
		/* First lbt context user */
		init_lbt();
		set_thread_flag(TIF_LBT_CTX_LIVE);
	} else {
		/* Enable and restore */
		own_lbt_inatomic(1);
	}
}
#else
static void init_restore_lbt(void)
{}
#endif

asmlinkage void do_lbt(struct pt_regs *regs)
{
	enum ctx_state prev_state;
	prev_state = exception_enter();

	if (!cpu_has_lbt) {
		force_sig(SIGILL, current);
		goto out;
	}

	preempt_disable();
	init_restore_lbt();
	preempt_enable();
out:
	exception_exit(prev_state);
}

asmlinkage void do_reserved(struct pt_regs *regs)
{
	/*
	 * Game over - no way to handle this if it ever occurs.	 Most probably
	 * caused by a new unknown cpu type or after another deadly
	 * hard/software error.
	 */
	enum ctx_state prev_state;
	prev_state = exception_enter();

	die_if_kernel("do_reserved Exception", regs);
	pr_err("pid:%d [%s] Caught reserved exception %u - should not happen\n",
		current->tgid, current->comm, read_csr_excode());
	force_sig(SIGUNUSED, current);

	exception_exit(prev_state);
}

asmlinkage void cache_parity_error(void)
{
	const int field = 2 * sizeof(unsigned long);
	unsigned int reg_val;

	/* For the moment, report the problem and hang. */
	printk("Cache error exception:\n");
	printk("csr_errorera == %0*llx\n", field, csr_read64(LOONGARCH_CSR_MERRERA));
	reg_val = csr_read32(LOONGARCH_CSR_ERRCTL);
	printk("csr_errctl == %08x\n", reg_val);

	printk("Decoded c0_cacheerr: %s cache fault in %s reference.\n",
	       reg_val & (1<<30) ? "secondary" : "primary",
	       reg_val & (1<<31) ? "data" : "insn");
	if (((current_cpu_data.processor_id & 0xff0000) == PRID_COMP_LOONGSON)) {
		pr_err("Error bits: %s%s%s%s%s%s%s%s\n",
			reg_val & (1<<29) ? "ED " : "",
			reg_val & (1<<28) ? "ET " : "",
			reg_val & (1<<27) ? "ES " : "",
			reg_val & (1<<26) ? "EE " : "",
			reg_val & (1<<25) ? "EB " : "",
			reg_val & (1<<24) ? "EI " : "",
			reg_val & (1<<23) ? "E1 " : "",
			reg_val & (1<<22) ? "E0 " : "");
	} else {
		pr_err("Error bits: %s%s%s%s%s%s%s\n",
			reg_val & (1<<29) ? "ED " : "",
			reg_val & (1<<28) ? "ET " : "",
			reg_val & (1<<26) ? "EE " : "",
			reg_val & (1<<25) ? "EB " : "",
			reg_val & (1<<24) ? "EI " : "",
			reg_val & (1<<23) ? "E1 " : "",
			reg_val & (1<<22) ? "E0 " : "");
	}
	printk("IDX: 0x%08x\n", reg_val & ((1<<22)-1));

	panic("Can't handle the cache error!");
}

unsigned long eentry;
EXPORT_SYMBOL_GPL(eentry);
unsigned long tlbrentry;
EXPORT_SYMBOL_GPL(tlbrentry);

static vi_handler_t ip_handlers[EXCCODE_INT_NUM];
void do_vi(int irq)
{
	vi_handler_t	action;

	action = ip_handlers[irq];
	if (action)
		action(irq);
	else
		pr_err("vi handler[%d] is not installed\n", irq);
}

void set_vi_handler(int n, vi_handler_t addr)
{
	if ((n < EXCCODE_INT_START) || (n >= EXCCODE_INT_END)) {
		pr_err("set invalid vector handler[%d] \n", n);
		return;
	}

	ip_handlers[n - EXCCODE_INT_START] = addr;
}

extern void tlb_init(int cpu);
extern void cache_error_setup(void);

long exception_handlers[VECSIZE * 128 / sizeof(long)] __aligned(SZ_64K);

static void configure_exception_vector(void)
{
	eentry		= (unsigned long)exception_handlers;
	tlbrentry	= (unsigned long)exception_handlers + 80*VECSIZE;

	csr_write64(eentry, LOONGARCH_CSR_EENTRY);
	csr_write64(tlbrentry, LOONGARCH_CSR_TLBRENTRY);
	csr_write64(eentry, LOONGARCH_CSR_MERRENTRY);
}

void per_cpu_trap_init(int cpu)
{
	setup_vint_size(VECSIZE);
	configure_exception_vector();

	mmgrab(&init_mm);
	current->active_mm = &init_mm;
	BUG_ON(current->mm);
	enter_lazy_tlb(&init_mm, current);

	tlb_init(cpu);
	cpu_cache_init();
}

/* Install CPU exception handler */
void set_handler(unsigned long offset, void *addr, unsigned long size)
{
	memcpy((void *)(eentry + offset), addr, size);
	local_flush_icache_range(eentry + offset, eentry + offset + size);
}

static const char panic_null_cerr[] =
	"Trying to set NULL cache error exception handler\n";

/*
 * Install uncached CPU exception handler.
 * This is suitable only for the cache error exception which is the only
 * exception handler that is being run uncached.
 */
void set_merr_handler(unsigned long offset, void *addr, unsigned long size)
{
	unsigned long uncached_eentry = TO_UNCAC(__pa(eentry));

	if (!addr)
		panic(panic_null_cerr);

	memcpy((void *)(uncached_eentry + offset), addr, size);
}

void set_tlb_handler(void)
{
	int i;

	/* Initialise exception handlers */
	for (i = 0; i < 64; i++)
		set_handler(i * VECSIZE, handle_reserved, VECSIZE);

	if (cpu_has_ptw) {
		exception_table[EXCCODE_TLBL] = handle_tlb_load_ptw;
		exception_table[EXCCODE_TLBS] = handle_tlb_store_ptw;
		exception_table[EXCCODE_TLBI] = handle_tlb_load_ptw;
		exception_table[EXCCODE_TLBM] = handle_tlb_modify_ptw;
	} else {
		memcpy((void *)tlbrentry, handle_tlb_refill, 0x80);
		local_flush_icache_range(tlbrentry, tlbrentry + 0x80);
	}

	for (i = EXCCODE_TLBL; i <= EXCCODE_TLBPE; i++)
		set_handler(i * VECSIZE, exception_table[i], VECSIZE);
}

void __init trap_init(void)
{
	unsigned long i;
	void *vec_start;

	/* set interrupt vector handler */
	for (i = EXCCODE_INT_START; i < EXCCODE_INT_END; i++) {
		vec_start = vector_table[i - EXCCODE_INT_START];
		set_handler(i * VECSIZE, vec_start, VECSIZE);
	}

	for (i = EXCCODE_ADE; i <= EXCCODE_BTDIS; i++)
		set_handler(i * VECSIZE, exception_table[i], VECSIZE);

	cache_error_setup();
}

static int trap_pm_notifier(struct notifier_block *self, unsigned long cmd,
			    void *v)
{
	switch (cmd) {
	case CPU_PM_ENTER_FAILED:
	case CPU_PM_EXIT:
		configure_exception_vector();

		/* Restore register with CPU number for TLB handlers */
		csr_write64((unsigned long) smp_processor_id(), LOONGARCH_CSR_TMID);

		break;
	}

	return NOTIFY_OK;
}

static struct notifier_block trap_pm_notifier_block = {
	.notifier_call = trap_pm_notifier,
};

static int __init trap_pm_init(void)
{
	return cpu_pm_register_notifier(&trap_pm_notifier_block);
}
arch_initcall(trap_pm_init);
