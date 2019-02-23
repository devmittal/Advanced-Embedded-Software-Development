#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define NUM_THREADS (2) //Number of threads spawned

pthread_t threads[NUM_THREADS]; //Thread descriptors

typedef struct
{
	char *filename;
}thread_info;

void *childthread1(void *tinfo)
{
	thread_info *fileinfo = (thread_info *)tinfo;
	FILE *fileptr_child1 = fopen(fileinfo->filename, "a");
	if(fileptr_child1 == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}
	fputs("\nCurrently in Child Thread 1", fileptr_child1);
	fprintf(fileptr_child1,"\nChild 1 PID: %d", getpid());
	fprintf(fileptr_child1,"\nChild 1 TID: %ld", syscall(SYS_gettid));
}

void *childthread2(void *tinfo)
{
	thread_info *fileinfo2 = (thread_info *)tinfo;
	FILE *fileptr_child2 = fopen(fileinfo2->filename, "a");
	if(fileptr_child2 == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}
	fputs("\nCurrently in Child Thread 2", fileptr_child2);
	fprintf(fileptr_child2,"\nChild 2 PID: %d", getpid());
	fprintf(fileptr_child2,"\nChild 2 TID: %ld", syscall(SYS_gettid));
	
}

int main (int argc, char *argv[])
{
	int i;
	thread_info tinfo[NUM_THREADS];
	
	FILE *fileptr_main = fopen(argv[1], "a");
	if(fileptr_main == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}
	fputs("Currently in Master thread", fileptr_main);
	fprintf(fileptr_main,"\nMaster PID: %d", getpid());
	fprintf(fileptr_main,"\nMaster TID: %ld", syscall(SYS_gettid));

	tinfo[0].filename = argv[1];
	tinfo[1].filename = argv[1];
	
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
	pthread_join(threads[0],NULL);
	pthread_join(threads[1],NULL);
}
	
		
	
	
	


