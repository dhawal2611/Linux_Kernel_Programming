#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include<linux/slab.h>

#define IRQ_NUM 1
int i = 0;


/* Work structure */
static struct work_struct workqueue;

void workqueue_fn(struct work_struct *work); 

/*Workqueue Function*/
void workqueue_fn(struct work_struct *work)
{
	printk(KERN_INFO "Executing Workqueue Function\n");
}

static irqreturn_t my_interrupt_handler(int irq, void *dev_id)
{
	printk(KERN_INFO "Interrupt occured and calling Workqueue %d\n", i++);
	// Schedule the tasklet
	schedule_work(&workqueue);
	return IRQ_HANDLED;
}

static int __init my_module_init(void)
{
	int ret;

	// Request an IRQ line and register the interrupt handler
	ret = request_irq(IRQ_NUM, my_interrupt_handler, IRQF_SHARED, "my_interrupt_handler", (void *)(my_interrupt_handler));
	if (ret) {
		printk(KERN_ERR "Failed to request IRQ %d\n", IRQ_NUM);
		return ret;
	}
	/*Creating work by Dynamic Method */
	INIT_WORK(&workqueue,workqueue_fn);
	printk(KERN_INFO "Module loaded with enabling interrupt for keyboard\n");
	return 0;
}

static void __exit my_module_exit(void)
{
	// Free the IRQ line
	free_irq(IRQ_NUM, (void *)(my_interrupt_handler));

	printk(KERN_INFO "Module unloaded and interrupt handler unregistered.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lad Dhawal Umesh <dhawal261195@gmail.com>");
MODULE_DESCRIPTION("Dynamic Workqueue Example");
MODULE_VERSION("1.0");

