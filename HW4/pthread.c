#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_THREADS (2) //Number of threads spawned

pthread_t threads[NUM_THREADS]; //Thread descriptors

typedef struct
{
	char *filename;
}thread_info;

void childthread1(void *tinfo)
{
	
}

int main (int argc, char *argv[])
{
	int i;
	thread_info tinfo[NUM_THREADS];
	
	tinfo[0]->filename = argv[1];
	tinfo[1]->filename = argv[1];
	
	if(pthread_create(&threads[0],  (void *)0, childthread1, (void *)&(tinfo[0]))) // parameters to pass in
	{
		perror("\nError! Could not create thread: ");
		exit(1);
	}

	if(pthread_create(&threads[1],  (void *)0, childthread2, (void *)&(tinfo[1]))) // parameters to pass in
	{
		perror("\nError! Could not create thread: ");
		exit(1);
	}
}
	
		
	
	
	


