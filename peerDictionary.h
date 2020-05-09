#ifndef PEERDICTIONARY_H
#define PEERDICTIONARY_H

#include "storageUnit.h"
#include "requestStruct.h"

typedef struct peer_dictionary
{
	storage_unit** hash_array;
	int current_size;
} peer_dictionary;

// uncomment this line to test this function
// int compare_request_struct(request_struct* request_1, request_struct* request_2);

// constructor
peer_dictionary* new_hash_table();

// destructor
void free_hash_table(peer_dictionary* dictionary);

// insert function
void table_insert(peer_dictionary* dictionary, request_struct* request_data, char* data_from_server);

// remove function (might not need)
void table_remove(peer_dictionary* hash_table, request_struct* request_data);

char* table_find(peer_dictionary* hash_table);

#endif