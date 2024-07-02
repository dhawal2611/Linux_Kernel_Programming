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
#include <linux/ioctl.h>

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
	//strcpy(KernelBuffer, "This is Dhawal Lad");
	if( copy_to_user(buf, KernelBuffer, MEM_SIZE) > 0) {
		pr_err("ERROR: Not all the bytes have been copied to user\n");
	}
	//copy_to_user(buf, KernelBuffer, MEM_SIZE);
	pr_info("Read function data : = %s \n", KernelBuffer);

	return MEM_SIZE;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	int ret = 0;
	if( copy_from_user(KernelBuffer, buf, len) != 0) {
		pr_err("ERROR: Not all the bytes have been copied from user\n");
	}
	//copy_from_user(KernelBuffer, buf, len);
	pr_info("Write Function Data : = %s\n", KernelBuffer);
	//i32Val = atoi(KernelBuffer);
	ret = kstrtoint(KernelBuffer, 10, &i32Val);  // Convert string to int (base 10)
	if (ret < 0) {
		printk(KERN_ERR "Failed to convert string to int\n");
		return ret;
	}
	pr_info("Write Function Data : = %d\n", i32Val);

	return len;
}

static long lCharIOCTL(struct file *file, unsigned int cmd, unsigned long arg) {
	switch(cmd) {
		case WR_DATA:
			if(copy_from_user(&i32Val, arg, sizeof(i32Val)) != 0) {
				pr_err("ERROR: Not all the bytes have been copied from user\n");
			}
			pr_info("Valus is: %d\n", i32Val);
			break;
		case RD_DATA:
			if(copy_to_user((int32_t *) arg, &i32Val, sizeof(i32Val)) > 0) {
				pr_err("ERROR: Not all the bytes have been copied to user\n");
			}
			break;
		default:
			pr_err("Invalid Option selected\n");
			break;
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

	pr_info("Device Driver Insert...Done!!!\n");
	return 0;

r_device:
	device_destroy(dev_class,dev);
r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}

static void __exit my_driver_exit(void)
{
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
