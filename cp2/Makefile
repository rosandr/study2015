ifneq ($(KERNELRELEASE),)
-include Kbuild
else
#KDIR := /lib/modules/`uname -r`/build
KDIR := /home/study/kernel/linux-4.3
PWD  := $(shell pwd)

default:
	$(MAKE) -C $(KDIR)  M=$(PWD)

mknod:
	sudo mknod -m 666 /dev/kbuf c 60 1

ls:
	ls -l /dev/kbuf

insert:
	`sudo insmod netdev.ko`

remove:
	`sudo rmmod netdev.ko`

clean:
#	rm *.o *.ko *.mod.c *odule*
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *odule*

# Module specific targets
genbin:
	echo "X" > 8123_bin.o_shipped

endif