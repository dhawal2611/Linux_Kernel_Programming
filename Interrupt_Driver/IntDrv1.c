#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/err.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#include<linux/kobject.h> 

#define MEM_SIZE 1024
#define IRQ_NO 11
unsigned int i = 0;

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
/******************************************************/

static struct file_operations fops =
{
	.owner          = THIS_MODULE,
	.read           = my_read,
	.write          = my_write,
	.open           = my_open,
	.release        = my_release,
};

static irqreturn_t irq_handler(int irq,void *dev_id) {
	pr_info("Interrupt occured...%d!!!\n", i);
	//printk(KERN_INFO "Handler is called %d\n\n\n", i);
	i++;
	return IRQ_HANDLED;
}

static int my_open(struct inode *inode, struct file *file)
{
	pr_info("Device File Opened...!!!\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	pr_info("Device File Closed...!!!\n");
	return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	pr_info("Read function data\n");
	return 0;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	pr_info("Write Function Data\n");
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

	if((dev_class = class_create(THIS_MODULE,"my_class")) == NULL){
		pr_err("Cannot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	if((device_create(dev_class,NULL,dev,NULL,"my_device")) == NULL){
		pr_err( "Cannot create the Device \n");
		goto r_device;
	}

	if(request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "my_device", (void *)(irq_handler))) {
		pr_err( "Cannot Init Interrupt\n");
		goto free_IRQ;
	}

	pr_info("Device Driver Insert...Done!!!\n");
	return 0;

free_IRQ:
	free_irq(IRQ_NO, (void *)(irq_handler));
r_device:
	device_destroy(dev_class,dev);
r_class:
	unregister_chrdev_region(dev,1);
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

