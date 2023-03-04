#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node *node;

struct node* insert_node_to_end(struct node** tail, void* data,int size);
struct node* search_list(struct node *head, void *data);
void destroy_list(struct node *head);
/*Temporary function*/
void print_list(struct node* head); 
