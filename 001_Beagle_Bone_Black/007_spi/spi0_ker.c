#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/slab.h>
#include <linux/signal.h>

struct spi_data {
    struct spi_device *spi;
};

static int my_spi_probe(struct spi_device *spi)
{	
    struct spi_data *data;
    int ret;
    u8 txbuf = 0xAB;
    u8 rxbuf;
	int sendCount = 10;

    dev_info(&spi->dev, "%s: mode=%u, max_freq=%u, chip-select=%u\n",
			__func__, spi->mode, (unsigned)spi->max_speed_hz, (unsigned)spi->chip_select);

    data = devm_kzalloc(&spi->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
    {
        dev_err(&spi->dev, "%s: Memory allocation failed\n", __func__);
        return -ENOMEM;
    }

    spi_set_drvdata(spi, data);
    data->spi = spi;

    spi->mode = SPI_MODE_0;
    spi->bits_per_word = 8;
    spi->max_speed_hz = 100000;
    dev_info(&spi->dev, "%s: Setting mode=%u\n, bits_per_word=%u\n, max_speed_hz=%u\n", 
			__func__, spi->mode, spi->bits_per_word, spi->max_speed_hz);

    ret = spi_setup(spi);
    if (ret)
    {
        dev_err(&spi->dev, "%s: spi_setup failed: %d\n", __func__, ret);
        return ret;
    }
    dev_info(&spi->dev, "%s: spi_setup succeeded\n", __func__);

    while ((txbuf[0] != rxbuf[0]) && sendCount--)
    {
        ret = spi_write_then_read(spi, &txbuf, sizeof(txbuf), &rxbuf, sizeof(rxbuf));
        if (ret < 0)
        {
            dev_warn(&spi->dev, "%s: SPI transfer failed: %d\n", ret, __func__);
            return ret;
        }
    }
    dev_info(&spi->dev, "%s: SPI Transfer: 0x%02x, SPI Receive: 0x%02x\n", __func__, txbuf[0], rxbuf[0]);
    return 0;
}

static void my_spi_remove(struct spi_device *spi)
{
    dev_info(&spi->dev, "%s: SPI device removed\n", __func__);
}

static const struct spi_device_id spi_id[] = {
    {"bbb_spi_device", 0},
    {}
};
MODULE_DEVICE_TABLE(spi, spi_id);

static const struct of_device_id spi_of[] = {
    {.compatible = "bbb_spi_device"},
    {}
};
MODULE_DEVICE_TABLE(of, spi_of);

static struct spi_driver my_spi_driver = {
    .driver = {
        .name = "bbb_spi_device",
        .of_match_table = spi_of,
    },
    .probe = my_spi_probe,
    .remove = my_spi_remove,
    .id_table = spi_id,
};
module_spi_driver(my_spi_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anis");
