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
	char s_receive[20];
	int led_receive;
}messages_receive;

int main()
{
	int fd;
	int i;
	//char *string_pid;
	messages_send mg_send;
	messages_receive mg_receive;
	//char out[20];
	//char in[20] = {"Transmitter sending"};
	struct timeval timestamp;

	//strcpy(mg_send.s_send,"from PID ");
	sprintf(mg_send.s_send,"From PID %d",getpid());
	//strcat(mg_send.s_send,string_pid);
	mg_send.led_send = 1;

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

	for(int i=0;i<10;i++)
	{
		fd = open(FIFONAME,O_WRONLY);
		gettimeofday(&timestamp,NULL);
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: %s LED Control: %d", timestamp.tv_sec, timestamp.tv_usec, mg_send.s_send, mg_send.led_send);
		write(fd,&mg_send,sizeof(mg_send));
		close(fd);

		fd = open(FIFONAME,O_RDONLY);
		gettimeofday(&timestamp,NULL);		
		read(fd,&mg_receive,sizeof(mg_receive));
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: %s LED Control: %d", timestamp.tv_sec, timestamp.tv_usec, mg_receive.s_receive, 				mg_receive.led_receive);
		close(fd);
	}
	return 0;
}
