#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>
#include <linux/device.h>
#include <linux/delay.h>

typedef struct {
    struct gpio_desc *rs;
    struct gpio_desc *en;
    struct gpio_desc *d4;
    struct gpio_desc *d5;
    struct gpio_desc *d6;
    struct gpio_desc *d7;
    struct device *dev;
} lcd16x2;

extern lcd16x2 *gp_lcd;

int lcd_chrdev_register(void);
void lcd_chrdev_unregister(void);

#define RS(x) gpiod_set_value(gp_lcd->rs, x)
#define EN(x) gpiod_set_value(gp_lcd->en, x)
#define D4(x) gpiod_set_value(gp_lcd->d4, x)
#define D5(x) gpiod_set_value(gp_lcd->d5, x)
#define D6(x) gpiod_set_value(gp_lcd->d6, x)
#define D7(x) gpiod_set_value(gp_lcd->d7, x)

static void lcd_enable_pulse(void)
{
    EN(1);
    mdelay(1);
    EN(0);
    mdelay(2);
}

static void lcd_write_char(uint8_t value)
{
    D4((value >> 4) & 0x01);
    D5((value >> 5) & 0x01);
    D6((value >> 6) & 0x01);
    D7((value >> 7) & 0x01);
    lcd_enable_pulse();

    D4((value >> 0) & 0x01);
    D5((value >> 1) & 0x01);
    D6((value >> 2) & 0x01);
    D7((value >> 3) & 0x01);
    lcd_enable_pulse();
}

static void lcd_command(uint8_t value)
{
    printk(KERN_DEBUG "lcd16x2: Sending command 0x%02X\n", value);
    RS(0);
    lcd_write_char(value);
}

static void lcd_write_8bit(uint8_t value)
{
    RS(1);
    lcd_write_char(value);
}

static void lcd_set_cursor(uint8_t col, uint8_t row)
{
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_command(0x80 | (col + row_offsets[row]));
}

static void lcd_send_string(char *msg)
{
    uint8_t i = 0;

    while (msg[i] != '\0')
    {
		if (i >= 32) {
			break;
		}
		else if (i == 16) {
			lcd_set_cursor(0, 1);
		}
		else {
			pr_info("%c", msg[i]);
		}

		lcd_write_8bit((uint8_t) msg[i++]);
    }
	pr_info("\n");
}

static void lcd_clear(void)
{
    lcd_command(0x01);
}

static void lcd_init(void)
{
    printk(KERN_INFO "lcd16x2: Initializing LCD\n");
    msleep(15);
    lcd_command(0x02);
    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x01);
    lcd_command(0x06);
    lcd_command(0x80);
    printk(KERN_INFO "lcd16x2: LCD initialization complete\n");
}

static ssize_t lcd_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
	static int pos;
    static char kbuf[33];
    size_t clen;

    if (!gp_lcd)
        return -ENODEV;

    clen = min(len, (sizeof(kbuf) - pos));

    if (copy_from_user((kbuf + pos), buf, clen)) {
		pos = 0;
		return -EFAULT;
	}
	
	pos += clen;
	
	if (kbuf[pos - 1] == '\n') {
		kbuf[pos - 1] = '\0';
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_send_string(kbuf);
		pr_info("lcd16x2: printed '%s'\n", kbuf);
		pos = 0;
	}
	
    return len;
}

static const struct file_operations lcd_fops = {
    .owner = THIS_MODULE,
    .write = lcd_write,
};

static struct miscdevice lcd_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "lcd16x2_chardev",
    .fops = &lcd_fops,
    .mode  = 0666
};

int lcd_chrdev_register(void)
{
    int ret = misc_register(&lcd_dev);
    if (ret) {
        pr_err("Failed to register lcd16x2_chardev\n");
        return ret;
    }
	
	if (gp_lcd == NULL) {
		printk(KERN_DEBUG "%s: No lcd16x2 driver loaded\n", __func__);
		return -ENODEV;
	}
	
	lcd_init();	
	lcd_clear();
    lcd_set_cursor(0, 0);	
	lcd_send_string("Hello Linux     You're Cool");	
	pr_info("%s: lcd16x2 init completed\n", __func__);
	
    pr_info("/dev/lcd16x2_chardev loaded\n");
    return 0;
}

void lcd_chrdev_unregister(void)
{
    misc_deregister(&lcd_dev);
    pr_info("lcd16x2_chardev unloaded\n");
}

EXPORT_SYMBOL(lcd_chrdev_register);
EXPORT_SYMBOL(lcd_chrdev_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
MODULE_DESCRIPTION("LCD16x2 char device");
