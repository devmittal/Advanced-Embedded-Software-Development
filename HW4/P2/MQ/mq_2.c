#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include<sys/types.h>
#include<unistd.h>
#include <stdlib.h>

#define QUEUENAME_RECEIVE "/mymq1"
#define QUEUENAME_SEND "/mymq2"
#define FILENAME "logfile"

typedef struct
{
	char s_send2[40];
	int led_send2;
}messages_send2;

typedef struct
{
	char s_receive2[20];
	//char led_receive2[5];
}messages_receive2;

int main()
{
	int n,i;
	char led[5];
	mqd_t mq_client_send;
	mqd_t mq_client_receive;
	messages_send2 mg_send2;
	messages_receive2 mg_receive2;
	//struct mq_attr attr;
	//char s[40];
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
	fflush(FP1);	

	mq_client_receive = mq_open(QUEUENAME_RECEIVE, O_RDONLY | O_CREAT, 0666, NULL);
	mq_client_send = mq_open(QUEUENAME_SEND, O_WRONLY | O_CREAT, 0666, NULL);
	if((mq_client_send || mq_client_receive) == -1)
	{
		perror("Queue not created: ");
		exit(0);
	}

	fprintf(FP1,"\nSending Message Queue Descriptor: %d | Receiving Message Queue Descriptor: %d",mq_client_send,mq_client_receive);
	fflush(FP1);

	for(i=0;i<9;i++)
	{
		n = mq_receive(mq_client_receive,mg_receive2.s_receive2,8192,NULL);
		if(n == -1)
		{
			perror("Receiving failed: ");
			exit(0);
		}
		gettimeofday(&timestamp2,NULL);
		if(n>5)
			fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
					timestamp2.tv_usec, mg_receive2.s_receive2);
		else
			fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: LED SIgnal - %s", timestamp2.tv_sec, 
					timestamp2.tv_usec, mg_receive2.s_receive2);
		fflush(FP1);
	}

	n = mq_send(mq_client_send,mg_send2.s_send2,40,0);
	if(n == -1)
	{
		perror("Sending failed: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);

	strcpy(mg_send2.s_send2,"LED signals received");
	n = mq_send(mq_client_send,mg_send2.s_send2,40,0);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit	(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);

	sprintf(mg_send2.s_send2,"From PID %d",getpid());
	n = mq_send(mq_client_send,mg_send2.s_send2,40,0);
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
	n = mq_send(mq_client_send,led,5,0);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Signal: %d", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.led_send2);
	fflush(FP1);

	strcpy(mg_send2.s_send2,"Change in LED signal");
	n = mq_send(mq_client_send,mg_send2.s_send2,40,0);
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
		n = mq_send(mq_client_send,led,5,0);	
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
	n = mq_send(mq_client_send,led,5,0);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Signal: %d", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.led_send2);
	fflush(FP1);

	n = mq_receive(mq_client_receive,mg_receive2.s_receive2,8192,NULL);
	if(n == -1)
	{
		perror("Receiving failed: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);

	strcpy(mg_send2.s_send2,"Affirmative - Communication Ended");
	n = mq_send(mq_client_send,mg_send2.s_send2,40,0);
	if(n == -1)
	{		
		perror("Error Reading/Writing: ");
		exit(0);
	}
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);

	if((mq_close(mq_client_send) || mq_close(mq_client_receive)) == -1)
	{
		perror("Closing file failed: ");
		exit(0);
	}

	if((mq_unlink(QUEUENAME_RECEIVE) || mq_unlink(QUEUENAME_SEND)) == -1)
	{
		perror("Unlinking file failed: ");
		exit(0);	
	}

	fclose(FP1);
	return 0;
}