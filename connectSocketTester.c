#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "connectSocket.h"

int main()
{
	connect_socket **head_of_list;
	connect_socket **found_node_1, **found_node_2, **found_node_3;

	head_of_list = malloc(sizeof(connect_socket *));
	*head_of_list = NULL;

	head_of_list = add_to_connected_sockets(head_of_list, 1, 2);
	head_of_list = add_to_connected_sockets(head_of_list, 3, 4);
	head_of_list = add_to_connected_sockets(head_of_list, 5, 6);
	found_node_1 = find_connect_socket(head_of_list, 1);
	assert((*found_node_1)->client == 1); 
	assert((*found_node_1)->server == 2); 
	found_node_2 = find_connect_socket(head_of_list, 3);
	assert((*found_node_1)->client == 1); 
	assert((*found_node_1)->server == 2); 
	found_node_3 = find_connect_socket(head_of_list, 5);
	assert(*head_of_list != NULL);
	head_of_list = remove_from_connected_sockets(head_of_list, found_node_3);
	assert(*head_of_list != NULL);
	head_of_list = remove_from_connected_sockets(head_of_list, found_node_2);
	assert(*head_of_list != NULL);
	head_of_list = remove_from_connected_sockets(head_of_list, found_node_1);
	assert(*head_of_list == NULL);

	free(head_of_list);
}
