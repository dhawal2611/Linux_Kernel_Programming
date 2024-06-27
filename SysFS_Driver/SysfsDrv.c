#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include<linux/sysfs.h> 
#include<linux/kobject.h> 
#include <linux/err.h>

volatile int etx_value = 0;

struct kobject *kobj_ref;

/*
 ** Function Prototypes
 */
static int      __init etx_driver_init(void);
static void     __exit etx_driver_exit(void);

/*************** Sysfs functions **********************/
static ssize_t  sysfs_show(struct kobject *kobj, 
		struct kobj_attribute *attr, char *buf);
static ssize_t  sysfs_store(struct kobject *kobj, 
		struct kobj_attribute *attr,const char *buf, size_t count);

struct kobj_attribute etx_attr = __ATTR(etx_value, 0660, sysfs_show, sysfs_store);

/*
 ** This function will be called when we read the sysfs file
 */
static ssize_t sysfs_show(struct kobject *kobj, 
		struct kobj_attribute *attr, char *buf)
{
	pr_info("Sysfs - Read!!!\n");
	return sprintf(buf, "%d", etx_value);
}

/*
 ** This function will be called when we write the sysfsfs file
 */
static ssize_t sysfs_store(struct kobject *kobj, 
		struct kobj_attribute *attr,const char *buf, size_t count)
{
	pr_info("Sysfs - Write!!!\n");
	sscanf(buf,"%d",&etx_value);
	return count;
}

static int __init etx_driver_init(void)
{
	/*Creating a directory in /sys/kernel/ */
	kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj);

	/*Creating sysfs file for etx_value*/
	if(sysfs_create_file(kobj_ref,&etx_attr.attr)){
		pr_err("Cannot create sysfs file......\n");
		goto r_sysfs;
	}
	pr_info("Device Driver Insert...Done!!!\n");
	return 0;

r_sysfs:
	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);

	return -1;
}

/*
 ** Module exit function
 */
static void __exit etx_driver_exit(void)
{
	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);
	pr_info("Device Driver Remove...Done!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lad Dhawal Umesh <dhawal261195@gmail.com>");
MODULE_DESCRIPTION("Simple sysfs Driver");
MODULE_VERSION("1.0");
