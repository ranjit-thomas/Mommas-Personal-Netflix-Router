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
#include <unistd.h>

#define MAXREQUESTSIZE 2048
#define MAXRESPONSESIZE 999999

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
    int sockfd, portno, n, total_nbytes, 
        last_nbytes_received, bytes_left_to_read,
        length_of_header, already_counted;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname, *line_from_response;
    char connect_request[MAXREQUESTSIZE];
    char get_request[MAXREQUESTSIZE];
    char get_response[MAXRESPONSESIZE];
    char get_response_copy[MAXRESPONSESIZE];
    char response_header[MAXREQUESTSIZE];
    char content_length_line[500];

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
    strcpy(connect_request, "CONNECT http://www.cs.tufts.edu/comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n");

    /* send the message line to the server */
    n = write(sockfd, connect_request, strlen(connect_request));
    if (n < 0) error("ERROR writing to socket");

    sleep(5);

    n = write(sockfd, "hello", 5);
    if (n < 0) error("ERROR writing to socket");

    printf("Wrote the second message\n");
    fflush(stdout);

    /* print the server's reply */
    bzero(get_response, MAXRESPONSESIZE);

    total_nbytes = 0;
    last_nbytes_received = 1;

    // read the header
    while ((last_nbytes_received != -1) && (strstr(get_response, "\r\n\r\n") == NULL))
    {
        last_nbytes_received = read(sockfd, get_response + total_nbytes, MAXRESPONSESIZE);
        total_nbytes += last_nbytes_received;
    }

    if (last_nbytes_received < 0) perror ("read");

    // figure out how many bytes after the header have been read
    length_of_header = strstr(get_response, "\r\n\r\n") - get_response + 4;
    already_counted = strlen(get_response) - length_of_header;

    strcpy(get_response_copy, get_response);

    // iterate over header and fine the length_line field
    line_from_response = strtok(get_response_copy, "\r\n");
    strcpy(content_length_line, "");

    while (line_from_response != NULL)
    {
        if (strstr(line_from_response, "Content-Length: ")) strcpy(content_length_line, line_from_response + 16);
        line_from_response = strtok(NULL, "\r\n");
    }

    // read until there's nothing left to read
    bytes_left_to_read = atoi(content_length_line) - already_counted;
    last_nbytes_received = 1;

    while ((last_nbytes_received != -1) && bytes_left_to_read > 0) 
    {
        last_nbytes_received = read(sockfd, get_response + total_nbytes, bytes_left_to_read);
        total_nbytes += last_nbytes_received;
        bytes_left_to_read -= last_nbytes_received;
    }

    // need to trigger an error if calling read caused an error on last call
    if (last_nbytes_received < 0) perror ("read");

    printf("%s", get_response);
    close(sockfd);

    return 0;
}