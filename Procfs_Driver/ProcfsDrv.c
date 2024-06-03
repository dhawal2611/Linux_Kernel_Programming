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
#include <linux/proc_fs.h>

char cDataForProc[1024] = "Hello From Kernel space. This is Proc Entry";

static int len = 1;
static struct proc_dir_entry *parent;


static int __init ProcDriverInit(void);
static void __exit ProcDriverExit(void);

/*************** Proc Driver functions **********************/
static int ProcOpen(struct inode *inode, struct file *file);
static int ProcRelease(struct inode *inode, struct file *file);
static ssize_t ProcRead(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t ProcWrite(struct file *filp, const char *buf, size_t len, loff_t * off);
/******************************************************/

static struct proc_ops Procfops =
{
	.proc_open 		= ProcOpen,
    .proc_read 		= ProcRead,
    .proc_write 	= ProcWrite,
    .proc_release 	= ProcRelease,
};

static int ProcOpen(struct inode *inode, struct file *file)
{
    pr_info("proc file opend.....\t");
    return 0;
}

static int ProcRelease(struct inode *inode, struct file *file)
{
    pr_info("proc file released.....\n");
    return 0;
}

static ssize_t ProcRead(struct file *filp, char __user *buffer, size_t length,loff_t * offset)
{
    pr_info("proc file read.....\n");
    if(len) {
        len=0;
    } else {
        len=1;
        return 0;
    }
    
    if(copy_to_user(buffer,cDataForProc,20)) {
        pr_err("Data Send : Err!\n");
    }
 
    return length;;
}

static ssize_t ProcWrite(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    pr_info("proc file wrote.....\n");
    
    if(copy_from_user(cDataForProc,buff,len)) {
        pr_err("Data Write : Err!\n");
    }
    
    return len;
}

static int __init ProcDriverInit(void)
{
	/*Create proc directory. It will create a directory under "/proc" */
    parent = proc_mkdir("ext",NULL);
        
    if( parent == NULL ) {
		pr_info("Error creating proc entry");
		return 0;
	}
        
	/*Creating Proc entry ur_devicender "/proc/etx/" */
    proc_create("ProcInt", 0666, parent, &Procfops);
	/*Creating Proc entry witout extra folder "/proc/" */
	//proc_create("ProcInt", 0666, NULL, &Procfops);

	pr_info("Device Driver Inserted\n");
	return 0;

}

static void __exit ProcDriverExit(void)
{
	remove_proc_entry("ProcInt", NULL);
	pr_info("Device Driver Remove...Done!!\n");
}

module_init(ProcDriverInit);
module_exit(ProcDriverExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lad Dhawal Umesh <dhawal261195@gmail.com>");
MODULE_DESCRIPTION("A simple Proc driver");
