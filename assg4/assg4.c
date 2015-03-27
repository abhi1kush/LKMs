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
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

extern int (*context_counter)(struct task_struct *,struct task_struct *,long,int);
extern int (* rq_size)(int,unsigned int,unsigned long);
int pid,prio;

//module_param(pid, int, 0);
//module_param(prio, int, 0);

static int sched_counter[4],per_pid_switch_counter[4],grqsize[4];
static int gcpu;
static unsigned int entryy,exitt,total;

/***********************proc file section start *************************************************************/
struct proc_dir_entry *assi4_proc_file;
struct proc_dir_entry *assi4_proc_file2;
int proc_open2(struct inode *, struct file *);
int proc_open(struct inode *, struct file *);
static int proc_readings(struct seq_file * , void * );
static int proc_readings2(struct seq_file *, void *);

struct file_operations proc_file_operations ={
	.owner = THIS_MODULE,
	.open = proc_open,
	.read = seq_read,
	.release = single_release
};

struct file_operations proc_file_operations2 ={
	.owner = THIS_MODULE,
	.open = proc_open2,
	.read = seq_read,
	.release = single_release
};

int proc_open(struct inode *pinode, struct file *pfile)
{
	return single_open(pfile,proc_readings,NULL);
}

int proc_open2(struct inode *pinode, struct file *pfile)
{
	return single_open(pfile,proc_readings2,NULL);
}

static int proc_readings(struct seq_file * sfile, void * any)
{
	seq_printf(sfile,"No of Reads: d\nNo of Writes: d");
	return 0;
}

static int proc_readings2(struct seq_file * sfile, void * any)
{
	seq_printf(sfile,"No of Reads: d\nNo of Writes: d\n");
	return 0;
}
/******************************** proc file section end **************************************************************/

/******************************* *hook section start ***************************************************************/
int hk_context_counter(struct task_struct *next,struct task_struct *prev,long tv_nsec,int cpu)
{
	sched_counter[cpu]++;
	if(next->pid == pid)
	{
		per_pid_switch_counter[cpu]++;
		entryy = tv_nsec;
	}
	else if(prev->pid == pid)
	{
		exitt= tv_nsec;
		total+= tv_nsec;
	}
	return 0;
}

int hk_rq_size(int cpu,unsigned int rqsize,unsigned long nr_switches)
{
	gcpu=cpu;
	grqsize[cpu]=rqsize;
	return 0;
}

/*************************************** hook section end ***************************************************/

/************************************* init(void) and cleanup section start ******************************/
static int init(void)
{
	assi4_proc_file = proc_create("my_proc_file",0,NULL,&proc_file_operations);
	if(assi4_proc_file == NULL)
	{
		proc_remove(assi4_proc_file);
		printk(KERN_ALERT"ERROR : failed to create /proc/my_proc_file \n");
		return -1;
	}
	assi4_proc_file2 = proc_create("my_proc_file2",0,NULL,&proc_file_operations2);
	if(assi4_proc_file2 == NULL)
	{
		proc_remove(assi4_proc_file2);
		printk(KERN_ALERT"ERROR : failed to create /proc/my_proc_file2 \n");
		return -1;
	}
	printk(KERN_INFO"proc entry created \n");
	context_counter = hk_context_counter;
	rq_size=hk_rq_size;  	
	printk(KERN_INFO "inside the %s function\n", __FUNCTION__);
  	return 0;
}

static void  cleanup(void)
{
  	rq_size=NULL;
  	context_counter=NULL;
	proc_remove(assi4_proc_file);
	proc_remove(assi4_proc_file2);
  	printk(KERN_INFO "lkm removed\n");
}
/*********************************************init() cleanup() section end *******************************/

module_init(init);
module_exit(cleanup);
MODULE_AUTHOR("Abhishek Singh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel tweaks");
