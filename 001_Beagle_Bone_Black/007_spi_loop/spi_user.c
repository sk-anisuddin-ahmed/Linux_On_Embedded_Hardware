#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>

#define SPI_DEVICE "/dev/spidev0.0"
#define SPI_MODE SPI_MODE_0
#define SPI_BITS_PER_WORD 8
#define SPI_SPEED 500000

int main() 
{
    int fd = open(SPI_DEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open SPI device");
        return 1;
    }

    uint8_t mode = SPI_MODE;
    uint8_t bits = SPI_BITS_PER_WORD;
    uint32_t speed = SPI_SPEED;

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) 
    {
        perror("Can't set SPI mode");
        close(fd);
        return 1;
    }
    
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1)
    {
        perror("Can't set bits per word");
        close(fd);
        return 1;
    }
    
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
    {
        perror("Can't set max speed");
        close(fd);
        return 1;
    }
    
    uint8_t tx[] = {0xAA, 0x55, 0xFF, 0xAB, 0xCD};
    uint8_t rx[sizeof(tx)] = {0};

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = sizeof(tx),
        .speed_hz = speed,
        .bits_per_word = bits,
        .delay_usecs = 0,
    };

    int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1) 
    {
        perror("Can't send SPI message");
        close(fd);
        return 1;
    }

    printf("Transmitted (TX): ");
    for (size_t i = 0; i < sizeof(tx); i++)
    {
        printf("%02X ", tx[i]);
    }
    printf("\n");

    printf("Received   (RX): ");
    for (size_t i = 0; i < sizeof(rx); i++)
    {
        printf("%02X ", rx[i]);
    }
    printf("\n");

    for (size_t i = 0; i < sizeof(tx); i++)
    {
        if (tx[i] != rx[i])
        {
            printf("Mismatch at byte %zu: TX=0x%02X, RX=0x%02X\n", i, tx[i], rx[i]);
        }
    }

    close(fd);
    return 0;
}
