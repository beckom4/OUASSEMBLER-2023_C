#include "preprocessor.h"


/*Defining a linked list that will contain the macros. The list is declared in the h file and defined here in order to keep the encapsulation principle.*/
struct Macros {
	int macro_index;
	char macro[MAX_LINE];
	char macro_name[MAX_LINE];
	struct Macros* next;
};


int preprocessor(FILE *fptr, char file_name[])
{	
	int size_before_endmcr, macro_counter,macro_flag, size, rows_counter, macro_index, error_flag;

	char mcr[] = "mcr";
	char endmcr[] = "endmcr";
	char original_line[MAX_LINE];	
	char *new_txt, *portion, *macro_name, *temp_char;

	struct Macros *head, *temp;

	new_txt = (char*)malloc(sizeof(char)*MAX_LINE);

	head = NULL;
	temp = NULL;
	macro_flag = 0;
	macro_counter = 0;
	rows_counter = 0;
	size = 0;
	macro_index = 0;
	error_flag = 0;
	
	while ((fgets(original_line, MAX_LINE, fptr)) != NULL) 
    	{
		++rows_counter;
		/*Checking if there's a word in the line that needs to be replaced with a macro*/
		macro_index = is_in_macro_list(head,original_line);
		if(macro_index != -1 && temp != NULL && error_flag == 0)
		{
			size += MAX_LINE;
			new_txt = (char*)realloc(new_txt, size + sizeof(char)); 
			insert_string(&new_txt, original_line, head, macro_index);	
		}
		/*Making sure it's not a macro's name or body */
		if(macro_flag == 0)
			portion = strstr(original_line, mcr);
		/*If there is nothing special about this line, add it to new_txt*/
		if(macro_index == -1 && macro_flag == 0 && portion == NULL && error_flag == 0)
		{	
			size += MAX_LINE;
			new_txt = (char*)realloc(new_txt, size + sizeof(char)); 
			strcat(new_txt, original_line);	
			continue;
		}
		/*Checking if portion is "mcr"*/
		if(macro_flag == 0 && portion != NULL)
		{
			macro_flag = 1;
			macro_name = strtok(portion," \t\n");
			printf("1st itteration: %s\n", macro_name);
			macro_name = strtok(NULL," \t\n");
			printf("2nd itterartion: %s\n",macro_name);
			if(is_command(macro_name) == 1)
			{
				error_flag = 1;
				macro_flag = 0;
				printf("The macro's name in row %d is already taken. \n", rows_counter);
			}
			else
			{	
				/*Adding the macro to the head of the list*/
				temp = create_macro(head,macro_counter,macro_name);
				if (temp == NULL)
				{
					printf("The system could not allocate enough memory for some of your text.\n");
					return END_PROGRAM;
				} 
				else
					head = temp;
				++macro_counter;
				continue;
			}
		}
		/*Checking for endmcr*/
		if(macro_flag == 1)
		{
			portion = strstr(original_line, endmcr);
			/*endmcr is in a later row.*/
			if(portion == NULL && error_flag == 0)
			{
				printf("original line is: %s\n", original_line);		
				strcat(head->macro,original_line);
			}
			/*endmcr is in this row*/
			else
			{
				/*Checking if endmcr appeared in the middle of the line.*/
				/*if(find_spaces(portion) == 0 && error_flag == 0)*/				
				size_before_endmcr = portion - original_line;
				strncat(head->macro,original_line,size_before_endmcr);
				macro_flag = 0;
				error_flag = 0;
			}
		}
	}
	printf("new_txt is: \n%s\n", new_txt);
	free_list(head);
	free(new_txt);
	if(produce_file(new_txt) != 0)
		return END_PROGRAM;
	/*TEMPORARY - DON'T FORGET TO DELETE*/
	for(temp = head; temp != NULL; temp = temp->next)
	{
		printf("macro_name is:%s\n", temp->macro_name);
		printf("macro is: \n%s\n", temp->macro);
		printf("macro_index is: %d\n", temp->macro_index);
	}
	return 0;
}
		



int is_in_macro_list(struct Macros *head, char original_line[]) 
{
	char *portion;
	struct Macros *current ;
	for(current = head; current != NULL; current = current -> next)
	{
		portion = strstr(original_line, current -> macro_name);
		if(portion != NULL)
			return current -> macro_index;	
	}		
   	return -1;
}


void insert_string(char **new_txt, char original_line[], struct Macros *head, int macro_index) 
{
	struct Macros *current ;
	
	for(current = head; current->macro_index != macro_index && current != NULL;current = current -> next);
	strcat(*new_txt,current->macro);
}


struct Macros *create_macro(struct Macros *next, int macro_counter, char *macro_name) 
{
	struct Macros *newMacro = (struct Macros*)malloc(sizeof(struct Macros));
	if(newMacro == NULL)
		return NULL;
	newMacro->macro_index = macro_counter;
	strcpy(newMacro->macro_name,macro_name);
    	newMacro->next = next;
   	return newMacro;
}

void free_list(struct Macros *head) 
{
   	struct Macros *current = head;
   	while (current != NULL) 
	{
        	struct Macros *next = current->next;
        	free(current);
       		current = next;
    	}
}

int produce_file(char *new_txt)
{
	 /*IMPORTANT - WE WILL PROBABLY NEED TO ADD THE ABILITY TO NAME THE FILE THE WAY THAT IT WAS NAMED BEFORE SO THIS VERSION IS PROBABLY NOT FINAL*/
	 FILE* fptr = fopen("extended_source_file.as", "w"); 
   	 if (fptr == NULL) 
	 { 
         	printf("Error opening file!\n"); 
        	return END_PROGRAM;
   	 }
    	 fputs(new_txt, fptr); 
         fclose(fptr); 
	 return 0;
}


/*int find_spaces(char portion[])
{
	int i;
	i = 0;
	for(i = 0; i < strlen(portion); i++)
		if(!isspace(portion[i]) || portion[i] == '\n')
			return 1;
	return 0;	
}*/


int is_command(char *macro_name)
{
	const char commands[][8] = {"mov\0", "cmp\0", "add\0", "sub\0",
                             "not\0", "clr\0", "lea\0", "inc\0",
                             "dec\0", "jmp\0", "bne\0", "red\0",
                             "prn\0", "jsr\0", "rts\0", "stop\0"};	
	int i; 

	for(i = 0; i < 16; i++)
	{
		if(strcmp(macro_name,commands[i]) == 0)
			return 1;
	}
	return 0;

}
