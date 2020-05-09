#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "dataChunk.h"

int main()
{
	data_chunk **head_of_list;
	data_chunk **some_chunk;

	head_of_list = malloc(sizeof(data_chunk *));
	some_chunk = malloc(sizeof(data_chunk *));

	for (int i = 0; i < 650; ++i) 
	{
		head_of_list = add_to_data_list(head_of_list, "I", 15);
	}

	for (int i = 0; i < 650; ++i)
	{
		some_chunk = remove_head_of_data_list(head_of_list);
		free(some_chunk);
		printf("removing\n");
	}

	/*head_of_list = add_to_data_list(head_of_list, "I", 15);
	head_of_list = add_to_data_list(head_of_list, "pledge", 15);
	head_of_list = add_to_data_list(head_of_list, "allegiance", 15);
	head_of_list = add_to_data_list(head_of_list, "to", 15);
	head_of_list = add_to_data_list(head_of_list, "the", 15);
	head_of_list = add_to_data_list(head_of_list, "flag", 15);
	head_of_list = add_to_data_list(head_of_list, "of", 15);
	head_of_list = add_to_data_list(head_of_list, "the", 15);
	head_of_list = add_to_data_list(head_of_list, "united", 15);
	head_of_list = add_to_data_list(head_of_list, "states", 15);
	head_of_list = add_to_data_list(head_of_list, "of", 15);
	head_of_list = add_to_data_list(head_of_list, "america", 15);
	head_of_list = add_to_data_list(head_of_list, "and", 15);
	head_of_list = add_to_data_list(head_of_list, "to", 15);
	head_of_list = add_to_data_list(head_of_list, "the", 15);
	head_of_list = add_to_data_list(head_of_list, "republic", 15);
	head_of_list = add_to_data_list(head_of_list, "for", 15);
	head_of_list = add_to_data_list(head_of_list, "which", 15);
	head_of_list = add_to_data_list(head_of_list, "it", 15);
	head_of_list = add_to_data_list(head_of_list, "stands", 15);
	head_of_list = add_to_data_list(head_of_list, "one", 15);
	head_of_list = add_to_data_list(head_of_list, "nation", 15);
	head_of_list = add_to_data_list(head_of_list, "under", 15);
	head_of_list = add_to_data_list(head_of_list, "God", 15);

	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "I"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "pledge"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "allegiance"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "to"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "the"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "flag"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "of"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "the"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "united"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "states"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "of"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "america"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "and"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "to"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "the"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "republic"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "for"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "which"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "it"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "stands"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "one"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "nation"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "under"));
	free(some_chunk);
	some_chunk = remove_head_of_data_list(head_of_list);
	assert(!strcmp((*some_chunk)->data_content, "God"));
	free(some_chunk);*/

	free(head_of_list);
	
	return 0;
}