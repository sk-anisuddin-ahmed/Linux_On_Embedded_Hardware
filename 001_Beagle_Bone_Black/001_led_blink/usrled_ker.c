#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/kthread.h>
#include <linux/delay.h>

typedef struct {
    struct gpio_desc *led[4];
    struct task_struct *blink_thread;
} myleds;

static int blink_fn(void *data)
{
    myleds *ml = data;
    int i;

    while (!kthread_should_stop()) 
	{
        for (i = 0; i < ARRAY_SIZE(ml->led); i++) 
		{
            gpiod_set_value(ml->led[i], 1);
            msleep(250);
            gpiod_set_value(ml->led[i], 0);
        }
    }
    return 0;
}

static int myleds_probe(struct platform_device *pdev)
{
    myleds *ml;
    int i;

    ml = devm_kzalloc(&pdev->dev, sizeof(*ml), GFP_KERNEL);
    if (!ml)
	{
        return -ENOMEM;
	}
	
	ml->led[0] = devm_gpiod_get(&pdev->dev, "anis-led0", GPIOD_OUT_LOW);
	ml->led[1] = devm_gpiod_get(&pdev->dev, "anis-led1", GPIOD_OUT_LOW);
	ml->led[2] = devm_gpiod_get(&pdev->dev, "anis-led2", GPIOD_OUT_LOW);
	ml->led[3] = devm_gpiod_get(&pdev->dev, "anis-led3", GPIOD_OUT_LOW);

    for (i = 0; i < ARRAY_SIZE(ml->led); i++) 
	{
        if (IS_ERR(ml->led[i])) 
		{
            dev_err(&pdev->dev, "Failed to get LED %d GPIO\n", i);
            return PTR_ERR(ml->led[i]);
        }
    }

    ml->blink_thread = kthread_run(blink_fn, ml, "led_pattern");
    if (IS_ERR(ml->blink_thread))
	{
        return PTR_ERR(ml->blink_thread);
	}
    platform_set_drvdata(pdev, ml);

    dev_info(&pdev->dev, "myleds driver probed\n");
    return 0;
}

static int myleds_remove(struct platform_device *pdev)
{
	int i;
    myleds *ml = platform_get_drvdata(pdev);
    
    if (ml->blink_thread)
	{
        kthread_stop(ml->blink_thread);
	}

    for (i = 0; i < ARRAY_SIZE(ml->led); i++)
	{
        gpiod_set_value(ml->led[i], 0);
	}

    dev_info(&pdev->dev, "myleds driver removed\n");
    return 0;
}

static const struct of_device_id myleds_dt_ids[] = {
    { .compatible = "anis-gpio-leds"},
    {}
};
MODULE_DEVICE_TABLE(of, myleds_dt_ids);

static struct platform_driver myleds_driver = {
    .probe  = myleds_probe,
    .remove = myleds_remove,
    .driver = {
        .name = "usr-leds",
        .of_match_table = myleds_dt_ids,
    },
};
module_platform_driver(myleds_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
MODULE_DESCRIPTION("On-Board LED blink driver");