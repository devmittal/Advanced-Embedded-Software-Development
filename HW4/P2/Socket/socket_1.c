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
	struct sockaddr_in socket_str, cli_addr;
	int portno, clilen;
	int client_sockfd;
	char buffer[256];
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

	bzero(buffer,256);

	n = read(client_sockfd,buffer,256);
	if(n == -1)
	{
		perror("Could not read: ");
		exit(0);
	}

	printf("Message: %s\n",buffer);

	n = write(client_sockfd,"I got your message",18);
	if(n == -1)
	{
		perror("Could not write: ");
		exit(0);
	}

	close(socketfd);

	return 0;
}
