#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Function declarations:*/

/**
 * creates a new list.
 * @return list - A pointer to the list.
 */
LinkedList* create_list();

/**
 * Adds a new to the end of the list.
 * 
 * @param list - A pointer to the list.
 *	  data - a void pointer to the data field.
 */
void add_to_end(LinkedList* list, void* data);

/**
 * Generic iterartion of the list.
 * 
 * @param list - A pointer to the list.
 *	  A void pointer to receive the function pointer.
 *    function_ctx - user defined context for the function.
 */
void iterate_list(LinkedList* list, void (*function)(void*));

/**
 * Freeing the memory allocated for the list.
 * 
 * @param list - A pointer to the list.
 */
void destroy_list(LinkedList* list);

typedef struct node {
    void* data;
    struct node* next;
} Node;

typedef struct linked_list {
    Node* head;
    Node* tail;
}LinkedList;

LinkedList* create_list() {
    LinkedList* list = (LinkedList*) malloc(sizeof(LinkedList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void add_to_end(LinkedList* list, void* data) {
    Node* new_node = (Node*) malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;
    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
}

void iterate_list(LinkedList* list, int (*function)(void *, void *), void *function_ctx) {
    Node* current = list->head;
    while (current != NULL) {
        list->iterator = current;
        if(function(current->data, function_ctx))
            return;
        current = current->next;
    }
}

void destroy_list(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}


