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
	int count[26] = {0};
	int i;
	char c;
	struct timeval startTime_child1, endTime_child1;
	gettimeofday(&startTime_child1, 0);
	thread_info *fileinfo = (thread_info *)tinfo;
	FILE *fileptr_child1 = fopen(fileinfo->filename, "a");
	if(fileptr_child1 == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}
	fputs("\n\nCurrently in Child Thread 1", fileptr_child1);
	fprintf(fileptr_child1,"\nChild 1 PID: %d", getpid());
	fprintf(fileptr_child1,"\nChild 1 TID: %ld", syscall(SYS_gettid));
	fprintf(fileptr_child1, "\nChild 1 thread start time: %lu seconds %lu microseconds", startTime_child1.tv_sec, startTime_child1.tv_usec);
	
	FILE *randwords = fopen("gdb.txt","r");
	if(randwords == NULL)
	{
		perror("File could not be opened for reading: ");
		exit(1);
	}

	while((c = getc(randwords)) != EOF)
	{
		for(i=0;i<26;i++)
		{
			if(c == (i+65) || c == (i+97))
				count[i]++;
		}
	}
	
	fputs("\n\nFollowing characters have less than 100 occurrences:",fileptr_child1);
	for(i=0;i<26;i++)
	{
		if(count[i] < 100)
			fprintf(fileptr_child1,"\n%c --> %d",(i+97),count[i]);
	}

	gettimeofday(&endTime_child1, 0);
	fprintf(fileptr_child1, "\nChild 1 thread end time: %lu seconds %lu microseconds", endTime_child1.tv_sec, endTime_child1.tv_usec);
}

void *childthread2(void *tinfo)
{
	struct timeval startTime_child2, endTime_child2;
	gettimeofday(&startTime_child2, 0);
	thread_info *fileinfo2 = (thread_info *)tinfo;
	FILE *fileptr_child2 = fopen(fileinfo2->filename, "a");
	if(fileptr_child2 == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}
	fputs("\n\nCurrently in Child Thread 2", fileptr_child2);
	fprintf(fileptr_child2,"\nChild 2 PID: %d", getpid());
	fprintf(fileptr_child2,"\nChild 2 TID: %ld", syscall(SYS_gettid));
	fprintf(fileptr_child2, "\nChild 2 thread start time: %lu seconds %lu microseconds", startTime_child2.tv_sec, startTime_child2.tv_usec);
	gettimeofday(&endTime_child2, 0);
	fprintf(fileptr_child2, "\nChild 2 thread end time: %lu seconds %lu microseconds", endTime_child2.tv_sec, endTime_child2.tv_usec);
	
}

int main (int argc, char *argv[])
{
	int i;
	thread_info tinfo[NUM_THREADS];
	struct timeval startTime, endTime;
	gettimeofday(&startTime, 0);

	FILE *fileptr_main = fopen(argv[1], "a");
	if(fileptr_main == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}	
	
	fputs("\n\nCurrently in Master thread", fileptr_main);
	fprintf(fileptr_main,"\nMaster PID: %d", getpid());
	fprintf(fileptr_main,"\nMaster TID: %ld", syscall(SYS_gettid));
	fprintf(fileptr_main, "\nMaster thread start time: %lu seconds %lu microseconds", startTime.tv_sec, startTime.tv_usec);

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
	gettimeofday(&endTime, 0);
	fputs("\nExiting Master Thread",fileptr_main);
	fprintf(fileptr_main, "\nMaster thread end time: %lu seconds %lu microseconds", endTime.tv_sec, endTime.tv_usec);
}
	
		
	
	
	


