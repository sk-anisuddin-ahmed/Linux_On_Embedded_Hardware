#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct gpio_desc *led;
static struct task_struct *blink_thread;

static int blink_fn(void *data)
{
    while (!kthread_should_stop()) 
    {
        gpiod_set_value(led, 1);
        msleep(500);
        gpiod_set_value(led, 0);
        msleep(500);
    }
    return 0;
}

static int myleds_probe(struct platform_device *pdev)
{
    led = devm_gpiod_get(&pdev->dev, NULL, 0, GPIOD_OUT_LOW);
    if (IS_ERR(led)) 
    {
        dev_err(&pdev->dev, "Failed to get LED%d\n", 0);
        return PTR_ERR(led);
    }

    blink_thread = kthread_run(blink_fn, NULL, "blink_thread");
    if (IS_ERR(blink_thread)) 
    {
        return PTR_ERR(blink_thread);
    }

    dev_info(&pdev->dev, "RPi LED driver probed\n");
    return 0;
}

static void myleds_remove(struct platform_device *pdev)
{
    if (blink_thread) 
    {
        kthread_stop(blink_thread);
    }
    gpiod_set_value(led, 0);

    dev_info(&pdev->dev, "RPi LED driver removed\n");
}

static const struct of_device_id led_dt[] = {
    { .compatible = "rpi,led" },
    { }
};
MODULE_DEVICE_TABLE(of, led_dt);

static struct platform_driver led_pltdrv = {
    .probe  = led_probe,
    .remove = led_remove,
    .driver = {
        .name = "rpi-led",
        .of_match_table = led_dt,
    },
};
module_platform_driver(led_pltdrv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");

