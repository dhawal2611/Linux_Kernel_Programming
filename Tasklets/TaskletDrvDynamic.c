#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include<linux/slab.h>

#define IRQ_NUM 1
int i = 0;

// The tasklet function
//static void tasklet_function(struct tasklet_struct *t)
static void tasklet_function(unsigned long data)
{
    printk(KERN_INFO "Tasklet function executed. %ld\n", data);
}

static struct tasklet_struct *my_tasklet;

static irqreturn_t my_interrupt_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Interrupt occured and calling tasklet %d\n", i++);
    // Schedule the tasklet
    tasklet_schedule(my_tasklet);
    return IRQ_HANDLED;
}

static int __init my_module_init(void)
{
    int ret;

    // Allocate and initialize the tasklet
    my_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
    if (!my_tasklet) {
        printk(KERN_ERR "Failed to allocate memory for tasklet.\n");
        return -ENOMEM;
    }

    tasklet_init(my_tasklet, tasklet_function, 0);

    // Request an IRQ line and register the interrupt handler
    ret = request_irq(IRQ_NUM, my_interrupt_handler, IRQF_SHARED, "my_interrupt_handler", (void *)(my_interrupt_handler));
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ %d\n", IRQ_NUM);
        return ret;
    }

    printk(KERN_INFO "Module loaded with enabling interrupt for keyboard\n");
    return 0;
}

static void __exit my_module_exit(void)
{
    // Kill the tasklet
    tasklet_kill(my_tasklet);

    // Free the tasklet memory
    kfree(my_tasklet);

    // Free the IRQ line
    free_irq(IRQ_NUM, (void *)(my_interrupt_handler));

    printk(KERN_INFO "Module unloaded and interrupt handler unregistered.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lad Dhawal Umesh <dhawal261195@gmail.com>");
MODULE_DESCRIPTION("Dynamic Tasklet Example");
MODULE_VERSION("1.0");

