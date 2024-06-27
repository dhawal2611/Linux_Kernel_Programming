#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/err.h>
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
 
static struct task_struct *etx_thread;
 
int thread_function(void *pv);

// Thread Function
int thread_function(void *pv)
{
    int i=0;
    while(!kthread_should_stop()) {
        pr_info("In EmbeTronicX Thread Function %d\n", i++);
        msleep(1000);
    }
    return 0;
}

static int __init etx_driver_init(void)
{
#if 0
        etx_thread = kthread_create(thread_function,NULL,"eTx Thread");
        if(etx_thread) {
            wake_up_process(etx_thread);
        } else {
            pr_err("Cannot create kthread\n");
            goto r_device;
        }
#endif
#if 1
        // Another Method
        etx_thread = kthread_run(thread_function,NULL,"eTx Thread");
        if(etx_thread) {
            pr_info("Kthread Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread\n");
             goto r_device;
        }
#endif
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
 
r_device:
        return -1;
}

static void __exit etx_driver_exit(void)
{
        kthread_stop(etx_thread);
        pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lad Dhawal Umesh <dhawal261195@gmail.com>");
MODULE_DESCRIPTION("Kernel Thread Example");
MODULE_VERSION("1.0");
