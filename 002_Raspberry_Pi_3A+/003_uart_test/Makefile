obj-m := uart_ker.o
KER_PATH = /lib/modules/$(shell uname -r)/build
MOD_NAME := $(basename $(obj-m))
KO_NAME := $(MOD_NAME).ko

all: clean modules insmod

modules:
	$(MAKE) -C $(KER_PATH) M=$(PWD) modules

clean: rmmod
	$(MAKE) -C $(KER_PATH) M=$(PWD) clean
	rm -f *.o *.ko *.mod* .*.cmd

insmod:
	sudo insmod $(KO_NAME)

rmmod:
	@if lsmod | grep -q "^$(MOD_NAME)"; then \
		sudo rmmod $(MOD_NAME); \
	fi

load: rmmod modules insmod
	sudo dmesg | tail -n 20
