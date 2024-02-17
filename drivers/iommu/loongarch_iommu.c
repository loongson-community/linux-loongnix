// SPDX-License-Identifier: GPL-2.0
/*
 * Loongson IOMMU Driver
 *
 * Copyright (C) 2020-2021 Loongson Technology Ltd.
 * Author:	Lv Chen <lvchen@loongson.cn>
 *		Wang Yang <wangyang@loongson.cn>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/printk.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/iommu.h>
#include <linux/sizes.h>
#include <asm/addrspace.h>
#include <asm/mach-la64/mem.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/pci_regs.h>
#include "loongarch_iommu.h"

#define LOOP_TIMEOUT		100000

/* Lock for domain allocing */
static DEFINE_SPINLOCK(domain_bitmap_lock);

/* Lock for priv->list */
static DEFINE_SPINLOCK(la_iommu_priv_lock);

/* Lock for bitmap of page table */
static DEFINE_SPINLOCK(pgtable_bitmap_lock);

/* Lock for iommu page table */
static DEFINE_SPINLOCK(la_iommu_pgtlock);

/* List of all domain privates */
static LIST_HEAD(la_iommu_priv_list);

/* List of all available dev_data structures */
static LIST_HEAD(la_dev_data_list);

/* List of shadow page table */
static LIST_HEAD(iommu_shadow_pglist);

/* Bitmap of global domains */
void *la_iommu_domain_alloc_bitmap;

/* Bitmap of devtable and pages for page table */
void *la_iommu_devtable_bitmap;
void *la_iommu_pgtable_alloc_bitmap;

static struct iommu_ops la_iommu_ops;
unsigned long iommu_mem_base;
unsigned long iommu_pgt_base;

int loongarch_iommu_disable;

static void iommu_write_regl(unsigned long off, u32 val)
{
	*(u32 *)(iommu_mem_base + off) = val;
	mb();
}

static u32 iommu_read_regl(unsigned long off)
{
	u32 val;

	val = *(u32 *)(iommu_mem_base + off);
	mb();
	return val;
}

static void iommu_translate_disable(void)
{
	u32 val = iommu_read_regl(LA_IOMMU_EIVDB);

	/* Disable */
	val &= ~(1 << 31);
	iommu_write_regl(LA_IOMMU_EIVDB, val);

	/* Write cmd */
	val = iommu_read_regl(LA_IOMMU_CMD);
	val &= 0xfffffffc;
	iommu_write_regl(LA_IOMMU_CMD, val);
}

static void iommu_translate_enable(void)
{
	u32 val = 0;

	val = iommu_read_regl(LA_IOMMU_EIVDB);

	/* Enable */
	val |= (1 << 31);
	iommu_write_regl(LA_IOMMU_EIVDB, val);

	/* Write cmd */
	val = iommu_read_regl(LA_IOMMU_CMD);
	val &= 0xfffffffc;
	iommu_write_regl(LA_IOMMU_CMD, val);
}

static bool la_iommu_capable(enum iommu_cap cap)
{
	switch (cap) {
	case IOMMU_CAP_CACHE_COHERENCY:
		return true;
	default:
		return false;
	}
}

static la_iommu_priv *to_la_iommu_priv(struct iommu_domain *dom)
{
	return container_of(dom, la_iommu_priv, domain);
}

/*
 * Check whether the system has a priv.
 * If yes, it returns 1 and if not, it returns 0
 */
static int check_has_priv(void)
{
	spin_lock(&la_iommu_priv_lock);
	while (!list_empty(&la_iommu_priv_list)) {
		spin_unlock(&la_iommu_priv_lock);
		return 1;
	}
	spin_unlock(&la_iommu_priv_lock);

	return 0;
}

static int update_dev_table(u16 domain_id,
		struct la_iommu_dev_data *dev_data, int flag)
{
	u32 val = 0;
	int index;
	unsigned short bdf;

	bdf = dev_data->bdf;

	/* Set device table */
	if (flag) {
		index = find_first_zero_bit(la_iommu_devtable_bitmap,
						MAX_ATTACHED_DEV_ID);
		if (index < MAX_ATTACHED_DEV_ID) {
			__set_bit(index, la_iommu_devtable_bitmap);
			dev_data->index = index;
		}

		val = bdf & 0xffff;
		val |= ((domain_id & 0xf) << 16);	/* domain id */
		val |= ((index & 0xf) << 24);		/* index */
		val |= (0x1 << 20);			/* valid */
		val |= (0x1 << 31);			/* enable */
		iommu_write_regl(LA_IOMMU_EIVDB, val);

		val = iommu_read_regl(LA_IOMMU_CMD);
		val &= 0xfffffffc;
		iommu_write_regl(LA_IOMMU_CMD, val);
	} else {
		/* Flush device table */
		index = dev_data->index;

		val = iommu_read_regl(LA_IOMMU_EIVDB);
		val &= ~(0x7fffffff);
		val |= ((index & 0xf) << 24);		/* index */
		iommu_write_regl(LA_IOMMU_EIVDB, val);

		val = iommu_read_regl(LA_IOMMU_CMD);
		val &= 0xfffffffc;
		iommu_write_regl(LA_IOMMU_CMD, val);

		if (index < MAX_ATTACHED_DEV_ID)
			__clear_bit(index, la_iommu_devtable_bitmap);
	}

	return 0;
}

static void flush_iotlb(void)
{
	u32 val, cmd;

	val = iommu_read_regl(LA_IOMMU_VBTC);
	val &= ~0x1f;

	/* Flush all tlb */
	val |= 0x5;
	iommu_write_regl(LA_IOMMU_VBTC, val);

	cmd = iommu_read_regl(LA_IOMMU_CMD);
	cmd &= 0xfffffffc;
	iommu_write_regl(LA_IOMMU_CMD, cmd);
}

static int flush_pgtable_is_busy(void)
{
	u32 val = iommu_read_regl(LA_IOMMU_VBTC);

	return val & IOMMU_PGTABLE_BUSY;
}

static int iommu_flush_iotlb_all(la_iommu_priv *priv)
{
	u32 retry = 0;

	flush_iotlb();
	while (flush_pgtable_is_busy()) {
		if (retry == LOOP_TIMEOUT) {
			pr_err("LA-IOMMU: iotlb flush busy\n");
			return -EIO;
		}
		retry++;
		udelay(1);
	}
	iommu_translate_enable();

	return 0;
}

static void priv_flush_iotlb_pde(la_iommu_priv *priv)
{
	iommu_flush_iotlb_all(priv);
}

static void do_attach(struct la_iommu_priv *priv,
			struct la_iommu_dev_data *dev_data)
{
	if (!dev_data->count)
		return;
	dev_data->priv = priv;
	list_add(&dev_data->list, &priv->dev_list);
	priv->dev_cnt += 1;

	update_dev_table(priv->id, dev_data, 1);
	if (priv->dev_cnt > 0)
		priv_flush_iotlb_pde(priv);
}

static void do_detach(struct la_iommu_priv *priv,
			struct la_iommu_dev_data *dev_data)
{
	if (dev_data->count)
		return;

	list_del(&dev_data->list);
	priv->dev_cnt -= 1;
	update_dev_table(priv->id, dev_data, 0);

	dev_data->priv = NULL;
}

static void cleanup_domain(struct la_iommu_priv *priv)
{
	struct la_iommu_dev_data *entry;

	spin_lock(&priv->devlock);

	while (!list_empty(&priv->dev_list)) {
		entry = list_first_entry(&priv->dev_list,
				struct la_iommu_dev_data, list);
		do_detach(priv, entry);
	}

	spin_unlock(&priv->devlock);
}

static int domain_id_alloc(void)
{
	int id = -1;

	spin_lock(&domain_bitmap_lock);
	id = find_first_zero_bit(la_iommu_domain_alloc_bitmap, MAX_DOMAIN_ID);
	if (id < MAX_DOMAIN_ID)
		__set_bit(id, la_iommu_domain_alloc_bitmap);
	else
		pr_err("LA-IOMMU: Alloc domain id over max domain id\n");

	spin_unlock(&domain_bitmap_lock);

	return id;
}

static void domain_id_free(int id)
{
	spin_lock(&domain_bitmap_lock);
	if ((id >= 0) && (id < MAX_DOMAIN_ID))
		__clear_bit(id, la_iommu_domain_alloc_bitmap);

	spin_unlock(&domain_bitmap_lock);
}

/*
 *  * This function adds a private domain to the global domain list
 */
static void add_domain_to_list(struct la_iommu_priv *priv)
{
	spin_lock(&la_iommu_priv_lock);
	list_add(&priv->list, &la_iommu_priv_list);
	spin_unlock(&la_iommu_priv_lock);
}

static void del_domain_from_list(struct la_iommu_priv *priv)
{
	spin_lock(&la_iommu_priv_lock);
	list_del(&priv->list);
	spin_unlock(&la_iommu_priv_lock);
}

static void iommu_pgd_init(void *pgd)
{
	memset(pgd, 0x0, IOMMU_PAGE_SIZE);
}

static void iommu_pmd_init(void *pmd)
{
	memset(pmd, 0x0, IOMMU_PAGE_SIZE);
}

static inline int iommu_pgd_none(unsigned long *pgd)
{
	return *pgd == 0x0;
}

static inline int iommu_pmd_none(unsigned long *pmd)
{
	return *pmd == 0x0;
}

static shadow_pg_entry *iommu_zalloc_page(struct la_iommu_priv *priv)
{
	int index = 0;
	iommu_pte *new_pg;
	shadow_pg_entry *entry;
	unsigned long addr;

	entry = kmalloc(sizeof(*entry), GFP_ATOMIC);
	if (!entry) {
		kfree(entry);
		return NULL;
	}

	spin_lock(&pgtable_bitmap_lock);
	index = find_first_zero_bit(la_iommu_pgtable_alloc_bitmap,
							MAX_PAGES_NUM);
	if (index < MAX_PAGES_NUM) {
		addr = iommu_pgt_base + index * IOMMU_PAGE_SIZE;
		new_pg = (iommu_pte *)(addr);
		memset(new_pg, 0x0, IOMMU_PAGE_SIZE);
		entry->index = index;

		addr = get_zeroed_page(GFP_ATOMIC);
		entry->va = (unsigned long *)(addr);
		if (!entry->va) {
			spin_unlock(&pgtable_bitmap_lock);
			pr_err("LA-IOMMU: get zeroed page err\n");
			kfree(entry);
			return NULL;
		}
		__set_bit(index, la_iommu_pgtable_alloc_bitmap);

		entry->pa = virt_to_phys(entry->va) & IOMMU_PAGE_MASK;
		list_add_tail(&entry->pglist, &iommu_shadow_pglist);

		priv->used_pages++;
	} else {
		pr_err("LA-IOMMU: not enough memory for iommu page table\n");
		kfree(entry);
		return NULL;
	}
	spin_unlock(&pgtable_bitmap_lock);

	return entry;
}

static void iommu_free_page(struct la_iommu_priv *priv, shadow_pg_entry *entry)
{
	unsigned long addr;

	spin_lock(&pgtable_bitmap_lock);
	if (entry->index < MAX_PAGES_NUM) {
		addr = iommu_pgt_base + entry->index * IOMMU_PAGE_SIZE;
		memset((void *)(addr), 0x0, IOMMU_PAGE_SIZE);
		__clear_bit(entry->index, la_iommu_pgtable_alloc_bitmap);
		entry->index = -1;
		free_page((unsigned long)entry->va);
		list_del(&entry->pglist);
		kfree(entry);
		priv->used_pages--;
	}
	spin_unlock(&pgtable_bitmap_lock);
}

static shadow_pg_entry *index_to_pg_entry(int index)
{
	struct shadow_pg_entry *entry;

	list_for_each_entry(entry, &iommu_shadow_pglist, pglist) {
		if (entry->index == index)
			return entry;
	}

	return NULL;
}

static shadow_pg_entry *pa_to_pg_entry(unsigned long pa)
{
	struct shadow_pg_entry *entry;

	list_for_each_entry(entry, &iommu_shadow_pglist, pglist) {
		if (entry->pa == pa)
			return entry;
	}

	return NULL;
}

static void free_pagetable(struct la_iommu_priv *priv, void *vaddr)
{
	unsigned long *pgd, *shd_pgd_entry, *shd_pmd_entry, *shd_tmp_pmd_entry;
	int index, i, j;
	shadow_pg_entry *entry, *entry1;
	unsigned long pa, tmp;

	pgd = (unsigned long *)vaddr;
	index = ((unsigned long)pgd - iommu_pgt_base) / IOMMU_PAGE_SIZE;
	entry = index_to_pg_entry(index);
	if (!entry) {
		pr_err("LA-IOMMU: find err index:%d, pgd:0x%lx\n",
					index, (unsigned long)pgd);
		return;
	}

	for (i = 0; i < IOMMU_PTRS_PER_PGD; i++) {

		shd_pgd_entry = entry->va + i;

		if (!IOMMU_PTE_PRESENT(*shd_pgd_entry))
			continue;

		tmp = (*shd_pgd_entry) & IOMMU_PAGE_MASK;
		shd_pmd_entry = (unsigned long *)iommu_phys_to_virt(tmp);

		if (!priv->is_hugepage) {
			for (j = 0; j < IOMMU_PTRS_PER_PMD; j++) {

				shd_tmp_pmd_entry = shd_pmd_entry + j;

				if (!IOMMU_PTE_PRESENT(*shd_tmp_pmd_entry))
					continue;

				pa = (unsigned long)((*shd_tmp_pmd_entry) & IOMMU_PAGE_MASK);
				entry1 = pa_to_pg_entry(pa);
				if (!entry1) {
					pr_err("LA-IOMMU: find err id:%d,pgd:0x%lx\n",
						priv->id, (unsigned long)priv->pgd);
					continue;
				}
				iommu_free_page(priv, entry1);
			}
		}

		/* Free pmd page */
		pa = (unsigned long)((*shd_pgd_entry) & IOMMU_PAGE_MASK);
		entry1 = pa_to_pg_entry(pa);
		if (!entry1) {
			pr_err("LA-IOMMU: find err id:%d,pgd:0x%lx\n",
				priv->id, (unsigned long)priv->pgd);
			continue;
		}
		iommu_free_page(priv, entry1);
	}

	/* Free pgd page */
	iommu_free_page(priv, entry);
}

static int la_iommu_priv_init(struct la_iommu_priv *priv)
{
	unsigned long pgd_pa;
	int index;
	u32 dir_ctrl, pgd_lo, pgd_hi;
	shadow_pg_entry *entry;

	spin_lock_init(&priv->devlock);

	/* Alloc pgd page,set base in register */
	spin_lock(&la_iommu_pgtlock);

	entry = iommu_zalloc_page(priv);
	if (!entry) {
		spin_unlock(&la_iommu_pgtlock);
		pr_err("LA-IOMMU: alloc shadow page entry err\n");
		return -ENOMEM;
	}
	index = entry->index;
	priv->pgd = (iommu_pte *)(iommu_pgt_base + index * IOMMU_PAGE_SIZE);
	spin_unlock(&la_iommu_pgtlock);

	dir_ctrl = (IOMMU_PGDIR_WIDTH << 26) | (IOMMU_PGDIR_SHIFT << 20);
	dir_ctrl |= (IOMMU_PMD_WIDTH <<  16) | (IOMMU_PMD_SHIFT << 10);
	dir_ctrl |= (IOMMU_PTE_WIDTH << 6) | IOMMU_PAGE_SHIFT;

	pgd_pa = iommu_gmem_virt_to_phys((unsigned long)priv->pgd);

	pgd_hi = pgd_pa >> 32;
	pgd_lo = pgd_pa & 0xffffffff;

	iommu_write_regl(LA_IOMMU_DIR_CTRL(priv->id), dir_ctrl);
	iommu_write_regl(LA_IOMMU_PGD_HI(priv->id), pgd_hi);
	iommu_write_regl(LA_IOMMU_PGD_LO(priv->id), pgd_lo);

	INIT_LIST_HEAD(&priv->dev_list);

	/* 0x10000000~0x8fffffff */
	priv->virtio_pgtable = (unsigned long *)__get_free_pages(GFP_KERNEL, 6);
	if ((!priv->virtio_pgtable)) {
		pr_err("LA-IOMMU: get free page err\n");
		goto fail_nomem;
	}
	memset(priv->virtio_pgtable, 0x0, LA_VIRTIO_PAGE_SIZE * 64);

	iommu_pgd_init(priv->pgd);
	iommu_pgd_init(entry->va);

	return 0;

fail_nomem:
	spin_lock(&la_iommu_pgtlock);
	free_pagetable(priv, priv->pgd);
	spin_unlock(&la_iommu_pgtlock);
	priv->pgd = 0;

	return -ENOMEM;
}

static la_iommu_priv *la_iommu_alloc_priv(void)
{
	la_iommu_priv *priv;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);

	if (!priv)
		return NULL;

	priv->id = domain_id_alloc();

	if (la_iommu_priv_init(priv))
		goto out_err;
	add_domain_to_list(priv);

	return priv;

out_err:
	kfree(priv);

	return NULL;

}

static void la_iommu_priv_free(struct la_iommu_priv *priv)
{
	if (!priv)
		return;

	/* 0x10000000~0x8fffffff */
	if (priv->virtio_pgtable) {
		free_pages((unsigned long)priv->virtio_pgtable, 6);
		priv->virtio_pgtable = NULL;
	}

	del_domain_from_list(priv);

	if ((priv->id >= 0) && (priv->id < MAX_DOMAIN_ID)) {
		domain_id_free(priv->id);
		kfree(priv);
	}
}

static struct iommu_domain *la_iommu_domain_alloc(unsigned type)
{
	la_iommu_priv *priv;

	switch (type) {
	case IOMMU_DOMAIN_UNMANAGED:
		priv = la_iommu_alloc_priv();
		if (!priv)
			return NULL;

		priv->domain.geometry.aperture_start	= 0;
		priv->domain.geometry.aperture_end	= ~0ULL;
		priv->domain.geometry.force_aperture	= true;

		break;
	default:
		return NULL;
		}

	return &priv->domain;
}

static void la_iommu_domain_free(struct iommu_domain *domain)
{

	la_iommu_priv *priv;

	priv = to_la_iommu_priv(domain);
	if (!priv) {
		pr_info("LA-IOMMU: priv is null\n");
		return;
	}

	if (priv->dev_cnt > 0)
		cleanup_domain(priv);

	spin_lock(&la_iommu_pgtlock);
	free_pagetable(priv, priv->pgd);
	spin_unlock(&la_iommu_pgtlock);

	la_iommu_priv_free(priv);
	iommu_flush_iotlb_all(priv);
	if (!check_has_priv())
		iommu_translate_disable();
}

static int iommu_init_device(struct device *dev)
{
	struct la_iommu_dev_data *dev_data;
	struct pci_dev	*pdev = to_pci_dev(dev);
	struct pci_bus	*bus = pdev->bus;
	unsigned short bdf, devid;
	unsigned char busnum;

	bdf = pdev->devfn & 0xff;
	busnum = bus->number;
	if (busnum != 0) {
		while (bus->parent->parent)
			bus = bus->parent;
		bdf = bus->self->devfn & 0xff;

		list_for_each_entry(dev_data, &la_dev_data_list, glist) {
			if (dev_data->bdf == bdf) {
				pr_info("LA-IOMMU: bdf:0x%x has added\n", bdf);
				return 0;
			}
		}

	}

	dev_data = kzalloc(sizeof(*dev_data), GFP_KERNEL);
	if (!dev_data)
		return -ENOMEM;

	devid = PCI_DEVID(bus->number, bdf);

	dev_data->bdf = devid;
	/* The initial state is 0, and 1 is added only when attach dev */
	dev_data->count = 0;

	dev->archdata.iommu = dev_data;
	list_add_tail(&dev_data->glist, &la_dev_data_list);

	return 0;
}

static int la_iommu_add_device(struct device *dev)
{
	struct iommu_group *group;
	int ret = 0;

	ret = iommu_init_device(dev);
	if (ret)
		pr_err("LA-IOMMU: unable to alloc memory for dev_data\n");

	group = iommu_group_get_for_dev(dev);
	if (IS_ERR(group))
		return PTR_ERR(group);

	iommu_group_put(group);

	return 0;
}

static struct iommu_group *la_iommu_device_group(struct device *dev)
{
	struct iommu_group *group;

	/*
	 * We don't support devices sharing stream IDs other than PCI RID
	 * aliases, since the necessary ID-to-device lookup becomes rather
	 * impractical given a potential sparse 32-bit stream ID space.
	 */
	if (dev_is_pci(dev))
		group = pci_device_group(dev);
	else
		group = generic_device_group(dev);

	return group;
}

static void la_iommu_remove_device(struct device *dev)
{
	struct la_iommu_dev_data *dev_data;

	iommu_group_remove_device(dev);

	dev_data = dev->archdata.iommu;
	if (dev_data) {
		list_del(&dev_data->glist);
		kfree(dev_data);
	}

}

static struct la_iommu_dev_data *iommu_get_devdata(la_iommu_priv *priv,
							unsigned long bdf)
{
	struct la_iommu_dev_data *dev_data;
	/* Find from priv list */
	list_for_each_entry(dev_data, &priv->dev_list, list) {
		if (dev_data->bdf == bdf)
			return dev_data;
	}

	return NULL;
}

static int la_iommu_attach_dev(struct iommu_domain *domain,
							struct device *dev)
{
	la_iommu_priv *priv = to_la_iommu_priv(domain);
	struct pci_dev  *pdev = to_pci_dev(dev);
	struct pci_bus  *bus = pdev->bus;
	unsigned char busnum = pdev->bus->number;
	struct la_iommu_dev_data *dev_data;
	unsigned short bdf;

	bdf = pdev->devfn & 0xff;
	if (busnum != 0) {
		while (bus->parent->parent)
			bus = bus->parent;
		bdf = bus->self->devfn & 0xff;
	}

	spin_lock(&priv->devlock);
	dev_data = iommu_get_devdata(priv, bdf);
	spin_unlock(&priv->devlock);

	if (dev_data) {
		dev_data->count++;
		pr_info("LA-IOMMU: bdf 0x%x devfn %x has attached,count:0x%x\n",
			bdf, pdev->devfn, dev_data->count);
		return 0;
	}

	dev_data = dev->archdata.iommu;
	dev_data->count++;
	spin_lock(&priv->devlock);
	do_attach(priv, dev_data);
	spin_unlock(&priv->devlock);

	return 0;
}

static void la_iommu_detach_dev(struct iommu_domain *domain,
				 struct device *dev)
{
	la_iommu_priv *priv = to_la_iommu_priv(domain);
	struct pci_dev *pdev = to_pci_dev(dev);
	struct pci_bus *bus = pdev->bus;
	unsigned char busnum = pdev->bus->number;
	struct la_iommu_dev_data *dev_data;
	unsigned short bdf;

	bdf = pdev->devfn & 0xff;
	if (busnum != 0) {
		while (bus->parent->parent)
			bus = bus->parent;
		bdf = bus->self->devfn & 0xff;
	}

	spin_lock(&priv->devlock);
	dev_data = iommu_get_devdata(priv, bdf);
	spin_unlock(&priv->devlock);

	if (dev_data == NULL) {
		pr_info("LA-IOMMU: bdf 0x%x devfn %x count:0x%x, domainid:%d\n",
			bdf, pdev->devfn & 0xff, dev_data->count, priv->id);
			return;
	}

	dev_data->count--;

	spin_lock(&priv->devlock);
	do_detach(priv, dev_data);
	spin_unlock(&priv->devlock);
}

static unsigned long *la_iommu_walk_pgd(struct la_iommu_priv *priv,
					iommu_pte *pgd, unsigned long iova)
{

	unsigned long *pgd_entry, *shd_pgd_entry, *pmd_base;
	unsigned long *pmd_entry, *shd_pmd_entry, *pte, *pte_base;
	unsigned long va, pa;
	int index;
	shadow_pg_entry *entry, *entry1, *new_entry;

	index = ((unsigned long)pgd - iommu_pgt_base) / IOMMU_PAGE_SIZE;
	entry = index_to_pg_entry(index);
	if (!entry) {
		pr_err("LA-IOMMU: pgd:0x%lx, iova:0x%lx, index:%d\n",
			(unsigned long)pgd, iova, index);
		return NULL;
	}

	va = (unsigned long)entry->va;
	shd_pgd_entry = (unsigned long *)iommu_pgd_offset(va, iova);
	pgd_entry = (unsigned long *)iommu_pgd_offset((unsigned long)pgd, iova);

	if (iommu_pgd_none(shd_pgd_entry)) {
		iommu_pte *new_pmd;

		new_entry = iommu_zalloc_page(priv);
		if (!new_entry) {
			pr_err("LA-IOMMU: new_entry alloc err iova:0x%lx\n",
				iova);
			return NULL;
		}
		index = new_entry->index;
		new_pmd = (iommu_pte *)(iommu_pgt_base + index * IOMMU_PAGE_SIZE);

		iommu_pmd_init(new_entry->va);
		iommu_pmd_init(new_pmd);

		/* fill shd_pgd_entry */
		*shd_pgd_entry = new_entry->pa & IOMMU_PAGE_MASK;
		*shd_pgd_entry |= IOMMU_PTE_PR | IOMMU_PTE_IR | IOMMU_PTE_IW;

		/* fill gmem pgd_entry */
		*pgd_entry = iommu_gmem_virt_to_phys((unsigned long)new_pmd);
		*pgd_entry &= IOMMU_PAGE_MASK;
		*pgd_entry |= IOMMU_PTE_PR | IOMMU_PTE_IR | IOMMU_PTE_IW;
	}

	shd_pmd_entry = iommu_pmd_offset((unsigned long *)shd_pgd_entry, iova);

	/* *shd_pgd_entry is pmd base ,
	 * so entry1->index is the index of pmd_base in gmem
	 */
	pa = (unsigned long)(*shd_pgd_entry & IOMMU_PAGE_MASK);
	entry1 = pa_to_pg_entry(pa);
	index =  entry1->index;
	if (!entry1) {
		pr_err("LA-IOMMU: index:%d, id:%d, priv->pgd:0x%lx\n",
			index, priv->id, (unsigned long)priv->pgd);
		return NULL;
	}
	pmd_base = (unsigned long *)(iommu_pgt_base + index * IOMMU_PAGE_SIZE);
	pmd_entry = pmd_base + iommu_pmd_index(iova);

	if (priv->is_hugepage)
		return pmd_entry;

	if (iommu_pmd_none(shd_pmd_entry)) {
		iommu_pte *new_pte;

		new_entry = iommu_zalloc_page(priv);
		index = new_entry->index;
		if (!new_entry) {
			pr_err("LA-IOMMU: new_entry alloc err iova:0x%lx\n",
				iova);
			return NULL;
		}
		new_pte = (iommu_pte *)(iommu_pgt_base + index * IOMMU_PAGE_SIZE);

		/* fill shd_pmd_entry */
		*shd_pmd_entry = new_entry->pa & IOMMU_PAGE_MASK;
		*shd_pmd_entry |= IOMMU_PTE_PR | IOMMU_PTE_IR | IOMMU_PTE_IW;

		/* fill gmem pmd_entry */
		*pmd_entry = iommu_gmem_virt_to_phys((unsigned long)new_pte);
		*pmd_entry &= IOMMU_PAGE_MASK;
		*pmd_entry |= IOMMU_PTE_PR | IOMMU_PTE_IR | IOMMU_PTE_IW;
	}

	/* *shd_pmd_entry is pte base,
	 *  so entry1->index is the index of pte_base in gmem
	 */
	pa = (unsigned long)(*shd_pmd_entry & IOMMU_PAGE_MASK);
	entry1 = pa_to_pg_entry(pa);
	index = entry1->index;
	if (!entry1) {
		pr_err("LA-IOMMU: index:%d, id:%d, priv->pgd:0x%lx\n",
			index, priv->id, (unsigned long)priv->pgd);
		return NULL;
	}
	/* clear gmem pte */
	pte_base = (unsigned long *)(iommu_pgt_base + index * IOMMU_PAGE_SIZE);
	pte = pte_base + iommu_pte_index(iova);

	return pte;
}

static unsigned long *la_iommu_pte_for_iova(struct la_iommu_priv *priv,
					unsigned long iova)
{
	return  la_iommu_walk_pgd(priv, priv->pgd, iova);
}

static unsigned long *iommu_fetch_pte(iommu_pte *pgd, unsigned long iova,
							bool iommu_is_hugepage)
{
	unsigned long *pgd_entry;
	unsigned long *pmd_entry;

	pgd_entry = (unsigned long *)iommu_pgd_offset((unsigned long)pgd, iova);

	if (!IOMMU_PTE_PRESENT(*pgd_entry)) {
		pr_err("LA-IOMMU: pmd is not present\n");
		return NULL;
	}

	pmd_entry = iommu_pmd_offset((unsigned long *)pgd_entry, iova);

	if (iommu_is_hugepage || IOMMU_PTE_HUGEPAGE(*pmd_entry))
		return pmd_entry;

	if (!IOMMU_PTE_PRESENT(*pmd_entry)) {
		pr_err("LA-IOMMU: pte is not present\n");
		return NULL;
	}


	return iommu_pte_offset(pmd_entry, iova);
}

static unsigned long *iommu_fetch_pmd_entry(iommu_pte *pgd, unsigned long iova,
							bool iommu_is_hugepage)
{
	unsigned long *pgd_entry;
	unsigned long *pmd_entry;

	pgd_entry = (unsigned long *)iommu_pgd_offset((unsigned long)pgd, iova);

	if (iommu_is_hugepage)
		return pgd_entry;

	if (!IOMMU_PTE_PRESENT(*pgd_entry)) {
		pr_err("LA-IOMMU: pmd is not present\n");
		return NULL;
	}

	pmd_entry = iommu_pmd_offset((unsigned long *)pgd_entry, iova);
	if (!IOMMU_PTE_PRESENT(*pmd_entry)) {
		pr_err("LA-IOMMU: pte is not present\n");
		return NULL;
	}

	return pmd_entry;
}

static int iommu_map_page(struct la_iommu_priv *priv, unsigned long iova,
			phys_addr_t pa, size_t size, int prot, gfp_t gfp)
{

	unsigned long *pte, *shd_pte;
	int ret = 0, index;
	shadow_pg_entry *entry;
	unsigned long page_size, page_mask;

	/* 0x10000000~0x8fffffff */
	if ((iova >= SZ_256M) && (iova < LOONGSON_HIGHMEM_START)) {
		iova -= SZ_256M;
		pte = (unsigned long *)(priv->virtio_pgtable);
		while (size > 0) {
			pte[iova >> LA_VIRTIO_PAGE_SHIFT] =
					pa & LA_VIRTIO_PAGE_MASK;
			size -= 0x4000;
			iova += 0x4000;
			pa += 0x4000;
		}
		return 0;
	}

	index = ((unsigned long)priv->pgd - iommu_pgt_base) / IOMMU_PAGE_SIZE;

	spin_lock(&la_iommu_pgtlock);

	entry = index_to_pg_entry(index);
	if (!entry) {
		pr_err("LA-IOMMU: index:%d, iova:0x%lx, size:0x%lx\n",
							index, iova, size);
		ret = -EFAULT;
		goto out;
	}

	while (size > 0) {

		/* page_size/mask is set to huge page,
		 * only when iova and size are both aligned to huge page ,
		 */

		if ((iova | size) & (IOMMU_PMD_SIZE - 1)) {
			page_size = IOMMU_PAGE_SIZE;
			page_mask = IOMMU_PAGE_MASK;
			priv->is_hugepage = 0;
		} else {
			page_size = IOMMU_PMD_SIZE;
			page_mask = IOMMU_PMD_MASK;
			priv->is_hugepage = 1;
		}

		/* Fetch gmem pte */
		pte = la_iommu_pte_for_iova(priv, iova);

		/* Fetch shadow pte */
		shd_pte = iommu_fetch_pte((iommu_pte *)entry->va, iova, priv->is_hugepage);

		if (!shd_pte) {
			pr_err("LA-IOMMU: fetch shadow pte for iova 0x%lx err\n",
							iova);
			ret = -EFAULT;
			goto out;
		}

		/* Fill gmem pte */
		*pte = pa & page_mask;
		*pte |= IOMMU_PTE_PR | IOMMU_PTE_IR | IOMMU_PTE_IW | (priv->is_hugepage << 1);

		/* Fill shadow pte */
		*shd_pte = pa & page_mask;
		*shd_pte |= IOMMU_PTE_PR | IOMMU_PTE_IR | IOMMU_PTE_IW | (priv->is_hugepage << 1);

		size -= page_size;
		iova += page_size;
		pa += page_size;
	}

	if (check_has_priv())
		iommu_flush_iotlb_all(priv);

	spin_unlock(&la_iommu_pgtlock);

	return ret;

out:
	if (check_has_priv())
		iommu_flush_iotlb_all(priv);

	spin_unlock(&la_iommu_pgtlock);

	return ret;

}

static size_t iommu_unmap_page(struct la_iommu_priv *priv,
				unsigned long iova, size_t size)
{
	unsigned long *pte, *shd_pte, *shd_pmd_entry, *pte_base;
	unsigned long pa, page_size, page_mask;
	iommu_pte *va;
	int index;
	size_t unmap_len = 0;
	shadow_pg_entry *entry, *entry1;

	/* 0x10000000~0x8fffffff */
	if ((iova >= SZ_256M) && (iova < LOONGSON_HIGHMEM_START)) {
		iova -= SZ_256M;
		pte = (unsigned long *)priv->virtio_pgtable;
		while (size > 0) {
			pte[iova >> LA_VIRTIO_PAGE_SHIFT] = 0;
			size -= 0x4000;
			unmap_len += 0x4000;
			iova += 0x4000;
		}
		unmap_len += size;
		return unmap_len;
	}

	index = ((unsigned long)priv->pgd - iommu_pgt_base) / IOMMU_PAGE_SIZE;

	spin_lock(&la_iommu_pgtlock);

	entry = index_to_pg_entry(index);
	if (!entry) {
		pr_err("LA-IOMMU: index:%d, iova:0x%lx,size:0x%lx\n",
						index, iova, size);
		spin_unlock(&la_iommu_pgtlock);
		return unmap_len;
	}

	while (unmap_len < size) {

		if ((iova | size) & (IOMMU_PMD_SIZE - 1)) {
			page_size = IOMMU_PAGE_SIZE;
			page_mask = IOMMU_PAGE_MASK;
			priv->is_hugepage = 0;
		} else {
			page_size = IOMMU_PMD_SIZE;
			page_mask = IOMMU_PMD_MASK;
			priv->is_hugepage = 1;
		}

		shd_pte = iommu_fetch_pte((iommu_pte *)entry->va, iova, priv->is_hugepage);

		if (shd_pte && IOMMU_PTE_PRESENT(*shd_pte)) {
			/* clear shd_pte*/
			va = (iommu_pte *)entry->va;
			shd_pmd_entry = iommu_fetch_pmd_entry(va, iova, priv->is_hugepage);
			/* *shd_pmd_entry is pte base,
			 * so entry1->index is the index of pte_base in gmem
			 */
			pa = (unsigned long)(*shd_pmd_entry & IOMMU_PAGE_MASK);
			entry1 = pa_to_pg_entry(pa);
			if (!entry1) {
				spin_unlock(&la_iommu_pgtlock);
				return unmap_len;
			}
			/* clear gmem pte*/
			index = entry1->index;
			pte_base = (unsigned long *)(iommu_pgt_base + index * IOMMU_PAGE_SIZE);
			pte = pte_base + iommu_pte_index(iova);
			*shd_pte = 0ULL;
			*pte = 0ULL;
		}

		unmap_len += page_size;
		iova += page_size;

	}

	if (check_has_priv())
		iommu_flush_iotlb_all(priv);

	spin_unlock(&la_iommu_pgtlock);

	return unmap_len;

}

static int la_iommu_map(struct iommu_domain *domain, unsigned long iova,
			 phys_addr_t pa, size_t len, int prot)
{
	struct la_iommu_priv *priv = to_la_iommu_priv(domain);
	int ret;

	ret = iommu_map_page(priv, iova, pa, len, prot, GFP_KERNEL);

	return ret;
}

static size_t la_iommu_unmap(struct iommu_domain *domain, unsigned long iova,
				size_t len)
{
	struct la_iommu_priv *priv = to_la_iommu_priv(domain);
	int ret;

	ret = iommu_unmap_page(priv, iova, len);

	return ret;
}

static phys_addr_t la_iommu_iova_to_pa(struct iommu_domain *domain,
						dma_addr_t iova)
{
	struct la_iommu_priv *priv = to_la_iommu_priv(domain);
	unsigned long *shd_pte, *pte;
	unsigned long pa, offset, tmpva, page_size, page_mask;
	int ret = 0, index;
	shadow_pg_entry *entry;

	/* 0x10000000~0x8fffffff */
	if ((iova >= SZ_256M) && (iova < LOONGSON_HIGHMEM_START)) {
		tmpva = iova & LA_VIRTIO_PAGE_MASK;
		pte = (unsigned long *)priv->virtio_pgtable;
		offset = iova & ((1ULL << LA_VIRTIO_PAGE_SHIFT) - 1);
		pa = pte[(tmpva - SZ_256M) >> 14] + offset;
		return pa;
	}

	index = ((unsigned long)priv->pgd - iommu_pgt_base) / IOMMU_PAGE_SIZE;

	spin_lock(&la_iommu_pgtlock);

	entry = index_to_pg_entry(index);
	if (!entry) {
		pr_err("LA-IOMMU: index:%d, iova:0x%llx\n", index, iova);
		ret = -EFAULT;
		goto out;
	}

	shd_pte = iommu_fetch_pte((iommu_pte *)entry->va, iova, 0);

	if (!shd_pte || !IOMMU_PTE_PRESENT(*shd_pte)) {
		ret = -EFAULT;
		pr_err("LA-IOMMU: shadow pte is null or not present\n");
		goto out;
	}

	if (IOMMU_PTE_HUGEPAGE(*shd_pte)) {
		page_size = IOMMU_PMD_SIZE;
		page_mask = IOMMU_PMD_MASK;
	} else {
		page_size = IOMMU_PAGE_SIZE;
		page_mask = IOMMU_PAGE_MASK;
	}

	pa = *shd_pte & page_mask;
	pa |= (iova & (page_size - 1));

	if (check_has_priv())
		iommu_flush_iotlb_all(priv);

	spin_unlock(&la_iommu_pgtlock);
	return (phys_addr_t)pa;
out:
	spin_unlock(&la_iommu_pgtlock);
	return ret;
}

static phys_addr_t la_iommu_iova_to_phys(struct iommu_domain *domain,
					dma_addr_t iova)
{
	phys_addr_t pa;

	pa = la_iommu_iova_to_pa(domain, iova);

	return pa;
}

static void la_iommu_flush_iotlb_all(struct iommu_domain *domain)
{
	la_iommu_priv *priv = to_la_iommu_priv(domain);
	int ret;

	spin_lock(&la_iommu_pgtlock);
	ret = iommu_flush_iotlb_all(priv);
	spin_unlock(&la_iommu_pgtlock);
}

static struct iommu_ops la_iommu_ops = {
	.capable = la_iommu_capable,
	.domain_alloc = la_iommu_domain_alloc,
	.domain_free = la_iommu_domain_free,
	.attach_dev = la_iommu_attach_dev,
	.detach_dev = la_iommu_detach_dev,
	.map = la_iommu_map,
	.unmap = la_iommu_unmap,
	.iova_to_phys = la_iommu_iova_to_phys,
	.add_device = la_iommu_add_device,
	.remove_device = la_iommu_remove_device,
	.device_group = la_iommu_device_group,
	.pgsize_bitmap = LA_IOMMU_PGSIZE,
	.flush_iotlb_all = la_iommu_flush_iotlb_all,
	.iotlb_sync = la_iommu_flush_iotlb_all,

};

static inline int la_iommu_init_api(void)
{
	int ret;

	ret = bus_set_iommu(&pci_bus_type, &la_iommu_ops);
	return ret;
}

static int loongarch_iommu_probe(struct pci_dev *pdev,
				const struct pci_device_id *ent)
{
	int ret = 1;
	int bitmap_sz = 0;
	int tmp;

	iommu_mem_base = (unsigned long)pci_resource_start(pdev, 0);
	iommu_mem_base += UNCAC_BASE;
	iommu_pgt_base = (unsigned long)pci_resource_start(pdev, 2);
	iommu_pgt_base += UNCAC_BASE;

	pci_info(pdev, "iommu memory address:0x%lx,page table address:0x%lx\n",
				 iommu_mem_base, iommu_pgt_base);

	tmp = MAX_DOMAIN_ID / 8;
	bitmap_sz = (MAX_DOMAIN_ID % 8) ? (tmp + 1) : tmp;
	la_iommu_domain_alloc_bitmap = bitmap_zalloc(bitmap_sz, GFP_KERNEL);
	if (la_iommu_domain_alloc_bitmap == NULL) {
		pr_err("LA-IOMMU: domain bitmap alloc err bitmap_sz:%d\n",
								bitmap_sz);
		goto out_err;
	}

	tmp = MAX_ATTACHED_DEV_ID / 8;
	bitmap_sz = (MAX_ATTACHED_DEV_ID % 8) ? (tmp + 1) : tmp;
	la_iommu_devtable_bitmap = bitmap_zalloc(bitmap_sz, GFP_KERNEL);
	if (la_iommu_devtable_bitmap == NULL) {
		pr_err("LA-IOMMU: devtable bitmap alloc err bitmap_sz:%d\n",
								bitmap_sz);
		goto out_err_1;
	}

	tmp = MAX_PAGES_NUM / 8;
	bitmap_sz = (MAX_PAGES_NUM % 8) ? (tmp + 1) : tmp;
	la_iommu_pgtable_alloc_bitmap = bitmap_zalloc(bitmap_sz, GFP_KERNEL);
	if (la_iommu_pgtable_alloc_bitmap == NULL) {
		pr_err("LA-IOMMU: pgtable bitmap alloc err bitmap_sz:%d\n",
								bitmap_sz);
		goto out_err_2;
	}

	la_iommu_init_api();

	return 0;

out_err_2:
	kfree(la_iommu_devtable_bitmap);
out_err_1:
	kfree(la_iommu_domain_alloc_bitmap);
out_err:

	return ret;
}

static void loongarch_iommu_remove(struct pci_dev *pdev)
{
	kfree(la_iommu_domain_alloc_bitmap);
	la_iommu_domain_alloc_bitmap = NULL;

	kfree(la_iommu_devtable_bitmap);
	la_iommu_devtable_bitmap = NULL;

	kfree(la_iommu_pgtable_alloc_bitmap);
	la_iommu_pgtable_alloc_bitmap = NULL;
}

static int __init la_iommu_setup(char *str)
{
	if (!str)
		return -EINVAL;
	while (*str) {
		if (!strncmp(str, "on", 2)) {
			loongarch_iommu_disable = 0;
			pr_info("IOMMU enabled\n");
		} else if (!strncmp(str, "off", 3)) {
			loongarch_iommu_disable = 1;
			pr_info("IOMMU disabled\n");
		}
		str += strcspn(str, ",");
		while (*str == ',')
			str++;
	}
	return 0;
}
__setup("loongarch_iommu=", la_iommu_setup);

static const struct pci_device_id loongson_iommu_pci_tbl[] = {
	{ PCI_DEVICE(0x14, 0x7a1f) },
	{ 0, }
};

static struct pci_driver loongarch_iommu_driver = {
	.name = "loongarch-iommu",
	.id_table = loongson_iommu_pci_tbl,
	.probe	= loongarch_iommu_probe,
	.remove	= loongarch_iommu_remove,
};

static int __init loongarch_iommu_driver_init(void)
{
	int ret = 0;

	if (loongarch_iommu_disable == 0) {
		ret = pci_register_driver(&loongarch_iommu_driver);
		if (ret != 0) {
			pr_err("Failed to register IOMMU driver\n");
			return ret;
		}
	}

	return ret;
}

static void __exit loongarch_iommu_driver_exit(void)
{
	if (loongarch_iommu_disable == 0)
		pci_unregister_driver(&loongarch_iommu_driver);
}

module_init(loongarch_iommu_driver_init);
module_exit(loongarch_iommu_driver_exit);
