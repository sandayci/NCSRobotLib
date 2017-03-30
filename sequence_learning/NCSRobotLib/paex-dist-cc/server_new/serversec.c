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
 
#include "comm/aerctl.h"
#define AER_BUFFER_SIZE 64000
#define PORT_MON 5002


int main(int argc , char *argv[])
	{
		    int socket_desc , client_sock , c , read_size;
				struct sockaddr_in server , client;
				bzero((char *) &server,sizeof(server));
				int ww,i;
				char client_message[4096];
				char *valueBias; 
				valueBias = (char*)malloc(4095);					
				char *valueAer; 
				valueAer = (char*)malloc(4096);

				//open device for reading events
				aerOpenDevice();

				//Create socket
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

				//Bind
				if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
				  {
				    //print the error message
				    perror("bind failed. Error");
				    return 1;
				  }
				puts("bind done");

				//Listen
				listen(socket_desc , 3);

				//Accept and incoming connection
				puts("Waiting for incoming connections...");
				c = sizeof(struct sockaddr_in);

        //accept connection from an incoming client
				client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
				if (client_sock < 0)
				 {
				    perror("accept failed");
				    return 1;
				 }
				puts("Connection accepted");
				
				//write event
				while(1){
					read_size = recv(client_sock , client_message , sizeof(client_message) , 0);
					//read_size = recv(client_sock , client_message , 4096 , 0);
					//write(client_sock , client_message , 2000);
					write(client_sock , client_message , strlen(client_message));
					if (client_message[0] =='B')
					{
						valueBias =  &client_message[1];
						ww = (int)valueBias;
						BGSet(atoi(valueBias));
						client_message[read_size] = '\0';
						printf("Message: %s\n", valueBias);
				  
					}	
					else
					{
						client_message[read_size] = '\0';
						valueAer =  &client_message[0];
						printf("message received: %s\n", client_message);
						printf("message aer: %d\n", (atoi(valueAer)));
					  aerWriteBits(atoi(valueAer));
						//usleep(1000);
						client_message[read_size] = '\0';
						//printf("Message: %d\n", atoi(valueAer));
					}
					
				}
}	
