#include <linux/io.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/timer.h>
#include <asm/page.h>
#include <asm/current.h>
#include <linux/sched.h>

struct kobject *k_obj;
static struct kobj_attribute k_obj_attr =
        __ATTR(partition_id, S_IRUGO, partition_id_show, NULL);


static int init(void)
{
  printk(KERN_INFO "inside the %s function\n", __FUNCTION__);
  return 0;
}

static void  cleanup(void)
{
  ret = sysfs_create_file(k_obj, &k_obj_attr.attr);
	if (ret) 
  {
    printk(KERN_ALERT "sysfs_create_file k_obj_attr failed \n");
		return ret;
	}
  	printk(KERN_INFO "lkm removed\n");
}

module_init(init);
module_exit(cleanup);
MODULE_AUTHOR("Abhishek Singh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel tweaks");
