#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "peerDictionary.h"

#define CACHESIZE 100

int compare_request_struct(request_struct* request_1, request_struct* request_2)
{
	return (!(strcmp(request_1->get_request, request_2->get_request)) && !(strcmp(request_1->hostname, request_2->hostname)) && (request_1->portno == request_2->portno));
}

int get_index(request_struct* request)
{
	// convert struct to int
	/*int struct_as_int = atoi(request->get_request) + atoi(request->hostname) + request->portno;

	printf("struct as int is %d\n", struct_as_int);
	fflush(stdout);
	printf("gonna try to make this the index %s\n", (struct_as_int * struct_as_int * struct_as_int) % CACHESIZE);
	fflush(stdout);

	return ((struct_as_int * struct_as_int * struct_as_int) % CACHESIZE);*/
	return 69;
}

peer_dictionary* new_hash_table()
{
	peer_dictionary* new_dictionary;

	new_dictionary = malloc(sizeof(peer_dictionary));
	new_dictionary->hash_array = malloc(sizeof(storage_unit *) * CACHESIZE);
	for (int i = 0; i < CACHESIZE; ++i) new_dictionary->hash_array[i] = NULL;
	new_dictionary->current_size = 0;

	return new_dictionary;
}

void free_hash_table(peer_dictionary* dictionary)
{
	storage_unit *current_storage_unit, *next_storage_unit;

	// iterate over the array that "hash_array" points to and free its memory
	for (int i = 0; i < CACHESIZE; ++i) 
	{
		current_storage_unit = dictionary->hash_array[i];

		while (current_storage_unit != NULL) 
		{
			next_storage_unit = current_storage_unit->next;
			free(current_storage_unit);
			current_storage_unit = next_storage_unit;
		}
	}

	// free the hash_array, itself
	free(dictionary->hash_array);

	// free entire peer_dictionary node
	free(dictionary);
} 

void table_insert(peer_dictionary* dictionary, request_struct* request_data, char* data_from_server)
{
	int hash_index;
	storage_unit *current_storage_unit, *next_storage_unit;

	hash_index = get_index(request_data);

	/*printf("index is %s\n", hash_index);
	fflush(stdout);
*/
	current_storage_unit = dictionary->hash_array[hash_index];

	while (current_storage_unit != NULL && compare_request_struct(current_storage_unit->key, request_data)) current_storage_unit = current_storage_unit->next;

	if (current_storage_unit == NULL) 
	{
		current_storage_unit = malloc(sizeof(storage_unit));
		current_storage_unit->next = NULL;
	}

	strcpy(current_storage_unit->key->get_request, request_data->get_request);
	strcpy(current_storage_unit->key->hostname, request_data->hostname);
	current_storage_unit->key->portno = request_data->portno;
}
