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
	char s_send2[20];
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
	int i;
	char led[5];
	//char *string_pid2;
	messages_send2 mg_send2;
	messages_receive2 mg_receive2;
	//char out1[20];
	//char in1[20] = "Receiver sending";
	struct timeval timestamp2;

	//strcpy(mg_send2.s_send2,"from PID ");
	sprintf(mg_send2.s_send2,"From PID %d",getpid());
	//strcat(mg_send2.s_send2,string_pid2);
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

	if(mkfifo(FIFONAME,0666) == -1)
			perror("Process 1 fifo: ");

	fd1 = open(FIFONAME,O_RDONLY);
	gettimeofday(&timestamp2,NULL);
	read(fd1,mg_receive2.s_receive2,20);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	//close(fd1);

	gettimeofday(&timestamp2,NULL);
	read(fd1,mg_receive2.led_receive2,5);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: LED Controller - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.led_receive2);
	close(fd1);

	fd1 = open(FIFONAME,O_WRONLY);
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	write(fd1,mg_send2.s_send2,20);
	

	strcpy(mg_send2.s_send2,"LED signal received");
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	write(fd1,mg_send2.s_send2,20);

	sprintf(mg_send2.s_send2,"From PID %d",getpid());
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	write(fd1,mg_send2.s_send2,20);
	
	mg_send2.led_send2 = 0;
	sprintf(led,"%d",mg_send2.led_send2);
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Control: %d", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.led_send2);
	write(fd1,led,5);
	close(fd1);

	fd1 = open(FIFONAME,O_RDONLY);
	gettimeofday(&timestamp2,NULL);
	read(fd1,mg_receive2.s_receive2,20);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	close(fd1);
	return 0;
}
