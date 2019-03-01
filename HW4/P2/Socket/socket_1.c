#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>

#define FILENAME "logfile"
int socketfd;
FILE *FP;

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

void kill_signal_handler(int signum)
{
	struct timeval timestamp_kill;
	if(signum == SIGINT)
	{
		gettimeofday(&timestamp_kill,NULL);
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] CTRL+C signal received", timestamp_kill.tv_sec,
				 timestamp_kill.tv_usec);
		fclose(FP);
		close(socketfd);
	}
}

int main(int argc, char *argv[])
{
	struct sockaddr_in socket_str, cli_addr;
	int portno, clilen;
	int client_sockfd;
	char buffer[256];
	int n, i;
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
	fprintf(FP,"\nUsing Sockets");
	fflush(FP);

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd == -1)
	{
		perror("Socket Creation failed: ");
		exit(0);
	}

	fprintf(FP,"\nSocket file descriptor: %d",socketfd);
	fflush(FP);

	bzero((char *) &socket_str, sizeof(socket_str));
	portno = atoi(argv[1]);

	socket_str.sin_family = AF_INET;
	socket_str.sin_addr.s_addr = INADDR_ANY;
	socket_str.sin_port = htons(portno);

	if(bind(socketfd, (struct sockaddr *) &socket_str, sizeof(socket_str)) == -1)
	{
		perror("Binding error: ");
		exit(0);
	} 

	if(listen(socketfd,5) == -1)
	{
		perror("Listening error: ");
		exit(0);
	}

	clilen = sizeof(cli_addr);
	client_sockfd = accept(socketfd, (struct sockaddr *) &cli_addr, &clilen);
	if(client_sockfd == -1)
	{
		perror("Client socket not received: ");
		exit(0);
	}

	n = write(client_sockfd,mg_send.s_send,20);
	if(n == -1)
	{
		perror("Could not read: ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	n = write(client_sockfd,led,5);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
	fflush(FP);

	n = read(client_sockfd,mg_receive.s_receive,40);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);
	fflush(FP);
	
	n = read(client_sockfd,mg_receive.s_receive,40);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);	
	fflush(FP);

	n = read(client_sockfd,mg_receive.s_receive,40);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);	
	fflush(FP);

	n = read(client_sockfd,mg_receive.led_receive,5);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: LED Signal: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.led_receive);	
	fflush(FP);

	n = write(client_sockfd,mg_send.s_send,20);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	strcpy(mg_send.s_send,"LED signal received");	
	n = write(client_sockfd,mg_send.s_send,20);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
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
		n = write(client_sockfd,led,5);	
		if(n == -1)
		{
			perror("Error Reading/Writing ");
			exit(0);
		}
		gettimeofday(&timestamp,NULL);
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
		fflush(FP);
	}
	
	mg_send.led_send = 1;
	sprintf(led,"%d",mg_send.led_send);
	n = write(client_sockfd,led,5);
	if(n == -1)
	{	
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: LED Signal - %d", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.led_send);
	fflush(FP);

	n = read(client_sockfd,mg_receive.s_receive,40);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);
	fflush(FP);

	for(i=0;i<3;i++)
	{
		n = read(client_sockfd,mg_receive.led_receive,5);
		if(n == -1)
		{
			perror("Error Reading/Writing ");
			exit(0);
		}
		gettimeofday(&timestamp,NULL);		
		fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: LED Signal: %s", timestamp.tv_sec, 
				timestamp.tv_usec, mg_receive.led_receive);	
		fflush(FP);
	}

	n = read(client_sockfd,mg_receive.led_receive,5);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: LED Signal: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.led_receive);	
	fflush(FP);

	strcpy(mg_send.s_send,"LED signal received");	
	n = write(client_sockfd,mg_send.s_send,20);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	strcpy(mg_send.s_send,"End of Communication");	
	n = write(client_sockfd,mg_send.s_send,20);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Sending from Process 1: Message - %s", timestamp.tv_sec,
			 timestamp.tv_usec, mg_send.s_send);
	fflush(FP);

	n = read(client_sockfd,mg_receive.s_receive,40);
	if(n == -1)
	{
		perror("Error Reading/Writing ");
		exit(0);
	}
	gettimeofday(&timestamp,NULL);		
	fprintf(FP,"\n\n[%lu seconds %lu microseconds] Receiving at Process 1: Message: %s", timestamp.tv_sec, 
			timestamp.tv_usec, mg_receive.s_receive);
	fflush(FP);
	close(socketfd);

	fclose(FP);
	return 0;
}
