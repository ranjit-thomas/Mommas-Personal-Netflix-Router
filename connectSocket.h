#ifndef CONNECTSOCKET_H
#define CONNECTSOCKET_H

typedef struct connect_socket
{
    int client;
    int server;
    struct connect_socket *next;
} connect_socket;

connect_socket** add_to_connected_sockets(connect_socket **head_of_list, int client_socket, int server_socket);

connect_socket**  remove_from_connected_sockets(connect_socket **head_of_list, connect_socket **to_remove);

connect_socket** find_connect_socket(connect_socket** head_of_list, int node_socket);

int get_connect_list_length(connect_socket** head_of_list);

#endif