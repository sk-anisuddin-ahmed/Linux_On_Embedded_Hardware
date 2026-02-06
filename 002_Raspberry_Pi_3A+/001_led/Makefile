obj-m := led_ker.o
KER_PATH = /lib/modules/$(shell uname -r)/build

DT_NAME := RPI_LED
MOD_NAME := $(basename $(obj-m))
KO_NAME := $(MOD_NAME).ko

all: clean modules dtbo

modules:
	$(MAKE) -C $(KER_PATH) M=$(PWD) modules

clean: rmmod
	$(MAKE) -C $(KER_PATH) M=$(PWD) clean
	sudo rm -f *.o *.ko *.mod* .*.cmd *.symvers *.dtbo
	sudo rm -f /boot/overlays/$(DT_NAME).dtbo

dtbo:
	dtc -I dts -O dtb -o $(DT_NAME).dtbo $(DT_NAME).dts
	sudo cp -f $(DT_NAME).dtbo /boot/overlays/

insmod:
	sudo insmod $(KO_NAME)

rmmod:
	sudo rmmod $(MOD_NAME) || true

load: rmmod modules insmod
	sudo dmesg | tail -n 20

