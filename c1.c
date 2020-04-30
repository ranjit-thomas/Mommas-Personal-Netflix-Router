/* 
* tcpclient.c - A simple TCP client
* usage: tcpclient <host> <port>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define MAXMSGSIZE 1024

/* 
* error - wrapper for perror
*/
void error(char *msg) 
{
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) 
{
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char getRequest[MAXMSGSIZE];

    /* check command line arguments */
    if (argc != 3) 
    {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }

    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) 
    {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
      (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) error("ERROR connecting");

    // hard-coded easy getRequest example
    strcpy(getRequest, "GET http://www.cs.tufts.edu/comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n");

    /* send the message line to the server */
    n = write(sockfd, getRequest, strlen(getRequest));

    printf("wrote %d characters\n", n);
    if (n < 0) error("ERROR writing to socket");

    // uncomment this shit when I"m ready to start reading shit back from server

    /* print the server's reply */
    /*bzero(buf, BUFSIZE);
    n = read(sockfd, buf, BUFSIZE);
    if (n < 0) error("ERROR reading from socket");
    printf("Echo from server: %s", buf);*/
    close(sockfd);
    return 0;
}