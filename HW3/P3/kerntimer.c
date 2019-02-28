/******************************************************************************************
 *@file: kerntimer.c
 *@brief: Creates a kernel module that uses a kernel timer to wake up every 500msec and
 *	  print some informtion to the kernel log buffers.
 *@tools_used: Compiler: arm-linux-gcc. Program run on BeagleBone Green
 *@author: Devansh Mittal
 *@date: 02/16/2019
 *@version: 1.0
*******************************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>

static char * name;
static int count = 0;
struct timer_list my_timer;

module_param(name,charp,0644); //Parameter that accepts name from the user
module_param(count,int,0644); //Parameter that accepts timer count time in seconds 

/* Callback function called once the kernel timer expires */
void wakeup_callback(struct timer_list *my_t)
{
	static int no = 1;
	if(no <= (count*2)) //Execute the timer and the printing until period specified by user
	{
		/* Set expiration time to 500msec + jiffies, to offset from current time */
		mod_timer(&my_timer, jiffies + msecs_to_jiffies(500)); 
		printk("In callback function: Name: %s, count: %d", name, no);
		no++; //counts number of times the timer has fired
	}
}

/* Entry point of module. Invoked when module is loaded in kernel 
 * static used as function is typically not directly invoked by external code
 */
static int kerntimer_init(void)
{ 
	int ret;
	printk(KERN_ALERT "Entering Module");
	timer_setup(&my_timer, wakeup_callback, 0); //Initialize timer and set callback function

	/* Set expiration time to 500msec + jiffies, to offset from current time */
	ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(500));
	if(ret)
		printk(KERN_ERR "Error in mod_timer");	

	return 0;
}

/* Module exit point when module is removed from memory */
static void kerntimer_exit(void)
{
	int ret;
	ret = del_timer(&my_timer); //delete timer
	if(ret)
			printk(KERN_ERR "Timer still in use");
	printk(KERN_ALERT "Exiting Module");
}

module_init(kerntimer_init); //registers module entry point
module_exit(kerntimer_exit); //registers module exit point

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Devansh Mittal");
MODULE_DESCRIPTION("Problem 3 of AESD HW3");


