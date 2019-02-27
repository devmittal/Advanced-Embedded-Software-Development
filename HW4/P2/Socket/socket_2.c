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

#define FILENAME "logfile"

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

int main(int argc, char *argv[])
{
	struct sockaddr_in socket_str;
	int portno;
	char buffer[256] = "Sending data from client. Test 1..2..3...";
	char s[20];
	int n;
	int socketfd;
	int i;
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
	fprintf(FP1,"\nUsing Scokets");
	fprintf(FP1,"\nAllocated file descriptor: %d",fileno(FP1));
	fflush(FP1);

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd == -1)
	{
		perror("Socket Creation failed: ");
		exit(0);
	}

	bzero((char *) &socket_str, sizeof(socket_str));
	portno = atoi(argv[1]);

	socket_str.sin_family = AF_INET;
	socket_str.sin_addr.s_addr = INADDR_ANY;
	socket_str.sin_port = htons(portno);

	if(bind(socketfd, (struct sockaddr *) &socket_str, sizeof(socket_str)) == -1)
	{
		perror("Binding error: ");
		//exit(0);
	} 

	if(connect(socketfd, (struct sockaddr *) &socket_str, sizeof(socket_str)) == -1)
	{
		perror("Conecting error: ");
		exit(0);
	}

	//n = write(socketfd,buffer,256);
	gettimeofday(&timestamp2,NULL);
	n = read(socketfd,mg_receive2.s_receive2,20);
	if(n == -1)
	{
		perror("Could not write: ");
		exit(0);
	}
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);

	gettimeofday(&timestamp2,NULL);
	n = read(socketfd,mg_receive2.led_receive2,5);
	if(n == -1)
	{
		perror("Could not write: ");
		exit(0);
	}
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: LED Signal - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.led_receive2);
	fflush(FP1);

	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);
	write(socketfd,mg_send2.s_send2,40);
	

	strcpy(mg_send2.s_send2,"LED signal received");
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);
	write(socketfd,mg_send2.s_send2,40);

	sprintf(mg_send2.s_send2,"From PID %d",getpid());
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);
	write(socketfd,mg_send2.s_send2,40);
	
	mg_send2.led_send2 = 0;
	sprintf(led,"%d",mg_send2.led_send2);
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Signal: %d", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.led_send2);
	fflush(FP1);
	write(socketfd,led,5);

	gettimeofday(&timestamp2,NULL);
	read(socketfd,mg_receive2.s_receive2,20);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);

	gettimeofday(&timestamp2,NULL);		
	read(socketfd,mg_receive2.s_receive2,20);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);

	for(i=0;i<3;i++)
	{
		gettimeofday(&timestamp2,NULL);
		read(socketfd,mg_receive2.led_receive2,5);
		fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: LED Signal - %s", timestamp2.tv_sec, 
				timestamp2.tv_usec, mg_receive2.led_receive2);
		fflush(FP1);
	}

	gettimeofday(&timestamp2,NULL);
	read(socketfd,mg_receive2.led_receive2,5);	
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: LED Signal - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.led_receive2);
	fflush(FP1);

	strcpy(mg_send2.s_send2,"LED signal received");
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);
	write(socketfd,mg_send2.s_send2,40);

	mg_send2.led_send2 = 1;
	sprintf(led,"%d",mg_send2.led_send2);	
	for(i=0;i<3;i++)
	{
		gettimeofday(&timestamp2,NULL);
		fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Signal: %d", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.led_send2);
		fflush(FP1);
		write(socketfd,led,5);	
	}

	mg_send2.led_send2 = 0;
	sprintf(led,"%d",mg_send2.led_send2);
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: LED Signal: %d", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.led_send2);
	fflush(FP1);
	write(socketfd,led,5);

	gettimeofday(&timestamp2,NULL);		
	read(socketfd,mg_receive2.s_receive2,20);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);

	gettimeofday(&timestamp2,NULL);		
	read(socketfd,mg_receive2.s_receive2,20);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Receiving at Process 2: Message - %s", timestamp2.tv_sec, 
			timestamp2.tv_usec, mg_receive2.s_receive2);
	fflush(FP1);

	strcpy(mg_send2.s_send2,"Affirmative - Communication Ended");
	gettimeofday(&timestamp2,NULL);
	fprintf(FP1,"\n\n[%lu seconds %lu microseconds] Sending from Process 2: Message: %s", timestamp2.tv_sec, timestamp2.tv_usec, mg_send2.s_send2);
	fflush(FP1);
	write(socketfd,mg_send2.s_send2,40);
	

	/*n = read(socketfd,s,20);
	if(n == -1)
	{
		perror("Could not read: ");
		exit(0);
	}
	printf("Message: %s\n",s);*/

	close(socketfd);

	return 0;
}
