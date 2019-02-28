obj-m= kerntimer.o
LINUXDIR:= ~/Broot/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220/
all:
	make ARCH=arm CROSS_COMPILE=/home/devansh3/Broot/buildroot/output/host/usr/bin/arm-linux- -C $(LINUXDIR) M=$(shell pwd) modules

clean:
	make -C $(LINUXDIR) M=$(shell pwd) clean
