/*
 *  linux/drivers/video/vfb.c -- Virtual frame buffer device
 *
 *      Copyright (C) 2002 James Simmons
 *
 *	Copyright (C) 1997 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

#include <asm/uaccess.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/platform_data/simplefb.h>

static char mode_option[32] = "1280x1024-32@2M";
module_param_string(mode, mode_option, sizeof(mode_option), 0444);

static int vfb_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	struct simplefb_platform_data mode;
	struct resource res;
	struct platform_device *pd;
	long phybase,  videooffset, videomemorysize;
	char *pmode = mode_option;
	int depth;
	char *penv;
	if (!dev->bus->number)
		return -ENODEV;
	pci_enable_device(dev);

	/* pcimem bar last 16M free, 2MB offset from free for framebuffer */
	phybase = pci_resource_start(dev, 0);
	phybase += pci_resource_len(dev, 0) - 0x1000000;
	penv = ioremap(phybase, 0x1000);
	/*env at last 16M's beginning, first env is video*/
	if (!strncmp(penv, "video=", 6)) {
		pmode = penv + 6;
	}

	mode.width = simple_strtoul(pmode, &pmode, 0);
	pmode++;
	mode.height = simple_strtoul(pmode, &pmode, 0);
	pmode++;
	depth = simple_strtoul(pmode, &pmode, 0);
	if (pmode && pmode[0]) {
		pmode++;
		videooffset = simple_strtoul(pmode, &pmode, 0);
		if (pmode && pmode[0]) {
			switch (pmode[0]) {
			case 'M':
			case 'm':
				videooffset *= 0x100000;
				break;
			case 'K':
			case 'k':
				videooffset *= 1024;
				break;
			}
		}
	} else
		videooffset = 0x200000;
	phybase += videooffset;
	mode.stride = mode.width * depth/8;
	mode.format = depth == 32 ? "a8r8g8b8" : "r5g6b5";

	videomemorysize = mode.height * mode.width * depth/8;

	res.start = phybase;
	res.end = phybase + videomemorysize - 1;
	res.flags = IORESOURCE_MEM;
	res.parent = &dev->resource[0];

	pd = platform_device_register_resndata(NULL, "simple-framebuffer", 0,
					       &res, 1, &mode, sizeof(mode));
	return PTR_ERR_OR_ZERO(pd);
}


static struct pci_device_id vfb_devices[] = {
	{PCI_DEVICE(0x14, 0x1a05)},
	{0, 0, 0, 0, 0, 0, 0}
};

static struct pci_driver vfb_driver = {
	.name		= "vfb",
	.id_table	= vfb_devices,
	.probe	= vfb_probe,
	.driver = {
		.name	= "vfb",
	},
};

static int __init vfb_init(void)
{
	int ret = 0;
	ret = pci_register_driver (&vfb_driver);
	return ret;
}

module_init(vfb_init);

#ifdef MODULE
static void __exit vfb_exit(void)
{
	pci_unregister_driver (&vfb_driver);
}

module_exit(vfb_exit);
#endif

MODULE_LICENSE("GPL");
