/******************************************************************************************
 *@file: sort.c
 *@brief: This program adds a system call which sorts an array of numbers.
 *@tools_used: Compiler: arm-linux-gcc. Program run on BeagleBone Green
 *@author: Devansh Mittal
 *@date: 02/10/2019
 *@version: 1.0
*******************************************************************************************/

#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<time.h>

#define ARRAY_SIZE 256

int main(void)
{
	int32_t unsorted[ARRAY_SIZE];
	int32_t sorted[ARRAY_SIZE];
	int32_t i;
	long retStatus;

	printf("\nSize of Buffer(user-space): %d items, %d bytes", ARRAY_SIZE, ARRAY_SIZE*sizeof(int32_t)); 	
	
	printf("\nUnsorted Array(user-space):\n");
	srand(time(0)); //value returned by function time is used as seed value to generate random numbers
	for(i=0;i<ARRAY_SIZE;i++)
	{
		unsorted[i] = rand()%1000; //Random numbers generated from 0-999
		printf("%d ", unsorted[i]);
	}	

	retStatus = syscall(399, NULL, ARRAY_SIZE, sorted); //Unsuccessful system call passing NULL pointer
	
	if(retStatus == 0) //Check if successful
	{
		printf("\n\nSorting successful(user-space):\n");
		for(i=0;i<ARRAY_SIZE;i++)
		{
			printf("%d ", sorted[i]);
		}
		printf("\n");
	}

	else
	{
		perror("\nSorting failed: ");
	}

	retStatus = syscall(399, unsorted, 250, sorted); //Unsuccessful system call passing length less than 256

	if(retStatus == 0) //Check if successful
	{
		printf("\n\nSorting successful(user-space):\n");
		for(i=0;i<ARRAY_SIZE;i++)
		{
			printf("%d ", sorted[i]);
		}
		printf("\n");
	}

	else
	{
		perror("\nSorting failed: "); 
	}

	retStatus = syscall(399, unsorted, -1, sorted); //Unsuccessful system call passing negative length

	if(retStatus == 0) //Check if successful
	{
		printf("\n\nSorting successful(user-space):\n");
		for(i=0;i<ARRAY_SIZE;i++)
		{
			printf("%d ", sorted[i]);
		}
		printf("\n");
	}

	else
	{
		perror("\nSorting failed: ");
	}

	retStatus = syscall(399, unsorted, ARRAY_SIZE, sorted); //Successful system call
	
	if(retStatus == 0) //Check if successful
	{
		printf("\n\nSorting successful(user-space):\n");
		for(i=0;i<ARRAY_SIZE;i++)
		{
			printf("%d ", sorted[i]);
		}
		printf("\n");
	}

	else
	{
		perror("\nSorting failed: ");
	}

	return 0;

}
