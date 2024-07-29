// SPDX-License-Identifier: GPL-2.0+
// PCI interface driver for Loongson SPI Support
// Copyright (C) 2023 Loongson Technology Corporation Limited

#include <linux/mod_devicetable.h>
#include <linux/pci.h>
#include <linux/module.h>

#include "spi-loongson.h"

static struct spi_master *ls7a_spi_master;

struct spi_master *get_ls7a_spi_master(void)
{
	return ls7a_spi_master;
}
EXPORT_SYMBOL(get_ls7a_spi_master);

static int loongson_spi_pci_register(struct pci_dev *pdev,
			const struct pci_device_id *ent)
{
	int ret;
	void __iomem *reg_base;
	struct device *dev = &pdev->dev;
	int pci_bar = 0;

	ret = pcim_enable_device(pdev);
	if (ret < 0) {
		dev_err(dev, "cannot enable pci device\n");
		return ret;
	}

	ret = pcim_iomap_regions(pdev, BIT(pci_bar), pci_name(pdev));
	if (ret) {
		dev_err(dev, "failed to request and remap memory\n");
		return ret;
	}

	reg_base = pcim_iomap_table(pdev)[pci_bar];

	ret = loongson_spi_init_controller(dev, reg_base);
	if (ret) {
		dev_err(dev, "failed to initialize controller\n");
		return ret;
	}
	ls7a_spi_master = dev_get_drvdata(dev);

	return 0;
}

static struct pci_device_id loongson_spi_devices[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_LOONGSON, 0x7a0b) },
	{ PCI_DEVICE(PCI_VENDOR_ID_LOONGSON, 0x7a1b) },
	{ }
};
MODULE_DEVICE_TABLE(pci, loongson_spi_devices);

static struct pci_driver loongson_spi_pci_driver = {
	.name       = "loongson-spi-pci",
	.id_table   = loongson_spi_devices,
	.probe      = loongson_spi_pci_register,
	.driver	= {
		.bus = &pci_bus_type,
		.pm = &loongson_spi_dev_pm_ops,
	},
};
module_pci_driver(loongson_spi_pci_driver);

MODULE_DESCRIPTION("Loongson spi pci driver");
MODULE_LICENSE("GPL");
