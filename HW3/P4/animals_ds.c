/******************************************************************************************
 *@file: animals_ds.c
 *@brief: Create a kernel module to initialize 2 lists. One list consists of all unique 
 *	  elements provided by user and the second list consists of filtered elements based 
 *        on users filter choices.
 *@tools_used: Compiler: arm-linux-gcc. Program run on BeagleBone Green
 *@author: Devansh Mittal
 *@date: 02/16/2019
 *@version: 1.1
*******************************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/list_sort.h>
#include <linux/errno.h>

#define MAX_ANIMALS 50 //Maximum animal types input by user

static char *animals[MAX_ANIMALS];
static int count = 0;
static char *animal_type = "NULL";
static int count_greater_than = 0;
int no_of_nodes2 = 0;
int no_of_nodes = 0;

module_param_array(animals,charp,&count,0644); //array of animal types input by user
module_param(animal_type,charp,0644); //animal type filter input by user
module_param(count_greater_than,int,0644); //count greater than filter input by user

/* Structure for first linked list */
struct my_data_struct
{
	char *unique_animals; //Stores animal list without duplicates 
	int no; //stores Count of occurrences of each animal
	struct list_head duplicate_free_list; //list head
};

/* Structure for second linked list */
struct my_data_struct2
{
	char *filtered_animals; //Stores filtered animal list 
	struct list_head filtered_list; //list head
};

/* declaration and initializatio of lists */
LIST_HEAD(my_duplicate_free_list);
LIST_HEAD(my_filtered_list);

/* Entry point of module. Invoked when module is loaded in kernel 
 * static used as function is typically not directly invoked by external code
 */
static int animals_ds_init(void)
{ 
	struct my_data_struct2 *filter_obj, *my_obj2;
	struct list_head *pos2;
	struct my_data_struct *obj, *my_obj;
	struct list_head *pos;
	int i,j,count_animal, freq[MAX_ANIMALS];
	int k = 0;
	char *temp;

	printk(KERN_ALERT "Entering Module");
	printk("The data structure type used is Linked List.");

	printk("Entries in the seed array: ");
	for(i=0;i<count;i++)
	{
		printk("%s ", animals[i]);
	}	

	/* Bubble sort entries in the seed array */
	for(i=0;i<count-1;i++)
	{
		for(j=0;j<count-i-1;j++)
		{
			if(strcmp(animals[j],animals[j+1]) < 0)
			{
				temp = animals[j];
				animals[j] = animals[j+1];
				animals[j+1] = temp;
			}
		}		
	}

	/* Count occurrences for each animal type */
	for(i=0;i<count;i++)
	{
		count_animal = 1;
		for(j=i+1;j<count;j++)
		{
			if(strcmp(animals[i],animals[j]) == 0)
			{
				count_animal++;
			}
		}
		freq[i] = count_animal;
	}
		
	/* Determine duplicates in the seed array */
	for(i=0;i<count;i++)
	{
		for(j=0;j<i;j++)
		{
			if(strcmp(animals[i],animals[j]) == 0)
			{
				k++; //variable to sync the count with the correct animal
				printk("Duplicate Occurrence of %s",animals[i]);
				break;	
			}
		}
		
		/* Allocate memory and add to list elements without any duplicates */
		if(j==i)
		{
			obj = (struct my_data_struct *)kmalloc(sizeof(struct my_data_struct), GFP_KERNEL); //allocate memory
			if(obj == NULL)
			{
				printk(KERN_ERR "Error allocating structure");
				return -ENOMEM; //Return "Cannot allocate memory" on error
			}
			obj->unique_animals = animals[i];  
			obj->no = freq[k];
			/* Add the current animal and its occurrence to the first list */
			list_add(&obj->duplicate_free_list, &my_duplicate_free_list); 
			no_of_nodes++; //Track number of nodes allocated
			k++;
		}
	}

	printk("Alphabetically sorted list without duplicates and with occurrences of each animal: ");
	
	/* Print each element in the first list */
	list_for_each(pos,&my_duplicate_free_list)
	{
		my_obj = list_entry(pos, struct my_data_struct, duplicate_free_list);
		printk("%s --> %d\n",my_obj->unique_animals,my_obj->no);
	}

	printk("The ecosytem size (Set-1): %d", no_of_nodes);	

	printk("Total amount of memory dynamically allocated for nodes (Set-1): %zu bytes", (no_of_nodes * sizeof(struct my_data_struct)));

	/* Filter 1: no filter (display all animals in the seed array) */
	if((strcmp(animal_type,"NULL") == 0) && count_greater_than == 0)
	{
		no_of_nodes2 = 0;
		printk("No filter criteria selected. Therefore, all entries in the seed array are printed:");
		
		/* Assign each element in the unique list to the filtered list */
		list_for_each_entry(my_obj,&my_duplicate_free_list,duplicate_free_list)
		{
			/* allocate memory */
			filter_obj = (struct my_data_struct2 *)kmalloc(sizeof(struct my_data_struct2), GFP_KERNEL); 
			if(filter_obj == NULL)
			{
				printk(KERN_ERR "Error allocating structure");
				return -ENOMEM;
			}
			filter_obj->filtered_animals = my_obj->unique_animals;
			list_add(&filter_obj->filtered_list, &my_filtered_list);//Add filtered animals to the second list
			no_of_nodes2++; //Track number of nodes allocated
		}

		/* Print each element in the second list */
		list_for_each(pos2,&my_filtered_list)
		{
			my_obj2 = list_entry(pos2, struct my_data_struct2, filtered_list);
			printk("%s\n",my_obj2->filtered_animals);
		}
	
		printk("The ecosytem size (Set-2): %d", no_of_nodes2);	
	
		printk("Total amount of memory dynamically allocated for nodes (Set-2): %zu bytes", (no_of_nodes2 * sizeof(struct my_data_struct2)));
	}

	/* Filter 2: Animal Type */
	else if(count_greater_than == 0)
	{
		no_of_nodes2 = 0;	
		printk("Filter criteria: Animal Type = %s.\nFollowing is the filtered list:", animal_type);
		list_for_each_entry(my_obj,&my_duplicate_free_list,duplicate_free_list)
		{
			/* Check if animal in unique list matches the filter requested by user */
			if(strcmp(animal_type,my_obj->unique_animals) == 0) 
			{
				/* Allocate memory */
				filter_obj = (struct my_data_struct2 *)kmalloc(sizeof(struct my_data_struct2), GFP_KERNEL);
				if(filter_obj == NULL)
				{
					printk(KERN_ERR "Error allocating structure");
					return -ENOMEM;
				}
				filter_obj->filtered_animals = my_obj->unique_animals;
				list_add(&filter_obj->filtered_list, &my_filtered_list); //Add filtered animals to the second list
				no_of_nodes2++; //Track number of nodes allocated
				printk("%s --> %d\n",filter_obj->filtered_animals,my_obj->no);
				break;
			}
		}
		
		printk("The ecosytem size (Set-2): %d", no_of_nodes2);	

		printk("Total amount of memory dynamically allocated for nodes (Set-2): %zu bytes", (no_of_nodes2 * sizeof(struct my_data_struct2)));
	}
	
	/* Filter 3: Count greater than */
	else if(strcmp(animal_type,"NULL") == 0)
	{
		no_of_nodes2 = 0;
		printk("Filter criteria: Count > %d.\nFollowing is the filtered list:", count_greater_than);
		
		/* Check if occurrence of entry in first list is greater than that input by user */
		list_for_each_entry(my_obj,&my_duplicate_free_list,duplicate_free_list)
		{
			if(my_obj->no > count_greater_than)
			{
				/* Allocate memory */
				filter_obj = (struct my_data_struct2 *)kmalloc(sizeof(struct my_data_struct2), GFP_KERNEL);
				if(filter_obj == NULL)
				{
					printk(KERN_ERR "Error allocating structure");
					return -ENOMEM;
				}
				filter_obj->filtered_animals = my_obj->unique_animals;
				list_add(&filter_obj->filtered_list, &my_filtered_list); //Add filtered animals to the second list
				no_of_nodes2++; //Track number of nodes allocated
			}
		}

		/* Print each element in the second list */
		list_for_each(pos2,&my_filtered_list)
		{
			my_obj2 = list_entry(pos2, struct my_data_struct2, filtered_list);
			printk("%s\n",my_obj2->filtered_animals);
		}
		
		printk("The ecosytem size (Set-2): %d", no_of_nodes2);	

		printk("Total amount of memory dynamically allocated for nodes (Set-2): %zu bytes", (no_of_nodes2 * sizeof(struct my_data_struct2)));
	}

	/* Filter 4: Animal type and count greater than */
	else
	{
		no_of_nodes2 = 0;
		printk("Filter criteria: Count > %d and Animal Type = %s.\nFollowing is the filtered list:", count_greater_than, animal_type);
		
		/* Check if occurrence of entry in first list is greater than that input by user and equal to animal type input by user */
		list_for_each_entry(my_obj,&my_duplicate_free_list,duplicate_free_list)
		{
			if(my_obj->no > count_greater_than && (strcmp(my_obj->unique_animals, animal_type) == 0))
			{
				/* Allocate memory */
				filter_obj = (struct my_data_struct2 *)kmalloc(sizeof(struct my_data_struct2), GFP_KERNEL);
				if(filter_obj == NULL)
				{
					printk(KERN_ERR "Error allocating structure");
					return -ENOMEM;
				}
				filter_obj->filtered_animals = my_obj->unique_animals;
				list_add(&filter_obj->filtered_list, &my_filtered_list); //Add filtered animals to the second list
				no_of_nodes2++; //Track number of nodes allocated
				printk("%s --> %d\n",filter_obj->filtered_animals,my_obj->no);
				break;
			}
		}
		
		printk("The ecosytem size (Set-2): %d", no_of_nodes2);	

		printk("Total amount of memory dynamically allocated for nodes (Set-2): %zu bytes", (no_of_nodes2 * sizeof(struct my_data_struct2)));
	}
	
	return 0;
}

/* Module exit point when module is removed from memory */
static void animals_ds_exit(void)
{
	struct list_head *pos, *q;
	list_for_each_safe( pos, q, &my_duplicate_free_list ) {
    	struct my_data_struct *tmp;
    	tmp = list_entry( pos, struct my_data_struct, duplicate_free_list );
    	list_del( pos ); //delete first list
    	kfree( tmp ); //free memory allocated to first list
  	}

	list_for_each_safe( pos, q, &my_filtered_list ) {
    	struct my_data_struct2 *tmp2;
    	tmp2 = list_entry( pos, struct my_data_struct2, filtered_list );
    	list_del( pos ); //delete second list
    	kfree( tmp2 ); //free memory allocated to second list
  	}
	
	printk("Freed memory for set 1: %zu bytes", no_of_nodes * sizeof(struct my_data_struct)); //Memory freed from 1st set
	printk("Freed memory for set 2: %zu bytes", no_of_nodes2 * sizeof(struct my_data_struct2)); //Memory freed from 2nd set

	printk("Exiting Module");
}

module_init(animals_ds_init); //registers module entry point
module_exit(animals_ds_exit); //registers module exit point

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Devansh Mittal");
MODULE_DESCRIPTION("Module to remove duplicates from animals list");
	
