#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>

extern int lcd_chrdev_register(void);
extern void lcd_chrdev_unregister(void);

typedef struct {
    struct gpio_desc *rs;
    struct gpio_desc *en;
    struct gpio_desc *d4;
    struct gpio_desc *d5;
    struct gpio_desc *d6;
    struct gpio_desc *d7;
	struct device *dev;
}lcd16x2;

lcd16x2 *gp_lcd;
EXPORT_SYMBOL(gp_lcd);

static int lcd16x2_probe(struct platform_device *pdev)
{
	int ret;

    gp_lcd = devm_kzalloc(&pdev->dev, sizeof(lcd16x2), GFP_KERNEL);
    if (!gp_lcd) {
        return -ENOMEM;
	}

    gp_lcd->rs = devm_gpiod_get(&pdev->dev, "rs", GPIOD_OUT_LOW);
    gp_lcd->en = devm_gpiod_get(&pdev->dev, "en", GPIOD_OUT_LOW);
    gp_lcd->d4 = devm_gpiod_get(&pdev->dev, "d4", GPIOD_OUT_LOW);
    gp_lcd->d5 = devm_gpiod_get(&pdev->dev, "d5", GPIOD_OUT_LOW);
    gp_lcd->d6 = devm_gpiod_get(&pdev->dev, "d6", GPIOD_OUT_LOW);
    gp_lcd->d7 = devm_gpiod_get(&pdev->dev, "d7", GPIOD_OUT_LOW);

    if (IS_ERR(gp_lcd->rs) || IS_ERR(gp_lcd->en) || 
		IS_ERR(gp_lcd->d4) || IS_ERR(gp_lcd->d5) || 
		IS_ERR(gp_lcd->d6) || IS_ERR(gp_lcd->d7))
	{
        dev_err(&pdev->dev, "Failed to get GPIOs\n");
        return -EINVAL;
    }
	
	gp_lcd->dev = &pdev->dev;
	platform_set_drvdata(pdev, gp_lcd);
	
    dev_info(&pdev->dev, "%s: lcd16x2 driver probed\n", __func__);	
	
	ret = lcd_chrdev_register(); 
	if (ret) { 
		dev_err(&pdev->dev, "Failed to register char device\n"); 
		return ret; 
	}
	
    return 0;
}

static void lcd16x2_remove(struct platform_device *pdev)
{
	lcd_chrdev_unregister();
    dev_info(&pdev->dev, "%s: lcd16x2 driver removed\n", __func__);
}

static const struct of_device_id lcd16x2_dt_ids[] = {
    { .compatible = "lcd16x2,anis"},
    {}
};
MODULE_DEVICE_TABLE(of, lcd16x2_dt_ids);

static struct platform_driver lcd16x2_driver = {
    .probe  = lcd16x2_probe,
    .remove = lcd16x2_remove,
    .driver = {
        .name = "lcd16x2_driver",
        .of_match_table = lcd16x2_dt_ids,
    },
};
module_platform_driver(lcd16x2_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
MODULE_DESCRIPTION("LCD16x2 platform driver");
