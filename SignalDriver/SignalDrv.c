#include <linux/ioctl.h>
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

#define SIGETX 44

#define REG_CURRENT_TASK _IOW('a','a',int32_t*)


/* Signaling to Application */
static struct task_struct *task = NULL;
static int signum = 0;

#define IRQ_NO 11

unsigned int i = 0;

//Interrupt handler for IRQ 1.
static irqreturn_t irq_handler(int irq,void *dev_id, struct pt_regs *regs) {
	struct kernel_siginfo info;
	printk(KERN_INFO "Interrupt Occurred");

	//Sending signal to app
	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = SIGETX;
	info.si_code = SI_QUEUE;
	info.si_int = 1;

	if (task != NULL) {
		printk(KERN_INFO "Sending signal to app\n");
		if(send_sig_info(SIGETX, &info, task) < 0) {
			printk(KERN_INFO "Unable to send signal\n");
		}
	}
	return IRQ_HANDLED;
}

#define MEM_SIZE 1024

// IOCTL commands
#define WR_DATA _IOW('a', 'a', int32_t*)
#define RD_DATA _IOR('a', 'b', int32_t*)

int32_t i32Val = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_cdev;
uint8_t *KernelBuffer;

static int __init my_driver_init(void);
static void __exit my_driver_exit(void);

/*************** Driver functions **********************/
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t my_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long lCharIOCTL(struct file *file, unsigned int cmd, unsigned long arg);
/******************************************************/

static struct file_operations fops =
{
	.owner          = THIS_MODULE,
	.read           = my_read,
	.write          = my_write,
	.open           = my_open,
	.release        = my_release,
	.unlocked_ioctl = lCharIOCTL,
};

static int my_open(struct inode *inode, struct file *file)
{
	if((KernelBuffer = kmalloc(MEM_SIZE, GFP_KERNEL)) == 0) {
		pr_err("Cannot allocate memory\n");
		return -1;
	}
	pr_info("Device File Opened...!!!\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	kfree(KernelBuffer);
	pr_info("Device File Closed...!!!\n");
	return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	strcpy(KernelBuffer, "This is Dhawal Lad");
	if( copy_to_user(buf, KernelBuffer, MEM_SIZE) > 0) {
		pr_err("ERROR: Not all the bytes have been copied to user\n");
	}
	//copy_to_user(buf, KernelBuffer, MEM_SIZE);
	pr_info("Read function data : = %s \n", KernelBuffer);

	return MEM_SIZE;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{

	if( copy_from_user(KernelBuffer, buf, len) != 0) {
		pr_err("ERROR: Not all the bytes have been copied from user\n");
	}
	//copy_from_user(KernelBuffer, buf, len);
	pr_info("Write Function Data : = %s\n", KernelBuffer);

	return len;
}

static long lCharIOCTL(struct file *file, unsigned int cmd, unsigned long arg) {
	if (cmd == REG_CURRENT_TASK) {
		printk(KERN_INFO "REG_CURRENT_TASK\n");
		task = get_current();
		signum = SIGETX;
	}
	return 0;
}

static int __init my_driver_init(void)
{
	/*Allocating Major number*/
	if((alloc_chrdev_region(&dev, 0, 1, "my_Dev")) <0){
		pr_err("Cannot allocate major number\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

	cdev_init(&my_cdev,&fops);

	if((cdev_add(&my_cdev,dev,1)) < 0){
		pr_err("Cannot add the device to the system\n");
		goto r_class;
	}

	if((dev_class = class_create("my_class")) == NULL){
		pr_err("Cannot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	if((device_create(dev_class,NULL,dev,NULL,"my_device")) == NULL){
		pr_err( "Cannot create the Device \n");
		goto r_device;
	}

	if (request_irq(IRQ_NO, (irq_handler_t)irq_handler, IRQF_SHARED, "my_device", (void *)(irq_handler)) == 0) {
		printk(KERN_INFO "Device Driver Insert...Done!!!\n");
		pr_info("Device Driver Insert...*******************Done!!!\n");
	}
	else
		printk(KERN_INFO "Device Driver Insert...Not done abdiubfdka!!!\n");


	pr_info("Device Driver Insert...Done!!!\n");
	return 0;

r_device:
	device_destroy(dev_class,dev);
r_class:
	unregister_chrdev_region(dev,1);\
		return -1;
}

static void __exit my_driver_exit(void)
{
	free_irq(IRQ_NO, (void *)(irq_handler));
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Device Driver Remove...Done!!\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lad Dhawal Umesh <dhawal261195@gmail.com>");
MODULE_DESCRIPTION("A simple device driver");
MODULE_VERSION("1.0");
