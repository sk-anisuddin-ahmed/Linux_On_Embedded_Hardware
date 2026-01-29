#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/file.h>
#include <linux/miscdevice.h>

#define UART2_DEVICE "/dev/uart2_ker"

static int uart2_comm_open(struct inode *inode, struct file *file)
{
    pr_info("UART2 Communication Driver: Open called\n");
    return 0;
}

static ssize_t uart2_comm_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    struct file *uart2_file;
    mm_segment_t old_fs;
    char *kbuf;
    ssize_t ret;

    uart2_file = filp_open(UART2_DEVICE, O_WRONLY, 0);
    if (IS_ERR(uart2_file)) {
        pr_err("Failed to open %s\n", UART2_DEVICE);
        return PTR_ERR(uart2_file);
    }

    kbuf = kmalloc(len, GFP_KERNEL);
    if (!kbuf) {
        filp_close(uart2_file, NULL);
        return -ENOMEM;
    }

    if (copy_from_user(kbuf, buf, len)) {
        kfree(kbuf);
        filp_close(uart2_file, NULL);
        return -EFAULT;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    ret = vfs_write(uart2_file, kbuf, len, offset);
    set_fs(old_fs);

    kfree(kbuf);
    filp_close(uart2_file, NULL);
    return ret;
}

static ssize_t uart2_comm_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    struct file *uart2_file;
    mm_segment_t old_fs;
    char *kbuf;
    ssize_t ret;

    uart2_file = filp_open(UART2_DEVICE, O_RDONLY, 0);
    if (IS_ERR(uart2_file)) {
        pr_err("Failed to open %s\n", UART2_DEVICE);
        return PTR_ERR(uart2_file);
    }

    kbuf = kmalloc(len, GFP_KERNEL);
    if (!kbuf) {
        filp_close(uart2_file, NULL);
        return -ENOMEM;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    ret = vfs_read(uart2_file, kbuf, len, offset);
    set_fs(old_fs);

    if (ret > 0 && copy_to_user(buf, kbuf, ret)) {
        ret = -EFAULT;
    }

    kfree(kbuf);
    filp_close(uart2_file, NULL);
    return ret;
}

static int uart2_comm_release(struct inode *inode, struct file *file)
{
    pr_info("UART2 Communication Driver: Release called\n");
    return 0;
}

static const struct file_operations uart2_comm_fops = {
    .owner = THIS_MODULE,
    .open = uart2_comm_open,
    .read = uart2_comm_read,
    .write = uart2_comm_write,
    .release = uart2_comm_release,
};

static struct miscdevice uart2_comm_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "uart2_comm",
    .fops = &uart2_comm_fops,
};

static int __init uart2_comm_init(void)
{
    int ret;

    ret = misc_register(&uart2_comm_device);
    if (ret) {
        pr_err("Failed to register UART2 Communication Driver\n");
        return ret;
    }

    pr_info("UART2 Communication Driver initialized\n");
    return 0;
}

static void __exit uart2_comm_exit(void)
{
    misc_deregister(&uart2_comm_device);
    pr_info("UART2 Communication Driver exited\n");
}

module_init(uart2_comm_init);
module_exit(uart2_comm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
MODULE_DESCRIPTION("UART2 Communication Kernel Driver");