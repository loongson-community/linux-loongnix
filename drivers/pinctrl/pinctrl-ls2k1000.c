// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf-generic.h>
#include "core.h"
#include "pinctrl-utils.h"

#define PMX_GROUP(grp, offset, bitv)					\
	{								\
		.name = #grp,						\
		.pins = grp ## _pins,					\
		.num_pins = ARRAY_SIZE(grp ## _pins),			\
		.reg = offset,						\
		.bit = bitv,						\
	}

#define SPECIFIC_GROUP(group)						\
	static const char * const group##_groups[] = {			\
		#group							\
	}

#define FUNCTION(fn)							\
	{								\
		.name = #fn,						\
		.groups = fn ## _groups,				\
		.num_groups = ARRAY_SIZE(fn ## _groups),		\
	}


struct ls2k_pinctrl {
	struct device *dev;
	struct pinctrl_dev *pcdev;
	struct pinctrl_desc desc;
	struct device_node *of_node;
	raw_spinlock_t lock;
	void * __iomem reg_base;
	u32 *saved_regs;
};

struct ls2k_pmx_group {
	const char *name;
	const unsigned int *pins;
	unsigned int num_pins;
	unsigned int reg;
	unsigned int bit;
};

struct ls2k_pmx_func {
	const char *name;
	const char * const *groups;
	unsigned int num_groups;
};


#define LS2K_PIN(x) PINCTRL_PIN(x, "gpio"#x)
static const struct pinctrl_pin_desc ls2k_pctrl_pins[] = {
	LS2K_PIN(0),  LS2K_PIN(1),  LS2K_PIN(2),  LS2K_PIN(3),
	LS2K_PIN(4),  LS2K_PIN(5),  LS2K_PIN(6),  LS2K_PIN(7),
	LS2K_PIN(8),  LS2K_PIN(9),  LS2K_PIN(10), LS2K_PIN(11),
	LS2K_PIN(12), LS2K_PIN(13), LS2K_PIN(14),
	LS2K_PIN(16), LS2K_PIN(17), LS2K_PIN(18), LS2K_PIN(19),
	LS2K_PIN(20), LS2K_PIN(21), LS2K_PIN(22), LS2K_PIN(23),
	LS2K_PIN(24), LS2K_PIN(25), LS2K_PIN(26), LS2K_PIN(27),
	LS2K_PIN(28), LS2K_PIN(29), LS2K_PIN(30),
	LS2K_PIN(32), LS2K_PIN(33), LS2K_PIN(34), LS2K_PIN(35),
	LS2K_PIN(36), LS2K_PIN(37), LS2K_PIN(38), LS2K_PIN(39),
	LS2K_PIN(40), LS2K_PIN(41),
	LS2K_PIN(44), LS2K_PIN(45), LS2K_PIN(46), LS2K_PIN(47),
	LS2K_PIN(48), LS2K_PIN(49), LS2K_PIN(50), LS2K_PIN(51),
	LS2K_PIN(52), LS2K_PIN(53), LS2K_PIN(54), LS2K_PIN(55),
	LS2K_PIN(56), LS2K_PIN(57), LS2K_PIN(58), LS2K_PIN(59),
	LS2K_PIN(60), LS2K_PIN(61), LS2K_PIN(62), LS2K_PIN(63),
};

static const unsigned int gpio_pins[] = { 0, 1, 2, 3, 4, 5, 6, 7,
					8, 9, 10, 11, 12, 13, 14,
					16, 17, 18, 19, 20, 21, 22, 23,
					24, 25, 26, 27, 28, 29, 30,
					32, 33, 34, 35, 36, 37, 38, 39,
					40,         43, 44, 45, 46, 47,
					48, 49, 50, 51, 52, 53, 46, 55,
					56, 57, 58, 59, 60, 61, 62, 63};
static const unsigned int sdio_pins[] = { 36, 37, 38, 39, 40, 41 };
static const unsigned int can1_pins[] = { 34, 35 };
static const unsigned int can0_pins[] = { 32, 33 };
static const unsigned int pwm3_pins[] = { 23 };
static const unsigned int pwm2_pins[] = { 22 };
static const unsigned int pwm1_pins[] = { 21 };
static const unsigned int pwm0_pins[] = { 20 };
static const unsigned int i2c1_pins[] = { 18, 19 };
static const unsigned int i2c0_pins[] = { 16, 17 };
static const unsigned int nand_pins[] = { 44, 45, 46, 47, 48, 49, 50, 51,
					52, 53, 54, 55, 56, 57, 58, 59, 60,
					61, 62, 63 };
static const unsigned int sata_led_pins[] = { 14 };
static const unsigned int lio_pins[] = { };
static const unsigned int i2s_pins[] = { 24, 25, 26, 27, 28, };
static const unsigned int hda_pins[] = { 24, 25, 26, 27, 28, 29, 30 };
static const unsigned int uart2_pins[] = { };
static const unsigned int uart1_pins[] = { };
static const unsigned int camera_pins[] = { };
static const unsigned int dvo1_pins[] = { };
static const unsigned int dvo0_pins[] = { };

static struct ls2k_pmx_group ls2k_pmx_groups[] = {
	PMX_GROUP(gpio, 0, 64),
	PMX_GROUP(sdio, 0, 20),
	PMX_GROUP(can1, 0, 17),
	PMX_GROUP(can0, 0, 16),
	PMX_GROUP(pwm3, 0, 15),
	PMX_GROUP(pwm2, 0, 14),
	PMX_GROUP(pwm1, 0, 13),
	PMX_GROUP(pwm0, 0, 12),
	PMX_GROUP(i2c1, 0, 11),
	PMX_GROUP(i2c0, 0, 10),
	PMX_GROUP(nand, 0, 9),
	PMX_GROUP(sata_led, 0, 8),
	PMX_GROUP(lio, 0, 7),
	PMX_GROUP(i2s, 0, 6),
	PMX_GROUP(hda, 0, 4),
	PMX_GROUP(uart2, 8, 13),
	PMX_GROUP(uart1, 8, 12),
	PMX_GROUP(camera, 0x10, 5),
	PMX_GROUP(dvo1, 0x10, 4),
	PMX_GROUP(dvo0, 0x10, 1),

};

SPECIFIC_GROUP(sdio);
SPECIFIC_GROUP(can1);
SPECIFIC_GROUP(can0);
SPECIFIC_GROUP(pwm3);
SPECIFIC_GROUP(pwm2);
SPECIFIC_GROUP(pwm1);
SPECIFIC_GROUP(pwm0);
SPECIFIC_GROUP(i2c1);
SPECIFIC_GROUP(i2c0);
SPECIFIC_GROUP(nand);
SPECIFIC_GROUP(sata_led);
SPECIFIC_GROUP(lio);
SPECIFIC_GROUP(i2s);
SPECIFIC_GROUP(hda);
SPECIFIC_GROUP(uart2);
SPECIFIC_GROUP(uart1);
SPECIFIC_GROUP(camera);
SPECIFIC_GROUP(dvo1);
SPECIFIC_GROUP(dvo0);

static const char * const gpio_groups[] = {
	"sdio", "can1", "can0", "pwm3", "pwm2", "pwm1", "pwm0", "i2c1",
	"i2c0", "nand", "sata_led", "lio", "i2s", "hda", "uart2", "uart1",
	"camera", "dvo1", "dvo0"
};

static struct ls2k_pmx_func ls2k_pmx_functions[] = {
	FUNCTION(gpio),
	FUNCTION(sdio),
	FUNCTION(can1),
	FUNCTION(can0),
	FUNCTION(pwm3),
	FUNCTION(pwm2),
	FUNCTION(pwm1),
	FUNCTION(pwm0),
	FUNCTION(i2c1),
	FUNCTION(i2c0),
	FUNCTION(nand),
	FUNCTION(sata_led),
	FUNCTION(lio),
	FUNCTION(i2s),
	FUNCTION(hda),
	FUNCTION(uart2),
	FUNCTION(uart1),
	FUNCTION(camera),
	FUNCTION(dvo1),
	FUNCTION(dvo0),
};

static int ls2k_get_groups_count(struct pinctrl_dev *pcdev)
{
	return ARRAY_SIZE(ls2k_pmx_groups);
}

static const char *ls2k_get_group_name(struct pinctrl_dev *pcdev,
					unsigned int selector)
{
	return ls2k_pmx_groups[selector].name;
}

static int ls2k_get_group_pins(struct pinctrl_dev *pcdev, unsigned int selector,
			const unsigned int **pins, unsigned int *num_pins)
{
	*pins = ls2k_pmx_groups[selector].pins;
	*num_pins = ls2k_pmx_groups[selector].num_pins;

	return 0;
}

static void ls2k_pin_dbg_show(struct pinctrl_dev *pcdev, struct seq_file *s,
			       unsigned int offset)
{
	seq_printf(s, " %s", dev_name(pcdev->dev));
}

static const struct pinctrl_ops ls2k_pctrl_ops = {
	.get_groups_count	= ls2k_get_groups_count,
	.get_group_name		= ls2k_get_group_name,
	.get_group_pins		= ls2k_get_group_pins,
	.dt_node_to_map		= pinconf_generic_dt_node_to_map_all,
	.dt_free_map		= pinctrl_utils_free_map,
	.pin_dbg_show		= ls2k_pin_dbg_show,
};

static int ls2k_pmx_set_mux(struct pinctrl_dev *pcdev, unsigned int func_num,
			      unsigned int group_num)
{
	struct ls2k_pinctrl *pctrl = pinctrl_dev_get_drvdata(pcdev);
	unsigned long reg = (unsigned long)pctrl->reg_base +
				ls2k_pmx_groups[group_num].reg;
	unsigned int mux_bit = ls2k_pmx_groups[group_num].bit;
	unsigned int val;
	unsigned long flags;

	raw_spin_lock_irqsave(&pctrl->lock, flags);
	val = readl((void *)reg);
	if (func_num == 0)
		val &= ~(1<<mux_bit);
	else
		val |= (1<<mux_bit);
	writel(val, (void *)reg);
	raw_spin_unlock_irqrestore(&pctrl->lock, flags);

	return 0;
}

int ls2k_pmx_get_funcs_count(struct pinctrl_dev *pcdev)
{
	return ARRAY_SIZE(ls2k_pmx_functions);
}

const char *ls2k_pmx_get_func_name(struct pinctrl_dev *pcdev,
				    unsigned int selector)
{
	return ls2k_pmx_functions[selector].name;
}

int ls2k_pmx_get_groups(struct pinctrl_dev *pcdev, unsigned int selector,
			 const char * const **groups,
			 unsigned int * const num_groups)
{
	*groups = ls2k_pmx_functions[selector].groups;
	*num_groups = ls2k_pmx_functions[selector].num_groups;

	return 0;
}

const struct pinmux_ops ls2k_pmx_ops = {
	.set_mux = ls2k_pmx_set_mux,
	.get_functions_count = ls2k_pmx_get_funcs_count,
	.get_function_name = ls2k_pmx_get_func_name,
	.get_function_groups = ls2k_pmx_get_groups,
};

int ls2k_pinctrl_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ls2k_pinctrl *pctrl;
	struct resource *res;

	pctrl = devm_kzalloc(dev, sizeof(struct ls2k_pinctrl), GFP_KERNEL);
	if (!pctrl)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pctrl->reg_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(pctrl->reg_base))
		return PTR_ERR(pctrl->reg_base);

	raw_spin_lock_init(&pctrl->lock);

	pctrl->dev = dev;
	pctrl->desc.name	= "pinctrl-ls2k";
	pctrl->desc.owner	= THIS_MODULE;
	pctrl->desc.pctlops	= &ls2k_pctrl_ops;
	pctrl->desc.pmxops	= &ls2k_pmx_ops;
	pctrl->desc.confops	= NULL;
	pctrl->desc.pins	= ls2k_pctrl_pins;
	pctrl->desc.npins	= ARRAY_SIZE(ls2k_pctrl_pins);

	pctrl->pcdev = devm_pinctrl_register(pctrl->dev, &pctrl->desc, pctrl);
	if (IS_ERR(pctrl->pcdev)) {
		dev_err(pctrl->dev, "can't register pinctrl device");
		return PTR_ERR(pctrl->pcdev);
	}

#ifdef CONFIG_PM_SLEEP
	pctrl->saved_regs = devm_kcalloc(&pdev->dev, ARRAY_SIZE(ls2k_pmx_groups),
				sizeof(*pctrl->saved_regs), GFP_KERNEL);
	if (!pctrl->saved_regs)
		return -ENOMEM;
#endif

	platform_set_drvdata(pdev, pctrl);

	return 0;
}

static const struct of_device_id ls2k_pinctrl_dt_match[] = {
	{
		.compatible = "loongson,2k1000-pinctrl",
	},
	{ },
};

#ifdef CONFIG_PM_SLEEP
static int ls2k_pinctrl_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct ls2k_pinctrl *pctrl = platform_get_drvdata(pdev);
	unsigned int ngroups = ARRAY_SIZE(ls2k_pmx_groups);
	void * __iomem reg;
	unsigned long flags;
	int i, mux_bit;

	raw_spin_lock_irqsave(&pctrl->lock, flags);
	for (i = 0; i < ngroups; i++) {
		reg = pctrl->reg_base + ls2k_pmx_groups[i].reg;
		mux_bit = ls2k_pmx_groups[i].bit;
		pctrl->saved_regs[i] = (readl(reg) & (1 << mux_bit));
	}
	raw_spin_unlock_irqrestore(&pctrl->lock, flags);

	return 0;
}

static int ls2k_pinctrl_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct ls2k_pinctrl *pctrl = platform_get_drvdata(pdev);
	unsigned int ngroups = ARRAY_SIZE(ls2k_pmx_groups);
	void * __iomem reg;
	unsigned long flags;
	unsigned int val;
	int i, mux_bit;

	raw_spin_lock_irqsave(&pctrl->lock, flags);
	for (i = 0; i < ngroups; i++) {
		reg = pctrl->reg_base + ls2k_pmx_groups[i].reg;
		mux_bit = ls2k_pmx_groups[i].bit;
		val = readl(reg) & ~(1 << mux_bit);
		val |= pctrl->saved_regs[i];
		writel(val, reg);
	}
	raw_spin_unlock_irqrestore(&pctrl->lock, flags);

	return 0;
}

static const struct dev_pm_ops ls2k_pinctrl_pm_ops = {
	SET_LATE_SYSTEM_SLEEP_PM_OPS(ls2k_pinctrl_suspend, ls2k_pinctrl_resume)
};
#endif

static struct platform_driver ls2k_pinctrl_driver = {
	.probe		= ls2k_pinctrl_probe,
	.driver = {
		.name	= "ls2k-pinctrl",
		.of_match_table = ls2k_pinctrl_dt_match,
#ifdef CONFIG_PM_SLEEP
		.pm = &ls2k_pinctrl_pm_ops,
#endif
	},
};

static int __init ls2k_pinctrl_init(void)
{
	return platform_driver_register(&ls2k_pinctrl_driver);
}
arch_initcall(ls2k_pinctrl_init);

static void __exit ls2k_pinctrl_exit(void)
{
	platform_driver_unregister(&ls2k_pinctrl_driver);
}
module_exit(ls2k_pinctrl_exit);
