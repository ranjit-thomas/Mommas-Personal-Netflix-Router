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
#include "serverData.h"
#include "requestStruct.h"

#define MAXREQUESTSIZE 2048

typedef struct connect_socket
{
    int client;
    int server;
    struct connect_socket *next;
} connect_socket;

server_data* process_get_request(char http_request [], char get_request_line [], char hostname [], int portno);

void add_to_connected_sockets(connect_socket** head_of_list, int client_socket, int server_socket)
{
    connect_socket** current_socket;

    printf("Adding client socked %d and server socket %d\n", client_socket, server_socket);
    current_socket = head_of_list;
    while (*current_socket != NULL) *current_socket = (*current_socket)->next;

    *current_socket = malloc(sizeof(connect_socket));
    (*current_socket)->client = client_socket;
    (*current_socket)->server = server_socket;
    (*current_socket)->next = NULL;
}

connect_socket* find_connect_socket(connect_socket* head_of_list, int node_socket)
{
    connect_socket* current_socket;

    printf("Looking for nocde socket %d\n", node_socket);
    current_socket = head_of_list;
    while (current_socket != NULL && current_socket->client != node_socket && current_socket->server != node_socket) current_socket = current_socket->next;

    if (current_socket == NULL) printf("Couldn't find anything\n");
    return current_socket;
}

request_struct* read_from_client (int filedes)
{
    char http_request[MAXREQUESTSIZE], http_request_copy[MAXREQUESTSIZE];
    int total_nbytes, last_nbytes_received, portno_int;
    char hostname_line[MAXREQUESTSIZE];
    char *line_from_request, *get_request_line, *portno_string, *hostname;

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

    /*printf("%d bytes were read\n", total_nbytes);
    fflush(stdout);*/

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

        // fourth step: get portnumber if specified
        if (strstr(hostname_line, ":"))
        {
            hostname = strtok(hostname_line, ":");
            portno_string = strtok(NULL, "\0");
            portno_int = atoi(portno_string);
        }

        // fourth step (cont.): or else set to default
        else
        {
            hostname = strtok(hostname_line, "\0");
            portno_int = 80;
        }

        /*printf("the get_request_line was %s \nthe hostname was %s\nthe portno is %d\n", 
            get_request_line, hostname, portno_int);
        fflush(stdout);*/

        if (strstr(get_request_line, "GET"))
        {
            server_data *data_to_return;
            int bytes_to_write;

            data_to_return = process_get_request(http_request_copy, get_request_line, hostname, portno_int);
            bytes_to_write = data_to_return->total_bytes;

            // write until there's nothing left to write
            total_nbytes = 0;
            last_nbytes_received = 1;

            while (last_nbytes_received != -1 && bytes_to_write > 0) 
            {
                last_nbytes_received = write(filedes, data_to_return->data + total_nbytes, bytes_to_write);
                total_nbytes += last_nbytes_received;
                bytes_to_write -= last_nbytes_received;
            }

            // need to trigger an error if calling write caused an error on last call
            if (last_nbytes_received < 0)
            {
                perror ("read");
                exit (EXIT_FAILURE);
            }

            // free memory for storage_unit
            free(data_to_return);
            return NULL;
        }

        else if (strstr(get_request_line, "CONNECT"))
        {
            request_struct *server_info;

            server_info = malloc(sizeof(request_struct));
            strcpy(server_info->hostname, hostname);
            server_info->portno = portno_int;

            return server_info;
        }

        /* there is nothing in the "else" condition because we're only
           looking for "GET" and "REQUEST" connects */



        // MIGHT NEED TO CHANGE LATER: return 1 since we're done with this connection
        return NULL;
    }
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
    connect_socket* connected_sockets;
    connect_socket* set_connect_socket;

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
    connected_sockets = NULL;

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

        printf("before select call\n");
        fflush(stdout);
        /* Block until input arrives on one or more active sockets. */
        if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
          perror ("select");
          exit (EXIT_FAILURE);
        }

        printf("after select call\n");
        fflush(stdout);

        // iterate over all sockets in the "read_fd_set" list
        for (i = 0; i < FD_SETSIZE; ++i)
        {
            // if input arrived on a given socket, proceed
            if (FD_ISSET (i, &read_fd_set))
            {
                // figure out if this is from a node that is currently CONNECT-ed to another node
                set_connect_socket = find_connect_socket(connected_sockets, i);

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
                else if (set_connect_socket != NULL)
                {
                    printf("yayyyyy\n");
                    fflush(stdout);
                }

                /* if a file descriptor that doesnt represent "main" socket is set,
                   read whatever data was recieved */
                else
                {
                    printf("should only see this once i think\n");
                    fflush(stdout);

                    request_struct *server_info;

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
                        int new_connect;
                        struct hostent *server;

                        new_connect = socket(AF_INET, SOCK_STREAM, 0);
                        if (new_connect < 0) 
                        {
                            perror("ERROR opening socket");
                            exit (EXIT_FAILURE);
                        }

                        server = gethostbyname(server_info->hostname);
                        if (server == NULL) 
                        {
                            fprintf(stderr,"ERROR, no such host\n");
                            exit(0);
                        }

                        /* set up the server_addr so we can set up a "client-side" socket that
                           that can connect to the server we need to query */
                        bzero((char *) &servername, sizeof(servername));
                        servername.sin_family = AF_INET;
                        bcopy((char *)server->h_addr, 
                             (char *)&servername.sin_addr.s_addr,
                             server->h_length);
                        servername.sin_port = htons(server_info->portno);

                        if (connect(new_connect,(struct sockaddr *) &servername,sizeof(servername)) < 0) 
                        {
                            fprintf(stderr,"ERROR, no such host\n");
                            exit(0);
                        }

                        // set the file descriptor to "on" in the 
                        FD_SET (new_connect, &active_fd_set);

                        // add both the client and server socket to the linkedList that has all Connect sockets
                        add_to_connected_sockets(&connected_sockets, i, new_connect);
                        if (connected_sockets == NULL) printf("we never added anything!!!\n");

                        printf("Should have added to the linked list here\n");
                        fflush(stdout);
                    }
                }
            }
        }
    }
}