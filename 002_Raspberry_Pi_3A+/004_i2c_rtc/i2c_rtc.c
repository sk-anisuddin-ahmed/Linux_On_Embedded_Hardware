#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/of.h>
#include <linux/bcd.h>
#include <linux/slab.h>

struct rpi_rtc_data {
    struct i2c_client *client;
    struct rtc_device *rtc;
};

static int rpi_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
    struct rpi_rtc_data *data = dev_get_drvdata(dev);
    u8 buf[7];
    int ret;
    
    /* Read time registers from DS1307 RTC */
    ret = i2c_smbus_read_i2c_block_data(data->client, 0x00, 7, buf);
    if (ret < 0) {
        dev_err(dev, "Failed to read time\n");
        return ret;
    }
    
    /* Convert BCD to binary */
    tm->tm_sec = bcd2bin(buf[0] & 0x7F);
    tm->tm_min = bcd2bin(buf[1] & 0x7F);
    tm->tm_hour = bcd2bin(buf[2] & 0x3F);
    tm->tm_mday = bcd2bin(buf[4] & 0x3F);
    tm->tm_mon = bcd2bin(buf[5] & 0x1F) - 1;
    tm->tm_year = bcd2bin(buf[6]) + 100;
    
    dev_info(dev, "RTC Time: %04d-%02d-%02d %02d:%02d:%02d\n",
             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
             tm->tm_hour, tm->tm_min, tm->tm_sec);
    
    return 0;
}

static int rpi_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
    struct rpi_rtc_data *data = dev_get_drvdata(dev);
    u8 buf[7];
    
    /* Convert binary to BCD */
    buf[0] = bin2bcd(tm->tm_sec);
    buf[1] = bin2bcd(tm->tm_min);
    buf[2] = bin2bcd(tm->tm_hour);
    buf[3] = bin2bcd(tm->tm_wday + 1);
    buf[4] = bin2bcd(tm->tm_mday);
    buf[5] = bin2bcd(tm->tm_mon + 1);
    buf[6] = bin2bcd(tm->tm_year - 100);
    
    /* Write time to DS1307 */
    return i2c_smbus_write_i2c_block_data(data->client, 0x00, 7, buf);
}

static const struct rtc_class_ops rpi_rtc_ops = {
    .read_time = rpi_rtc_read_time,
    .set_time = rpi_rtc_set_time,
};

static int rpi_rtc_probe(struct i2c_client *client)
{
    struct rpi_rtc_data *data;
    
    data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;
    
    data->client = client;
    i2c_set_clientdata(client, data);
    
    data->rtc = devm_rtc_device_register(&client->dev, "rpi-ds1307",
                                          &rpi_rtc_ops, THIS_MODULE);
    if (IS_ERR(data->rtc)) {
        dev_err(&client->dev, "Failed to register RTC device\n");
        return PTR_ERR(data->rtc);
    }
    
    dev_info(&client->dev, "RPi DS1307 RTC driver probed (I2C address 0x%02X)\n", 
             client->addr);
    return 0;
}

static void rpi_rtc_remove(struct i2c_client *client)
{
    dev_info(&client->dev, "RTC driver removed\n");
}

static const struct of_device_id rpi_rtc_ids[] = {
    { .compatible = "rpi,ds1307" },
    { }
};
MODULE_DEVICE_TABLE(of, rpi_rtc_ids);

static const struct i2c_device_id rpi_rtc_i2c_ids[] = {
    { "rpi-ds1307", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, rpi_rtc_i2c_ids);

static struct i2c_driver rpi_rtc_driver = {
    .driver = {
        .name = "rpi-ds1307",
        .of_match_table = rpi_rtc_ids,
    },
    .probe = rpi_rtc_probe,
    .remove = rpi_rtc_remove,
    .id_table = rpi_rtc_i2c_ids,
};

module_i2c_driver(rpi_rtc_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RPi Developer");
MODULE_DESCRIPTION("Raspberry Pi 3A+ DS1307 RTC Driver");
MODULE_VERSION("1.0");
