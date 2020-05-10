#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include "storageUnit.h"
#include "requestStruct.h"
#include "connectSocket.h"
#include "dataChunk.h"

#define MAXREQUESTSIZE 2048
#define MAXRESPONSESIZE 9999
#define CONTENTLENGTHLINESIZE 500

data_chunk** process_get_request(char http_request [], char hostname [], int portno) 
{
    int sockfd, total_nbytes, last_nbytes_received,
            length_of_header, length_of_body_read;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char http_response_body [MAXRESPONSESIZE];
    char http_response_header [MAXRESPONSESIZE];
    char http_response_header_copy [MAXRESPONSESIZE];
    char *line_from_response;
    data_chunk **head_of_list;
    char content_length_line [CONTENTLENGTHLINESIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    server = gethostbyname(hostname);
    if (server == NULL) 
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    /* set up the server_addr so we can set up a "client-side" socket that
       that can connect to the server we need to query */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    total_nbytes = write(sockfd, http_request, strlen(http_request));
    if (total_nbytes < 0)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }


    head_of_list = malloc(sizeof(data_chunk *));
    *head_of_list = NULL;

    // read from server untill we got the entire http_response_header
    bzero(http_response_header, MAXRESPONSESIZE);
    total_nbytes = 0;
    last_nbytes_received = 1;

    while (last_nbytes_received != -1 && strstr(http_response_header, "\r\n\r\n") == NULL)
    {
        last_nbytes_received = read(sockfd, http_response_header + total_nbytes, MAXREQUESTSIZE);
        total_nbytes += last_nbytes_received;
    }

    if (last_nbytes_received == -1)
    {
        perror ("read");
        exit (EXIT_FAILURE);
    }

    length_of_header = strstr(http_response_header, "\r\n\r\n") - http_response_header + 4;
    length_of_body_read = total_nbytes - length_of_header;
    bzero(http_response_body, MAXRESPONSESIZE);
    memcpy(http_response_body, http_response_header + length_of_header, length_of_body_read);
    bzero(http_response_header + length_of_header, length_of_body_read);
    head_of_list = add_to_data_list(head_of_list, http_response_header, length_of_header);
    head_of_list = add_to_data_list(head_of_list, http_response_body, length_of_body_read);

    //parse the header to figure out the size of the data
    bzero(http_response_header_copy, MAXRESPONSESIZE);
    memcpy(http_response_header_copy, http_response_header, length_of_header);
    line_from_response = strtok(http_response_header_copy, "\r\n");
    strcpy(content_length_line, "");

    while (line_from_response != NULL)
    {
        if (strstr(line_from_response, "Content-Length: ")) strcpy(content_length_line, line_from_response + 16);
        line_from_response = strtok(NULL, "\r\n");
    }



    // read from server until there's nothing left to read
    total_nbytes = atoi(content_length_line) - length_of_body_read;

    last_nbytes_received = 1;
    while (last_nbytes_received > 0 && total_nbytes) 
    {
        bzero(http_response_body, MAXRESPONSESIZE);
        last_nbytes_received = read(sockfd, http_response_body, MAXRESPONSESIZE - 1);
        total_nbytes -= last_nbytes_received;
        if (last_nbytes_received > 0) head_of_list = add_to_data_list(head_of_list, http_response_body, last_nbytes_received);
    }

    // need to trigger an error if calling read caused an error on last call
    if (last_nbytes_received < 0)
    {
        perror ("read");
        exit (EXIT_FAILURE);
    }

    close(sockfd);
    return head_of_list;
}

request_struct** read_from_client (int filedes)
{
    char http_request[MAXREQUESTSIZE], http_request_copy[MAXREQUESTSIZE];
    int total_nbytes, last_nbytes_received, portno_int, length_of_header, last_nbytes_sent;
    char hostname_line[MAXREQUESTSIZE];
    char *line_from_request, *get_request_line, *portno_string, *hostname;
    data_chunk **head_of_list, **old_head_of_list;

    // read until there's nothing left to read
    total_nbytes = 0;
    last_nbytes_received = 1;

    while (last_nbytes_received != -1 && strstr(http_request, "\r\n\r\n") == NULL)
    {
        last_nbytes_received = read(filedes, http_request + total_nbytes, MAXREQUESTSIZE);
        total_nbytes += last_nbytes_received;
    }

    if (last_nbytes_received < 0)
    {
        perror ("read");
        exit (EXIT_FAILURE);
    }

    // figure out how many bytes after the header have been read, and clear all that
    length_of_header = strstr(http_request, "\r\n\r\n") - http_request + 4;
    bzero(http_request + length_of_header, sizeof(http_request) - length_of_header);

    // n > 0 ==> data was read
    if (total_nbytes > 0)
    {
        // copy the original request for future use
        strcpy(http_request_copy, http_request);

        // parse the http request... only concerned with request-line and hostName

        /* first step: initialize the strings that will hold the importnt
           fields to be empty */
        strcpy(hostname_line, "");

        /* second step: get first line as the first line of HTTP request header
         is always the request-line */
        get_request_line = strtok(http_request, "\r\n");

        // third step: iterate over each line and find the field that has hostname
        line_from_request = strtok(NULL, "\r\n");

        while (line_from_request != NULL)
        {
            if (strstr(line_from_request, "Host: ")) strcpy(hostname_line, line_from_request + 6);
            line_from_request = strtok(NULL, "\r\n");
        }

        // a nonvalid portnumber that will be set either explciitly by client, or in the GET/CONNECT if statements
        portno_int = -999;

        // fourth step: get portnumber if specified
        if (strstr(hostname_line, ":"))
        {
            hostname = strtok(hostname_line, ":");
            portno_string = strtok(NULL, "\r\n");
            portno_int = atoi(portno_string);
        }

        // fourth step (cont.): or else set to default
        else hostname = strtok(hostname_line, "\r\n");

        if (strstr(get_request_line, "GET")) 
        {
            if (portno_int == -999) portno_int = 80;

            head_of_list = process_get_request(http_request_copy, hostname, portno_int);

            total_nbytes = 0;
            last_nbytes_sent = 1;
            while (last_nbytes_sent > 0 && *head_of_list != NULL)
            {
                old_head_of_list = remove_head_of_data_list(head_of_list);
                last_nbytes_sent = write(filedes, (*old_head_of_list)->data_content, (*old_head_of_list)->data_size);
                free(*old_head_of_list);
                free(old_head_of_list);
            }

            if (last_nbytes_sent < 0)
            {
                perror ("write");
                exit (EXIT_FAILURE);
            }
        }

        else if (strstr(get_request_line, "CONNECT"))
        {
            if (portno_int == -999) portno_int = 443;

            request_struct **server_info;

            server_info = malloc(sizeof(request_struct *));
            *server_info = malloc(sizeof(request_struct));
            strcpy((*server_info)->hostname, hostname);
            (*server_info)->portno = portno_int;

            //while (*head_of_list != NULL) remove_head_of_data_list(head_of_list);

            return server_info;
        }

        else return NULL;
    }

    return NULL;
}

int make_socket (uint16_t port)
{
    int sock;
    struct sockaddr_in name;

    /* Create the socket. */
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    /* Give the socket a name. */
    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl (INADDR_ANY);
    
    if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0)
    {
        perror ("bind");
        exit (EXIT_FAILURE);
    }

    return sock;
}

int main (int argc, char **argv)
{
    extern int make_socket (uint16_t port);
    int sock;
    fd_set active_fd_set, read_fd_set;
    int i;
    struct sockaddr_in clientname, servername;
    size_t size;
    connect_socket** connected_sockets;
    connect_socket** set_connect_socket;

    // check that an argument for port number was supplied
    if (argc != 2) 
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    /* Create the socket and set it up to accept connections. */
    sock = make_socket (atoi(argv[1]));
    if (listen (sock, 1) < 0)
    {
        perror ("listen");
        exit (EXIT_FAILURE);
    }

    // initialize an empty linkedList of connected sockets
    connected_sockets = malloc(sizeof(connect_socket *));
    *connected_sockets = NULL;

    /* Initialize the set of active sockets. */
    // all sockets are being set off since there should be no active sockets at begining...
    FD_ZERO (&active_fd_set);
    // ... with the exception of the socket through which the proxy is hearing the requests
    FD_SET (sock, &active_fd_set);

    // loop should continue forever
    while (1)
    {
        // reset the fd_set to be off for all sockets (except the one proxy recieves on)
        read_fd_set = active_fd_set;

        /* Block until input arrives on one or more active sockets. */
        if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
          perror ("select");
          exit (EXIT_FAILURE);
        }

        // iterate over all sockets in the "read_fd_set" list
        for (i = 0; i < FD_SETSIZE; ++i)
        {
            // if input arrived on a given socket, proceed
            if (FD_ISSET (i, &read_fd_set))
            {
                // figure out if this is from a node that is currently CONNECT-ed to another node
                if (i != sock) set_connect_socket = find_connect_socket(connected_sockets, i);

                /* if a connection request from a new client came to our "main" socket,
                   set that socket */ 
                if (i == sock)
                {
                    int new;
                    size = sizeof (clientname);

                    // create a new socket and return the int that is the file descriptor
                    new = accept (sock,
                                  (struct sockaddr *) &clientname,
                                  &size);

                    // if there was an error on "accept" call, say so
                    if (new < 0)
                    {
                        perror ("accept");
                        exit (EXIT_FAILURE);
                    }

                    fprintf (stderr,
                             "Server: connect from host %s, port %hd.\n",
                             inet_ntoa (clientname.sin_addr),
                             ntohs (clientname.sin_port));

                    // set the file descriptor to "on" in the 
                    FD_SET (new, &active_fd_set);

                }

                // condition where the socket is currently CONNECT-ed to another
                else if (*set_connect_socket != NULL)
                {
                    int sender, reciever, bytes_read, bytes_sent;
                    char data_to_exchange[MAXRESPONSESIZE];
                    char data_to_exchange_copy[MAXRESPONSESIZE];

                    if ((*set_connect_socket)->client == i)
                    {
                        sender = (*set_connect_socket)->client;
                        reciever = (*set_connect_socket)->server;
                    }

                    else
                    {
                        sender = (*set_connect_socket)->server;
                        reciever = (*set_connect_socket)->client;
                    }                        

                    bzero(data_to_exchange, MAXRESPONSESIZE);
                    bzero(data_to_exchange_copy, MAXRESPONSESIZE);

                    // read from sender
                    bytes_read = read(sender, data_to_exchange, MAXRESPONSESIZE);

                    if (bytes_read < 0)
                    {
                        perror ("read");
                        exit (EXIT_FAILURE);
                    }

                    if (bytes_read == 0)
                    {
                        close(sender);
                        FD_CLR(sender, &active_fd_set);
                        close(reciever);
                        FD_CLR(reciever, &active_fd_set);

                        connected_sockets = remove_from_connected_sockets(connected_sockets, set_connect_socket);
                    }
                    else
                    {
                        memcpy(data_to_exchange_copy, data_to_exchange, bytes_read);
                        // if there were any bytes read, send that to reciever
                        bytes_sent = write(reciever, data_to_exchange_copy, bytes_read);

                        if (bytes_sent < 0)
                        {
                            perror ("write");
                            exit (EXIT_FAILURE);
                        }
                    }
                }

                /* if a file descriptor that doesnt represent "main" socket is set,
                   read whatever data was recieved */
                else
                {
                    request_struct **server_info;

                    server_info = read_from_client(i);

                    /* call "read_from_client" function, and clear the file_descriptor's
                       value in the set of active sockets if we recieve 0 (i.e. done reading) */ 
                    if (server_info == NULL)
                    {
                        close (i);
                        FD_CLR (i, &active_fd_set);
                    }

                    /* if the function returned something nonNULL, we assume that I "CONNECT" request
                       was recieved */
                    else
                    {
                        int new_connect, bytes_sent;
                        struct hostent **server;

                        char success_response[] = "HTTP/1.1 200 OK\r\n\r\n";

                        server = malloc(sizeof(struct hostent *));

                        new_connect = socket(AF_INET, SOCK_STREAM, 0);
                        if (new_connect < 0) 
                        {
                            perror("ERROR opening socket");
                            exit (EXIT_FAILURE);
                        }

                        *server = gethostbyname((*server_info)->hostname);
                        if (*server == NULL) 
                        {
                            fprintf(stderr,"ERROR, no such host\n");
                            exit(0);
                        }

                        /*if (strcmp((*server_info)->hostname, "www.google.com") && strcmp((*server_info)->hostname, "www.bing.com"))
                        {
                            close (i);
                            FD_CLR (i, &active_fd_set);
                            continue;
                        }*/

                        /* set up the server_addr so we can set up a "client-side" socket that
                           that can connect to the server we need to query */
                        bzero((char *) &servername, sizeof(servername));
                        servername.sin_family = AF_INET;
                        bcopy((char *)(*server)->h_addr, 
                             (char *)&servername.sin_addr.s_addr,
                             (*server)->h_length);
                        servername.sin_port = htons((*server_info)->portno);

                        if (connect(new_connect,(struct sockaddr *) &servername,sizeof(servername)) < 0) 
                        {
                            fprintf(stderr,"ERROR, no such host\n");
                            exit(0);
                        }

                        // free memory
                        free(server_info);
                        free(server);

                        // set the file descriptor to "on" in the 
                        FD_SET (new_connect, &active_fd_set);

                        // send success status code
                        bytes_sent = write(i, success_response, sizeof(success_response));
                        if (bytes_sent < 0)
                        {
                            perror ("write");
                            exit (EXIT_FAILURE);
                        }

                        connected_sockets = add_to_connected_sockets(connected_sockets, i, new_connect);
                    }
                }
            }
        }
    }
}