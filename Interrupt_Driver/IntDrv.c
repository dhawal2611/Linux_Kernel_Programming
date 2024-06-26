#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include<linux/sysfs.h> 
#include<linux/kobject.h> 
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/err.h>
#define IRQ_NO 1

unsigned int i = 0;

//Interrupt handler for IRQ 1.
static irqreturn_t irq_handler(int irq,void *dev_id, struct pt_regs *regs) {
	printk(KERN_ERR "Shared IRQ: Interrupt Occurred %d\n", i++);
	return IRQ_HANDLED;
}

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

static int __init etx_driver_init(void)
{

	if (request_irq(1, (irq_handler_t)irq_handler, IRQF_SHARED, "etx_device", (void *)(irq_handler)) == 0) {
		printk(KERN_INFO "Device Driver Insert...Done!!!\n");
		pr_info("Device Driver Insert...*******************Done!!!\n");
	}
	else
		printk(KERN_INFO "Device Driver Insert...Not done abdiubfdka!!!\n");
	return 0;
}

static void __exit etx_driver_exit(void)
{
	free_irq(1,(void *)(irq_handler));
	printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lad Dhawal Umesh <dhawal261195@gmail.com>");
MODULE_DESCRIPTION("A simple device driver");
MODULE_VERSION("1.0");
