// SPDX-License-Identifier: GPL-2.0
//
// loongson_i2s_pci.c -- Loongson I2S controller driver
//
// Copyright (C) 2023 Loongson Technology Corporation Limited
// Author: Yingkun Meng <mengyingkun@loongson.cn>
//

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/pm_runtime.h>
#include <linux/dma-mapping.h>
#include <linux/acpi.h>
#include <linux/pci.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include "loongson_i2s.h"
#include "loongson_pcm.h"

static int loongson_i2s_pci_probe(struct pci_dev *pdev,
				  const struct pci_device_id *pid)
{
	struct fwnode_handle *fwnode = pdev->dev.fwnode;
	struct loongson_dma_data *tx_param, *rx_param;
	struct loongson_i2s *i2s;
	int ret;

	/* There is no acpi companion for I2S controller device in old firmware,
	 * so we use it to check whether we are using old firmware.
	 * The new version of I2S driver isn't compatible with the old firmware.
	 */
	if (!has_acpi_companion(&pdev->dev)) {
		dev_warn(&pdev->dev, "upgrade firmware to use new I2S driver");
		return -ENODEV;
	}

	if (pcim_enable_device(pdev)) {
		dev_err(&pdev->dev, "pcim_enable_device failed\n");
		return -ENODEV;
	}

	i2s = devm_kzalloc(&pdev->dev, sizeof(*i2s), GFP_KERNEL);
	if (!i2s)
		return -ENOMEM;

	i2s->rev_id = pdev->revision;
	i2s->dev = &pdev->dev;
	pci_set_drvdata(pdev, i2s);

	ret = pcim_iomap_regions(pdev, 1 << 0, dev_name(&pdev->dev));
	if (ret < 0)  {
		dev_err(&pdev->dev, "iomap_regions failed");
		return ret;
	}
	i2s->reg_base = pcim_iomap_table(pdev)[0];

	tx_param = &i2s->tx_dma_data;
	rx_param = &i2s->rx_dma_data;

	tx_param->dev_addr = pci_resource_start(pdev, 0) + LS_IIS_TX_DATA;
	rx_param->dev_addr = pci_resource_start(pdev, 0) + LS_IIS_RX_DATA;

	tx_param->order_addr = i2s->reg_base + LS_IIS_TX_ORDER;
	rx_param->order_addr = i2s->reg_base + LS_IIS_RX_ORDER;

	tx_param->irq = fwnode_irq_get_byname(fwnode, "tx");
	if (tx_param->irq < 0) {
		dev_err(&pdev->dev, "dma tx irq invalid\n");
		return tx_param->irq;
	}

	rx_param->irq = fwnode_irq_get_byname(fwnode, "rx");
	if (rx_param->irq < 0) {
		dev_err(&pdev->dev, "dma rx irq invalid\n");
		return rx_param->irq;
	}

	device_property_read_u32(&pdev->dev, "clock-frequency", &i2s->clk_rate);
	if (!i2s->clk_rate) {
		dev_err(&pdev->dev, "clock-frequency property invalid\n");
		return ret;
	}

	dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));

	loongson_i2s_init(i2s);

	ret = devm_snd_soc_register_component(&pdev->dev,
					      &loongson_i2s_component_driver,
					      &loongson_i2s_dai, 1);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to register DAI: %d\n", ret);
		return ret;
	}

	return 0;
}

static const struct pci_device_id loongson_i2s_ids[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_LOONGSON, 0x7a27) },
	{}
};
MODULE_DEVICE_TABLE(pci, loongson_i2s_ids);

static struct pci_driver loongson_i2s_driver = {
	.name = "loongson-i2s-pci",
	.id_table = loongson_i2s_ids,
	.probe = loongson_i2s_pci_probe,
	.driver = {
		.owner = THIS_MODULE,
		.pm = &loongson_i2s_pm,
	},
};
module_pci_driver(loongson_i2s_driver);

MODULE_DESCRIPTION("Loongson I2S Master Mode ASoC Driver");
MODULE_AUTHOR("Loongson Technology Corporation Limited");
MODULE_LICENSE("GPL");
