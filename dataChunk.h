#ifndef DATACHUNK_H
#define DATACHUNK_H

#define MAXRESPONSESIZE 9999

typedef struct data_chunk
{
    char data_content[MAXRESPONSESIZE];
    int data_size;
    struct data_chunk* next;
} data_chunk;

data_chunk** add_to_data_list(data_chunk **head_of_list, char data_content[], int data_size);

data_chunk** remove_head_of_data_list(data_chunk **head_of_list);

void write_list_to_file(data_chunk **head_of_list);

int get_list_length(data_chunk **head_of_list);

#endif