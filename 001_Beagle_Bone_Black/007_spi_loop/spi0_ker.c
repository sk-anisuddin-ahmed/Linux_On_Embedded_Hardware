#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/init.h>

struct spi_device *g_spi_device;

static int spi0_probe(struct spi_device *spi)
{
    int ret;
    u8 tx = 0xA5;
    u8 rx = 0x00;
    struct spi_message msg;
    struct spi_transfer tr = {
        .tx_buf = &tx,
        .rx_buf = &rx,
        .len = 1,
        .bits_per_word = 8,
    };

    g_spi_device = spi;
    ret = spi_setup(spi);
    if (ret)
    {
        dev_err(&spi->dev, "FAILED to setup SPI device\n");
        return ret;
    }

    dev_info(&spi->dev, "SPI Loopback: Transfer 0x%02X\n", tx);
    spi_message_init(&msg);
    spi_message_add_tail(&tr, &msg);
    ret = spi_sync(spi, &msg);
    if (ret)
    {
        dev_err(&spi->dev, "SPI transfer failed: %d\n", ret);
    }
    else if (rx != tx) 
    {
        dev_err(&spi->dev, "SPI loopback mismatch: sent 0x%02X, received 0x%02X\n", tx, rx);
    }
    else 
    {
        dev_info(&spi->dev, "SPI received: 0x%02X\n", rx);
    }
    return 0;
}

static void spi0_remove(struct spi_device *spi)
{
    dev_info(&spi->dev, "SPI loopback driver unloaded\n");
}

static const struct of_device_id spi0_of_match[] = {
    { .compatible = "bbb_spi0_dev" },
    { }
};
MODULE_DEVICE_TABLE(of, spi0_of_match);

static const struct spi_device_id spi0_id_table[] = {
    { "bbb_spi0_dev", 0 },
    { }
};
MODULE_DEVICE_TABLE(spi, spi0_id_table);

static struct spi_driver spi0_driver = {
    .driver = {
        .name = "bbb_spi0_device",
        .of_match_table = spi0_of_match,
    },
    .id_table = spi0_id_table,
    .probe = spi0_probe,
    .remove = spi0_remove,
};

module_spi_driver(spi0_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
