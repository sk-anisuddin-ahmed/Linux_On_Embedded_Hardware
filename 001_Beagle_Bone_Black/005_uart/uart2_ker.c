#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/io.h>

struct uart2_priv {
    void __iomem *base;
} *priv;

#define UART_TX        0x00     // Transmit Buffer
#define UART_RX        0x00     // Receiver Buffer
#define UART_LSR       0x14     // Status Register

#define UART_LSR_THRE  (1 << 5) // TX Empty bit
#define UART_LSR_DR    (1 << 0) // RX Ready bit


static void uart2_send_char(char c)
{
    while (!(readb(priv->base + UART_LSR) & UART_LSR_THRE))
    {
        cpu_relax();
    }

    writeb(c, priv->base + UART_TX);
}

static char uart2_receive_char(void)
{
    while (!(readb(priv->base + UART_LSR) & UART_LSR_DR))
    {
        cpu_relax();
    }

    return readb(priv->base + UART_RX);
}

static ssize_t uart2_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    char kbuf[256];
    size_t i;

    if (!priv || !priv->base)
    {
        return -ENODEV;
    }

    if (len > sizeof(kbuf))
    {
        len = sizeof(kbuf);
    }

    for (i = 0; i < len; i++)
    {
        kbuf[i] = uart2_receive_char();
        if (kbuf[i] == '\n')
        {
            break;
        }
    }

    if (copy_to_user(buf, kbuf, i))
	{
        return -EFAULT;
	}

    return i + 1;
}

static ssize_t uart2_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    char kbuf[256];
    size_t i;

    if (!priv || !priv->base)
    {
        return -ENODEV;
    }

    if (len > sizeof(kbuf))
    {
        len = sizeof(kbuf);
    }

    if (copy_from_user(kbuf, buf, len))
    {
        return -EFAULT;
    }

    for (i = 0; i < len; i++) 
    {
        uart2_send_char(kbuf[i]);
    }

    return len;
}

static const struct file_operations uart2_fops = {
    .owner = THIS_MODULE,
    .read  = uart2_read,
    .write = uart2_write,
};

static struct miscdevice uart2_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "uart2_ker",
    .fops  = &uart2_fops,
};

static int uart2_probe(struct platform_device *pdev)
{
    struct device_node *parent_np;
    struct resource *res;
    int ret;    

    pr_info("%s: function called\n", __func__);

    parent_np = of_get_parent(pdev->dev.of_node);
    if (!parent_np) 
    {
        pr_err("Parent node not found\n");
        return -ENODEV;
    }

    pr_info("Parent node name: %s\n", parent_np->name);
    pr_info("Device node name: %s\n", pdev->dev.of_node->name);
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) 
    {
        pr_err("Failed to get platform resource\n");
        return -ENODEV;
    }

    pr_info("Resource start: 0x%lx, end: 0x%lx\n", (unsigned long)res->start, (unsigned long)res->end);

    priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv) 
    {
        pr_err("Failed to allocate memory for priv\n");
        return -ENOMEM;
    }

    platform_set_drvdata(pdev, priv);

    priv->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(priv->base)) 
    {
        pr_err("Failed to map UART2 regs\n");
        return PTR_ERR(priv->base);
    }

    pr_info("UART2 mapped at %p\n", priv->base);

    ret = misc_register(&uart2_misc);
    if (ret) 
    {
        pr_err("Failed to register misc device\n");
        return ret;
    }

    pr_info("Created /dev/uart2_ker\n");
    return 0;
}

static void uart2_remove(struct platform_device *pdev)
{
    misc_deregister(&uart2_misc);
    pr_info("%s: function called\n", __func__);
}

static const struct of_device_id uart2_of_match[] = {
    { .compatible = "anis,uart2-test" },
    {},
};
MODULE_DEVICE_TABLE(of, uart2_of_match);

static struct platform_driver uart2_driver = {
    .probe  = uart2_probe,
    .remove = uart2_remove,
    .driver = {
        .name = "uart2_driver",
        .of_match_table = uart2_of_match,
    },
};
module_platform_driver(uart2_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");