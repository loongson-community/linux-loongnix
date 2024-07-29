// SPDX-License-Identifier: GPL-2.0
/*
 * boot.c - Architecture-Specific Low-Level ACPI Boot Support
 *
 * Copyright (C) 2020 Loongson Technology Corporation Limited
 * Author: lvjianmin <lvjianmin@loongson.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/acpi.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/bootmem.h>
#include <linux/acpi.h>
#include <linux/memblock.h>
#include <linux/serial_core.h>
#include <linux/of_fdt.h>
#include <asm/io.h>
#include <asm/numa.h>
#include <asm/cpu-info.h>
#include <loongson.h>
#include <loongson-pch.h>

int acpi_disabled;
EXPORT_SYMBOL(acpi_disabled);
int acpi_noirq;
int acpi_pci_disabled;
EXPORT_SYMBOL(acpi_pci_disabled);
int acpi_strict = 1; /* We have no workarounds on LoongArch */
int num_processors;
int disabled_cpus;
enum acpi_irq_model_id acpi_irq_model = ACPI_IRQ_MODEL_PIC;

u64 acpi_saved_sp;

struct acpi_madt_core_pic cpu_madt_core_pic[NR_CPUS];

int pptt_enabled;
EXPORT_SYMBOL(pptt_enabled);

#define MAX_LOCAL_APIC 256

#define PREFIX			"ACPI: "

/*
 * Following __acpi_xx functions should be implemented for sepecific cpu.
 * */
int acpi_gsi_to_irq(u32 gsi, unsigned int *irqp)
{
	if (irqp != NULL)
		*irqp = acpi_register_gsi(NULL, gsi, -1, -1);
	return (*irqp > 0) ? 0 : -EINVAL;
}
EXPORT_SYMBOL_GPL(acpi_gsi_to_irq);

int acpi_isa_irq_to_gsi(unsigned int isa_irq, u32 *gsi)
{
	if (gsi)
		*gsi = isa_irq;
	return 0;
}

/*
 * success: return IRQ number (>0)
 * failure: return < 0
 */
int acpi_register_gsi(struct device *dev, u32 gsi, int trigger, int polarity)
{
	struct irq_fwspec fwspec;

	if (gsi < GSI_MIN_CPU_IRQ) {
		fwspec.fwnode = pch_lpc_get_fwnode();
		fwspec.param[0] = gsi;
		fwspec.param[1] = acpi_dev_get_irq_type(trigger, polarity);
		fwspec.param_count = 2;
		return irq_create_fwspec_mapping(&fwspec);
	} else if (gsi >= GSI_MIN_CPU_IRQ && gsi <= GSI_MAX_CPU_IRQ) {
		fwspec.fwnode = liointc_get_fwnode();
		fwspec.param[0] = gsi - GSI_MIN_CPU_IRQ;
		fwspec.param[1] = acpi_dev_get_irq_type(trigger, polarity);
		fwspec.param_count = 2;
		return irq_create_fwspec_mapping(&fwspec);
	} else if (gsi >= GSI_MIN_PCH_IRQ) {
#ifdef CONFIG_LOONGSON_PCH_PIC
		int id;
		struct fwnode_handle *handle;

		id = find_pch_pic(gsi);
		if (id < 0)
			return -1;
		handle = pch_pic_get_fwnode(id);
		if (handle) {
			fwspec.fwnode = handle;
			fwspec.param[0] = gsi;
			fwspec.param[1] = acpi_dev_get_irq_type(trigger, polarity);
			fwspec.param_count = 2;
			return irq_create_fwspec_mapping(&fwspec);
		}
#endif
			return gsi;
	}
	return -1;
}
EXPORT_SYMBOL_GPL(acpi_register_gsi);

void acpi_unregister_gsi(u32 gsi)
{

}
EXPORT_SYMBOL_GPL(acpi_unregister_gsi);

static struct fwnode_handle *acpi_get_gsi_domain_id(u32 gsi)
{
	int id;
	struct fwnode_handle *domain_handle = NULL;

	switch (gsi) {
	case GSI_MIN_CPU_IRQ ... GSI_MAX_CPU_IRQ:
		domain_handle = liointc_get_fwnode();
		break;

	case LOONGSON_LPC_IRQ_BASE ... LOONGSON_LPC_IRQ_END:
		domain_handle = pch_lpc_get_fwnode();
		break;

	case LOONGSON_PCH_IRQ_BASE ... LOONGSON_PCH_IRQ_END:
		id = find_pch_pic(gsi);
		if (id >= 0)
			domain_handle = pch_pic_get_fwnode(id);
		break;
	}

	return domain_handle;
}

/**
 * acpi_get_irq_source_fwhandle() - Retrieve fwhandle from IRQ resource source.
 * @source: acpi_resource_source to use for the lookup.
 *
 * Description:
 * Retrieve the fwhandle of the device referenced by the given IRQ resource
 * source.
 *
 * Return:
 * The referenced device fwhandle or NULL on failure
 */
static struct fwnode_handle *
acpi_get_irq_source_fwhandle(const struct acpi_resource_source *source,
			     u32 gsi)
{
	struct fwnode_handle *result;
	struct acpi_device *device;
	acpi_handle handle;
	acpi_status status;

	if (!source->string_length)
		return acpi_get_gsi_domain_id(gsi);

	status = acpi_get_handle(NULL, source->string_ptr, &handle);
	if (WARN_ON(ACPI_FAILURE(status)))
		return NULL;

	device = acpi_bus_get_acpi_device(handle);
	if (WARN_ON(!device))
		return NULL;

	result = &device->fwnode;
	acpi_bus_put_acpi_device(device);
	return result;
}

/*
 * Context for the resource walk used to lookup IRQ resources.
 * Contains a return code, the lookup index, and references to the flags
 * and fwspec where the result is returned.
 */
struct acpi_irq_parse_one_ctx {
	int rc;
	unsigned int index;
	unsigned long *res_flags;
	struct irq_fwspec *fwspec;
};

/**
 * acpi_irq_parse_one_match - Handle a matching IRQ resource.
 * @fwnode: matching fwnode
 * @hwirq: hardware IRQ number
 * @triggering: triggering attributes of hwirq
 * @polarity: polarity attributes of hwirq
 * @polarity: polarity attributes of hwirq
 * @shareable: shareable attributes of hwirq
 * @ctx: acpi_irq_parse_one_ctx updated by this function
 *
 * Description:
 * Handle a matching IRQ resource by populating the given ctx with
 * the information passed.
 */
static inline void acpi_irq_parse_one_match(struct fwnode_handle *fwnode,
					    u32 hwirq, u8 triggering,
					    u8 polarity, u8 shareable,
					    struct acpi_irq_parse_one_ctx *ctx)
{
	if (!fwnode)
		return;
	ctx->rc = 0;
	*ctx->res_flags = acpi_dev_irq_flags(triggering, polarity, shareable);
	ctx->fwspec->fwnode = fwnode;
	ctx->fwspec->param[0] = hwirq;
	ctx->fwspec->param[1] = acpi_dev_get_irq_type(triggering, polarity);
	ctx->fwspec->param_count = 2;
}

/**
 * acpi_irq_parse_one_cb - Handle the given resource.
 * @ares: resource to handle
 * @context: context for the walk
 *
 * Description:
 * This is called by acpi_walk_resources passing each resource returned by
 * the _CRS method. We only inspect IRQ resources. Since IRQ resources
 * might contain multiple interrupts we check if the index is within this
 * one's interrupt array, otherwise we subtract the current resource IRQ
 * count from the lookup index to prepare for the next resource.
 * Once a match is found we call acpi_irq_parse_one_match to populate
 * the result and end the walk by returning AE_CTRL_TERMINATE.
 *
 * Return:
 * AE_OK if the walk should continue, AE_CTRL_TERMINATE if a matching
 * IRQ resource was found.
 */
static acpi_status acpi_irq_parse_one_cb(struct acpi_resource *ares,
					 void *context)
{
	struct acpi_irq_parse_one_ctx *ctx = context;
	struct acpi_resource_irq *irq;
	struct acpi_resource_extended_irq *eirq;
	struct fwnode_handle *fwnode;

	switch (ares->type) {
	case ACPI_RESOURCE_TYPE_IRQ:
		irq = &ares->data.irq;
		if (ctx->index >= irq->interrupt_count) {
			ctx->index -= irq->interrupt_count;
			return AE_OK;
		}
		fwnode = acpi_get_gsi_domain_id(irq->interrupts[ctx->index]);
		acpi_irq_parse_one_match(fwnode, irq->interrupts[ctx->index],
					 irq->triggering, irq->polarity,
					 irq->sharable, ctx);
		return AE_CTRL_TERMINATE;
	case ACPI_RESOURCE_TYPE_EXTENDED_IRQ:
		eirq = &ares->data.extended_irq;
		if (eirq->producer_consumer == ACPI_PRODUCER)
			return AE_OK;
		if (ctx->index >= eirq->interrupt_count) {
			ctx->index -= eirq->interrupt_count;
			return AE_OK;
		}
		fwnode = acpi_get_irq_source_fwhandle(&eirq->resource_source,
						      eirq->interrupts[ctx->index]);
		acpi_irq_parse_one_match(fwnode, eirq->interrupts[ctx->index],
					 eirq->triggering, eirq->polarity,
					 eirq->sharable, ctx);
		return AE_CTRL_TERMINATE;
	}

	return AE_OK;
}

/**
 * acpi_irq_parse_one - Resolve an interrupt for a device
 * @handle: the device whose interrupt is to be resolved
 * @index: index of the interrupt to resolve
 * @fwspec: structure irq_fwspec filled by this function
 * @flags: resource flags filled by this function
 *
 * Description:
 * Resolves an interrupt for a device by walking its CRS resources to find
 * the appropriate ACPI IRQ resource and populating the given struct irq_fwspec
 * and flags.
 *
 * Return:
 * The result stored in ctx.rc by the callback, or the default -EINVAL value
 * if an error occurs.
 */
static int acpi_irq_parse_one(acpi_handle handle, unsigned int index,
			      struct irq_fwspec *fwspec, unsigned long *flags)
{
	struct acpi_irq_parse_one_ctx ctx = { -EINVAL, index, flags, fwspec };

	acpi_walk_resources(handle, METHOD_NAME__CRS, acpi_irq_parse_one_cb, &ctx);
	return ctx.rc;
}

/**
 * acpi_irq_get - Lookup an ACPI IRQ resource and use it to initialize resource.
 * @handle: ACPI device handle
 * @index:  ACPI IRQ resource index to lookup
 * @res:    Linux IRQ resource to initialize
 *
 * Description:
 * Look for the ACPI IRQ resource with the given index and use it to initialize
 * the given Linux IRQ resource.
 *
 * Return:
 * 0 on success
 * -EINVAL if an error occurs
 * -EPROBE_DEFER if the IRQ lookup/conversion failed
 */
int acpi_irq_get(acpi_handle handle, unsigned int index, struct resource *res)
{
	struct irq_fwspec fwspec;
	struct irq_domain *domain;
	unsigned long flags;
	int rc;

	rc = acpi_irq_parse_one(handle, index, &fwspec, &flags);
	if (rc)
		return rc;

	domain = irq_find_matching_fwnode(fwspec.fwnode, DOMAIN_BUS_ANY);
	if (!domain)
		return -EPROBE_DEFER;

	rc = irq_create_fwspec_mapping(&fwspec);
	if (rc <= 0)
		return -EINVAL;

	res->start = rc;
	res->end = rc;
	res->flags = flags;

	return 0;
}
EXPORT_SYMBOL_GPL(acpi_irq_get);

static int __init
acpi_parse_pch_pic(struct acpi_subtable_header *header,
		const unsigned long end)
{
	struct acpi_madt_io_apic *pch_pic = NULL;

	pch_pic = (struct acpi_madt_io_apic *)header;

	if (BAD_MADT_ENTRY(pch_pic, end))
		return -EINVAL;

	acpi_table_print_madt_entry(header);

	register_pch_pic(pch_pic->id, pch_pic->address,
			pch_pic->global_irq_base);

	return 0;
}

/*
 * Parse PCH_PIC related entries in MADT
 * returns 0 on success, < 0 on error
 */
static int __init acpi_parse_madt_pch_pic_entries(void)
{
	int count;

	/*
	 * ACPI interpreter is required to complete interrupt setup,
	 * so if it is off, don't enumerate the io-apics with ACPI.
	 * If MPS is present, it will handle them,
	 * otherwise the system will stay in PIC mode
	 */
	if (acpi_disabled || acpi_noirq)
		return -ENODEV;

	count = acpi_table_parse_madt(ACPI_MADT_TYPE_IO_APIC,
			acpi_parse_pch_pic, MAX_PCH_PICS);
	if (!count) {
		printk(KERN_ERR PREFIX "No PCH_PIC entries present\n");
		return -ENODEV;
	} else if (count < 0) {
		printk(KERN_ERR PREFIX "Error parsing PCH_PIC entry\n");
		return count;
	}

	return 0;
}

void __iomem *__init __acpi_map_table(unsigned long phys, unsigned long size)
{

	if (!phys || !size)
		return NULL;

	return early_memremap(phys, size);
}
void __init __acpi_unmap_table(void __iomem *map, unsigned long size)
{
	if (!map || !size)
		return;

	early_memunmap(map, size);
}

static int __init acpi_parse_fadt(struct acpi_table_header *table)
{
	u64 gpe0_ena;

	if (acpi_gbl_reduced_hardware)
		return 0;

	if (acpi_gbl_FADT.xgpe0_block.space_id != ACPI_ADR_SPACE_SYSTEM_MEMORY)
		goto err;
	gpe0_ena = acpi_gbl_FADT.xgpe0_block.address +
			acpi_gbl_FADT.gpe0_block_length / 2;
	if (!gpe0_ena)
		goto err;

	loongson_sysconf.gpe0_ena_reg = TO_UNCAC(gpe0_ena);

	return 0;
err:
	pr_err(PREFIX "Invalid BIOS FADT, disabling ACPI\n");
	disable_acpi();
	return -1;
}

static int set_processor_mask(u32 id, u32 flags)
{

	int cpu, cpuid = id;

	if (num_processors >= nr_cpu_ids) {
		pr_warn("acpi: nr_cpus/possible_cpus limit of %i reached."
			"processor 0x%x ignored.\n", nr_cpu_ids, cpuid);
			return -ENODEV;
	}
	if (cpuid == loongson_sysconf.boot_cpu_id)
		cpu = 0;
	else
		cpu = cpumask_next_zero(-1, cpu_present_mask);

	if (flags & ACPI_MADT_ENABLED) {
		set_cpu_possible(cpu, true);
		set_cpu_present(cpu, true);
		__cpu_number_map[cpuid] = cpu;
		__cpu_logical_map[cpu] = cpuid;
		num_processors++;
		loongson_sysconf.reserved_cpus_mask &= (~(1 << cpuid));
	} else
		disabled_cpus++;
	return cpu;
}

static int __init
acpi_parse_lapic(struct acpi_subtable_header *header, const unsigned long end)
{
	struct acpi_madt_local_apic *processor = NULL;
	processor = (struct acpi_madt_local_apic *)header;

	if (BAD_MADT_ENTRY(processor, end))
		return -EINVAL;

	acpi_table_print_madt_entry(header);

	set_processor_mask(processor->id, processor->lapic_flags);
	return 0;
}

static int __init
acpi_parse_core_pic(struct acpi_subtable_header *header, const unsigned long end)
{
	struct acpi_madt_core_pic *processor = NULL;
	processor = (struct acpi_madt_core_pic *)header;

	if (BAD_MADT_ENTRY(processor, end))
		return -EINVAL;
	cpu_madt_core_pic[processor->core_id] = *processor;
	acpi_table_print_madt_entry(header);

	set_processor_mask(processor->core_id, processor->flags);
	return 0;
}

static int __init acpi_parse_madt_core_pic_entries(void)
{
	int ret;
	struct acpi_subtable_proc madt_proc[1];

	memset(madt_proc, 0, sizeof(madt_proc));
	madt_proc[0].id = ACPI_MADT_TYPE_CORE_PIC;
	madt_proc[0].handler = acpi_parse_core_pic;
	ret = acpi_table_parse_entries_array(ACPI_SIG_MADT,
				sizeof(struct acpi_table_madt),
				madt_proc, ARRAY_SIZE(madt_proc),
				MAX_LOCAL_APIC);
	if (ret < 0) {
		pr_err(PREFIX "Error parsing LAPIC entries\n");
		return ret;
	}

	return 0;
}

static int __init acpi_parse_madt_lapic_entries(void)
{
	int ret;
	struct acpi_subtable_proc madt_proc[1];

	memset(madt_proc, 0, sizeof(madt_proc));
	madt_proc[0].id = ACPI_MADT_TYPE_LOCAL_APIC;
	madt_proc[0].handler = acpi_parse_lapic;
	ret = acpi_table_parse_entries_array(ACPI_SIG_MADT,
				sizeof(struct acpi_table_madt),
				madt_proc, ARRAY_SIZE(madt_proc),
				MAX_LOCAL_APIC);
	if (ret < 0) {
		pr_err(PREFIX "Error parsing LAPIC entries\n");
		return ret;
	}

	return 0;
}

static void __init acpi_process_madt(void)
{
	int i;

	for (i = 0; i < NR_CPUS; i++) {
		__cpu_number_map[i] = -1;
		__cpu_logical_map[i] = -1;
	}
	loongson_sysconf.reserved_cpus_mask = 0xFFFF;

	if (loongson_sysconf.bpi_version <= BPI_VERSION_V1 && efi_bp) {
		/* Parse MADT LAPIC entries */
		if (acpi_parse_madt_lapic_entries())
			goto err;
		/* Parse MADT IO APIC entries */
		acpi_parse_madt_pch_pic_entries();
	} else {
		/* Parse MADT CORE PIC entries */
		if (acpi_parse_madt_core_pic_entries())
			goto err;
	}
	acpi_irq_model = ACPI_IRQ_MODEL_LPIC;
	loongson_sysconf.nr_cpus = num_processors;

	return;
err:
	pr_err(PREFIX "Invalid BIOS MADT, disabling ACPI\n");
	disable_acpi();
	loongson_sysconf.nr_cpus = num_processors;
}

static int __init acpi_parse_pptt(void)
{
	int cpu, topology_id;

	for_each_possible_cpu(cpu) {

		topology_id = find_acpi_cpu_topology(cpu, 0);
		if (topology_id < 0) {
			pr_warn("Invalid BIOS PPTT\n");
			return -ENOENT;
		}

		if (acpi_pptt_cpu_is_thread(cpu) > 0) {
			topology_id = find_acpi_cpu_topology(cpu, 1);
			if (topology_id < 0)
				return -ENOENT;
			cpu_data[cpu].core = topology_id;
		} else {
			cpu_data[cpu].core = topology_id;
		}
	}

	pptt_enabled = 1;
	return 0;
}

#ifndef CONFIG_SUSPEND
int (*acpi_suspend_lowlevel)(void);
#else
int (*acpi_suspend_lowlevel)(void) = loongarch_acpi_suspend;
#endif

void __init acpi_boot_table_init(void)
{
	/*
	 * If acpi_disabled, bail out after check earlycon.
	 */
	if (acpi_disabled)
		goto fdt_earlycon;

	/*
	 * Initialize the ACPI boot-time table parser.
	 */
	if (acpi_table_init()) {
		disable_acpi();
		goto fdt_earlycon;
	}

	loongson_sysconf.boot_cpu_id = read_csr_cpuid();

	acpi_table_parse(ACPI_SIG_FADT, acpi_parse_fadt);

	/*
	 * Process the Multiple APIC Description Table (MADT), if present
	 */
	acpi_process_madt();
	/*
	 * Parse ACPI-PPTT table
	 */
	acpi_parse_pptt();

	/* Do not enable ACPI SPCR console by default */
	acpi_parse_spcr(earlycon_acpi_spcr_enable, false);

fdt_earlycon:
	if (earlycon_acpi_spcr_enable)
		early_init_dt_scan_chosen_stdout();
}

#ifdef CONFIG_ACPI_NUMA

static __init int setup_node(int pxm)
{
	return acpi_map_pxm_to_node(pxm);
}

/*
 * Callback for SLIT parsing.  pxm_to_node() returns NUMA_NO_NODE for
 * I/O localities since SRAT does not list them.  I/O localities are
 * not supported at this point.
 */
extern unsigned char __node_distances[MAX_NUMNODES][MAX_NUMNODES];
unsigned int numa_distance_cnt;

static inline unsigned int get_numa_distances_cnt(struct acpi_table_slit *slit)
{
	return slit->locality_count;
}

void __init numa_set_distance(int from, int to, int distance)
{
	if ((u8)distance != distance || (from == to && distance != LOCAL_DISTANCE)) {
		pr_warn_once("Warning: invalid distance parameter, from=%d to=%d distance=%d\n",
				from, to, distance);
		return;
	}

	__node_distances[from][to] = distance;
}

/* Callback for Proximity Domain -> CPUID mapping */
void __init
acpi_numa_processor_affinity_init(struct acpi_srat_cpu_affinity *pa)
{
	int pxm, node;

	if (srat_disabled())
		return;
	if (pa->header.length != sizeof(struct acpi_srat_cpu_affinity)) {
		bad_srat();
		return;
	}
	if ((pa->flags & ACPI_SRAT_CPU_ENABLED) == 0)
		return;
	pxm = pa->proximity_domain_lo;
	if (acpi_srat_revision >= 2) {
		pxm |= (pa->proximity_domain_hi[0] << 8);
		pxm |= (pa->proximity_domain_hi[1] << 16);
		pxm |= (pa->proximity_domain_hi[2] << 24);
	}
	node = setup_node(pxm);
	if (node < 0) {
		printk(KERN_ERR "SRAT: Too many proximity domains %x\n", pxm);
		bad_srat();
		return;
	}

	if (pa->apic_id >= CONFIG_NR_CPUS) {
		printk(KERN_INFO "SRAT: PXM %u -> CPU 0x%02x -> Node %u skipped apicid that is too big\n",
				pxm, pa->apic_id, node);
		return;
	}

	early_numa_add_cpu(pa->apic_id, node);

	set_cpuid_to_node(pa->apic_id, node);
	node_set(node, numa_nodes_parsed);
	acpi_numa = 1;
	printk(KERN_INFO "SRAT: PXM %u -> CPU 0x%02x -> Node %u\n",
		pxm, pa->apic_id, node);
}

void __init acpi_numa_arch_fixup(void) {}
#endif

void __init arch_reserve_mem_area(acpi_physical_address addr, size_t size)
{
	memblock_mark_nomap(addr, size);
}
#ifdef CONFIG_ACPI_HOTPLUG_CPU
#include <acpi/processor.h>
static int __ref acpi_map_cpu2node(acpi_handle handle, int cpu, int physid)
{
#ifdef CONFIG_ACPI_NUMA
	int nid;

	nid = acpi_get_node(handle);
	if (nid != NUMA_NO_NODE) {
		set_cpuid_to_node(physid, nid);
		node_set(nid, numa_nodes_parsed);
		set_cpu_numa_node(cpu, nid);
	}
#endif
	return 0;
}

int acpi_map_cpu(acpi_handle handle, phys_cpuid_t physid, u32 acpi_id,
		 int *pcpu)
{
	int cpu;

	cpu = set_processor_mask(physid, ACPI_MADT_ENABLED);
	if (cpu < 0) {
		pr_info(PREFIX "Unable to map lapic to logical cpu number\n");
		return cpu;
	}

	acpi_map_cpu2node(handle, cpu, physid);

	*pcpu = cpu;

	return 0;
}
EXPORT_SYMBOL(acpi_map_cpu);

int acpi_unmap_cpu(int cpu)
{
#ifdef CONFIG_ACPI_NUMA
	set_cpuid_to_node(cpu_logical_map(cpu), NUMA_NO_NODE);
#endif
	set_cpu_present(cpu, false);
	num_processors--;

	pr_info("cpu%d hot remove!\n", cpu);

	return 0;
}
EXPORT_SYMBOL(acpi_unmap_cpu);
#endif /* CONFIG_ACPI_HOTPLUG_CPU */
