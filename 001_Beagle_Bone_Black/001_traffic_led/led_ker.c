#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct gpio_desc *led[3];
static struct task_struct *blink_thread;

static int blink_fn(void *data)
{
    int i;

    while (!kthread_should_stop()) {
        for (i = 0; i < ARRAY_SIZE(led); i++) {
            gpiod_set_value(led[i], 1);
            msleep(333);
            gpiod_set_value(led[i], 0);
        }
    }
    return 0;
}

static int myleds_probe(struct platform_device *pdev)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(led); i++) {
	led[i] = devm_gpiod_get_index(&pdev->dev, NULL, i, GPIOD_OUT_LOW);
        if (IS_ERR(led[i])) {
            dev_err(&pdev->dev, "Failed to get LED%d\n", i);
            return PTR_ERR(led[i]);
        }
    }

    blink_thread = kthread_run(blink_fn, NULL, "traffic_leds");
    if (IS_ERR(blink_thread)) {
        return PTR_ERR(blink_thread);
    }

    dev_info(&pdev->dev, "myleds driver probed\n");
    return 0;
}

static void myleds_remove(struct platform_device *pdev)
{
    int i;

    if (blink_thread) {
        kthread_stop(blink_thread);
    }

    for (i = 0; i < ARRAY_SIZE(led); i++) {
        gpiod_set_value(led[i], 0);
    }

    dev_info(&pdev->dev, "myleds driver removed\n");
}

static const struct of_device_id myleds_dt[] = {
    { .compatible = "anis,myleds" },
    {}
};
MODULE_DEVICE_TABLE(of, myleds_dt);

static struct platform_driver myleds_driver = {
    .probe  = myleds_probe,
    .remove = myleds_remove,
    .driver = {
        .name = "usr-leds",
        .of_match_table = myleds_dt,
    },
};
module_platform_driver(myleds_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
