/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ALSA ASoC interface for the Loongson platform
 *
 * Copyright (C) 2023 Loongson Technology Corporation Limited
 * Author: Yingkun Meng <mengyingkun@loongson.cn>
 */

#ifndef _LOOGNSON_PCM_H
#define _LOONGSON_PCM_H

#include <sound/soc.h>
#include "loongson_i2s.h"

/* DMA dma_order Register */
#define DMA_ORDER_STOP          (1 << 4) /* DMA stop */
#define DMA_ORDER_START         (1 << 3) /* DMA start */
#define DMA_ORDER_ASK_VALID     (1 << 2) /* DMA ask valid flag */
#define DMA_ORDER_AXI_UNCO      (1 << 1) /* Uncache access */
#define DMA_ORDER_ADDR_64       (1 << 0) /* 64bits address support */

#define DMA_ORDER_ASK_MASK      (~0x1fUL) /* Ask addr mask */
#define DMA_ORDER_CTRL_MASK     (0x0fUL)  /* Control mask  */

/*
 * @ordered - DMA order addr low
 */
struct loongson_dma_desc {
	u32 order;		/* Next descriptor address register */
	u32 saddr;		/* Source address register */
	u32 daddr;		/* Device address register */
	u32 length;		/* Total length register */
	u32 step_length;	/* Memory stride register */
	u32 step_times;		/* Repeat time register */
	u32 cmd;		/* Command register */
	u32 stats;		/* Status register */
	u32 order_hi;		/* Next descriptor high address register */
	u32 saddr_hi;		/* High source address register */
	u32 res[6];		/* Reserved */
} __packed;

struct loongson_runtime_data {
	struct loongson_dma_data *dma_data;

	struct loongson_dma_desc *dma_desc_arr;
	dma_addr_t dma_desc_arr_phy;
	int dma_desc_arr_size;

	struct loongson_dma_desc *dma_pos_desc;
	dma_addr_t dma_pos_desc_phy;
};

extern const struct snd_soc_component_driver loongson_i2s_component_driver;

#endif
