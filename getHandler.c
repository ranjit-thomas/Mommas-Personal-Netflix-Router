#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "storageUnit.h"
#include "serverData.h"

#define MAXREQUESTSIZE 2048
#define MAXRESPONSESIZE 9999

storage_unit* isInCache()
{
	return NULL;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
/*
server_data* process_get_request(char http_request [], char hostname [], int portno) 
{
	int sockfd, total_nbytes, last_nbytes_received;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char http_response [MAXRESPONSESIZE];
	storage_unit* data_found;
	server_data* data_to_return;

	data_found = isInCache();

	// if we found the content in a cache, use it
	if (data_found != NULL)
	{
		data_to_return = NULL;
	}

	// else, query the server directly
	else
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) error("ERROR opening socket");

		server = gethostbyname(hostname);
		if (server == NULL) 
		{
    		fprintf(stderr,"ERROR, no such host\n");
    		exit(0);
		}

		// set up the server_addr so we can set up a "client-side" socket that
		// that can connect to the server we need to query
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, 
		     (char *)&serv_addr.sin_addr.s_addr,
		     server->h_length);
		serv_addr.sin_port = htons(portno);

		printf("Hostname is %s and portno is %d\n", hostname, portno);
		fflush(stdout);
		
		if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");

		total_nbytes = write(sockfd, http_request, strlen(http_request));
		if (total_nbytes < 0) error("ERROR writing to socket");

		// printf("Just wrote %d bytes in the message %s\n", total_nbytes, http_request);

		bzero(http_response, MAXRESPONSESIZE);

	    // read from server until there's nothing left to read
	    total_nbytes = 0;
	    last_nbytes_received = 1;
	    while (last_nbytes_received > 0) 
	    {
	        last_nbytes_received = read(sockfd, http_response + total_nbytes, MAXRESPONSESIZE);
	        total_nbytes += last_nbytes_received;
	        // printf("Just read %d bytes making our grand total %d\n", last_nbytes_received, total_nbytes);
	        // fflush(stdout);
	    }

	    //printf("Just read %d bytes in the message%s\n", total_nbytes, http_response);

	    // need to trigger an error if calling read caused an error on last call
	    if (last_nbytes_received < 0)
	    {
	        perror ("read");
	        exit (EXIT_FAILURE);
	    }

	    //printf("Just read %s\n", http_response);

	    data_to_return = malloc(sizeof(server_data));
	    strcpy(data_to_return->data, http_response);
	    data_to_return->total_bytes = total_nbytes;

	    close(sockfd);
	}

	return data_to_return;
}*/