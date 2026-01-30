#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_DEVICE "/dev/i2c-2"

int main()
{
    int fd = open(I2C_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return 1;
    }

    int device_addr = 0x3C;
    if (ioctl(fd, I2C_SLAVE, device_addr) < 0)
    {
        perror("Failed to set I2C slave address");
        close(fd);
        return 1;
    }

    uint8_t data_to_send[2] = {0x00, 0xFF};
    ssize_t bytes_written = write(fd, data_to_send, sizeof(data_to_send));
    if (bytes_written < 0)
    {
        perror("Failed to write to I2C device");
    }
    else
    {
        printf("Wrote %zd bytes\n", bytes_written);
    }

    uint8_t read_buffer[2];
    ssize_t bytes_read = read(fd, read_buffer, sizeof(read_buffer));
    if (bytes_read < 0)
    {
        perror("Failed to read from I2C device");
    }
    else
    {
        printf("Read %zd bytes: %02X %02X\n", bytes_read, read_buffer[0], read_buffer[1]);
    }

    close(fd);
    return 0;
}