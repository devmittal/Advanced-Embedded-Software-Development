#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/time.h>

#define FILENAME "logfile"
#define FIFONAME "fifofile"

typedef struct
{
	char s_send2[40];
	int led_send2;
}messages_send2;

typedef struct
{
	char s_receive2[20];
	char led_receive2[5];
}messages_receive2;
int main()
{
	int fd1;
	int i,n;
	char led[5];
	messages_send2 mg_send2;
	messages_receive2 mg_receive2;
	struct timeval timestamp2;

	sprintf(mg_send2.s_send2,"From PID %d",getpid());
	mg_send2.led_send2 = 1;

	FILE *FP1 = fopen(FILENAME,"a");	
	if(FP1 == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}	
	fprintf(FP1,"\n\nProcess 2 PID: %d",getpid());
	fprintf(FP1,"\nUsing Named Pipes");
	fprintf(FP1,"\nAllocated file descriptor: %d",fileno(FP1));
	fflush(FP1);

	if(mkfifo(FIFONAME,0666) == -1)
			perror("Process 1 fifo: ");

	fd1 = open(FIFONAME,O_RDONLY);
	n = read(fd1,mg_receive2.s_receive2,20);
	if(n == -1)
	{		
		perror("Could not read: ");
		exit	(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);

	n = read(fd1,mg_receive2.led_receive2,5);
	if(n == -1)
	{		
		perror("Could not read: ");
		exit	(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: LED Signal - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.led_receive2);
	fflush(FP1);
	close(fd1);

	fd1 = open(FIFONAME,O_WRONLY);
	n = write(fd1,mg_send2.s_send2,40);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit	(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);
	

	strcpy(mg_send2.s_send2,"LED signal received");
	n = write(fd1,mg_send2.s_send2,40);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit	(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);

	sprintf(mg_send2.s_send2,"From PID %d",getpid());
	n = write(fd1,mg_send2.s_send2,40);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit	(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);
	
	mg_send2.led_send2 = 0;
	sprintf(led,"%d",mg_send2.led_send2);
	n = write(fd1,led,5);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Signal: %d", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.led_send2);
	fflush(FP1);
	close(fd1);

	fd1 = open(FIFONAME,O_RDONLY);
	n = read(fd1,mg_receive2.s_receive2,20);
	if(n == -1)
	{		
		perror("Could not read: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);

	n = read(fd1,mg_receive2.s_receive2,20);
	if(n == -1)
	{		
		perror("Could not read: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);		
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);

	for(i=0;i<3;i++)
	{
		n = read(fd1,mg_receive2.led_receive2,5);
		if(n == -1)
		{		
			perror("Could not read: ");
			exit(0);
		}
		gettimeofday(&timestamp2,NULL);
		fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: LED Signal - %s", timestamp2.tv_sec, 
				timestamp2.tv_usec, mg_receive2.led_receive2);
		fflush(FP1);
	}

	n = read(fd1,mg_receive2.led_receive2,5);	
	if(n == -1)
	{		
		perror("Could not read: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: LED Signal - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.led_receive2);
	fflush(FP1);
	close(fd1);

	fd1 = open(FIFONAME,O_WRONLY);
	strcpy(mg_send2.s_send2,"LED signal received");
	n = write(fd1,mg_send2.s_send2,40);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);

	mg_send2.led_send2 = 1;
	sprintf(led,"%d",mg_send2.led_send2);	
	for(i=0;i<3;i++)
	{
		n = write(fd1,led,5);	
		if(n == -1)
		{		
			perror("Error Reading/Writing: ");
			exit(0);
		}
		gettimeofday(&timestamp2,NULL);
		fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Signal: %d", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.led_send2);
		fflush(FP1);
	}

	mg_send2.led_send2 = 0;
	sprintf(led,"%d",mg_send2.led_send2);
	n = write(fd1,led,5);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Signal: %d", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.led_send2);
	fflush(FP1);
	close(fd1);

	fd1 = open(FIFONAME,O_RDONLY);
	n = read(fd1,mg_receive2.s_receive2,20);
	if(n == -1)
	{		
		perror("Could not read: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);		
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);
	
	n = read(fd1,mg_receive2.s_receive2,20);
	if(n == -1)
	{		
		perror("Could not read: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);		
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);
	close(fd1);
	
	strcpy(mg_send2.s_send2,"Affirmative - Communication Ended");
	fd1 = open(FIFONAME,O_WRONLY);
	n = write(fd1,mg_send2.s_send2,40);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);
	close(fd1);

	fclose(FP1);
	return 0;
}
