#ifndef PEERDICTIONARY_H
#define PEERDICTIONARY_H

#include "storageUnit.h"

typedef struct peer_dictionary
{
	storage_unit** hash_array;
	int current_size;
} peer_dictionary;

typedef struct request_struct
{
	char* get_request;
	char* hostname;
	int portno;
} request_struct;

// constructor
peer_dictionary* new_hash_table();

// insert function
void table_insert(peer_dictionary* dictionary, request_struct* request_data, char* data_from_server);

// remove function (might not need)
void table_remove(peer_dictionary* hash_table, request_struct* request_data);

char* table_find(peer_dictionary* hash_table);

#endif