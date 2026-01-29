obj-m += uart2_ker.o

DTS_NAME := BBB_UART2
KER_PATH := /lib/modules/$(shell uname -r)/build
MOD_NAME := $(basename $(obj-m))

all: clean load dtbo

modules:
	$(MAKE) -C $(KER_PATH) M=$(PWD) modules

clean: rmmod
	$(MAKE) -C $(KER_PATH) M=$(PWD) clean
	sudo rm -f *.dtbo *.o *.ko *.mod* .*.cmd
	sudo rm -f /boot/dtbs/$(shell uname -r)/overlays/$(DTS_NAME).dtbo

dtbo:
	cpp -nostdinc -I $(KER_PATH)/include $(DTS_NAME).dts | dtc -O dtb -o $(DTS_NAME).dtbo -b 0 -@
	sudo cp -f $(DTS_NAME).dtbo /boot/dtbs/$(shell uname -r)/overlays/
	sudo nano /boot/uEnv.txt

insmod:
	sudo insmod $(MOD_NAME).ko

rmmod:
	@if lsmod | grep -q "^$(MOD_NAME)"; then \
		sudo rmmod $(MOD_NAME); \
	fi

load: rmmod modules insmod
	sudo dmesg | tail -n 10