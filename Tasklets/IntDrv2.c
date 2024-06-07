#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/io.h>

#define KEYBOARD_IRQ 1
#define KEYBOARD_DATA_PORT 0x60

static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    // Read the scancode from the keyboard data port
    scancode = inb(KEYBOARD_DATA_PORT);

    printk(KERN_INFO "Keyboard Interrupt: Scancode %x\n", scancode);

    return IRQ_HANDLED;
}

static int __init keyboard_interrupt_init(void)
{
    int result;

    // Request the IRQ for the keyboard (IRQ 1)
    result = request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler, IRQF_SHARED, "keyboard_interrupt", (void *)(keyboard_interrupt_handler));
    if (result) {
        printk(KERN_ERR "keyboard_interrupt: Can't allocate IRQ %d\n", KEYBOARD_IRQ);
        return result;
    }

    printk(KERN_INFO "keyboard_interrupt: Module loaded\n");
    return 0;
}

static void __exit keyboard_interrupt_exit(void)
{
    // Free the IRQ
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    printk(KERN_INFO "keyboard_interrupt: Module unloaded\n");
}

module_init(keyboard_interrupt_init);
module_exit(keyboard_interrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Custom Keyboard Interrupt Handler");
