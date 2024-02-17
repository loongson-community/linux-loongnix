/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2014-2017 Linaro Ltd. <ard.biesheuvel@linaro.org>
 *
 * Copyright (C) 2018 Andes Technology Corporation <zong@andestech.com>
 */

#include <linux/elf.h>
#include <linux/ftrace.h>
#include <linux/kernel.h>
#include <linux/module.h>

unsigned long module_emit_plt_entry(struct module *mod, unsigned long val)
{
	struct mod_section *plt_idx_sec = &mod->arch.plt_idx;
	struct plt_idx_entry *plt_idx;
	struct mod_section *plt_sec = &mod->arch.plt;
	struct plt_entry *plt = get_plt_entry(val, plt_sec, plt_idx_sec);
	int i = plt_sec->num_entries;

	if (plt)
		return (unsigned long)plt;

	/* There is no duplicate entry, create a new one */
	plt_idx = (struct plt_idx_entry *)plt_idx_sec->shdr->sh_addr;
	plt_idx[i] = emit_plt_idx_entry(val);
	plt = (struct plt_entry *)plt_sec->shdr->sh_addr;
	plt[i] = emit_plt_entry(val);

	plt_sec->num_entries++;
	plt_idx_sec->num_entries++;
	BUG_ON(plt_sec->num_entries > plt_sec->max_entries);

	return (unsigned long)&plt[i];
}

static int is_rela_equal(const Elf_Rela *x, const Elf_Rela *y)
{
	return x->r_info == y->r_info && x->r_addend == y->r_addend;
}

static bool duplicate_rela(const Elf_Rela *rela, int idx)
{
	int i;
	for (i = 0; i < idx; i++) {
		if (is_rela_equal(&rela[i], &rela[idx]))
			return true;
	}
	return false;
}

static void count_max_entries(Elf_Rela *relas, int num, unsigned int *plts)
{
	unsigned int type, i;

	for (i = 0; i < num; i++) {
		type = ELF_R_TYPE(relas[i].r_info);
		if (type == R_LARCH_SOP_PUSH_PLT_PCREL) {
			if (!duplicate_rela(relas, i))
				(*plts)++;
		}
	}
}

int module_frob_arch_sections(Elf_Ehdr *ehdr, Elf_Shdr *sechdrs,
			      char *secstrings, struct module *mod)
{
	Elf_Shdr *tramp = NULL;
	unsigned int num_plts = 0;
	int i;

	/*
	 * Find the empty .plt sections.
	 */
	for (i = 0; i < ehdr->e_shnum; i++) {
		if (!strcmp(secstrings + sechdrs[i].sh_name, ".plt"))
			mod->arch.plt.shdr = sechdrs + i;
		else if (!strcmp(secstrings + sechdrs[i].sh_name, ".plt.idx"))
			mod->arch.plt_idx.shdr = sechdrs + i;
		else if (!strcmp(secstrings + sechdrs[i].sh_name, ".ftrace_trampoline"))
			tramp = sechdrs + i;
	}

	if (!mod->arch.plt.shdr) {
		pr_err("%s: module PLT section(s) missing\n", mod->name);
		return -ENOEXEC;
	}
	if (!mod->arch.plt_idx.shdr) {
		pr_err("%s: module PLT.IDX section(s) missing\n", mod->name);
		return -ENOEXEC;
	}

	/* Calculate the maxinum number of entries */
	for (i = 0; i < ehdr->e_shnum; i++) {
		Elf_Rela *relas = (void *)ehdr + sechdrs[i].sh_offset;
		int num_rela = sechdrs[i].sh_size / sizeof(Elf_Rela);
		Elf_Shdr *dst_sec = sechdrs + sechdrs[i].sh_info;

		if (sechdrs[i].sh_type != SHT_RELA)
			continue;

		/* ignore relocations that operate on non-exec sections */
		if (!(dst_sec->sh_flags & SHF_EXECINSTR))
			continue;

		count_max_entries(relas, num_rela, &num_plts);
	}

	mod->arch.plt.shdr->sh_type = SHT_NOBITS;
	mod->arch.plt.shdr->sh_flags = SHF_EXECINSTR | SHF_ALLOC;
	mod->arch.plt.shdr->sh_addralign = L1_CACHE_BYTES;
	mod->arch.plt.shdr->sh_size = (num_plts + 1) * sizeof(struct plt_entry);
	mod->arch.plt.num_entries = 0;
	mod->arch.plt.max_entries = num_plts;

	mod->arch.plt_idx.shdr->sh_type = SHT_NOBITS;
	mod->arch.plt_idx.shdr->sh_flags = SHF_ALLOC;
	mod->arch.plt_idx.shdr->sh_addralign = L1_CACHE_BYTES;
	mod->arch.plt_idx.shdr->sh_size = (num_plts + 1) * sizeof(struct plt_idx_entry);
	mod->arch.plt_idx.num_entries = 0;
	mod->arch.plt_idx.max_entries = num_plts;

	if (tramp) {
		tramp->sh_type = SHT_NOBITS;
		tramp->sh_flags = SHF_EXECINSTR | SHF_ALLOC;
		tramp->sh_addralign = __alignof__(struct plt_entry);
		tramp->sh_size = NR_FTRACE_PLTS * sizeof(struct plt_entry);
	}

	return 0;
}
