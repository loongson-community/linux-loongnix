// SPDX-License-Identifier: GPL-2.0
//
// Common functions for loongson I2S controller driver
//
// Copyright (C) 2023 Loongson Technology Corporation Limited.
// Author: Yingkun Meng <mengyingkun@loongson.cn>
//

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/pm_runtime.h>
#include <linux/dma-mapping.h>
#include <linux/acpi.h>
#include <linux/pci.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include "loongson_i2s.h"
#include "loongson_pcm.h"

#define LS_I2S_FORMATS (SNDRV_PCM_FMTBIT_S8 | \
			SNDRV_PCM_FMTBIT_S16_LE | \
			SNDRV_PCM_FMTBIT_S20_3LE | \
			SNDRV_PCM_FMTBIT_S24_LE)

#define DRV_NAME "loongson-i2s"

static inline void i2s_writel(struct loongson_i2s *i2s, u32 reg, u32 value)
{
	writel(value, i2s->reg_base + reg);
}

static inline u32 i2s_readl(struct loongson_i2s *i2s, u32 reg)
{
	return readl(i2s->reg_base + reg);
}

static int loongson_i2s_startup(struct snd_pcm_substream *substream,
				struct snd_soc_dai *dai)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	if (substream->pcm->device & 1) {
		runtime->hw.info &= ~SNDRV_PCM_INFO_INTERLEAVED;
		runtime->hw.info |= SNDRV_PCM_INFO_NONINTERLEAVED;
	}
	if (substream->pcm->device & 2)
		runtime->hw.info &= ~(SNDRV_PCM_INFO_MMAP |
				      SNDRV_PCM_INFO_MMAP_VALID);

	/*
	 * For mysterious reasons (and despite what the manual says)
	 * playback samples are lost if the DMA count is not a multiple
	 * of the DMA burst size.  Let's add a rule to enforce that.
	 */
	snd_pcm_hw_constraint_step(runtime, 0,
				   SNDRV_PCM_HW_PARAM_PERIOD_BYTES, 128);
	snd_pcm_hw_constraint_step(runtime, 0,
				   SNDRV_PCM_HW_PARAM_BUFFER_BYTES, 128);

	snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);

	return 0;
}

static int loongson_i2s_trigger(struct snd_pcm_substream *substream, int cmd,
				struct snd_soc_dai *dai)
{
	struct loongson_i2s *i2s = snd_soc_dai_get_drvdata(dai);
	u32 value;
	int ret = 0;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		value = i2s_readl(i2s, LS_IIS_CTRL);
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			value |= (IIS_CTRL_TX_EN | IIS_CTRL_TX_DMA_EN);
		else
			value |= (IIS_CTRL_RX_EN | IIS_CTRL_RX_DMA_EN);
		i2s_writel(i2s, LS_IIS_CTRL, value);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		value = i2s_readl(i2s, LS_IIS_CTRL);
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			value &= ~(IIS_CTRL_TX_EN | IIS_CTRL_TX_DMA_EN);
		else
			value &= ~(IIS_CTRL_RX_EN | IIS_CTRL_RX_DMA_EN);
		i2s_writel(i2s, LS_IIS_CTRL, value);
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

static int loongson_i2s_hw_params(struct snd_pcm_substream *substream,
				  struct snd_pcm_hw_params *params,
				  struct snd_soc_dai *dai)
{
	struct loongson_i2s *i2s = snd_soc_dai_get_drvdata(dai);
	u32 clk_rate = i2s->clk_rate;
	u32 sysclk = i2s->sysclk;
	u32 bits = params_width(params);
	u32 chans = params_channels(params);
	u32 fs = params_rate(params);
	u32 bclk_ratio, mclk_ratio;
	u32 mclk_ratio_frac;
	u32 value = 0;

	switch (i2s->rev_id) {
	case 0:
		bclk_ratio = DIV_ROUND_CLOSEST(clk_rate,
					       (bits * chans * fs * 2)) - 1;
		mclk_ratio = DIV_ROUND_CLOSEST(clk_rate, (sysclk * 2)) - 1;

		/* According to 2k1000LA user manual, set bits == depth */
		value |= (bits << 24);
		value |= (bits << 16);
		value |= (bclk_ratio << 8);
		value |= mclk_ratio;
		i2s_writel(i2s, LS_IIS_CFG, value);

		break;
	case 1:
		bclk_ratio = DIV_ROUND_CLOSEST(sysclk,
					       (bits * chans * fs * 2)) - 1;
		mclk_ratio = clk_rate / sysclk;
		mclk_ratio_frac = DIV_ROUND_CLOSEST(((u64)clk_rate << 16),
						sysclk) - (mclk_ratio << 16);

		value = (bits << 24) | (bclk_ratio << 8);
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			value |= (bits << 16);
			value |= (i2s_readl(i2s, LS_IIS_CFG) & 0xff);
		} else {
			value |= bits;
			value |= (i2s_readl(i2s, LS_IIS_CFG) & 0xff0000);
		}
		i2s_writel(i2s, LS_IIS_CFG, value);

		value = (mclk_ratio_frac << 16) | mclk_ratio;
		i2s_writel(i2s, LS_IIS_CFG1, value);

		break;
	default:
		dev_err(i2s->dev, "I2S revision invalid\n");
		return -EINVAL;
	}

	return 0;
}

static int loongson_i2s_set_dai_sysclk(struct snd_soc_dai *dai, int clk_id,
				 unsigned int freq, int dir)
{
	struct loongson_i2s *i2s = snd_soc_dai_get_drvdata(dai);

	i2s->sysclk = freq;

	return 0;
}

static int loongson_i2s_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct loongson_i2s *i2s = snd_soc_dai_get_drvdata(dai);
	u32 value;

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		break;
	default:
		return -EINVAL;
	}

	value = i2s_readl(i2s, LS_IIS_CTRL);

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		break;
	case SND_SOC_DAIFMT_CBS_CFM:
		/* Enable master mode */
		value |= IIS_CTRL_MASTER;
		i2s_writel(i2s, LS_IIS_CTRL, value);

		if (i2s->rev_id == 1) {
			/* Wait BCLK ready */
			while (!(i2s_readl(i2s, LS_IIS_CTRL)
				 & IIS_CTRL_CLK_READY))
				udelay(10);
		}
		break;
	case SND_SOC_DAIFMT_CBM_CFS:
		/* Enable MCLK */
		if (i2s->rev_id == 1) {
			value |= IIS_CTRL_MCLK_EN;
			i2s_writel(i2s, LS_IIS_CTRL, value);
			while (!(i2s_readl(i2s, LS_IIS_CTRL)
				 & IIS_CTRL_MCLK_READY))
				udelay(10);
		}
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		/* Enable MCLK */
		if (i2s->rev_id == 1) {
			value |= IIS_CTRL_MCLK_EN;
			i2s_writel(i2s, LS_IIS_CTRL, value);
			while (!(i2s_readl(i2s, LS_IIS_CTRL)
				 & IIS_CTRL_MCLK_READY))
				udelay(10);
		}
		/* Enable master mode */
		value |= IIS_CTRL_MASTER;
		i2s_writel(i2s, LS_IIS_CTRL, value);

		if (i2s->rev_id == 1) {
			/* Wait BCLK ready */
			while (!(i2s_readl(i2s, LS_IIS_CTRL)
				 & IIS_CTRL_CLK_READY))
				udelay(10);
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static const struct snd_soc_dai_ops ls_i2s_dai_ops = {
	.startup	= loongson_i2s_startup,
	.trigger	= loongson_i2s_trigger,
	.hw_params	= loongson_i2s_hw_params,
	.set_sysclk	= loongson_i2s_set_dai_sysclk,
	.set_fmt	= loongson_i2s_set_fmt,
};

static int loongson_i2s_dai_probe(struct snd_soc_dai *cpu_dai)
{
	struct loongson_i2s *i2s = dev_get_drvdata(cpu_dai->dev);

	snd_soc_dai_init_dma_data(cpu_dai, &i2s->playback_dma_data,
				  &i2s->capture_dma_data);
	snd_soc_dai_set_drvdata(cpu_dai, i2s);

	return 0;
}

struct snd_soc_dai_driver loongson_i2s_dai = {
	.name = "loongson-i2s",
	.probe = loongson_i2s_dai_probe,
	.playback = {
		.stream_name = "CPU-Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_96000,
		.formats = LS_I2S_FORMATS,
	},
	.capture = {
		.stream_name = "CPU-Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_96000,
		.formats = LS_I2S_FORMATS,
	},
	.ops = &ls_i2s_dai_ops,
	.symmetric_rates = 1,
};

#ifdef CONFIG_PM_SLEEP
static int i2s_suspend(struct device *dev)
{
	struct loongson_i2s *i2s = dev_get_drvdata(dev);

	i2s->cfg_reg = i2s_readl(i2s, LS_IIS_CFG);
	i2s->ctrl_reg = i2s_readl(i2s, LS_IIS_CTRL);

	return 0;
}

static int i2s_resume(struct device *dev)
{
	struct loongson_i2s *i2s = dev_get_drvdata(dev);

	i2s_writel(i2s, LS_IIS_CFG, i2s->cfg_reg);
	i2s_writel(i2s, LS_IIS_CTRL, i2s->ctrl_reg);

	return 0;
}

const struct dev_pm_ops loongson_i2s_pm = {
	SET_SYSTEM_SLEEP_PM_OPS(i2s_suspend, i2s_resume)
};
#endif

void loongson_i2s_init(struct loongson_i2s *i2s)
{
	if (i2s->rev_id == 1) {
		/* Soft reset */
		i2s_writel(i2s, LS_IIS_CTRL, IIS_CTRL_RESET);
		udelay(1000);
	}
}
