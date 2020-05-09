#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "peerDictionary.h"

request_struct* make_request_struct(char get_request[], char hostname[], int portno)
{
	request_struct* request;

	request = malloc(sizeof(request_struct));

	strcpy(request->get_request, get_request);
	strcpy(request->hostname, hostname);
	request->portno = portno;

	return request;
}

void test_compare_function()
{
	/* The following lines cannot be executed unless the compare function
	   is declared in peerDictionary headerfile */

	/*
	request_struct *request_1, *request_2, *request_3, *request_4, *request_5;

	request_1 = make_request_struct("Ranjit", "Google", 2);
	request_2 = make_request_struct("Thomas", "Google", 2);
	request_3 = make_request_struct("Ranjit", "Amazon", 2);
	request_4 = make_request_struct("Ranjit", "Google", 1);
	request_5 = make_request_struct("Ranjit", "Google", 2);

	// function returns false when non-Identical "get_request" attributes
	assert(!compare_request_struct(request_1, request_2));

	// function returns false when  non-identical "hostname" attributes
	assert(!compare_request_struct(request_1, request_3));

	// function returns false when non-Identical "portno" attributes
	assert(!compare_request_struct(request_1, request_4));

	// function returns true when all attributes are identical
	assert(compare_request_struct(request_1, request_5));
	*/
}

int main()
{
	peer_dictionary *dictionary_1;
	request_struct *request_1;

	test_compare_function();

	dictionary_1 = new_hash_table();
	request_1 = make_request_struct("Ranjit", "Google", 2);

	table_insert(dictionary_1, request_1, "exampleData");

	free_hash_table(dictionary_1);
}
