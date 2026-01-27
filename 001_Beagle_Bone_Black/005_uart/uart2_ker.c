#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/io.h>

struct uart2_priv {
    void __iomem *base;
} *priv;

void uart2_init()
{
    
}

void uart2_send(char* msg)
{
    
}

void uart2_wait(char* buf, int len)
{
    
}

static ssize_t uart2_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    size_t i;
    char kbuf[256];
    size_t to_write;
    u32 lsr;

    if (!priv || !priv->base)
        return -ENODEV;

    to_write = min(len, sizeof(kbuf));
    if (copy_from_user(kbuf, buf, to_write))
        return -EFAULT;
	
	uart2_send(kbuf);
    return to_write;
}

static ssize_t uart2_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    char kbuf[32];
    u32 lsr;
    size_t to_read = min(len, 16);

    if (!priv || !priv->base)
        return -ENODEV;
	
	uart2_wait(kbuf, to_read);
    if (copy_to_user(buf, kbuf, to_read)) {
        return -EFAULT;
	}

    return to_read;
}

static struct file_operations uart2_fops = {
	.owner = THIS_MODULE,
	.write = uart2_write,
	.read = uart2_read
};

static struct miscdevice uart2_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "uart2_chardev",
    .fops = &uart2_fops,
    .mode = 0666
};

static int uart2_probe(struct platform_device *pdev)
{
    int ret;
    priv = devm_kzalloc(&pdev->dev, sizeof(struct uart2_priv), GFP_KERNEL);
    if (!priv) {
        return -ENOMEM;
	}

    priv->base = devm_platform_ioremap_resource(pdev, 0);
    if (IS_ERR(priv->base)) {
        dev_err(&pdev->dev, "Failed to ioremap registers\n");
        return PTR_ERR(priv->base);
    }
	
    platform_set_drvdata(pdev, priv);
    dev_info(&pdev->dev, "UART2 driver probed\n");
	
	uart2_init();

    ret = misc_register(&uart2_dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to register misc device\n");
        return ret;
    }
    return 0;
}

static void uart2_remove(struct platform_device *pdev)
{
    misc_deregister(&uart2_dev);
    dev_info(&pdev->dev, "UART2 driver removed\n");
}

static const struct of_device_id uart2_dt_ids[] = {
    { .compatible = "uart2,anis"},
    {}
};
MODULE_DEVICE_TABLE(of, uart2_dt_ids);

static struct platform_driver uart2_driver = {
    .probe  = uart2_probe,
    .remove = uart2_remove,
    .driver = {
        .name = "UART2_Custom_Driver",
        .of_match_table = uart2_dt_ids,
    },
};
module_platform_driver(uart2_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
