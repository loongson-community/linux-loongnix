/*
 * Loongson Platforms GUTS Driver
 *
 * Copyright (C) 2016 Loongson Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/io.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/of_fdt.h>
#include <linux/sys_soc.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/loongson/guts.h>

struct guts {
	struct scfg_guts __iomem *regs;
	bool little_endian;
};

struct ls_soc_die_attr {
	char	*die;
	u32	svr;
	u32	mask;
};

static struct guts *guts;
static struct soc_device_attribute soc_dev_attr;
static struct soc_device *soc_dev;


/* SoC die attribute definition for loongson platform */
static const struct ls_soc_die_attr ls_soc_die[] = {

	/*
	 * MIPS/LA-based SoCs LS Series
	 */

	/* Die: 2k1000la, SoC: 2k1000la */
	{ .die		= "2K1000LA",
	  .svr		= 0x00000013,
	  .mask		= 0x000000ff,
	},
	{ },
};

static const struct ls_soc_die_attr *ls_soc_die_match(
	u32 svr, const struct ls_soc_die_attr *matches)
{
	while (matches->svr) {
		if (matches->svr == (svr & matches->mask))
			return matches;
		matches++;
	};
	return NULL;
}

u32 ls_guts_get_svr(void)
{
	u32 svr = 0;

	if (!guts || !guts->regs)
		return svr;

	if (guts->little_endian)
		svr = ioread32(&guts->regs->svr);
	else
		svr = ioread32be(&guts->regs->svr);

	return svr;
}
EXPORT_SYMBOL(ls_guts_get_svr);

static int ls_guts_probe(struct platform_device *pdev)
{
	struct device_node *root, *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct resource *res;
	const struct ls_soc_die_attr *soc_die;
	const char *machine;
	u32 svr;

	/* Initialize guts */
	guts = devm_kzalloc(dev, sizeof(*guts), GFP_KERNEL);
	if (!guts)
		return -ENOMEM;

	guts->little_endian = of_property_read_bool(np, "little-endian");

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	guts->regs = ioremap(res->start, res->end - res->start + 1);
	if (IS_ERR(guts->regs))
		return PTR_ERR(guts->regs);

	/* Register soc device */
	root = of_find_node_by_path("/");
	if (of_property_read_string(root, "model", &machine))
		of_property_read_string_index(root, "compatible", 0, &machine);
	of_node_put(root);
	if (machine)
		soc_dev_attr.machine = devm_kstrdup(dev, machine, GFP_KERNEL);

	svr = ls_guts_get_svr();
	soc_die = ls_soc_die_match(svr, ls_soc_die);
	if (soc_die) {
		soc_dev_attr.family = devm_kasprintf(dev, GFP_KERNEL,
						     "Loongson %s", soc_die->die);
	} else {
		soc_dev_attr.family = devm_kasprintf(dev, GFP_KERNEL, "Loongson");
	}
	if (!soc_dev_attr.family)
		return -ENOMEM;
	soc_dev_attr.soc_id = devm_kasprintf(dev, GFP_KERNEL,
					     "svr:0x%08x", svr);
	if (!soc_dev_attr.soc_id)
		return -ENOMEM;
	soc_dev_attr.revision = devm_kasprintf(dev, GFP_KERNEL, "%d.%d",
					       (svr >>  4) & 0xf, svr & 0xf);
	if (!soc_dev_attr.revision)
		return -ENOMEM;

	soc_dev = soc_device_register(&soc_dev_attr);
	if (IS_ERR(soc_dev))
		return PTR_ERR(soc_dev);

	pr_info("Machine: %s\n", soc_dev_attr.machine);
	pr_info("SoC family: %s\n", soc_dev_attr.family);
	pr_info("SoC ID: %s, Revision: %s\n",
		soc_dev_attr.soc_id, soc_dev_attr.revision);
	return 0;
}

static int ls_guts_remove(struct platform_device *dev)
{
	soc_device_unregister(soc_dev);
	return 0;
}

/*
 * Table for matching compatible strings, for device tree
 * guts node, for Loongson loongson SOCs.
 */
static const struct of_device_id ls_guts_of_match[] = {
	{ .compatible = "loongson,2k1000la-scfg", },
	{}
};
MODULE_DEVICE_TABLE(of, ls_guts_of_match);

static struct platform_driver ls_guts_driver = {
	.driver = {
		.name = "ls-guts",
		.of_match_table = ls_guts_of_match,
	},
	.probe = ls_guts_probe,
	.remove = ls_guts_remove,
};

static int __init ls_guts_init(void)
{
	return platform_driver_register(&ls_guts_driver);
}
core_initcall(ls_guts_init);

static void __exit ls_guts_exit(void)
{
	platform_driver_unregister(&ls_guts_driver);
}
module_exit(ls_guts_exit);
