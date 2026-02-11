#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/delay.h>

static struct gpio_desc *led;
static struct gpio_desc *btn;
static int irq_num, led_stat;

static irqreturn_t button_irq_thread(int irq, void *dev_id)
{
	led_stat ^= 1;
    gpiod_set_value(led, led_stat);
    pr_info("btn IRQ (threaded): LEDs toggled\n");
    return IRQ_HANDLED;
}

static int btn_led_probe(struct platform_device *pdev)
{
    int ret;
	
	led = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
	btn = devm_gpiod_get(&pdev->dev, "btn", GPIOD_IN);
	irq_num = platform_get_irq(pdev, 0);
	
	if (IS_ERR(led))
	{
		dev_err(&pdev->dev, "Failed to get led-gpio\n");
		return PTR_ERR(led);
	}
    if (IS_ERR(btn)) 
    {
        dev_err(&pdev->dev, "Failed to get btn-gpio\n");
        return PTR_ERR(btn);
    }
    if (irq_num < 0) 
    {
        dev_err(&pdev->dev, "Failed to get IRQ\n");
        return irq_num;
    }

    ret = devm_request_threaded_irq(&pdev->dev, irq_num, NULL, button_irq_thread, IRQF_TRIGGER_FALLING, "button_irq", NULL);
    if (ret) 
    {
        dev_err(&pdev->dev, "Failed to request threaded IRQ\n");
        return ret;
    }

    dev_info(&pdev->dev, "Button IRQ driver probed\n");
    return 0;
}

static void btn_led_remove(struct platform_device *pdev)
{
    gpiod_set_value(led, 0);
    dev_info(&pdev->dev, "Button IRQ driver removed\n");
}

static const struct of_device_id btn_led_dt[] = {
    { .compatible = "rpi,btn-led" },
    {}
};
MODULE_DEVICE_TABLE(of, btn_led_dt);

static struct platform_driver btn_led_drv = {
    .probe  = btn_led_probe,
    .remove = btn_led_remove,
    .driver = {
        .name = "btn_led_irq",
        .of_match_table = btn_led_dt,
    },
};
module_platform_driver(btn_led_drv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
