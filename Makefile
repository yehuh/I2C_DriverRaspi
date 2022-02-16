obj-m := myI2C.o

myled.ko: myI2C.c
	make -C /usr/src/linux-headers-$(shell uname -r) M=$(PWD) V=1 modules
clean:
	make -C /usr/src/linux-headers-$(shell uname -r) M=$(PWD) V=1 clean
