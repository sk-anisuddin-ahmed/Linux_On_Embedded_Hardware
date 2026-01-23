obj-m := usrled_ker.o

all: 
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f *.dtbo *.o *.ko *.mod* .*.cmd

dtbo:
	dtc -@ -I dts -O dtb -o led.dtbo led.dts