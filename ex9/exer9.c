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

static int finput;
static int foutput;


static ssize_t f_show(struct kobject *kobj, struct kobj_attribute *attr,char *buf)
{
    int var;

    if (strcmp(attr->attr.name, "finput") == 0)
      var = finput;
    else
      var = foutput;
    return sprintf(buf, "%d\n", var);
}

static ssize_t f_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
    int var;

    sscanf(buf, "%du", &var);
    if (strcmp(attr->attr.name, "finput") == 0)
      finput = var;
    else
      foutput = var;
    return count;
}

static struct kobj_attribute finput_attribute =
  __ATTR(finput, 0664, f_show, f_store);
static struct kobj_attribute foutput_attribute =
  __ATTR(foutput, 0664, f_show, f_store);

static struct attribute *attrs[] = {
    &finput_attribute.attr,
    &foutput_attribute.attr,
    NULL, /* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

int fs_details(char *filename,int pid)
{
  int i =0;
  struct inode *ind;
  struct task_struct *ts;
  struct fdtable *fdt;
  struct file * f;
  struct path files_path;
  char *cwd;
  char *buf = (char *)kmalloc(GFP_KERNEL,100*sizeof(char));
  
  ts = pid_task(find_vpid((pid_t)pid), PIDTYPE_PID);
  rcu_read_lock();
  fdt = files_fdtable(ts->file);
  while(fdt->fd[i] !=NULL)
  {
    files_path = fdt->fd[i]->f_path;
    cwd= d_path(&files_path,buf,100*sizeof(char));
    printk(KERN_INFO"file fd %d %s",i,cwd);
    i++;
  }
  ind = ts->files->dentry->d_inode;
  printk(KERN_INFO"inode no: %ld",ind->i_ino);
}


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
