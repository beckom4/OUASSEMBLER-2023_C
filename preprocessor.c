/*-----INCLUDES-----*/

#include "preprocessor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*-----DEFINES-----*/

#define END_PROGRAM 0
#define MAX_LINE 80
#define MAX_CMD_ARR 16
#define MCR 1
#define MCR_BODY 2
#define ENDMCR 3
#define DONE 4
#define MCR_SIZE 3
#define ENDMCR_SIZE 6
#define MAX_FILE_NAME 100
#define FOUND_ERROR 0


typedef struct Macro_list{
	char macro_name[MAX_LINE];
	char *macro_body;
	struct Macro_list *next;
}Macro;



Macro* create_macro(char macro_name[]);

void add_to_end(Macro **head, Macro *new_macro);

Macro* search(Macro* head, char name[]);

void destroy_list(Macro* head);



/*Checks whether a string consists of white spaces only.
	@param: 
		str - The string to check.
	@return:
		0 if the string is not empty.
		1 if the string is empty.
*/
int is_empty_pre(char str[]);


/*
	Produces the files that has the extended text including the content of the macros.
	@param: 
		new_txt is a pointer to the newtext that will be printed to the file. 
		old_file_name is a pointer to the old name of the file. 
	@return:
		0 if any issues emerged that required the program to be terminated.
        1 if it went smoothly.
*/

int produce_file(char *new_txt, char *file_name);
/*
	Checks whhether or not a macro's name is a command's name which is an error.
	@param: 
		macro_name - a pointer to the macro's name.
	@return:
		0 if the macro's name is not a command's name.
		1 if it is a command's name.
*/
int is_command_pre(char *macro_name);

/*
	Compares between a given string and a macro name.
	@param: 
		data - void pointer that will be casted to a macro.
        	ctx - void pointer to a context given to us by the user. In this case, it would be the string to compare.
	@return:
		0 if the string is not a macro name.
		1 if it is a macro name.
*/
int compare(void* data, void* ctx);

/*
	frees the memory allocated for the macro.
	@param: 
		macro - The macro to destroy.
       	 	dummy - a dummy void pointer.
	@return:
		0 - dummy value
*/
int destroy_macro(void *macro, void *dummy);



int preprocessor(char *file_name)
{	
	int i, flag, size, index, num_lines, len;

	char mcr[] = "mcr";
	char endmcr[] = "endmcr";
	char original_line[MAX_LINE];
	char copy[MAX_LINE], copy2[MAX_LINE];	
	char *new_txt, *portion1, *portion2, *portion, *macro_name, *macro_body;
	Macro* head, *current, *macro;
	FILE *fptr;

	new_txt = (char*)malloc(sizeof(char)*MAX_LINE);
	strcat(file_name, ".as");
	head = NULL, current = NULL, macro= NULL;
	fptr = fopen(file_name, "r");
	if(fptr == NULL){
		fprintf(stderr, "unable to open file\n");
		return END_PROGRAM;
	}
	portion1 = NULL;
	portion2 = NULL;

	size = 0, num_lines = 0, flag =0;

	while ((fgets(original_line, MAX_LINE, fptr)) != NULL) 
	{
		i = 0;
		size += MAX_LINE;
		new_txt = (char*)realloc(new_txt, size + sizeof(char));
		strcpy(copy, original_line);
		portion = strtok(copy," \t");
		while(portion != NULL)
		{
			/*Checking if there's a word in the line that needs to be replaced with a macro*/
			current = search(head, portion);
			if(current != NULL)
			{
				size += MAX_LINE;
				portion2 = strstr(original_line, portion);

				index = (int)(portion2 - original_line);
				for(i = 0; i < index; i++)
					copy2[i] = original_line[i];
				copy2[i] = '\0';				
				strcat(new_txt, copy2);
			   	macro_body = current->macro_body;
				size += strlen(macro_body);
				new_txt = (char*)realloc(new_txt, size * sizeof(char));
				strcat(new_txt, macro_body);		
				len = strlen(current->macro_name);
				if(portion2[len] != '\0')
				{
					i = 0;
					while(portion2[len] != '\0')
					{
						copy[i] = portion2[len];
						++len;
					}
					copy[i] = '\0';
					strcat(new_txt, copy);
				}
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
             		case 0: 
				flag = 0;
				strcat(new_txt, original_line);
			    	break; 
			/*mcr found*/
			 case MCR: 
				portion2 = strtok(portion1, " \t");
				macro_name = strtok(NULL, " \t");
				current = search(head,macro_name);
				/*Making sure that the macro's name is not taken.*/
				if(current != NULL)
				{
						current = NULL;
						break;
				}
				else
				{
					macro = create_macro(macro_name);
					macro->macro_body = (char*)malloc(MAX_LINE * sizeof(char));
					macro->macro_body[0] = '\0';
					flag = MCR_BODY;
			    		break;
				}
			/*MCR_BODY found*/
			 case MCR_BODY: 
				++num_lines;
				size += MAX_LINE;
				macro->macro_body = (char*)realloc(macro->macro_body, size *  sizeof(char));
				strcat(macro->macro_body, original_line);
				i = 0;
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
	if(produce_file(new_txt, file_name) == FOUND_ERROR)
		return FOUND_ERROR;;
	fclose(fptr); 	
	destroy_list(head);
	free(new_txt);
	return 1;
}
		


int produce_file(char *new_txt, char *file_name) {
    FILE* fptr;
    char *dot;
    dot = strchr(file_name, '.');
    if (dot != NULL && strcmp(dot, ".as") == 0)
        *dot = '\0'; 
    strcat(file_name, ".am"); 
    fptr = fopen(file_name, "w");
    if (fptr == NULL) {
        printf("Failed to open file %s\n", file_name);
        return FOUND_ERROR;
    }
    fputs(new_txt, fptr); 
    fclose(fptr);    
    return 1;
}

int compare(void* data, void* ctx) {
    Macro* macro = (Macro*) data;
    char *macro_name_check ;

    macro_name_check = (char*) ctx;
 
    if (strcmp(macro_name_check, macro->macro_name) == 0)
            return 1;
    return 0;
}


int is_command_pre(char *macro_name)
{
	const char commands[][8] = {"mov\0", "cmp\0", "add\0", "sub\0",
                             "not\0", "clr\0", "lea\0", "inc\0",
                             "dec\0", "jmp\0", "bne\0", "red\0",
                             "prn\0", "jsr\0", "rts\0", "stop\0"};	
	int i; 

	for(i = 0; i < MAX_CMD_ARR; i++)
	{
		if(strcmp(macro_name,commands[i]) == 0)
			return 1;
	}
	return 0;

}

int is_empty_pre(char str[])
{
	int i;
	for(i = 0; i < strlen(str);i++)
		if(!isspace(str[i]))
			return 0;
	return 1;
}




Macro* create_macro(char macro_name[]) {
    Macro *new_macro = (Macro*) malloc(sizeof(Macro));
    strcpy(new_macro->macro_name, macro_name);
    new_macro->next = NULL;
    return new_macro;
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
        free(temp->macro_body);
        free(temp);
    }
    head = NULL;
}
