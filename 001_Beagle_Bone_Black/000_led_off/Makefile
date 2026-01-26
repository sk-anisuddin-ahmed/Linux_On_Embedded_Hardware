DTS_NAME = BBB_LED_OFF

all: clean dtbo

dtbo:
	cpp -nostdinc -I $(KER_PATH)/include $(DTS_NAME).dts | dtc -O dtb -o $(DTS_NAME).dtbo -b 0 -@
	sudo cp -f $(DTS_NAME).dtbo /boot/dtbs/$(shell uname -r)/overlays/
	
clean:
	rm -f *.dtbo
	sudo rm -f /boot/dtbs/$(shell uname -r)/overlays/$(DTS_NAME).dtbo