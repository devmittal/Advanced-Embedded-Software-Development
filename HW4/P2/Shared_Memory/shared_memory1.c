#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/time.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<semaphore.h>
#include<signal.h>

#define MEMORYNAME "/myshmfd"
#define SIZE 32
#define SEMNAME "/mysem"
#define SEMNAME2 "/mysem2"
#define FILENAME "logfile"
int flag = 0;
FILE *FP;

void kill_signal_handler(int signum)
{
	struct timeval timestamp_kill;
	if(signum == SIGINT)
	{
		gettimeofday(&timestamp_kill,NULL);
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] CTRL+C signal received", timestamp_kill.tv_sec,
				 timestamp_kill.tv_usec);
		fflush(FP);
		fclose(FP);
		sem_unlink(SEMNAME);
		sem_unlink(SEMNAME2);
		shm_unlink(MEMORYNAME);
	}
}

int main()
{
	int shmfd,i;
	void *ptr;
	sem_t *sem;
	sem_t *sem1;
	struct timeval timestamp;
	struct sigaction act2;

	memset(&act2,0,sizeof(struct sigaction));

	act2.sa_handler = &kill_signal_handler;
	if(sigaction(SIGINT,&act2,NULL) == -1)
		perror("sigaction: ");

	FP = fopen(FILENAME,"a");	
	if(FP == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}	
	fprintf(FP,"\n\nProcess 1 PID: %d",getpid());
	fprintf(FP,"\nUsing Shared Memory");
	fflush(FP);
	
	shmfd = shm_open(MEMORYNAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if(shmfd == -1)
	{
		perror("Unable to create shared memory: ");
		exit(0);
	}

	fprintf(FP,"\nShared Memory file descriptor: %d",shmfd);
	fflush(FP);	

	if(ftruncate(shmfd,SIZE))
	{
		perror("Unable to configure size of memory: ");
		exit(0);
	}

	sem = sem_open(SEMNAME,O_CREAT,0666,0);
	if(sem == NULL)
	{
		perror("Could not create semaphore: ");
		exit(0);
	}

	sem1 = sem_open(SEMNAME2,O_CREAT,0666,0);
	if(sem1 == NULL)
	{
		perror("Could not create semaphore: ");
		exit(0);
	}

	ptr = mmap(0,SIZE,PROT_WRITE,MAP_SHARED,shmfd,0);
	if(ptr == (void *)-1)
	{
		perror("Mapping failed: ");
		exit(0);
	}

	for(i=0;i<10;i++)
	{
		if(i<=4)
		{
			sprintf(ptr,"From PID %d #%d",getpid(),i);
			gettimeofday(&timestamp,NULL);
			fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
					 timestamp.tv_usec, (char *)ptr);
			fflush(FP);	
		}
		else
		{
			sprintf(ptr,"%d",i%2);
			gettimeofday(&timestamp,NULL);
			fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %s", timestamp.tv_sec,
					 timestamp.tv_usec, (char *)ptr);
			fflush(FP);			
		}
		
		if(flag == 1)
		{
			sem_post(sem1);
		}
		if(sem_wait(sem) < 0)
		{
			perror("sem wait: ");
			exit(0);
		}

		if(i<=4)
		{
			gettimeofday(&timestamp,NULL);
			fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message - %s", timestamp.tv_sec,
					 timestamp.tv_usec, (char *)ptr);
			fflush(FP);	
		}
		else
		{
			gettimeofday(&timestamp,NULL);
			fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: LED Signal - %s", timestamp.tv_sec,
					 timestamp.tv_usec, (char *)ptr);
			fflush(FP);	
		}
		flag = 1;
	}

	sem_unlink(SEMNAME);
	sem_unlink(SEMNAME2);
	shm_unlink(MEMORYNAME);
	fclose(FP);

	return 0;
}