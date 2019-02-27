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
	//char *string_pid;
	messages_send mg_send;
	messages_receive mg_receive;
	char led[5];
	//char out[20];
	//char in[20] = {"Transmitter sending"};
	struct timeval timestamp;

	//strcpy(mg_send.s_send,"from PID ");
	sprintf(mg_send.s_send,"From PID %d",getpid());
	//strcat(mg_send.s_send,string_pid);
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

	if(mkfifo(FIFONAME,0666) == -1)
		perror("Process 1 fifo: ");

	fd = open(FIFONAME,O_WRONLY);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	write(fd,mg_send.s_send,20);
	//close(fd);

	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
	write(fd,led,5);
	close(fd);

	fd = open(FIFONAME,O_RDONLY);
	gettimeofday(&timestamp,NULL);		
	read(fd,mg_receive.s_receive,40);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);
	
	gettimeofday(&timestamp,NULL);		
	read(fd,mg_receive.s_receive,40);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);	

	gettimeofday(&timestamp,NULL);		
	read(fd,mg_receive.s_receive,40);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);	

	gettimeofday(&timestamp,NULL);		
	read(fd,mg_receive.led_receive,5);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: LED Signal: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.led_receive);	
	close(fd);

	fd = open(FIFONAME,O_WRONLY);
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	write(fd,mg_send.s_send,20);

	strcpy(mg_send.s_send,"LED signal received");	
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	write(fd,mg_send.s_send,20);

	strcpy(mg_send.s_send,"End of Communication");	
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	write(fd,mg_send.s_send,20);
	close(fd);

	fd = open(FIFONAME,O_RDONLY);
	gettimeofday(&timestamp,NULL);		
	read(fd,mg_receive.s_receive,40);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);
	close(fd);
	return 0;
}
