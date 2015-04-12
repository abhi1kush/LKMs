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
#include <linux/dcache.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/path.h>


static int finput;
static int foutput;
char finput_buf[100];
char foutput_buf[100];
char filename[50];
int pid;

//module_param(pid,int,S_IRUGO);
//module_param(filename,charp,S_IRUGO);

static ssize_t finput_show(struct kobject *kobj, struct kobj_attribute *attr,char *buf)
{
    return sprintf(buf, "%s\n", finput_buf);
}

static ssize_t finput_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
    sscanf(buf, "%s", finput_buf);
    return count;
}

static ssize_t foutput_show(struct kobject *kobj, struct kobj_attribute *attr,char *buf)
{
    return sprintf(buf, "%s\n", foutput_buf);
}

static ssize_t foutput_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
    sscanf(buf, "%s", foutput_buf);
    return count;
}

static struct kobj_attribute finput_attribute =
  __ATTR(finput, 0664, finput_show, finput_store);
static struct kobj_attribute foutput_attribute =
  __ATTR(foutput, 0664, foutput_show, foutput_store);

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
  struct path files_path;
  struct file f;
  char *cwd;
  char *buf = (char *)kmalloc(GFP_KERNEL,100*sizeof(char));
  
  ts = pid_task(find_vpid((pid_t)pid), PIDTYPE_PID);
  rcu_read_lock();
  fdt = ts->files->fdt; //files_fdtable(ts->file);
  while(fdt->fd[i] !=NULL)
  {
    f=(*fdt->fd[i]);
    if(strcmp(f.f_path.dentry->d_name.name,filename)==0)
    {
      files_path = f.f_path;
      cwd= d_path(&files_path,buf,100*sizeof(char));
      printk(KERN_INFO"file fd %d %s",i,cwd);
    }
    i++;
  }
  //ind = ts->files->dentry->d_inode;
  printk(KERN_INFO"inode no: %ld",ind->i_ino);
  rcu_read_unlock();
  return 0;
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
  fs_details(filename,pid);
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
