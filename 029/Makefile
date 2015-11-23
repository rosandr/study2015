ifneq ($(KERNELRELEASE),)
-include Kbuild
else
KDIR := /lib/modules/`uname -r`/build
#KDIR := /home/study/kernel/linux-4.3
PWD  := $(shell pwd)

default:
	$(MAKE) -C $(KDIR)  M=$(PWD)

insert:
	`sudo insmod print_hello.ko`
	`sudo insmod niitm_hello.ko`

remove:
	`sudo rmmod niitm_hello.ko`
	`sudo rmmod print_hello.ko`

clean:
	rm *.o *.ko *.mod.c *odule*


# Module specific targets
genbin:
	echo "X" > 8123_bin.o_shipped

endif