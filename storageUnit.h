#ifndef STORAGEUNIT_H
#define STORAGEUNIT_H

#define MAXRESPONSESIZE 999999

typedef struct storage_unit
{
	char* key;
	char data_from_server[MAXRESPONSESIZE];
	struct storage_unit* next;
} storage_unit;

storage_unit *process_get_request(char http_request [], char get_request_line [], char hostname [], int portno);

#endif