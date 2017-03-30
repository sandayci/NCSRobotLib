#ifndef TCP_CONNECTOR_H
#define TCP_CONNECTOR_H

/*! \file TCPConnector.h
    \brief All the functions managing communications over TCP/IP are defined here.
*/

/*! \fn int getTCPFileDescriptor(const char* hostname, unsigned int portno)
    \brief Opens a file descriptor to a TCP socket.
    \param hostname The name of the device we want to connect to (IP addess for example)
    \param portno Number of the port we want to connect to.
    \returns if successful, a File Descriptor to the opened devive, else -1.
*/

int getTCPFileDescriptor( const char* hostname, unsigned int portno )
{
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  {
			std::cout<<"ERROR opening socket"<<std::endl;
        return -1;
    }
    server = gethostbyname(hostname);
    if (server == NULL) {
			std::cout<<"ERROR, no such host"<<std::endl;
        return -1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
            (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        return -1;

    return sockfd;
}
#endif
