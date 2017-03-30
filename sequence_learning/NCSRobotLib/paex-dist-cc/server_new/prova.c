#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 


int main() {
	struct sockaddr_in saddr,caddr; 
	socklen_t clen;
	int sock, csock, reuse = 1, ret=0;
	socklen_t ntrcv, ntsnd;
	struct timeval tout, tsnd, trcv;

	   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) perror("failed to create socket");
		 else {
				      if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) perror("failed allowing server socket to reuse address");
						  else {
								memset((char *) &saddr, 0, sizeof(saddr));
							  saddr.sin_family = AF_INET;
								saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
													          saddr.sin_port = htons(45454);		
