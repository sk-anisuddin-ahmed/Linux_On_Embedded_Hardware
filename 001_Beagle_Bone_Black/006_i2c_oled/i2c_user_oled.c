#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_DEVICE "/dev/i2c-2"
#define OLED_ADDR 0x3C

int OLED_SendCommand(int fd, uint8_t cmd)
{
    uint8_t buffer[2] = {0x00, cmd};
    return write(fd, buffer, 2);
}

int OLED_SendData(int fd, uint8_t data)
{
    uint8_t buffer[2] = {0x40, data};
    return write(fd, buffer, 2);
}

void OLED_Clear(int fd)
{
    for (uint16_t i = 0; i < 1024; i++)
    {
        OLED_SendData(fd, 0x00);
    }
}

void OLED_Fill(int fd)
{
    for (uint16_t i = 0; i < 1024; i++)
    {
        OLED_SendData(fd, 0xFF);
    }
}

void OLED_SetCursor(int fd, uint8_t col, uint8_t page)
{
    OLED_SendCommand(fd, 0x21);
    OLED_SendCommand(fd, col);
    OLED_SendCommand(fd, 127);
    OLED_SendCommand(fd, 0x22);
    OLED_SendCommand(fd, page);
    OLED_SendCommand(fd, 7);
}

void OLED_DrawHLine(int fd, uint8_t x1, uint8_t x2, uint8_t y)
 {
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    uint8_t data = (1 << bit);
    for (uint8_t x = x1; x <= x2; x++)
    {
        OLED_SetCursor(fd, x, page);
        OLED_SendData(fd, data);
    }
}

void OLED_DrawVLine(int fd, uint8_t x, uint8_t y1, uint8_t y2)
{
    uint8_t page_start = y1 / 8;
    uint8_t page_end   = y2 / 8;
    for (uint8_t page = page_start; page <= page_end; page++)
    {
        OLED_SetCursor(fd, x, page);
        OLED_SendData(fd, 0xFF);
    }
}

void OLED_DrawBox(int fd, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    OLED_DrawHLine(fd, x1, x2, y1);
    OLED_DrawHLine(fd, x1, x2, y2);
    OLED_DrawVLine(fd, x1, y1, y2);
    OLED_DrawVLine(fd, x2, y1, y2);
}

void OLED_Init(int fd)
{
    OLED_SendCommand(fd, 0xAE); // Display OFF
    OLED_SendCommand(fd, 0x20); // Set Memory Addressing Mode
    OLED_SendCommand(fd, 0x00); // Horizontal addressing mode
    OLED_SendCommand(fd, 0x8D); // Charge Pump
    OLED_SendCommand(fd, 0x14); // Enable charge pump
    OLED_SendCommand(fd, 0xA6); // Normal display (not inverted)
    OLED_SendCommand(fd, 0xAF); // Display ON
    OLED_Clear(fd);
    OLED_DrawBox(fd, 27, 14, 100, 50);
}

int main()
{
    int fd = open(I2C_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return 1;
    }

    if (ioctl(fd, I2C_SLAVE, OLED_ADDR) < 0) {
        perror("Failed to set I2C slave address");
        close(fd);
        return 1;
    }

    OLED_Init(fd);

    for (uint8_t x = 0; x < 150; x += 5)
    {
        OLED_Clear(fd);
        OLED_DrawBox(fd, x, 20, x + 20, 40);
    }
    sleep(1);

    close(fd);
    return 0;
}
