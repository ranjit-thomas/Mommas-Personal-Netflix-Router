#include <stdlib.h>

#include "peerDictionary.h"

#define CACHESIZE 100

peer_dictionary* new_hash_table()
{
	peer_dictionary* new_dictionary;

	new_dictionary = malloc(sizeof(peer_dictionary));
	new_dictionary->hash_array = malloc(sizeof(storage_unit *) * CACHESIZE);
	for (int i = 0; i < CACHESIZE; ++i) new_dictionary->hash_array[i] = NULL;
	new_dictionary->current_size = 0;

	return new_dictionary;
}

void table_insert(peer_dictionary* dictionary, request_struct* request_data, char* data_from_server)
{
	
}
