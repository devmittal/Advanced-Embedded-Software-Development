#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>

#define QUEUENAME_SEND "/mymq1"
#define QUEUENAME_RECEIVE "/mymq2"
#define FILENAME "logfile"

mqd_t mq_server_send;
mqd_t mq_server_receive;
FILE *FP;

typedef struct
{
	char s_send[20];
	int led_send;
}messages_send; 

typedef struct
{
	char s_receive[40];
	//char led_receive[5];
}messages_receive; 

void kill_signal_handler(int signum)
{
	struct timeval timestamp_kill;
	if(signum == SIGINT)
	{
		gettimeofday(&timestamp_kill,NULL);
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] CTRL+C signal received", timestamp_kill.tv_sec,
				 timestamp_kill.tv_usec);
		fclose(FP);
		if((mq_close(mq_server_send) || mq_close(mq_server_receive)) == -1)
		{
			perror("Closing file failed: ");
			exit(0);
		}

		if((mq_unlink(QUEUENAME_RECEIVE) || mq_unlink(QUEUENAME_SEND)) == -1)
		{
			perror("Unlinking file failed: ");
			exit(0);	
		}
	}
}

int main()
{
	int n;
	int i;
	messages_send mg_send;
	messages_receive mg_receive;
	char led[5];
	struct timeval timestamp;
	struct sigaction act2;

	memset(&act2,0,sizeof(struct sigaction));

	act2.sa_handler = &kill_signal_handler;
	if(sigaction(SIGINT,&act2,NULL) == -1)
		perror("sigaction: ");

	sprintf(mg_send.s_send,"From PID %d",getpid());
	mg_send.led_send = 1;
	sprintf(led,"%d",mg_send.led_send);

	FP = fopen(FILENAME,"a");	
	if(FP == NULL)
	{
		perror("File could not be created/opened: ");
		exit(1);
	}	
	fprintf(FP,"\n\nProcess 1 PID: %d",getpid());
	fprintf(FP,"\nUsing Message Queues");
	fflush(FP);

	mq_server_send = mq_open(QUEUENAME_SEND, O_WRONLY | O_CREAT, 0666, NULL);
	mq_server_receive = mq_open(QUEUENAME_RECEIVE, O_RDONLY | O_CREAT, 0666, NULL);
	if((mq_server_send || mq_server_receive) == -1)
	{
		perror("Queue not created: ");
		exit(0);
	}

	fprintf(FP,"\nSending Message Queue Descriptor: %d | Receiving Message Queue Descriptor: %d",mq_server_send,mq_server_receive);
	fflush(FP);

	n = mq_send(mq_server_send,mg_send.s_send,20,0);
	if(n == -1)
	{
		perror("Sending failed: ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	n = mq_send(mq_server_send,led,5,0);
	if(n == -1)
	{
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
	fflush(FP);

	n = mq_send(mq_server_send,mg_send.s_send,20,0);
	if(n == -1)
	{
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	strcpy(mg_send.s_send,"Change in LED signal");	
	n = mq_send(mq_server_send,mg_send.s_send,20,0);
	if(n == -1)
	{
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	mg_send.led_send = 0;
	sprintf(led,"%d",mg_send.led_send);
	for(i=0;i<3;i++)
	{
		n = mq_send(mq_server_send,led,5,0);
		if(n == -1)
		{
			perror("Error Reading/Writing: ");
			exit(0);
		}	
		gettimeofday(&timestamp,NULL);
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
		fflush(FP);
	}
	
	mg_send.led_send = 1;
	sprintf(led,"%d",mg_send.led_send);
	n = mq_send(mq_server_send,led,5,0);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit	(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
	fflush(FP);

	strcpy(mg_send.s_send,"LED signalling done");	
	n = mq_send(mq_server_send,mg_send.s_send,20,0);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit	(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	for(i=0;i<9;i++)
	{
		n = mq_receive(mq_server_receive,mg_receive.s_receive,8192,NULL);
		if(n == -1)
		{
			perror("Receiving failed: ");
			exit(0);
		}
		gettimeofday(&timestamp,NULL);
		if(n>2)
			fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message - %s", timestamp.tv_sec, 
					timestamp.tv_usec, mg_receive.s_receive);
		else
			fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: LED SIgnal - %s", timestamp.tv_sec, 
					timestamp.tv_usec, mg_receive.s_receive);
		fflush(FP);
	}

	strcpy(mg_send.s_send,"End of communication");	
	n = mq_send(mq_server_send,mg_send.s_send,20,0);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit	(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	n = mq_receive(mq_server_receive,mg_receive.s_receive,8192,NULL);
	if(n == -1)
	{
		perror("Receiving failed: ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);
	fflush(FP);

	if((mq_close(mq_server_send) || mq_close(mq_server_receive)) == -1)
	{
		perror("Closing file failed: ");
		exit(0);
	}

	if((mq_unlink(QUEUENAME_RECEIVE) || mq_unlink(QUEUENAME_SEND)) == -1)
	{
		perror("Unlinking file failed: ");
		exit(0);	
	}

	fclose(FP);
	return 0;
}