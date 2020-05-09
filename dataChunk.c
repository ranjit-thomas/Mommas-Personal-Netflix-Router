#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dataChunk.h"

data_chunk** add_to_data_list(data_chunk **head_of_list, char data_content[], int data_size)
{
	if (*head_of_list != NULL) 
	{
		(*head_of_list)->next = *(add_to_data_list(&((*head_of_list)->next), data_content, data_size));
	}

	else
	{
		*head_of_list = malloc(sizeof(data_chunk));
		memmove((*head_of_list)->data_content, data_content, data_size);
		(*head_of_list)->data_size = data_size;
		(*head_of_list)->next = NULL;
	}

	return head_of_list;
}

data_chunk** remove_head_of_data_list(data_chunk **head_of_list)
{
	if (*head_of_list == NULL) return head_of_list;

	else
	{
		data_chunk **old_head_of_list;

		old_head_of_list = malloc(sizeof(data_chunk *));
		*old_head_of_list = *head_of_list;
		*head_of_list = (*head_of_list)->next;

		return old_head_of_list;
	}
}

int get_list_length(data_chunk **head_of_list)
{
	if (*head_of_list == NULL) return 0;
	else return 1 + get_list_length(&((*head_of_list)->next));
}

void write_list_to_file(data_chunk **head_of_list)
{
	FILE *fptr;
	data_chunk** current;

	fptr = fopen("listContents.txt", "w");

	if (fptr == NULL)
	{
		printf("Error!");
		exit(1);
	}

	current = malloc(sizeof(data_chunk *));

	*current = *head_of_list;

	while (*current != NULL)
	{
		fwrite((*current)->data_content, 1, (*current)->data_size, fptr);
		*current = (*current)->next;
	}

	free(current);
	fclose(fptr);

}