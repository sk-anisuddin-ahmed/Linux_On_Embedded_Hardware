#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define OLED_I2C_ADDR 0x3C

static int oled_send_command(struct i2c_client *client, u8 cmd)
{
    u8 buf[2] = {0x00, cmd};
    struct i2c_msg msg = {
        .addr = client->addr,
        .flags = 0,
        .len = 2,
        .buf = buf,
    };
    int ret = i2c_transfer(client->adapter, &msg, 1);
    if (ret == 1)
    {
        return 0;
    }
    else
    {
        return -EIO;
    }
}

static int oled_send_data(struct i2c_client *client, u8 data)
{
    u8 buf[2] = {0x40, data};
    struct i2c_msg msg = {
        .addr = client->addr,
        .flags = 0,
        .len = 2,
        .buf = buf,
    };
    int ret = i2c_transfer(client->adapter, &msg, 1);
    if (ret == 1)
    {
        return 0;
    }
    else
    {
        return -EIO;
    }
}

static void oled_set_cursor(struct i2c_client *client, u8 col, u8 page)
{
    oled_send_command(client, 0x21);
    oled_send_command(client, col);
    oled_send_command(client, 127);
    oled_send_command(client, 0x22);
    oled_send_command(client, page);
    oled_send_command(client, 7);
}

static void oled_draw_hline(struct i2c_client *client, u8 x1, u8 x2, u8 y)
{
    u8 page = y / 8;
    u8 bit = y % 8;
    u8 data = (1 << bit);
    u8 x;
    for (x = x1; x <= x2; x++)
    {
        oled_set_cursor(client, x, page);
        oled_send_data(client, data);
    }
}

static void oled_draw_vline(struct i2c_client *client, u8 x, u8 y1, u8 y2)
{
    u8 page_start = y1 / 8;
    u8 page_end = y2 / 8;
    u8 page;
    for (page = page_start; page <= page_end; page++)
    {
        oled_set_cursor(client, x, page);
        oled_send_data(client, 0xFF);
    }
}

static void oled_draw_box(struct i2c_client *client, u8 x1, u8 y1, u8 x2, u8 y2)
{
    oled_draw_hline(client, x1, x2, y1);
    oled_draw_hline(client, x1, x2, y2);
    oled_draw_vline(client, x1, y1, y2);
    oled_draw_vline(client, x2, y1, y2);
}

static void oled_clear(struct i2c_client *client)
{
    int i;
    for (i = 0; i < 1024; i++)
    {
        oled_send_data(client, 0x00);
    }
}

static void oled_init_sequence(struct i2c_client *client)
{
    oled_send_command(client, 0xAE);
    oled_send_command(client, 0x20);
    oled_send_command(client, 0x00);
    oled_send_command(client, 0x8D);
    oled_send_command(client, 0x14);
    oled_send_command(client, 0xA6);
    oled_send_command(client, 0xAF);
    oled_clear(client);
    oled_draw_box(client, 27, 14, 100, 50);
}

static int oled_probe(struct i2c_client *client)
{
    dev_info(&client->dev, "OLED I2C device probed\n");
    oled_init_sequence(client);
    return 0;
}

static void oled_remove(struct i2c_client *client)
{
    dev_info(&client->dev, "OLED I2C device removed\n");
}

static const struct of_device_id oled_of_match[] = {
    { .compatible = "i2c_oled" },
    { }
};
MODULE_DEVICE_TABLE(of, oled_of_match);

static const struct i2c_device_id oled_id[] = {
    { "i2c_oled", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, oled_id);

static struct i2c_driver oled_driver = {
    .driver = {
        .name = "i2c_oled_dev",
        .of_match_table = oled_of_match,
    },
    .probe = oled_probe,
    .remove = oled_remove,
    .id_table = oled_id,
};
module_i2c_driver(oled_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");