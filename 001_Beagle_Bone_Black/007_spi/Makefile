obj-m := spi0_ker.o
KER_PATH = /lib/modules/$(shell uname -r)/build
DTS_NAME = BBB_SPI0
MOD_NAME := $(basename $(obj-m))
KO_NAME := $(MOD_NAME).ko

all: load dtbo

modules:
	$(MAKE) -C $(KER_PATH) M=$(PWD) modules

clean: rmmod
	$(MAKE) -C $(KER_PATH) M=$(PWD) clean
	rm -f *.dtbo *.o *.ko *.mod* .*.cmd
	sudo rm -f /boot/dtbs/$(shell uname -r)/overlays/$(DTS_NAME).dtbo

dtbo:
	cpp -nostdinc -I $(KER_PATH)/include $(DTS_NAME).dts | dtc -O dtb -o $(DTS_NAME).dtbo -b 0 -@
	sudo cp -f $(DTS_NAME).dtbo /boot/dtbs/$(shell uname -r)/overlays/
	sudo nano /boot/uEnv.txt

insmod:
	sudo insmod $(KO_NAME)
	sudo dmesg | tail -n 10

rmmod:
	@if lsmod | grep -q "^$(MOD_NAME)"; then \
		sudo rmmod $(MOD_NAME); \
	fi

load: clean modules insmod
