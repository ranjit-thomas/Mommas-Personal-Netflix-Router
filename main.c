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

#define MAXREQUESTSIZE 2048

storage_unit *process_get_request(char http_request [], char get_request_line [], char hostname [], int portno);

int read_from_client (int filedes)
{
    char http_request[MAXREQUESTSIZE], http_request_copy[MAXREQUESTSIZE];
    int total_nbytes, last_nbytes_received, portno_int;
    char hostname_line[MAXREQUESTSIZE];
    char *line_from_request, *get_request_line, *portno_string, *hostname;

    // read until there's nothing left to read
    total_nbytes = 0;
    last_nbytes_received = 1;
    while (last_nbytes_received > 0) 
    {
        last_nbytes_received = read(filedes, http_request + total_nbytes, MAXREQUESTSIZE);
        total_nbytes += last_nbytes_received;
    }

    // need to trigger an error if calling read caused an error on last call
    if (last_nbytes_received < 0)
    {
        perror ("read");
        exit (EXIT_FAILURE);
    }

    // n > 0 ==> data was read
    if (total_nbytes > 0)
    {
        // copy the original request for future use
        strcpy(http_request_copy, http_request);

        printf("%d bytes were read\n", total_nbytes);
        fflush(stdout);

        // parse the http request... only concerned with request-line and hostName

        /* first step: initialize the strings that will hold the importnt
           fields to be empty */
        strcpy(hostname_line, "");

        /* second step: get first line as the first line of HTTP request header
         is always the request-line */
        get_request_line = strtok(http_request, "\n");

        // third step: iterate over each line and find the field that has hostname
        line_from_request = strtok(NULL, "\n");

        while (line_from_request != NULL)
        {
            if (strstr(line_from_request, "Host: ")) strcpy(hostname_line, line_from_request + 6);
            line_from_request = strtok(NULL, "\n");
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

        printf("the get_request_line was %s \nthe hostname was %s\nthe portno is %d\n", 
            get_request_line, hostname, portno_int);
        fflush(stdout);

        if (strstr(get_request_line, "GET"))
        {
            process_get_request(http_request_copy, get_request_line, hostname, portno_int);
        }

        else if (strstr(get_request_line, "CONNECT"))
        {

        }

        /* there is nothing in the "else" condition because we're only
           looking for "GET" and "REQUEST" connects */



        // MIGHT NEED TO CHANGE LATER: return 1 since we're done with this connection
        return 1;
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
    struct sockaddr_in clientname;
    size_t size;

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

                /* if a file descriptor that doesnt represent "main" socket is set,
                   read whatever data was recieved */
                else
                {
                    /* call "read_from_client" function, and clear the file_descriptor's
                       value in the set of active sockets if we recieve 0 (i.e. done reading) */ 
                    if (read_from_client (i))
                    {
                        close (i);
                        FD_CLR (i, &active_fd_set);
                    }
                }
            }
        }
    }
}