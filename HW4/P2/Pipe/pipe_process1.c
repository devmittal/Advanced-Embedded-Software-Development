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
	char s_send[20];
	int led_send;
}messages_send; 

typedef struct
{
	char s_receive[40];
	char led_receive[5];
}messages_receive; 

int main()
{
	int fd;
	int i;
	messages_send mg_send;
	messages_receive mg_receive;
	char led[5];
	struct timeval timestamp;

	sprintf(mg_send.s_send,"From PID %d",getpid());
	mg_send.led_send = 1;
	sprintf(led,"%d",mg_send.led_send);

	FILE *FP = fopen(FILENAME,"a");	
	if(FP == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}	
	fprintf(FP,"\n\nProcess 1 PID: %d",getpid());
	fprintf(FP,"\nUsing Named Pipes");
	fprintf(FP,"\nAllocated file descriptor: %d",fileno(FP));
	fflush(FP);

	if(mkfifo(FIFONAME,0666) == -1)
		perror("Process 1 fifo: ");

	fd = open(FIFONAME,O_WRONLY);
	write(fd,mg_send.s_send,20);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	write(fd,led,5);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
	fflush(FP);
	close(fd);

	fd = open(FIFONAME,O_RDONLY);		
	read(fd,mg_receive.s_receive,40);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);
	fflush(FP);

	read(fd,mg_receive.s_receive,40);
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);	
	fflush(FP);

	read(fd,mg_receive.s_receive,40);
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);	
	fflush(FP);

	read(fd,mg_receive.led_receive,5);
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: LED Signal: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.led_receive);	
	fflush(FP);
	close(fd);

	fd = open(FIFONAME,O_WRONLY);
	write(fd,mg_send.s_send,20);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	strcpy(mg_send.s_send,"LED signal received");	
	write(fd,mg_send.s_send,20);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	mg_send.led_send = 0;
	sprintf(led,"%d",mg_send.led_send);
	for(i=0;i<3;i++)
	{
		write(fd,led,5);	
		gettimeofday(&timestamp,NULL);
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
		fflush(FP);
	}
	
	mg_send.led_send = 1;
	sprintf(led,"%d",mg_send.led_send);
	write(fd,led,5);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
	fflush(FP);
	close(fd);	

	fd = open(FIFONAME,O_RDONLY);
	read(fd,mg_receive.s_receive,40);
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);
	fflush(FP);

	for(i=0;i<3;i++)
	{
		read(fd,mg_receive.led_receive,5);
		gettimeofday(&timestamp,NULL);		
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: LED Signal: %s", timestamp.tv_sec, 
				timestamp.tv_usec, mg_receive.led_receive);	
		fflush(FP);
	}

	read(fd,mg_receive.led_receive,5);
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: LED Signal: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.led_receive);	
	fflush(FP);
	close(fd);

	fd = open(FIFONAME,O_WRONLY);	
	strcpy(mg_send.s_send,"LED signal received");	
	write(fd,mg_send.s_send,20);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	strcpy(mg_send.s_send,"End of Communication");	
	write(fd,mg_send.s_send,20);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);
	close(fd);

	fd = open(FIFONAME,O_RDONLY);
	read(fd,mg_receive.s_receive,40);
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);
	fflush(FP);
	close(fd);
	return 0;
}
