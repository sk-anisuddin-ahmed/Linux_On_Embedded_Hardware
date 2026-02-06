#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/kthread.h>

static struct task_struct *uart_thread;

static int uart_test_thread(void *data)
{
    int count = 0;
    
    while (!kthread_should_stop()) 
    {
        pr_info("UART Test: Message %d from kernel\n", count++);
        msleep(2000);
    }
    
    return 0;
}

static int uart_probe(struct platform_device *pdev)
{    
    uart_thread = kthread_run(uart_test_thread, NULL, "uart_test");
    if (IS_ERR(uart_thread)) 
    {
        dev_err(&pdev->dev, "Failed to create thread\n");
        return PTR_ERR(uart_thread);
    }
    
    dev_info(&pdev->dev, "RPi UART test driver probed\n");
    return 0;
}

static void uart_remove(struct platform_device *pdev)
{    
    if (uart_thread) 
    {
        kthread_stop(uart_thread);
    }
    
    dev_info(&pdev->dev, "UART test driver removed\n");
}

static const struct of_device_id uart_ids[] = {
    { .compatible = "rpi,uart-test" },
    { }
};
MODULE_DEVICE_TABLE(of, uart_ids);

static struct platform_driver uart_pltdrv = {
    .probe = uart_probe,
    .remove = uart_remove,
    .driver = {
        .name = "rpi-uart-test",
        .of_match_table = uart_ids,
    },
};
module_platform_driver(uart_pltdrv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
