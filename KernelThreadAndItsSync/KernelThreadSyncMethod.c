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
#include <linux/mutex.h>


//Static method to initialize the read write spinlock
static DEFINE_RWLOCK(etx_rwlock); 

//Dynamic method to initialize the read write spinlock
//rwlock_t etx_rwlock;

unsigned long etx_global_variable = 0;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

static struct task_struct *etx1_thread, *etx2_thread;

int thread1_function(void *pv);
int thread2_function(void *pv);

// Thread1 Function
int thread1_function(void *pv)
{
	while(!kthread_should_stop()) {  
		write_lock(&etx_rwlock);
		etx_global_variable++;
		pr_info("In Thread Function1: Read value %lu\n", etx_global_variable);
		write_unlock(&etx_rwlock);
		msleep(1000);
	}
	return 0;
}

// Thread2 Function
int thread2_function(void *pv)
{
	while(!kthread_should_stop()) {
		read_lock(&etx_rwlock);
		pr_info("In Thread Function2: Read value %lu\n", etx_global_variable);
		read_unlock(&etx_rwlock);
		msleep(1000);
	}
	return 0;
}

static int __init etx_driver_init(void)
{
	// Thread 1
	etx1_thread = kthread_run(thread1_function,NULL,"eTx Thread1");
	if(etx1_thread) {
		pr_info("Kthread1 Created Successfully...\n");
	} else {
		pr_err("Cannot create kthread1\n");
		goto r_device;
	}
	// Thread 2
	etx2_thread = kthread_run(thread2_function,NULL,"eTx Thread2");
	if(etx2_thread) {
		pr_info("Kthread2 Created Successfully...\n");
	} else {
		pr_err("Cannot create kthread2\n");
		goto r_device;
	}
	//Dynamic method to initialize the read write spinlock
        //rwlock_init(&etx_rwlock);
	pr_info("Device Driver Insert...Done!!!\n");
	return 0;


r_device:
	return -1;
}

static void __exit etx_driver_exit(void)
{
	kthread_stop(etx1_thread);
	kthread_stop(etx2_thread);
	pr_info("Device Driver Remove...Done!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lad Dhawal Umesh <dhawal261195@gmail.com>");
MODULE_DESCRIPTION("Kernel Thread Example");
MODULE_VERSION("1.0");
