/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2020 Loongson Technology Co., Ltd.
 */
#ifndef __ASM_TOPOLOGY_H
#define __ASM_TOPOLOGY_H

#include <asm-generic/topology.h>
#include <asm/mach-la64/topology.h>
#include <linux/smp.h>

#ifdef CONFIG_SMP
extern unsigned int __max_packages;
#define topology_max_packages()                 (__max_packages)
#define topology_physical_package_id(cpu)	(cpu_data[cpu].package)
#define topology_core_id(cpu)			(cpu_data[cpu].core)
#define topology_core_cpumask(cpu)		(&cpu_core_map[cpu])
#define topology_sibling_cpumask(cpu)		(&cpu_sibling_map[cpu])
#else
#define topology_max_packages()                 (1)
#endif

static inline void arch_fix_phys_package_id(int num, u32 slot) { }
#endif /* __ASM_TOPOLOGY_H */
