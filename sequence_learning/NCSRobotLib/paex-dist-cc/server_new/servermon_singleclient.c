/*
    C server stream events to port 
*/
 
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
#define PORT_MON 5000

uint32_t GetTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint32_t)1000000+tv.tv_usec;
}


int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
		int i, tmp, rd_addr; 
		char recv_client_msg[100];

    
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

		//
		int yes = 1;
		if ( setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
		{
			perror("setsockopt");
		}
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
    
while(1)
	{
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
   

		//read events forever 
		rd_addr = aerFindHead();
		int counter = 0;
		int readval = 0;
		char * ptr;
		
		while(1){
		 i = aerReadMem(2*rd_addr);
		 //printf("prima: %d", i);
		 if (i != 0xffffffff) {
			rd_addr = (rd_addr+1)%AER_BUFFER_SIZE;
			counter = GetTimeStamp();
	    //printf("i %d", i);	
		  write(client_sock, &counter, sizeof(counter));
		  write(client_sock , &i , sizeof(i));
		 }
		 //check socket connection
  	 if ((readval = read(socket_desc, recv_client_msg, sizeof(recv_client_msg))) == 0)
		  {
        printf("disconnected %d", readval);
				break;
			}

		}
		
	}
		return 0;
}
