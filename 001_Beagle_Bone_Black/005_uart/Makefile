KER_PATH := /lib/modules/$(shell uname -r)/build
DTS_NAME := BBB_UART2
APP := uart2_app
SRC := uart2_usr.c

all: app

app:
	gcc -o $(APP) $(SRC)

clean:
	rm -f *.dtbo *.o $(APP)
	sudo rm -f /boot/dtbs/$(shell uname -r)/overlays/$(DTS_NAME).dtbo

dtbo:
	cpp -nostdinc -I $(KER_PATH)/include $(DTS_NAME).dts | dtc -O dtb -o $(DTS_NAME).dtbo -b 0 -@
	sudo cp -f $(DTS_NAME).dtbo /boot/dtbs/$(shell uname -r)/overlays/
	sudo nano /boot/uEnv.txt