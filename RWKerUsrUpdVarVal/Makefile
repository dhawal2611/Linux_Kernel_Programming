obj-m += RWKerUsrUpdVarVal.o
 
KDIR := /lib/modules/$(shell uname -r)/build
#KDIR := /workspaces/Linux_Kernel_Programming/linux/
#KDIR := $(shell pwd)
PWD := $(shell pwd)
 
all:
	$(MAKE) -C $(KDIR)  M=$(PWD) modules
install:
	$(MAKE) -C $(KDIR)  M=$(PWD) module_install
clean:
	$(MAKE) -C $(KDIR)  M=$(PWD) clean
