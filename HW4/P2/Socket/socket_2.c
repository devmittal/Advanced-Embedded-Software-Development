#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	struct sockaddr_in socket_str;
	int portno;
	char buffer[256] = "Sending data from client. Test 1..2..3...";
	char s[20];
	int n;
	int socketfd;

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

	n = write(socketfd,buffer,256);
	if(n == -1)
	{
		perror("Could not write: ");
		exit(0);
	}

	n = read(socketfd,s,20);
	if(n == -1)
	{
		perror("Could not read: ");
		exit(0);
	}
	printf("Message: %s\n",s);

	close(socketfd);

	return 0;
}
