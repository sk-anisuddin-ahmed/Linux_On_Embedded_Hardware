MOD_NAME := btn_irq_led
DTS_NAME := BBB_BTN_S2

obj-m := $(MOD_NAME).o
KER_PATH = /lib/modules/$(shell uname -r)/build

all: load dtbo

modules:
	$(MAKE) -C $(KER_PATH) M=$(PWD) modules

clean: rmmod
	$(MAKE) -C $(KER_PATH) M=$(PWD) clean
	rm -f *.o *.ko *.mod* .*.cmd
	sudo rm -f /boot/dtbs/$(shell uname -r)/overlays/$(DTS_NAME).dtbo

insmod:
	sudo insmod $(MOD_NAME).ko
	sudo dmesg | tail -n 15

rmmod:
	@if lsmod | grep -q "^$(MOD_NAME)"; then \
		sudo rmmod $(MOD_NAME); \
	fi

dtbo:
	cpp -nostdinc -I $(KER_PATH)/include $(DTS_NAME).dts | dtc -O dtb -o $(DTS_NAME).dtbo -b 0 -@
	sudo cp -f $(DTS_NAME).dtbo /boot/dtbs/$(shell uname -r)/overlays/
	sudo nano /boot/uEnv.txt

load: clean modules insmod

reload: rmmod insmod