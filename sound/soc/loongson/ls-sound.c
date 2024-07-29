#include <linux/module.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>

#include <linux/pci_ids.h>
#include <linux/pci.h>


static int loongson_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	return 0;

}

static struct snd_soc_ops loongson_ops = {
	.hw_params = loongson_hw_params,
};

static int loongson_es8388_init_paiftx(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}

enum {
	PRI_PLAYBACK = 0,
	PRI_CAPTURE,
};
/*
 *SND_SOC_DAIFMT_CBS_CFS  play ok, volume ok
 *SND_SOC_DAIFMT_CBM_CFM  Codec's BCLK is Master，LRCLK is Master
 */
#define LOONGSON_DAI_FMT (SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_IB_NF | \
	SND_SOC_DAIFMT_CBS_CFS)

static struct snd_soc_dai_link loongson_dai[] = {
	[PRI_PLAYBACK] = {
		/* Primary Playback i/f */
		.name = "ES8388 PAIF RX",
		.stream_name = "Playback",
		.cpu_dai_name = "loongson-i2s-dai",
		.codec_dai_name = "ES8323 HiFi",
		.platform_name = "loongson-i2s",
		.dai_fmt = LOONGSON_DAI_FMT,
		.ops = &loongson_ops,
	},
	[PRI_CAPTURE] = {
		/* Primary Capture i/f */
		.name = "ES8388 PAIF TX",
		.stream_name = "Capture",
		.cpu_dai_name = "loongson-i2s-dai",
		.codec_dai_name = "ES8323 HiFi",
		.platform_name = "loongson-i2s",
		.dai_fmt = LOONGSON_DAI_FMT,
		.init = loongson_es8388_init_paiftx,
		.ops = &loongson_ops,
	},
};

static struct snd_soc_card loongson = {
	.name = "LOONGSON-I2S",
	.owner = THIS_MODULE,
	.dai_link = loongson_dai,
	.num_links = ARRAY_SIZE(loongson_dai),

};

static struct platform_device *loongson_snd_device;

static int ls_sound_drv_probe(struct platform_device *pdev)
{
	int ret;

	loongson_snd_device = platform_device_alloc("soc-audio", -1);
	if (!loongson_snd_device)
		return -ENOMEM;

	platform_set_drvdata(loongson_snd_device, &loongson);
	ret = platform_device_add(loongson_snd_device);

	if (ret)
		platform_device_put(loongson_snd_device);

	return ret;
}

static int ls_sound_drv_remove(struct platform_device *pdev)
{
	platform_device_unregister(loongson_snd_device);
	return 0;
}
static const struct of_device_id snd_ls_sound_dt_match[] = {
	{ .compatible = "loongson,ls-sound", },
	{ .compatible = "loongson,loongson2-sound", },
	{ .compatible = "loongson,ls7a-sound", },
	{},
};
MODULE_DEVICE_TABLE(of, snd_ls_sound_dt_match);

static struct platform_driver ls_sound_driver = {
	.probe = ls_sound_drv_probe,
	.remove = ls_sound_drv_remove,
	.driver = {
		.name = "ls-sound",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(snd_ls_sound_dt_match),
	},
};

static int __init loongson_audio_init(void)
{
	struct pci_dev *pdev;

	pdev = pci_get_device(PCI_VENDOR_ID_LOONGSON, PCI_DEVICE_ID_LOONGSON_I2S, NULL);

	if (pdev) {
		loongson_dai[0].codec_name = "i2c-ESSX8323:00";
		loongson_dai[1].codec_name = "i2c-ESSX8323:00";
	} else {
		loongson_dai[0].codec_name = "ES8323.3-0010";
		loongson_dai[1].codec_name = "ES8323.3-0010";
	}

	return platform_driver_register(&ls_sound_driver);
}
module_init(loongson_audio_init);

static void __exit loongson_audio_exit(void)
{
	platform_driver_unregister(&ls_sound_driver);
}
module_exit(loongson_audio_exit);

MODULE_SOFTDEP("pre: ls_pcm ls_i2s snd_soc_es8323_i2c");
MODULE_AUTHOR("loongson");
MODULE_DESCRIPTION("ALSA SoC loongson");
MODULE_LICENSE("GPL");
