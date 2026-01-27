#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/ioport.h>

#define UART_THR     0x00
#define UART_RHR     0x00
#define UART_DLL     0x00
#define UART_IER     0x04
#define UART_DLH     0x04
#define UART_FCR     0x08
#define UART_LCR     0x0C
#define UART_LSR     0x14
#define UART_MDR1    0x20


#define UART_LSR_DR			(1 << 0)
#define UART_LSR_THRE   	(1 << 5)
#define UART_LCR_WLS_8  	(3 << 0)
#define UART_LCR_STOP1 		(0 << 2)
#define UART_LCR_DLAB  		(1 << 7)
#define UART_FCR_FIFO_EN   	(1 << 0)
#define UART_FCR_RX_CLR    	(1 << 1)
#define UART_FCR_TX_CLR    	(1 << 2)

#define UART_MDR1_DISABLE  	0x7
#define UART_MDR1_UART16   	0x0

struct uart2_priv {
    void __iomem *base;
} *priv;

static void uart2_init(void)
{
    writel(UART_MDR1_DISABLE, priv->base + UART_MDR1);
    writel(0x0, priv->base + UART_IER);
    writel(UART_LCR_DLAB, priv->base + UART_LCR);
    writel(0x1A, priv->base + UART_DLL);
    writel(0x00, priv->base + UART_DLH);
    writel(UART_LCR_WLS_8 | UART_LCR_STOP1, priv->base + UART_LCR);
    writel(UART_FCR_FIFO_EN | UART_FCR_RX_CLR | UART_FCR_TX_CLR, priv->base + UART_FCR);
    writel(UART_MDR1_UART16, priv->base + UART_MDR1);
}

static void uart2_send_char(char c)
{
    while (!(readl(priv->base + UART_LSR) & UART_LSR_THRE)) {
        cpu_relax();
	}
    writel(c, priv->base + UART_THR);
}

static char uart2_wait_char(void)
{
    while (!(readl(priv->base + UART_LSR) & UART_LSR_DR)) {
        cpu_relax();
	}
    return (char)(readl(priv->base + UART_RHR) & 0xFF);
}

static void uart2_send(char *msg)
{
    int i = 0;

    if (!msg) {
        return;
	}

    while (msg[i] != '\0') {
        uart2_send_char((char)msg[i]);
        i++;
    }
}

static void uart2_wait(char *buf, int len)
{
    int i;

    if (!buf || len <= 0) {
        return;
	}

    for (i = 0; i < len; i++) {
        buf[i] = (char)uart2_wait_char();
    }
}

static ssize_t uart2_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    char kbuf[256];
    size_t to_write;

    if (!priv || !priv->base) {
        return -ENODEV;
	}
	
    to_write = min(len, sizeof(kbuf));
    if (copy_from_user(kbuf, buf, to_write)) {
        return -EFAULT;
	}
	kbuf[to_write - 1] = '\0';
	uart2_send(kbuf);
    return to_write;
}

static ssize_t uart2_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    char kbuf[32];
    size_t to_read = min(len, 8);
	kbuf[to_read] = '\0';

    if (!priv || !priv->base) {
        return -ENODEV;
	}
	
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
	struct resource *res;
    int ret;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "No reg property found\n");
        return -ENODEV;
    }

    dev_info(&pdev->dev,
             "UART2 resource: start=0x%lx end=0x%lx size=0x%lx flags=0x%lx\n",
             (unsigned long)res->start,
             (unsigned long)res->end,
             (unsigned long)(resource_size(res)),
             (unsigned long)res->flags);
	
    priv = devm_kzalloc(&pdev->dev, sizeof(struct uart2_priv), GFP_KERNEL);
    if (!priv) {
        return -ENOMEM;
	}
	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) { 
		dev_err(&pdev->dev, "No reg property found\n"); 
		return -ENODEV;
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
        .name = "uart2_driver",
        .of_match_table = uart2_dt_ids,
    },
};
module_platform_driver(uart2_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
