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

char filename[50];
int pid=-1;
char foutput_buffer[500];

static ssize_t filename_show(struct kobject *kobj, struct kobj_attribute *attr,char *buf)
{
      return sprintf(buf, "%s\n", filename);
}

static ssize_t filename_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
      sscanf(buf, "%s", filename);
      return count;
}

static ssize_t pid_show(struct kobject *kobj, struct kobj_attribute *attr,char *buf)
{
      return sprintf(buf, "%d\n",pid);
}
static ssize_t pid_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
      sscanf(buf, "%d",&pid);
          return count;
}

static ssize_t foutput_show(struct kobject *kobj, struct kobj_attribute *attr,char *buf)
{
	return sprintf(buf,"%s\n",foutput_buffer);
}

static ssize_t foutput_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
      sscanf(buf, "%s", filename);
      return count;
}

static struct kobj_attribute pid_attribute =
  __ATTR(pid, 0664, pid_show, pid_store);
static struct kobj_attribute filename_attribute =
  __ATTR(filename, 0664, filename_show, filename_store);

static struct kobj_attribute foutput_attribute =
  __ATTR(output, 0664, foutput_show, foutput_store);


static struct attribute *fo_attrs[] = {
    &foutput_attribute.attr,
    NULL, /* need to NULL terminate the list of attributes */
};

static struct attribute *attrs[] = {
    &filename_attribute.attr,
    &pid_attribute.attr,
    NULL, /* need to NULL terminate the list of attributes */
};
static struct attribute_group fo_attr_group = {
    .attrs = fo_attrs,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *finput;
static struct kobject *foutput;

/********************************************************************/

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
  fdt = files_fdtable(ts->files); //ts->files->fdt;
  while(fdt->fd[i] !=NULL)
  {
   
    if(strcmp( fdt->fd[i]->f_path.dentry->d_name.name,filename)==0)
    {
      files_path = f.f_path;
      cwd= d_path(&files_path,buf,100*sizeof(char));
      printk(KERN_INFO"file fd %d %s",i,cwd);
    }
    i++;
  }
  //ind = ts->files->dentry->d_inode;
  printk(KERN_INFO"inode no: %ld",ind->i_ino);
  //fs_details(filename,pid)
  rcu_read_unlock();
  return 0;
}
/***********************************************************************/

static int init(void)
{
  int retval;
  finput = kobject_create_and_add("finput", kernel_kobj);
  if (!finput)
    return -ENOMEM; //out of memory error
  retval = sysfs_create_group(finput, &attr_group);
  if (retval)
    kobject_put(finput);
   foutput = kobject_create_and_add("foutput", kernel_kobj);
  if (!foutput)
    return -ENOMEM; //out of memory error
  retval = sysfs_create_group(foutput, &fo_attr_group);
  if (retval)
    kobject_put(foutput);
  while(pid == -1)
	msleep(1000);
  if(pid != -1)
  	fs_details(filename,pid);
  printk(KERN_INFO "inside the %s function\n", __FUNCTION__);
  return 0;
}

static void  cleanup(void)
{
  kobject_put(finput);
  kobject_put(foutput);
  printk(KERN_INFO "lkm removed\n");
}

module_init(init);
module_exit(cleanup);
MODULE_AUTHOR("Abhishek Singh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel tweaks");
