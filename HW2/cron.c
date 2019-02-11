/******************************************************************************************
 *@file: sort.c
 *@brief: This program adds a system call which sorts an array of numbers. It also
 * 	  calls other system calls to gather system information.
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
#include<sys/types.h>
#include<sys/time.h>
#include<unistd.h>

#define ARRAY_SIZE 256

int main(void)
{
	int32_t unsorted[ARRAY_SIZE];
	int32_t sorted[ARRAY_SIZE];
	int32_t i;
	long retStatus;
	struct timeval curTime;

	printf("\nCurrent Process ID: %lu", getpid()); //Get current process ID
	printf("\nCurrent User ID: %d", getuid()); //Get current user ID
	gettimeofday(&curTime, 0); //Get time since Epoch
	printf("\nCurrent time since Epoch: %lu seconds %lu microseconds", curTime.tv_sec, curTime.tv_usec);	
	
	printf("\nUnsorted Array (user-space):\n");
	srand(time(0)); //value returned by function time is used as seed value to generate random numbers
	for(i=0;i<ARRAY_SIZE;i++)
	{
		unsorted[i] = rand()%1000; //Random numbers generated from 0-999
		printf("%d ", unsorted[i]);
	}	

	retStatus = syscall(399, unsorted, ARRAY_SIZE, sorted); //System call
	
	if(retStatus == 0) //Check if system call is successfull
	{
		printf("\n\nSorting successful (user-space):\n");
		for(i=0;i<ARRAY_SIZE;i++)
		{
			printf("%d ", sorted[i]);
		}
		printf("\n");
	}

	else
	{
		perror("\nSorting failed");
	}

	return 0;

}
