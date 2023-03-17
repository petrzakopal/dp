/**
 * Testing kernel module which works for now as follows.
 * In the device tree, there must be and entry with
 * compatible = "vendor,mymodule" (definiton in kernel module: #define DEVNAME "vendor,mymodule")
 * 
 * Tested on AXI Timer with Interrupt Enabled
 * 1. insert module to kernel via "insmod <name_of_module>"
 * 2. test if module was properly inserted and IRQ number assigned "dmesg | tail"
 * 3. open multiple ssh connections and watch changes
 *      1. run "watch -n 0.25 "cat /proc/interrupts"" to watch processor interrupts
 *      2. run "watch -n 0.25 "devmem <AXI_TIMER_CNTR_ADDRESS>+0x8"" to watch counter register, edit for your needs as you designed address in vivado
 *         for my testing purposes its "watch -n 0.25 "devmem 0x42800008""
 * 4. for the first run following command with starting addresses edited to your needs of timer address,
 *    ending addreses can be found in AXI Timer v2.0 LogiCORE IP Product Guide (PG079)
 *    (https://www.xilinx.com/content/dam/xilinx/support/documents/ip_documentation/axi_timer/v2_0/pg079-axi-timer.pdf)
 *    1. "devmem 0x42800000 w 0x1C0&&echo 0x1 > /dev/uio2&&devmem 0x42800004 w 0xAFFFFFFF&&devmem 0x42800000 w 0xE0&&devmem 0x42800008 w 0x0&&devmem 0x42800000 w 0xC0", note taht the command "echo 0x1 > dev/uio<YOUR NUMBER>" should not evoke incrementing number of interrupts
 * 5. watch the timer counter register change
 * 6. after time, there will be an interrupt in AXI TIMER, it will show in "/proc/interrupts", if not shown in "/proc/interrupts", it will be only counted when using command
 *    "echo 0x1 > /dev/uio<YOUR NUMBER>", where <YOUR NUMBER> depends on your uio devices and architecture, when testing on our kernel and DT the command was used as follows
 *     "echo 0x1 > /dev/uio2", but the command in step 4.1. should manage handling/enabling interrupt
 * 7. The number of interrupts in "/proc/interrupts" will be incremented automatically when using propper command (from section 4.1.).
 * 
 * TODO: Polish code and better understand it. Change name showing in "cat /proc/interrupts"
*/



#include <linux/module.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/uio_driver.h>
#include <linux/interrupt.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h> 

static struct uio_info *info;
static struct device *dev;
static int irq = 6;
module_param(irq, int, S_IRUGO);

static void my_release(struct device *dev)
{
	pr_info("releasing my uio device\n");
}

static int uio_pdrv_genirq_irqcontrol(struct uio_info *info, s32 irq_on)
{
	

	
	if (irq_on) {
		
			enable_irq(info->irq);
	} else {
		
			disable_irq_nosync(info->irq);
	}
	

	return 0;
}

static irqreturn_t my_handler(int irq, struct uio_info *dev_info)
{
//	static int count = 0;
//	pr_info("In UIO handler, count=%d\n", ++count);
	disable_irq_nosync(irq);
	printk("Interrupt happend, disabled irq.");
	// enable_irq(irq);
	return IRQ_HANDLED;
}

#define DEVNAME "vendor,mymodule"
static int __init my_init(void)
{
	struct device_node * np = NULL;
	
	/* set IRQ */
	np = of_find_compatible_node(NULL,NULL,DEVNAME);
	if (np == NULL)
	{
		printk (KERN_INFO "node %s is not defined in DTS\n",DEVNAME );

		return 0;
	}
	irq  = irq_of_parse_and_map(np, 0); /* get IRQ # from device tree */


	dev = kzalloc(sizeof(struct device), GFP_KERNEL);
	
	
	dev_set_name(dev,DEVNAME);
	dev->release = my_release;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result" 
	(void)device_register(dev);
#pragma GCC diagnostic pop 

	info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
	info->name = DEVNAME;
	info->version = "0.0.1";
	info->irq = irq;
	info->irq_flags = IRQF_SHARED;
	info->handler = my_handler;
	info->irqcontrol = uio_pdrv_genirq_irqcontrol;



	if (uio_register_device(dev, info) < 0) {
		device_unregister(dev);
		kfree(dev);
		kfree(info);
		pr_info("Failing to register uio device\n");
		return -1;
	}
	
	pr_info("Registered UIO handler for IRQ=%d\n", irq);
	return 0;
}

static void __exit my_exit(void)
{
	uio_unregister_device(info);
	device_unregister(dev);
	pr_info("Un-Registered UIO handler for IRQ=%d\n", irq);
	kfree(info);
	kfree(dev);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Yair Gadelov");
MODULE_DESCRIPTION("axi gpio for zynq");
MODULE_LICENSE("GPL v2");