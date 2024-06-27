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
#include <linux/workqueue.h>            // Required for workqueues
#include <linux/err.h>
 
#define IRQ_NO 1
 
static struct workqueue_struct *own_workqueue;
 
static void workqueue_fn(struct work_struct *work); 
 
static DECLARE_WORK(work, workqueue_fn);
 
 
/*Workqueue Function*/
static void workqueue_fn(struct work_struct *work)
{
    printk(KERN_INFO "Executing Workqueue Function\n");
    return;
        
}
 
static irqreturn_t irq_handler(int irq,void *dev_id) {
        printk(KERN_INFO "Shared IRQ: Interrupt Occurred\n");
        /*Allocating work to queue*/
        queue_work(own_workqueue, &work);
        
        return IRQ_HANDLED;
}
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
 
static int __init etx_driver_init(void)
{
        if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "etx_device", (void *)(irq_handler))) {
            printk(KERN_INFO "my_device: cannot register IRQ \n");
                    goto irq;
        }
 
        /*Creating workqueue */
        own_workqueue = create_workqueue("own_wq");
        
        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
        return 0;
 
irq:
        free_irq(IRQ_NO,(void *)(irq_handler));
 
        return -1;
}

/*
** Module exit function
*/ 
static void __exit etx_driver_exit(void)
{
        /* Delete workqueue */
        destroy_workqueue(own_workqueue);
        free_irq(IRQ_NO,(void *)(irq_handler));
        printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lad Dhawal Umesh <dhawal261195@gmail.com>");
MODULE_DESCRIPTION("Simple Workqueue");
MODULE_VERSION("1.0");
