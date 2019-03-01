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

#define MEMORYNAME "/myshmfd"
#define SIZE 32
#define SEMNAME "mysem"
#define FILENAME "logfile"
#define SEMNAME2 "/mysem2"

int main()
{
	int shmfd2,i;
	void *ptr;
	char *s = "World";
	sem_t *sem;
	sem_t *sem1;
	sem_t sem2;

	struct timeval timestamp2;

	sem_init(&sem2,1,0);
	FILE *FP1 = fopen(FILENAME,"a");	
	if(FP1 == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}	
	fprintf(FP1,"\n\nProcess 2 PID: %d",getpid());
	fprintf(FP1,"\nUsing Shared Memory");
	fflush(FP1);	

	shmfd2 = shm_open(MEMORYNAME, O_RDWR, 0666);
	if(shmfd2 == -1)
	{
		perror("Unable to create shared memory: ");
		exit(0);
	}

	fprintf(FP1,"\nShared Memory file descriptor: %d",shmfd2);
	fflush(FP1);	

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
	ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd2, 0);
	if(ptr == (void *)-1)
	{
		perror("Mapping failed: ");
		exit(0);
	}

	for(i=0;i<10;i++)
	{
		if(i<=4)
		{
			gettimeofday(&timestamp2,NULL);
			fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
				timestamp2.tv_usec, (char *)ptr);
			fflush(FP1);	
		}
		else
		{
			gettimeofday(&timestamp2,NULL);
			fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: LED Signal - %s", timestamp2.tv_sec, 
				timestamp2.tv_usec, (char *)ptr);
			fflush(FP1);	
		}

		if(i<=4)
		{
			sprintf(ptr,"From PID %d #%d",getpid(), i);
			gettimeofday(&timestamp2,NULL);
			fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message - %s", timestamp2.tv_sec,
					 timestamp2.tv_usec, (char *)ptr);
			fflush(FP1);	
		}
		else
		{
			sprintf(ptr,"%d",i%2);
			gettimeofday(&timestamp2,NULL);
			fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Signal - %s", timestamp2.tv_sec,
					 timestamp2.tv_usec, (char *)ptr);
			fflush(FP1);			
		}

		sem_post(sem);
		if(i<=8)
			sem_wait(sem1);
	}
	sem_unlink(SEMNAME);
	sem_unlink(SEMNAME2);
	shm_unlink(MEMORYNAME);
	fclose(FP1);

	return 0;
}