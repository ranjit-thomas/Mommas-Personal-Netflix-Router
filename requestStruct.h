#ifndef REQUESTSTRUCT_H
#define REQUESTSTRUCT_H

#define MAXREQUESTSIZE 2048

typedef struct request_struct
{
	char get_request[MAXREQUESTSIZE];
	char hostname[MAXREQUESTSIZE];
	int portno;
} request_struct;

#endif