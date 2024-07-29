// SPDX-License-Identifier: GPL-2.0
//
// Loongson ALSA SoC Platform (DMA) driver
//
// Copyright (C) 2023 Loongson Technology Corporation Limited
// Author: Yingkun Meng <mengyingkun@loongson.cn>
//

#include <linux/module.h>
#include <linux/io-64-nonatomic-lo-hi.h>
#include <linux/delay.h>
#include <linux/pm_runtime.h>
#include <linux/dma-mapping.h>
#include <sound/soc.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>

#include "loongson_i2s.h"
#include "loongson_pcm.h"

#define LS_DMABUF_SIZE (1024 * 1024)

static const struct snd_pcm_hardware loongson_pcm_hardware = {
	.info = SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_MMAP_VALID |
		SNDRV_PCM_INFO_RESUME |
		SNDRV_PCM_INFO_PAUSE,
	.formats = (SNDRV_PCM_FMTBIT_S8 |
		SNDRV_PCM_FMTBIT_S16_LE |
		SNDRV_PCM_FMTBIT_S20_3LE |
		SNDRV_PCM_FMTBIT_S24_LE),
	.period_bytes_min = 128,
	.period_bytes_max = 128 * 1024,
	.periods_min = 1,
	.periods_max = PAGE_SIZE / sizeof(struct loongson_dma_desc),
	.buffer_bytes_max = LS_DMABUF_SIZE,
};

static struct
loongson_dma_desc *dma_desc_save(struct loongson_runtime_data *prtd)
{
	void __iomem *order_reg = prtd->dma_data->order_addr;
	u64 val;

	val = (u64)prtd->dma_pos_desc_phy & DMA_ORDER_ASK_MASK;
	val |= (readq(order_reg) & DMA_ORDER_CTRL_MASK);
	val |= DMA_ORDER_ASK_VALID;
	writeq(val, order_reg);

	while (readl(order_reg) & DMA_ORDER_ASK_VALID)
		udelay(2);

	return prtd->dma_pos_desc;
}

static int loongson_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct loongson_runtime_data *prtd = substream->runtime->private_data;
	struct device *dev = substream->pcm->card->dev;
	void __iomem *order_reg = prtd->dma_data->order_addr;
	u64 val;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		val = prtd->dma_pos_desc_phy & DMA_ORDER_ASK_MASK;
		if (dev->coherent_dma_mask == DMA_BIT_MASK(64))
			val |= DMA_ORDER_ADDR_64;
		else
			val &= ~DMA_ORDER_ADDR_64;
		val |= (readq(order_reg) & DMA_ORDER_CTRL_MASK);
		val |= DMA_ORDER_START;

		writeq(val, order_reg);
		while ((readl(order_reg) & DMA_ORDER_START))
			udelay(1);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		dma_desc_save(prtd);

		/* dma stop */
		val = readq(order_reg);
		val |= DMA_ORDER_STOP;
		writeq(val, order_reg);
		udelay(1000);

		break;
	default:
		dev_err(dev, "Invalid pcm trigger operation\n");
		return -EINVAL;
	}

	return 0;
}

static int loongson_pcm_hw_params(struct snd_pcm_substream *substream,
				  struct snd_pcm_hw_params *params)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct device *dev = substream->pcm->card->dev;
	struct loongson_runtime_data *prtd = runtime->private_data;
	size_t buf_len = params_buffer_bytes(params);
	size_t period_len = params_period_bytes(params);
	dma_addr_t order_addr, mem_addr;
	struct loongson_dma_desc *desc;
	u32 num_periods;
	int i;

	if (buf_len % period_len) {
		pr_err("buf len not multiply of period len\n");
		return -EINVAL;
	}

	num_periods = buf_len / period_len;
	if (!num_periods || num_periods > prtd->dma_desc_arr_size) {
		dev_err(dev, "dma data too small or too big\n");
		return -EINVAL;
	}

	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);
	runtime->dma_bytes = buf_len;

	/* initialize dma descriptor array */
	order_addr = prtd->dma_desc_arr_phy;
	mem_addr = runtime->dma_addr;
	for (i = 0; i < num_periods; i++) {
		desc = &prtd->dma_desc_arr[i];

		/* next descriptor physical address */
		order_addr += sizeof(*desc);
		desc->order = lower_32_bits(order_addr | BIT(0));
		desc->order_hi = upper_32_bits(order_addr);

		desc->saddr = lower_32_bits(mem_addr);
		desc->saddr_hi = upper_32_bits(mem_addr);
		desc->daddr = prtd->dma_data->dev_addr;

		desc->cmd = BIT(0);
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			desc->cmd |= BIT(12);

		desc->length = period_len >> 2;
		desc->step_length = 0;
		desc->step_times = 1;

		mem_addr += period_len;
	}
	desc = &prtd->dma_desc_arr[num_periods - 1];
	desc->order = lower_32_bits(prtd->dma_desc_arr_phy | BIT(0));
	desc->order_hi = upper_32_bits(prtd->dma_desc_arr_phy);

	/* init position descriptor */
	*prtd->dma_pos_desc = *prtd->dma_desc_arr;

	return 0;
}

static snd_pcm_uframes_t
loongson_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct loongson_runtime_data *prtd = runtime->private_data;
	struct loongson_dma_desc *desc;
	snd_pcm_uframes_t x;
	u64 addr;

	desc = dma_desc_save(prtd);
	addr = ((u64)desc->saddr_hi << 32) | desc->saddr;

	x = bytes_to_frames(runtime, addr - runtime->dma_addr);
	if (x == runtime->buffer_size)
		x = 0;

	return x;
}

static irqreturn_t loongson_pcm_dma_irq(int irq, void *devid)
{
	struct snd_pcm_substream *substream = devid;

	snd_pcm_period_elapsed(substream);
	return IRQ_HANDLED;
}

static int loongson_pcm_init_prtd(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_card *card = substream->pcm->card;
	struct loongson_runtime_data *prtd;
	struct loongson_dma_data *dma_data;
	int ret;

	prtd = kzalloc(sizeof(*prtd), GFP_KERNEL);
	if (!prtd)
		return -ENOMEM;

	prtd->dma_desc_arr = dma_alloc_coherent(card->dev, PAGE_SIZE,
						&prtd->dma_desc_arr_phy,
						GFP_KERNEL);
	if (!prtd->dma_desc_arr) {
		ret = -ENOMEM;
		goto desc_err;
	}
	prtd->dma_desc_arr_size = PAGE_SIZE / sizeof(*prtd->dma_desc_arr);

	prtd->dma_pos_desc = dma_alloc_coherent(card->dev,
						sizeof(*prtd->dma_pos_desc),
						&prtd->dma_pos_desc_phy,
						GFP_KERNEL);
	if (!prtd->dma_pos_desc) {
		ret = -ENOMEM;
		goto pos_err;
	}

	dma_data = snd_soc_dai_get_dma_data(rtd->cpu_dai, substream);
	ret = request_irq(dma_data->irq, loongson_pcm_dma_irq,
			  IRQF_TRIGGER_RISING, LS_I2S_DRVNAME, substream);
	if (ret < 0)
		goto irq_err;

	prtd->dma_data = dma_data;
	substream->runtime->private_data = prtd;

	return 0;
irq_err:
	dma_free_coherent(card->dev, sizeof(*prtd->dma_pos_desc),
			  prtd->dma_pos_desc, prtd->dma_pos_desc_phy);
pos_err:
	dma_free_coherent(card->dev, PAGE_SIZE, prtd->dma_desc_arr,
			  prtd->dma_desc_arr_phy);
desc_err:
	kfree(prtd);

	return ret;
}

static int loongson_pcm_open(struct snd_pcm_substream *substream)
{
	snd_soc_set_runtime_hwparams(substream, &loongson_pcm_hardware);

	return loongson_pcm_init_prtd(substream);
}

static int loongson_pcm_close(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_card *card = substream->pcm->card;
	struct loongson_runtime_data *prtd = substream->runtime->private_data;
	struct loongson_dma_data *dma_data;

	dma_free_coherent(card->dev, PAGE_SIZE, prtd->dma_desc_arr,
			  prtd->dma_desc_arr_phy);

	dma_free_coherent(card->dev, sizeof(*prtd->dma_pos_desc),
			  prtd->dma_pos_desc, prtd->dma_pos_desc_phy);

	dma_data = snd_soc_dai_get_dma_data(rtd->cpu_dai, substream);
	free_irq(dma_data->irq, substream);

	kfree(prtd);
	return 0;
}

static int loongson_pcm_mmap(struct snd_pcm_substream *substream,
			     struct vm_area_struct *vma)
{
	return remap_pfn_range(vma, vma->vm_start,
			       substream->dma_buffer.addr >> PAGE_SHIFT,
			       vma->vm_end - vma->vm_start,
			       vma->vm_page_prot);
}

static int loongson_pcm_hw_free(struct snd_pcm_substream *substream)
{
	snd_pcm_set_runtime_buffer(substream, NULL);

	return 0;
}

static struct snd_pcm_ops loongson_pcm_ops = {
	.open		= loongson_pcm_open,
	.close		= loongson_pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_free	= loongson_pcm_hw_free,
	.trigger	= loongson_pcm_trigger,
	.hw_params	= loongson_pcm_hw_params,
	.pointer	= loongson_pcm_pointer,
	.mmap		= loongson_pcm_mmap,
};

static int loongson_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_pcm *pcm = rtd->pcm;
	struct snd_pcm_substream *substream;
	int ret;

	substream = pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream;
	if (substream) {
		ret = snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV,
					  pcm->card->dev,
					  LS_DMABUF_SIZE,
					  &substream->dma_buffer);
		if (ret) {
			dev_err(rtd->dev, "Cannot allocate dma buffer\n");
			goto err;
		}
	}

	substream = pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream;
	if (substream) {
		ret = snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV,
					  pcm->card->dev,
					  LS_DMABUF_SIZE,
					  &substream->dma_buffer);
		if (ret) {
			dev_err(rtd->dev, "Cannot allocate dma buffer\n");
			return ret;
		}
	}

	return 0;
err:
	substream = pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream;
	if (substream)
		snd_dma_free_pages(&substream->dma_buffer);

	return ret;
}

static void loongson_pcm_free(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	int i;

	for (i = SNDRV_PCM_STREAM_PLAYBACK; i <= SNDRV_PCM_STREAM_LAST; i++) {
		substream = pcm->streams[i].substream;
		if (!substream)
			continue;

		snd_dma_free_pages(&substream->dma_buffer);
		substream->dma_buffer.area = NULL;
		substream->dma_buffer.addr = 0;
	}
}

const struct snd_soc_component_driver loongson_i2s_component_driver = {
	.name		= LS_I2S_DRVNAME,
	.pcm_new	= loongson_pcm_new,
	.pcm_free	= loongson_pcm_free,
	.ops		= &loongson_pcm_ops,
};
