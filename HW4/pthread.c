#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/syscall.h>

#define NUM_THREADS (2) //Number of threads spawned
#define MAXLINE 60

pthread_t threads[NUM_THREADS]; //Thread descriptors
char *name_file;

typedef struct
{
	char *filename;
}thread_info;

void thread_handler(union sigval sv)
{
	char line[MAXLINE];
	int i;	
	char *s = sv.sival_ptr;
	FILE *FP = fopen("/proc/stat", "r");
	if(FP == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}

	FILE *fileptr_timer = fopen(s,"a");
	if(fileptr_timer == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}

	fprintf(fileptr_timer,"\n\nCPU Utilization:\n");
	for(i=0;i<3;i++)
	{
		fgets(line, sizeof line,FP);
		fprintf(fileptr_timer,"%s",line);
	}
}

void kill_signal_handler(int signum)
{
	struct timeval endTime_child_kill;

	FILE *fileptr_kill_signal = fopen(name_file,"a");
	if(fileptr_kill_signal == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}

	if(signum == 10)
	{
		fprintf(fileptr_kill_signal,"\n\nChild thread 1 exiting due to USR1");	
		gettimeofday(&endTime_child_kill, 0);
		fprintf(fileptr_kill_signal, "\nChild thread 1 end time: %lu seconds %lu microseconds", endTime_child_kill.tv_sec, endTime_child_kill.tv_usec);
		pthread_cancel(threads[0]);
	}
	else if(signum == 12)
	{
		fprintf(fileptr_kill_signal,"\n\nChild thread 2 exiting due to USR2");
		gettimeofday(&endTime_child_kill, 0);
		fprintf(fileptr_kill_signal, "\nChild thread 2 end time: %lu seconds %lu microseconds", endTime_child_kill.tv_sec, endTime_child_kill.tv_usec);
		pthread_cancel(threads[1]);
	}
}

void *childthread1(void *tinfo)
{
	int count[26] = {0};
	int i;
	char c;
	struct timeval startTime_child1, endTime_child1;
	struct sigaction act;
	gettimeofday(&startTime_child1, 0);

	memset(&act,0,sizeof(struct sigaction));

	act.sa_handler = &kill_signal_handler;

	if(sigaction(SIGUSR1,&act,NULL) == -1)
		perror("sigaction: ");
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

	fprintf(fileptr_child1,"\n\nChild thread 1 exiting. Task completed");	
	gettimeofday(&endTime_child1, 0);
	fprintf(fileptr_child1, "\nChild 1 thread end time: %lu seconds %lu microseconds", endTime_child1.tv_sec, endTime_child1.tv_usec);
}

void *childthread2(void *tinfo)
{
	struct sigevent sev;
	struct itimerspec trigger;
	static timer_t timerid;
	struct timeval startTime_child2, endTime_child2;
	struct sigaction act2;
	gettimeofday(&startTime_child2, 0);

	memset(&act2,0,sizeof(struct sigaction));

	act2.sa_handler = &kill_signal_handler;
	if(sigaction(SIGUSR2,&act2,NULL) == -1)
		perror("sigaction: ");

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
	
	memset(&sev,0,sizeof(struct sigevent));
	memset(&trigger,0,sizeof(struct itimerspec));

	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = &thread_handler;	
	sev.sigev_value.sival_ptr = fileinfo2->filename;
	
	if(timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
		perror("timer_create: ");	
	trigger.it_value.tv_nsec = 100000000;
	trigger.it_interval.tv_nsec = 100000000;
	if(timer_settime(timerid,0,&trigger,NULL) == -1)
		perror("timer_settime: ");
	sleep(100000);	
	if(timer_delete(timerid) == -1)
		perror("timer_delete: ");

	fprintf(fileptr_child2,"\n\nChild thread 2 exiting. Task completed");
	gettimeofday(&endTime_child2, 0);
	fprintf(fileptr_child2, "\nChild 2 thread end time: %lu seconds %lu microseconds", endTime_child2.tv_sec, endTime_child2.tv_usec);
}

int main (int argc, char *argv[])
{
	int i;
	thread_info tinfo[NUM_THREADS];
	struct timeval startTime, endTime;
	gettimeofday(&startTime, 0);

	name_file = argv[1];

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
