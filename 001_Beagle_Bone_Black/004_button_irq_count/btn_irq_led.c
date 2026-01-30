#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/delay.h>

static struct gpio_desc *led[4];
static struct gpio_desc *btn;
static int irq_num, irq_cnt;

static irqreturn_t button_irq_handler(int irq, void *dev_id)
{       
    return IRQ_WAKE_THREAD;
}

static irqreturn_t button_irq_thread(int irq, void *dev_id)
{
    irq_cnt++;
    irq_cnt %= 16;
    gpiod_set_value(led[0], (irq_cnt & 0b0001) ? 1 : 0);
    gpiod_set_value(led[1], (irq_cnt & 0b0010) ? 1 : 0);
    gpiod_set_value(led[2], (irq_cnt & 0b0100) ? 1 : 0);
    gpiod_set_value(led[3], (irq_cnt & 0b1000) ? 1 : 0);
    pr_info("Button IRQ (threaded): LEDs toggled to %d\n", irq_cnt);
    return IRQ_HANDLED;
}

static int btn_led_probe(struct platform_device *pdev)
{
    int ret, i;

    for (i = 0; i < ARRAY_SIZE(led); i++) 
    {
        led[i] = devm_gpiod_get_index(&pdev->dev, "led", i, GPIOD_OUT_LOW);
        if (IS_ERR(led[i]))
        {
            dev_err(&pdev->dev, "Failed to get led-gpios[%d]\n", i);
            return PTR_ERR(led[i]);
        }
    }

    btn = devm_gpiod_get(&pdev->dev, "btn", GPIOD_IN);
    if (IS_ERR(btn)) 
    {
        dev_err(&pdev->dev, "Failed to get btn-gpios\n");
        return PTR_ERR(btn);
    }

    irq_num = platform_get_irq(pdev, 0);
    if (irq_num < 0) 
    {
        dev_err(&pdev->dev, "Failed to get IRQ\n");
        return irq_num;
    }

    ret = devm_request_threaded_irq(&pdev->dev, irq_num, button_irq_handler, button_irq_thread, IRQF_TRIGGER_FALLING, "button_irq", NULL);
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
    int i;
    for (i = 0; i < ARRAY_SIZE(led); i++) 
    {
        gpiod_set_value(led[i], 0);
    }
    dev_info(&pdev->dev, "Button IRQ driver removed\n");
}

static const struct of_device_id btn_led_dt[] = {
    { .compatible = "bbb-btn-led" },
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
