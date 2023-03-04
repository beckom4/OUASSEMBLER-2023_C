#include "linkedlist.h"

/*Defining a node in the generic list.*/
struct node
{
	void *data;
	struct node* next;
};


struct node* insert_node_to_end(struct node** tail, void* data,int size)
{
	/*Allocating memoryy for the new node*/	
	struct node *new_node = (struct node*)malloc(sizeof(struct node));
	/*ALlocating memory for the data fiel.*/
	new_node->data = malloc(size);
	/*Storing the data of the new node by copying from one address to another*/
	memcpy(new_node->data,data,size);
	/*Insering the new node after tail*/
	if((*tail) != NULL)
		(*tail)->next=new_node; 
	else
		(*tail) = new_node;	
   	/* updating tail node pointer*/
	*tail = new_node;
	return new_node;
}

struct node* search_list(struct node *head, void *data)
{
	struct node *current;
	/*Looping over the list and comparing the data field of the current node to the one passed as an argument*/
	for (current = head; current != NULL; current = current -> next)
		if(current -> data == data)
			return current;
	return NULL;
}

void destroy_list(struct node *head) 
{
    	struct node *current = head;
	/*Looping over the list in order to free its nodes' memory/*/
   	for(current = head; current != NULL; current = current -> next)
	{
		free(current -> data);
      		free(current);
	}
}


/*Temporary function which will serve us for testing if required.*/
void print_list(struct node* head) 
{
    	struct node* current = head;
  	while (current != NULL) {
        	printf("%d ", *(int*)current->data);
       		current = current->next;
  	  }
   	 printf("\n");
}
