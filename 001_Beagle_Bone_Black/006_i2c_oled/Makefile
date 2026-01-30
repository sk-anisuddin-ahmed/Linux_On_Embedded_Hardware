obj-m += oled_i2cdrv.o

APP1 := i2c_user
APP2 := i2c_user_oled
DTS := BBB_I2C2_OLED
KER_PATH := /lib/modules/$(shell uname -r)/build
OVERLAY_DIR := /boot/dtbs/$(shell uname -r)/overlays
UENV_PATH := /boot/uEnv.txt

all: clean modules dtbo

modules:
	make -C $(KER_PATH) M=$(PWD) modules

dtbo:
	dtc -O dtb -o $(DTS).dtbo $(DTS).dts
	sudo cp -f $(DTS).dtbo $(OVERLAY_DIR)/
	sudo nano $(UENV_PATH)

app:
	gcc -o $(APP1) $(APP1).c
	gcc -o $(APP1) $(APP2).c

clean:
	$(MAKE) -C $(KER_PATH) M=$(PWD) clean
	rm -f *.dtbo *.o *.ko *.mod* .*.cmd
	sudo rm -f $(OVERLAY_DIR)/$(DTS).dtbo	

build_load: clean modules reload

load:
	sudo insmod oled_i2cdrv.ko
	dmesg | tail -n 20

unload:
	sudo rmmod oled_i2cdrv.ko || true
	dmesg | tail -n 20

reload: unload load
