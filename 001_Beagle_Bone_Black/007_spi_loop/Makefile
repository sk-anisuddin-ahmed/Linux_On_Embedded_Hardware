APP_NAME := spi_user
MOD_NAME := spi0_ker
DTS_NAME := BBB_SPI0
obj-m := $(MOD_NAME).o
KER_PATH = /lib/modules/$(shell uname -r)/build

all: load dts dts_cp

app:
	gcc -o $(APP_NAME) spi_user.c

modules:
	$(MAKE) -C $(KER_PATH) M=$(PWD) modules

clean: rmmod
	$(MAKE) -C $(KER_PATH) M=$(PWD) clean
	rm -f *.o *.ko *.mod* .*.cmd $(APP_NAME)
	sudo rm -f /boot/dtbs/$(shell uname -r)/overlays/$(DTS_NAME).dtbo	

insmod:
	sudo insmod $(MOD_NAME).ko
	sudo dmesg | tail -n 15
rmmod:
	@if lsmod | grep -q "^$(MOD_NAME)"; then \
		sudo rmmod $(MOD_NAME); \
	fi

dts_c:
	cpp -nostdinc -I $(KER_PATH)/include $(DTS_NAME).dts | dtc -O dtb -o $(DTS_NAME).dtbo -b 0 -@
dts:
	dtc -@ -I dts -O dtb -b 0 -o $(DTS_NAME).dtbo $(DTS_NAME).dts

dts_cp:
	sudo cp -f $(DTS_NAME).dtbo /boot/dtbs/$(shell uname -r)/overlays/
	sudo nano /boot/uEnv.txt

load: clean modules insmod

reload: rmmod insmod