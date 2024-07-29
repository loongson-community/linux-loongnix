/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ALSA I2S interface for the Loongson chip
 *
 * Copyright (C) 2023 Loongson Technology Corporation Limited
 * Author: Yingkun Meng <mengyingkun@loongson.cn>
 */
#ifndef _LOONGSON_I2S_H
#define _LOONGSON_I2S_H

#include <sound/dmaengine_pcm.h>

/* I2S Common Registers */
#define LS_IIS_VER	0x00 /* IIS Version */
#define LS_IIS_CFG	0x04 /* IIS Config */
#define LS_IIS_CTRL	0x08 /* IIS Control */
#define LS_IIS_RX_DATA	0x0C /* IIS DMA RX Address */
#define LS_IIS_TX_DATA	0x10 /* IIS DMA TX Address */

/* 2K2000 I2S Specify Registers */
#define LS_IIS_CFG1	0x14 /* IIS Config1 */

/* 7A2000 I2S Specify Registers */
#define LS_IIS_TX_ORDER 0x100 /* TX DMA Order */
#define LS_IIS_RX_ORDER 0x110 /* RX DMA Order */

/* Loongson I2S IISControl Register */
#define IIS_CTRL_MCLK_READY	(1 << 16) /* MCLK ready */
#define IIS_CTRL_MASTER		(1 << 15) /* Master mode */
#define IIS_CTRL_MSB		(1 << 14) /* MSB bit order */
#define IIS_CTRL_RX_EN		(1 << 13) /* RX enable */
#define IIS_CTRL_TX_EN		(1 << 12) /* TX enable */
#define IIS_CTRL_RX_DMA_EN	(1 << 11) /* DMA RX enable */
#define IIS_CTRL_CLK_READY	(1 << 8)  /* BCLK ready */
#define IIS_CTRL_TX_DMA_EN	(1 << 7)  /* DMA TX enable */
#define IIS_CTRL_RESET		(1 << 4)  /* Controller soft reset */
#define IIS_CTRL_MCLK_EN	(1 << 3)  /* Enable MCLK */
#define IIS_CTRL_RX_INT_EN	(1 << 1)  /* RX interrupt enable */
#define IIS_CTRL_TX_INT_EN	(1 << 0)  /* TX interrupt enable */

#define LS_I2S_DRVNAME		"loongson-i2s"

struct loongson_dma_data {
	u64 dev_addr;			/* device physical address for DMA */
	void __iomem *order_addr;	/* DMA order register */
	int irq;			/* DMA irq */
};

struct loongson_i2s {
	struct device *dev;
	union {
		struct snd_dmaengine_dai_dma_data playback_dma_data;
		struct loongson_dma_data tx_dma_data;
	};
	union {
		struct snd_dmaengine_dai_dma_data capture_dma_data;
		struct loongson_dma_data rx_dma_data;
	};
	void __iomem *reg_base;
	u32 rev_id;
	u32 clk_rate;
	u32 sysclk;
	u32 cfg_reg;
	u32 ctrl_reg;
};

extern const struct dev_pm_ops loongson_i2s_pm;
extern struct snd_soc_dai_driver loongson_i2s_dai;

void loongson_i2s_init(struct loongson_i2s *i2s);

#endif
