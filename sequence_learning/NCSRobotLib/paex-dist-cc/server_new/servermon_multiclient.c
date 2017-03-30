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
#define AER_BUFFER_SIZE 64000
#define PORT_MON 5000
//#define MUL 0.4
#define MUL 1

void *connection_handler(void *);

uint32_t GetTimeStamp() {
	     struct timeval tv;
       gettimeofday(&tv,NULL);
	     return (tv.tv_sec*(uint32_t)1000000+tv.tv_usec)*MUL;
}


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
		       sizeof(int)) == -1){
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



void *connection_handler(void *socket_desc)
{
		int fd = (int)socket_desc;
		pthread_detach(pthread_self());

		//Get the socket descriptor
		int sock = *(int*)socket_desc;
		int i,rd_addr,read_size;
		char *message , client_message[4096];
		int ww;
		char *valueBias;
		char *valueAer;
		int counter = 0;
		int readval = 1;
		char recv_client_msg[100];
		rd_addr = aerFindHead();

  	aerOpenDevice();

		while(1){  
		 i = aerReadMem(2*rd_addr);
		 if(i!=0xffffffff){ 
			rd_addr = (rd_addr+1)%AER_BUFFER_SIZE; 
   	 	//printf("%d\n", i);
			counter = GetTimeStamp();
			write(sock, &counter, sizeof(counter));
			write(sock , &i , sizeof(i));
		 }
		}

		close(fd);
		pthread_exit(NULL);
		return 0;
}


