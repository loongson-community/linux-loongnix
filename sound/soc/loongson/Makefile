#Platform Support
obj-$(CONFIG_SND_LS) +=  ls-i2s.o
obj-$(CONFIG_SND_LS_PCM) += ls-pcm.o

#Machine Support
snd-soc-loongson-es8323-objs := ls-sound.o
obj-$(CONFIG_SND_SOC_LOONGSON_ES8323) += snd-soc-loongson-es8323.o

# I2S driver v2
snd-soc-loongson-i2s-pci-objs := loongson_i2s_pci.o loongson_i2s.o loongson_pcm.o
obj-$(CONFIG_SND_SOC_LOONGSON_I2S_PCI) += snd-soc-loongson-i2s-pci.o

snd-soc-loongson-card-objs := loongson_card.o
obj-$(CONFIG_SND_SOC_LOONGSON_CARD) += snd-soc-loongson-card.o
