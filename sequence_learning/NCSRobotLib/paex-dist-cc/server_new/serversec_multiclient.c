#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <libwebsockets.h>
#include <paex.h>
#include <pthread.h> //for threading 

#include "comm/aerctl.h"
#define PORT_MON 5002

void *connection_handler(void *);

int main(int argc , char *argv[])
{

	int socket_desc , client_sock , c , *new_sock;
	int yes =1;
	struct sockaddr_in server , client;
	struct timeval timeout = {5, 0};
	struct linger lng = {1, 5};

	aerOpenDevice();

	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");	        

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( PORT_MON );
	//memset(&server, 0, sizeof(server));


	//
	if ( setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
	{
		perror("setsockopt");
	}


	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	listen(socket_desc , 3);

	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	if (setsockopt(socket_desc, 
				SOL_SOCKET, 
				SO_REUSEADDR, 
				&yes, 
				sizeof(int)) == -1)
	{	
		perror("setsockopt");
		exit(1);
	} 

	//setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO,
	//		   &timeout, sizeof(struct timeval));
	setsockopt(socket_desc, SOL_SOCKET, SO_SNDTIMEO,
			&timeout, sizeof(struct timeval));
	setsockopt(socket_desc, SOL_SOCKET, SO_LINGER,
			&lng, sizeof(struct linger));

	pthread_t sniffer_thread;



	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");

		//pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;

		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			continue;
		} 
		puts("Handler assigned");
	}


	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;  
	}
	pthread_join(sniffer_thread,NULL);		 
	return 0;
}		


void substring(char* to, char* from, int length) 
{
	memcpy(to, from, length);
	to[length]='\0';
}

void *connection_handler(void *socket_desc)
{
	int fd = (int)socket_desc;
	pthread_detach(pthread_self());

	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	int messageLength;
	int messageStart;
	char client_messages[4096];
	char parsed_message[200];
	int ww;
	char valueAer;
	char aerLen;
	char* token;
	int c,i = 0;

//	aerOpenDevice();

	while(1) 
	{  
		read_size = recv(sock , client_messages , sizeof(client_messages) , 0);
		printf("Received: %s\n", client_messages);
		messageLength = strlen(client_messages);
		if (client_messages[0] == 'B' || client_messages[0] == 'M') 
		{
			//printf("Received a valid message\n");
			messageStart = 0;
			for (i = 0 ; i < messageLength; ++i) 
			{
				if (client_messages[i] == '.') 
				{
						substring(parsed_message, client_messages + messageStart, i-messageStart);
						//printf("Writing back: %s , %d \n", parsed_message, messageLength-1);
						write(sock, parsed_message, strlen(parsed_message));
						messageStart = i+1;
						if (parsed_message[0] =='B')
						{
							BGSet(atoi(parsed_message+1));
							printf("Stting bias: %s\n", parsed_message);
						}
						else
						{
							token = strtok(parsed_message, "#");
							while(token != NULL)
							{
								aerWriteBits(atoi(token));
								token = strtok(NULL,"#");
							}
						}
					}
				}
			}
		}
		close(fd);
		pthread_exit(NULL);
		return 0;
}


