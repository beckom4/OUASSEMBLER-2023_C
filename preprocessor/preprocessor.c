/*-----INCLUDES-----*/

#include "preprocessor.h"
#include "../dynarray/dynarray.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*-----DEFINES-----*/

#define END_PROGRAM 0
#define MAX_LINE 80
#define MAX_CMD_ARR 16
#define NUM_OF_REGS 8
#define MCR 1
#define MCR_BODY 2
#define ENDMCR 3
#define DONE 4
#define MCR_SIZE 3
#define MAX_FILE_NAME 100
#define FOUND_ERROR 0
#define WORD_SIZE 2
#define CMD_SIZE 10

/*-----STRUCTS-----*/


typedef struct Macro_list{
	char macro_name[MAX_LINE];
	char (*macro_body)[MAX_LINE];
	struct Macro_list *next;
}Macro;

/*-----FUNCTION DECLARATIONS-----*/


/*
Creates a new macro.
@param: 
	macro_name - The name of the macro that needs to be created. 
@return:
	A pointer to the newly created Macro if it went smoothly.
        NULL - If we had issues allocating memory.
*/
Macro* create_macro(char *macro_name);


/*
Adds a new macro to the end of the macro list.
@param: 
	head - A pointer to the end of the list. 
	new_macro - A ponter to the new macro that needs to be inserted.
@return:
	1 if it's a command.
        0 if it's not.
*/
void add_to_end(Macro **head, Macro *new_macro);

/*
Checks if a string is a macro from the macro list.
@param: 
	macro_name - The string to be checked. 
@return:
	A pointer to the macro if it's in the list.
        NULL if  it's not.
*/
Macro* search(Macro* head, char macro_name[]);

/*
Frees the memory allocated for the list.
@param: 
	head - A pointer to the head of the list. 
*/
void destroy_list(Macro* head);

/*
Checks if a macro name is a registr's name.
@param: 
	str - The string to be checked. 
@return:
	1 if it's a register.
        0 if it's not.
*/
int is_reg_pre(char str[]);

/*
Checks if a macro name is a command.
@param: 
	str - The string to be checked. 
@return:
	1 if it's a command.
        0 if it's not.
*/
int is_command_pre(char str[]);


/*-----FUNCTIONS-----*/


int preprocessor(char *file_name)
{	
	int i, flag, index, len;

	Macro *macro, *head, *current;

	char mcr[] = "mcr";
	char endmcr[] = "endmcr";
	char original_line[MAX_LINE];
	char copy[MAX_LINE], copy2[MAX_LINE];	
	char *portion1, *portion2, *portion, *macro_name, *as_file_name, *am_file_name;

	FILE *as_file, *am_file;
	as_file_name = malloc(strlen(file_name) +3 +1);
	am_file_name = malloc(strlen(file_name) +3 +1);
	current = NULL, portion1 = NULL, portion2 = NULL;

	strcat(strcpy(as_file_name,file_name),".as");
	as_file = fopen(as_file_name, "r");
	if(as_file == NULL){
		printf("unable to open file\n");
		return END_PROGRAM;
	}
	strcat(strcpy(am_file_name,file_name),".am");
    	am_file = fopen(am_file_name, "w");
   	 if (am_file == NULL) {
       		printf("Failed to open file %s\n", file_name);
		return FOUND_ERROR;
   	 }
	head = NULL;
        flag =0;

	while ((fgets(original_line, MAX_LINE,as_file)) != NULL) 
	{
		strcpy(copy, original_line);
		portion = strtok(copy," \t");
		while(portion != NULL)
		{
			/*Checking if there's a word in the line that needs to be replaced with a macro*/
			current = search(head,portion);
			if(current != NULL)
			{	
				portion2 = strstr(original_line, portion);
				index = (int)(portion2 - original_line);
				for(i = 0; i < index; i++)
					copy2[i] = original_line[i];
				copy2[i] = '\0';
				if(copy2[0] != '\0')					
					fprintf(am_file,"%s",copy2);
				for(i = 0; i < dynarray_length(current->macro_body) ; i++) 
       					 fprintf(am_file, "%s", current->macro_body[i]);		
				len = strlen(portion2);
				for(i = index + len; i < strlen(original_line);i++)
					copy2[i] = original_line[i];
				copy2[i] = '\0';
				if(copy2[0] != '\0')					
					fprintf(am_file,"%s",copy2);
				flag = DONE;
				current = NULL;
			}
			portion = strtok(NULL," \t");		
		}
		/*Checking for endmcr*/
		portion2 = strstr(original_line, endmcr);
		if(portion2 != NULL)
			flag = ENDMCR;
		/*Checking for mcr*/
		if(flag == 0)
		{
			portion1 = strstr(original_line, mcr);
			if(portion1 != NULL)
				flag = MCR;
		}
		switch(flag) {
			/*Nothing special in this line.*/
             		case 0: 
				fprintf(am_file,"%s",original_line);		
			    	break; 
			/*mcr found*/
			 case MCR: 
				portion2 = strtok(portion1, " \t");
				macro_name = strtok(NULL, " \t");
				current = search(head,macro_name);
				/*Making sure that the macro's name is not taken and that it's not a command/reg name.*/
				if(current != NULL) {
					current = NULL;
					break;
				}
				/*Checking if the macro's name is a command/ reg*/
				else if(is_command_pre(macro_name) || is_reg_pre(macro_name))
					return FOUND_ERROR;
				else{
					macro = create_macro(macro_name);
					flag = MCR_BODY;
			    		break;
				}
			/*MCR_BODY found*/
			 case MCR_BODY: 
				dynarray_push(macro->macro_body,original_line);
			    	break;
			/*endmcr found*/
			 case ENDMCR:
				flag = 0;
				add_to_end(&head, macro);
				macro = NULL;
			    	break;
			case DONE:
				flag = 0;
				break;
            	}  
	}
	free(as_file_name);
	free(am_file_name);
	destroy_list(head);
	fclose(am_file); 
	fclose(as_file);
	return 1;
}
	


int is_command_pre(char str[]) {
	const char commands[][CMD_SIZE] = {"mov", "cmp", "add", "sub",
                             "not", "clr", "lea", "inc",
                             "dec", "jmp", "bne", "red",
                             "prn", "jsr", "rts", "stop"};	
	int i; 
	
	for(i = 0; i < MAX_CMD_ARR; i++)
		if(strcmp(str,commands[i]) == 0)
			return 1;
	return 0;

}

int is_reg_pre(char str[]) {
    int i;
    const char regs_arr[][WORD_SIZE] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
  
    for(i = 0; i < NUM_OF_REGS; i++)
   	if (strcmp(str, regs_arr[i]) == 0) 
            return 1;
    return 0;
}

Macro *create_macro(char *macro_name)
{
    Macro *macro = malloc(sizeof(Macro));
    strcpy(macro->macro_name, macro_name);
    macro->macro_body = dynarray_create(char[MAX_LINE]);
    macro->next = NULL;
    return macro;
}



void add_to_end(Macro **head, Macro *new_macro) {
    Macro *current_node;
    if (*head == NULL) {
        
        *head = new_macro;
    } else {
        current_node = *head;
        while (current_node->next != NULL) {
            current_node = current_node->next;
        }
        current_node->next = new_macro;
    }
}

Macro* search(Macro* head, char macro_name[]) {
    Macro* current = head;
    while (current != NULL) {
        if (strcmp(current->macro_name, macro_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void destroy_list(Macro* head) {
    Macro* current;
    Macro *temp;
    current = head;
    while (current != NULL) {
	temp = current;
        current = current->next;
        dynarray_destroy(temp->macro_body);
        free(temp);
    }
    head = NULL;
}
