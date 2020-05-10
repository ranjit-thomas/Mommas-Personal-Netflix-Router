#include <stdlib.h>
#include <stdio.h>

#include "connectSocket.h"

connect_socket** add_to_connected_sockets(connect_socket **head_of_list, int client_socket, int server_socket)
{
	if (*head_of_list == NULL)
	{
		*head_of_list = malloc(sizeof(connect_socket));
		(*head_of_list)->client = client_socket;
	    (*head_of_list)->server = server_socket;
	    (*head_of_list)->next = NULL;
	}

	else (*head_of_list)->next = *(add_to_connected_sockets(&((*head_of_list)->next), client_socket, server_socket));

	return head_of_list;
}

connect_socket** remove_from_connected_sockets(connect_socket **head_of_list, connect_socket **to_remove)
{
    if (*head_of_list == *to_remove)
    {
    	*head_of_list = (*head_of_list)->next;
    	free(*to_remove);
    }

    else (*head_of_list)->next = *(remove_from_connected_sockets(&((*head_of_list)->next), to_remove));

    return head_of_list;


}

int get_connect_list_length(connect_socket** head_of_list)
{
    if (*head_of_list == NULL) return 0;

    else return 1 + get_connect_list_length(&((*head_of_list)->next));
}


connect_socket** find_connect_socket(connect_socket** head_of_list, int node_socket)
{
    connect_socket** current_socket;
    current_socket = malloc(sizeof(connect_socket *));

    *current_socket = *head_of_list;

    while (*current_socket != NULL && (*current_socket)->client != node_socket && (*current_socket)->server != node_socket)
    {
		*current_socket = (*current_socket)->next;
    }
    
    return current_socket;
}