#ifndef STORAGEUNIT_H
#define STORAGEUNIT_H

#include "requestStruct.h"

#define MAXRESPONSESIZE 9999

typedef struct storage_unit
{
	request_struct *key;
	char data_from_server[MAXRESPONSESIZE];
	int size_of_data;
	struct storage_unit *next;
} storage_unit;

#endif