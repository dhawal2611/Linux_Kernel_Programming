#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#define CDGPIO 529

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple gpio driver for setting a LED and reading a button");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_gpio_driver"
#define DRIVER_CLASS "MyModuleClass"

/**
 * @brief Read data out of the buffer
 */
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	char tmp[3] = " \n";

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(tmp));

	/* Read value of button */
	printk("Value of button: %d\n", gpio_get_value(CDGPIO));
	tmp[0] = gpio_get_value(CDGPIO) + '0';

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, &tmp, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/*static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	uint8_t gpio_state = 0;

	//reading GPIO value
	gpio_state = gpio_get_value(GPIO_21);

	//write to user
	len = 1;
	//strcpy(KernelBuffer, "This is Dhawal Lad");
	if( copy_to_user(buf, &gpio_state, len) > 0) {
		pr_err("ERROR: Not all the bytes have been copied to user\n");
	}
	//copy_to_user(buf, KernelBuffer, MEM_SIZE);
	pr_info("Read function data : = %d \n", gpio_state);

	return MEM_SIZE;
}
*/

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	uint8_t rec_buf[10] = {0};

	if(copy_from_user(rec_buf, user_buffer, count) > 0) {
		pr_err("ERROR: Not all the bytes have been copied from user\n");
	}

	pr_info("Write Function : GPIO_21 Set = %c\n", rec_buf[0]);

	if (rec_buf[0]=='1') {
		//set the GPIO value to HIGH
		gpio_set_value(CDGPIO, 1);
	} else if (rec_buf[0]=='0') {
		//set the GPIO value to LOW
		gpio_set_value(CDGPIO, 0);
	} else {
		pr_err("Unknown command : Please provide either 1 or 0 \n");
	}

	return count;
	
	/*int to_copy, not_copied, delta;
	char value;

	// Get amount of data to copy 
	to_copy = min(count, sizeof(value));

	// Copy data to user 
	not_copied = copy_from_user(&value, user_buffer, to_copy);

	printk(KERN_INFO"Value of data is %c", value);

	// Setting the LED 
	switch(value) {
		case '0':
			gpio_set_value(CDGPIO, 0);
			break;
		case '1':
			gpio_set_value(CDGPIO, 1);
			break;
		default:
			printk("Invalid Input!\n");
			break;
	}

	// Calculate data 
	delta = to_copy - not_copied;

	return delta;*/
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("Hello, Kernel!\n");

	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

	/* Create device class */
	if((my_class = class_create(DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	/* GPIO 4 init */
	/*if(gpio_request(4, "rpi-gpio-4")) {
		printk("Can not allocate GPIO 4\n");
		goto AddError;
	}*/

	/* Set GPIO 4 direction */
/*	if(gpio_direction_output(4, 0)) {
		printk("Can not set GPIO 4 to output!\n");
		goto Gpio4Error;
	}

*/	/* GPIO 17 init */
	if(gpio_request(529, "rpi-gpio-17")) {
		printk("Can not allocate GPIO 17\n");
		goto AddError;
	}

	/* Set GPIO 17 direction */
	if(gpio_direction_output(CDGPIO, 0)) {
		printk("Can not set GPIO 17 to input!\n");
		goto Gpio17Error;
	}
	printk("Hello, Kernel! INIT Done! \n");


	return 0;
Gpio17Error:
	gpio_free(CDGPIO);
//Gpio4Error:
	//gpio_free(4);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	//gpio_set_value(4, 0);
	gpio_free(CDGPIO);
	//gpio_free(4);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);


