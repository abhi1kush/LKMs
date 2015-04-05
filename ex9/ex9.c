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
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/init.h>

static int foo;
static int baz;
static int bar;
static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,char *buf)
{
  return sprintf(buf, "%d\n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
  sscanf(buf, "%du", &foo);
  return count;
}

/* Sysfs attributes cannot be world-writable so set permisson */
static struct kobj_attribute foo_attribute =
  __ATTR(foo, 0664, foo_show, foo_store);

static ssize_t b_show(struct kobject *kobj, struct kobj_attribute *attr,char *buf)
{
    int var;

    if (strcmp(attr->attr.name, "baz") == 0)
      var = baz;
    else
      var = bar;
    return sprintf(buf, "%d\n", var);
}

static ssize_t b_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
    int var;

    sscanf(buf, "%du", &var);
    if (strcmp(attr->attr.name, "baz") == 0)
      baz = var;
    else
      bar = var;
    return count;
}

static struct kobj_attribute baz_attribute =
  __ATTR(baz, 0664, b_show, b_store);
static struct kobj_attribute bar_attribute =
  __ATTR(baz, 0664, b_show, b_store);

static struct attribute *attrs[] = {
    &foo_attribute.attr,
    &baz_attribute.attr,
    &bar_attribute.attr,
    NULL, /* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *example_kobj;

static int init(void)
{
  int retval;
  example_kobj = kobject_create_and_add("kobject_example", kernel_kobj);
  if (!example_kobj)
    return -ENOMEM; //out of memory error

  /* Create the files associated with this kobject */
  retval = sysfs_create_group(example_kobj, &attr_group);
  if (retval)
    kobject_put(example_kobj);
  return retval;

  printk(KERN_INFO "inside the %s function\n", __FUNCTION__);
  return 0;
}

static void  cleanup(void)
{
  kobject_put(example_kobj);
  printk(KERN_INFO "lkm removed\n");
}

module_init(init);
module_exit(cleanup);
MODULE_AUTHOR("Abhishek Singh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel tweaks");
