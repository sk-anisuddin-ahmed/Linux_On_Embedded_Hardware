obj-m := usr_led_ker.o
KER_PATH = /lib/modules/$(shell uname -r)/build
DTS_NAME = BBB_USR_LED
MOD_NAME := $(basename $(obj-m))
KO_NAME := $(MOD_NAME).ko

all: clean dtbo load

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

rmmod:
	@if lsmod | grep -q "^$(MOD_NAME)"; then \
		sudo rmmod $(MOD_NAME); \
	fi

load: rmmod modules insmod
	sudo dmesg | tail -n 10

dtbo_pp:
	cpp -nostdinc -I $(KER_PATH)/include $(DTS_NAME).dts > $(DTS_NAME).pp.dts

decompile:
	dtc -I dtb -O dts -o am335x-boneblack.dts /boot/dtbs/`uname -r`/am335x-boneblack.dtb
