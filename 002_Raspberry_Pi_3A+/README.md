# Raspberry Pi 3 Model A+ - Linux Hardware Projects

Complete collection of Linux kernel driver examples for Raspberry Pi 3A+ hardware interfaces.

## Project Structure
```
002_Raspberry_Pi_3A+/
├── 001_led/              # Traffic LED driver (GPIO)
├── 002_btn_irq/          # Button interrupt with LED counter
├── 003_uart_test/        # UART communication test
├── 004_i2c_rtc/          # I2C RTC (DS1307) driver
├── 005_spi_loop/         # SPI loopback test
├── Documents/            # Hardware documentation
└── Program_Output/       # Test results and screenshots
```

## Hardware Requirements
- **Raspberry Pi 3 Model A+**
- **LEDs**: 3-7 LEDs with 330Ω resistors
- **Button**: Push button with 10kΩ pull-up
- **DS1307 RTC**: I2C real-time clock module (optional)
- **Breadboard and jumper wires**

## Software Requirements
- **Raspberry Pi OS** (Bookworm or later)
- **Linux Kernel Headers**: `sudo apt install raspberrypi-kernel-headers`
- **Build Tools**: `sudo apt install build-essential`
- **I2C Tools** (optional): `sudo apt install i2c-tools`
- **SPI Tools** (optional): `sudo apt install spi-tools`

## Quick Start

### 1. Install Prerequisites
```bash
sudo apt update
sudo apt install raspberrypi-kernel-headers build-essential git
```

### 2. Clone/Navigate to Project
```bash
cd "002_Raspberry_Pi_3A+"
```

### 3. Enable Hardware Interfaces
```bash
# Enable I2C, SPI, UART
sudo raspi-config
# Navigate to: Interface Options → Enable I2C, SPI, UART
sudo reboot
```

### 4. Build and Test a Project
```bash
cd 001_led
make load          # Build, load module, show dmesg
# Test the LEDs...
sudo rmmod led_ker # Unload when done
```

## Project Details

### [001_led](001_led/) - Traffic LED Driver
**GPIO**: 17, 27, 22  
**Purpose**: Sequential blinking traffic light pattern  
**Concepts**: Platform driver, GPIO consumer API, kernel threads

### [002_btn_irq](002_btn_irq/) - Button Interrupt Counter
**GPIO**: Button + 4 LEDs  
**Purpose**: Interrupt-driven binary counter  
**Concepts**: IRQ handling, threaded interrupts, GPIO input

### [003_uart_test](003_uart_test/) - UART Test
**GPIO**: 14 (TX), 15 (RX)  
**Purpose**: Kernel UART messaging  
**Concepts**: UART, kernel threads, serial communication

### [004_i2c_rtc](004_i2c_rtc/) - I2C RTC Driver
**I2C**: Address 0x68, GPIO2/3  
**Purpose**: DS1307 RTC interface  
**Concepts**: I2C drivers, RTC subsystem, BCD conversion

### [005_spi_loop](005_spi_loop/) - SPI Loopback
**SPI**: GPIO 8, 9, 10, 11  
**Purpose**: SPI communication test  
**Concepts**: SPI protocol, loopback testing

## Common Commands

### Building Modules
```bash
make              # Build kernel module
make clean        # Clean build artifacts
make load         # Build, load, and show kernel log
```

### Module Management
```bash
sudo insmod module.ko     # Load module
sudo rmmod module_name    # Unload module
lsmod | grep module       # Check if loaded
dmesg | tail -20          # View kernel messages
```

### Hardware Verification
```bash
# GPIO status
cat /sys/kernel/debug/gpio

# I2C devices
sudo i2cdetect -y 1

# SPI devices
ls /dev/spi*

# UART
ls /dev/ttyS* /dev/ttyAMA*
```

## GPIO Pin Reference
```
Pin  GPIO  Function          Project
---  ----  --------          -------
11   17    Red LED           001_led
13   27    Yellow LED        001_led
15   22    Green LED         001_led
3    2     I2C SDA           004_i2c_rtc
5    3     I2C SCL           004_i2c_rtc
8    14    UART TX           003_uart_test
10   15    UART RX           003_uart_test
19   10    SPI MOSI          005_spi_loop
21   9     SPI MISO          005_spi_loop
23   11    SPI SCLK          005_spi_loop
24   8     SPI CE0           005_spi_loop
```

## Troubleshooting

### Module Won't Load
```bash
# Check kernel version match
uname -r
ls /lib/modules/$(uname -r)/

# Install correct headers
sudo apt install raspberrypi-kernel-headers
```

### GPIO Not Working
```bash
# Check permissions
sudo usermod -a -G gpio $USER
# Log out and back in

# Verify GPIO export
echo 17 > /export
```

### I2C/SPI Not Enabled
```bash
# Check config
cat /boot/firmware/config.txt | grep -E 'i2c|spi|uart'

# Should see:
dtparam=i2c_arm=on
dtparam=spi=on
enable_uart=1
```

## Documentation
See [Documents/](Documents/) folder for:
- Hardware specifications
- GPIO reference
- Device tree guide
- Kernel driver development guide

## References
- [Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/)
- [BCM2835 Datasheet](https://datasheets.raspberrypi.com/bcm2835/bcm2835-peripherals.pdf)
- [Linux Device Drivers](https://lwn.net/Kernel/LDD3/)
- [GPIO Consumer API](https://www.kernel.org/doc/html/latest/driver-api/gpio/consumer.html)

## License
Educational use. Based on Raspberry Pi hardware examples and Linux kernel documentation.

---

**Hardware**: Raspberry Pi 3 Model A+  
**Processor**: BCM2837B0 (1.4 GHz quad-core ARMv7)  
**Memory**: 512 MB LPDDR2  
**Kernel**: Linux 6.x (Raspberry Pi OS)
