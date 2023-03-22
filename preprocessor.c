#innclude "preprocessor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Macro{
	char macro_name[MAX_LINE];
	char *macro_body;
}Macro;

/*Functions import:*/

extern LinkedList* create_list();
extern void add_to_end(LinkedList* list, void* data);
extern void iterate_list(LinkedList* list, int (*function)(void *, void *), void *function_ctx);
extern void destroy_list(LinkedList* list);

/*Function declarations:*/

/*Checks whether a string consists of white spaces only.
	@param: 
		str - The string to check.
	@return:
		0 if the string is not empty.
		1 if the string is empty.
*/
int is_empty(char str[]);

/*
	The main function of the preprocessor. It produces a file whose name is the same as the one it got, only with extended_ attached to it.
	The file produced will replace each macro with its content.
	@param: 
		fptr is a pointer to the file.
		file name is a pointer to the name of the file
	@return:
		0 if the process ran smoothly.
		1 if severe issues emerged that required the program to terminate.
*/
int preprocessor(FILE *fptr, char *file_name);

/*
	Creates a new node macro.
	@return:
		A new macro with a full macro name and yet incomplete macro body.
*/
Macro *create_macro(char *macro_name);

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
int is_command(char *macro_name);

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



int preprocessor(FILE *fptr, char file_name[])
{	
	int i, flag, error_flag, size, index, num_lines, len;

	char mcr[] = "mcr";
	char endmcr[] = "endmcr";
	char original_line[MAX_LINE];
	char copy[MAX_LINE], copy2[MAX_LINE];	
	char *new_txt, *portion1, *portion2, *portion, *macro_name, *macro_body;

	LinkedList* macro_list = create_list();
	Node* current;
	Macro* macro;

	new_txt = (char*)malloc(sizeof(char)*MAX_LINE);

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
			iterate_list(macro_list,compare,portion);
			if(macro_list->iterator != NULL)
			{
				size += MAX_LINE;
				portion2 = strstr(original_line, portion);

				index = (int)(portion2 - original_line);
				for(i = 0; i < index; i++)
					copy2[i] = original_line[i];
				copy2[i] = '\0';				
				strcat(new_txt, copy2);
				current = macro_list->iterator;
				macro = (Macro*) current->data;
			   	macro_body = macro->macro_body;
				size += strlen(macro_body);
				new_txt = (char*)realloc(new_txt, size * sizeof(char));
				strcat(new_txt, macro_body);		
				len = strlen(macro->macro_name);
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
				macro_list->iterator = NULL;
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
				iterate_list(macro_list,compare,macro_name);
				/*Making sure that the macro's name is not taken.*/
				if(macro_list->iterator != NULL)
				{
						macro_list->iterator = NULL;
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
				add_to_end(macro_list, macro);
				/*num_lines = 0;*/
			    	break;
			case DONE:
				flag = 0;
				break;
            	}  
	}
	printf("macro body is: %s\n", macro ->macro_body);
	printf("new txt is: %s\n", new_txt);	
	error_flag = produce_file(new_txt, file_name);

	iterate_list(macro_list, destroy_macro, NULL);
	destroy_list(macro_list);
	free(new_txt);
	return error_flag;
}
		

Macro *create_macro(char *macro_name) 
{
	Macro *newMacro = (Macro*)malloc(sizeof(Macro));
	strcpy(newMacro->macro_name, macro_name);
	newMacro->macro_body = NULL;
   	return newMacro;
}


int produce_file(char *new_txt, char *file_name)
{
	 char as[] =".as"; 
	 FILE* fptr;
	 strcat(file_name,as);

	 fptr = fopen(file_name, "w");
	
   	 if (fptr == NULL) 
	 { 
         	printf("Error opening file!\n"); 
        	return END_PROGRAM;
   	 }
    	 fputs(new_txt, fptr); 
         fclose(fptr); 
	 return 0;
}

int compare(void* data, void* ctx) {
    Macro* macro = (Macro*) data;
    char *macro_name_check ;

    macro_name_check = (char*) ctx;
 
    if (strcmp(macro_name_check, macro->macro_name) == 0)
            return 1;
    return 0;
}


int is_command(char *macro_name)
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


int is_empty(char str[])
{
	int i;
	for(i = 0; i < strlen(str);i++)
		if(!isspace(str[i]))
			return 0;
	return 1;
}


int destroy_macro(void *void_macro, void *dummy) {
    Macro *macro = (Macro *)void_macro;
    free(macro->macro_body);
    free(macro);
    return 0;
}
