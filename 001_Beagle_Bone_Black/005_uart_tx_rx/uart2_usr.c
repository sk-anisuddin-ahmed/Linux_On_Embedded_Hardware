#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() 
{
    int uart_fd;
    char send_buffer[64] = "Hello from Linux!";
    char receive_buffer[64];
    ssize_t bytes_written, bytes_read;

    uart_fd = open("/dev/ttyS2", O_RDWR);
    if (uart_fd < 0) 
    {
        perror("Failed to open UART device");
        return 1;
    }

    bytes_written = write(uart_fd, send_buffer, strlen(send_buffer));
    if (bytes_written < 0)
    {
        perror("Failed to write to UART device");
        close(uart_fd);
        return 1;
    }
    printf("Sent %zd bytes: %s\n", bytes_written, send_buffer);

    bytes_read = read(uart_fd, receive_buffer, 64 - 1);
    if (bytes_read < 0) 
    {
        perror("Failed to read from UART device");
        close(uart_fd);
        return 1;
    }
    receive_buffer[bytes_read] = '\0';
    printf("Received %zd bytes: %s\n", bytes_read, receive_buffer);

    close(uart_fd);
    return 0;
}