#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/interrupt.h>

/* Standard module information, edit as appropriate */
MODULE_LICENSE("GPL");
MODULE_AUTHOR
    ("Petr Zakopal");
MODULE_DESCRIPTION
    ("mymodule - just test of a linux kernel module");

#define DRIVER_NAME "mymodule"
#define GPIO_REQUEST_NUMBER 980
#define GPIO_NAME_IDENTIFICATION "zybo-gpio-980"


/* Global Variables */

/**
 * @brief Variable contains pin number of interrupt controller to which GPIO is mapped to.
*/
unsigned int irq_number;


/**
 * @brief Interrupt service routine is called when interrupt is triggered.
 * 
*/
static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
    printk("gpio_irq: Interrupt was triggered and ISR was called\n");
    return (irq_handler_t) IRQ_HANDLED;
}


/**
 * @brief This function is called when the module is loaded in the kernel.
 * 
*/

static int __init mymodule_init(void)
{
	printk("<1>gpio_irq: Loading moduule... \n");
	
    /* Setup the gpio */
    if(gpio_request(GPIO_REQUEST_NUMBER,GPIO_NAME_IDENTIFICATION))
    {
        printk("Cannot allocate GPIO %d.",GPIO_REQUEST_NUMBER);
        return (-1);
    }

    /* Set GPIO direction */
    if(gpio_direction_input(GPIO_REQUEST_NUMBER))
    {
        printk("Error!\nCan not set GPIO %d to input!\n",GPIO_REQUEST_NUMBER);
        gpio_free(GPIO_REQUEST_NUMBER);
        return (-1);
    }

    /* Setup the interrupt to get the number of PIN of interrupt */
    irq_number = gpio_to_irq(GPIO_REQUEST_NUMBER);



    /* Map interrupt to ISR */
    if(request_irq(irq_number, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0)
    {
        printk("Error!\n Can not request interrupt number %d\n", irq_number);
        gpio_free(GPIO_REQUEST_NUMBER);
        return (-1);
    }

    printk("Done!\n");
    printk("GPIO %d is mapped to IRQ number: %d.\n", GPIO_REQUEST_NUMBER, irq_number);

    return 0;
}


static void __exit mymodule_exit(void)
{
	printk("gpio_irq: Unloading module... ");
    gpio_free(GPIO_REQUEST_NUMBER);
    free_irq(irq_number, NULL);
}



module_init(mymodule_init);
module_exit(mymodule_exit);
