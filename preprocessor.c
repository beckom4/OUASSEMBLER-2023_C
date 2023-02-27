#include "preprocessor.h"

/*Defining a linked list that will contain the macros. The list is declared in the h file and defined here in order to keep the encapsulation principle.*/
struct Macros {
	int macro_index;
	char macro[MAX_LINE];
	char macro_name[MAX_LINE];
	struct Macros* next;
};


int main_pre_processor(FILE *fptr)
{	
	int macro_counter,macroflag,macro_in_list,size, rows_counter, macro_index, macro_name_flag;

	size_t len_of_txt;

	char mcr[] = "mcr\0";
	char endmcr[] = "endmcr\0";
	char original_line[MAX_LINE];	
	char *portion, *new_txt;

	struct Macros *head, *temp;

	new_txt = (char*)malloc(sizeof(char)*MAX_LINE);

	head = NULL;
	macroflag = 0;
	macro_counter = 0;
	len_of_txt = 0;
	rows_counter = 0;
	macro_index = 0;
	macro_name_flag = 0;
	size = 0;
	
	while ((fgets(original_line, MAX_LINE, fptr)) != NULL) 
    	{
		size += MAX_LINE;
		++rows_counter;
		portion = strtok(original_line," \t\n");
	
		while(portion != NULL)
		{
			macro_in_list = 0;
			printf("portion is: %s\n", portion);
			macro_in_list = is_in_macro_list(head, portion);
			printf("macro_in_list is : %d\n", macro_in_list);
			if(macro_in_list != -1)
			{
				new_txt = (char*)realloc(new_txt, size + sizeof(char)); 
				if(new_txt == NULL)
				{
					printf("The system could not allocate enough memory for some of your text.\n");
					return END_PROGRAM;
				}
				new_txt = insert_string(new_txt,macro_in_list,head);
			}
			/*Checking if this portion is the name of the macro*/
			else if(macroflag == 1 && macro_name_flag == 1)
			{
				/*checking if mcr is the last word in the line*/
				if(portion == NULL)
					continue;
				/*Checking for an empty macro*/
				if(strcmp(portion,endmcr) == 0)
					printf("The macro that should end in row %d is empty.\n", rows_counter);
				else
				{
					/*Adding the macro to the head of the list*/
					temp = createMacro(head,macro_counter,portion);
					++macro_counter;
					if (temp == NULL)
					{
						printf("The system could not allocate enough memory for some of your text.\n");
						return END_PROGRAM;
					} 
					else
						head = temp;
					macro_index = head->macro_index;
				}
				macro_name_flag = 0;
			}
			/*Checking if this portion a part of a macro.*/
			else if(macroflag != 0 && strcmp(portion,endmcr) != 0)
			{
				head = (struct Macros*)realloc(head, size + sizeof(struct Macros));
				strcat(head->macro,portion);
				len_of_txt = strlen(head->macro);
				head->macro[len_of_txt] = ' ';
				head->macro[len_of_txt + 1] = '\0';
			}		
			/*Checking if this portion is the end of the macro.*/
			else if(macroflag != 0 && (strcmp(portion,endmcr) == 0))
				macroflag = 0;		
			else if(strcmp(portion,mcr) == 0)
			{
				macroflag = 1;
				macro_name_flag = 1;
			}
			else
			{
				new_txt = (char*)realloc(new_txt, size + sizeof(char)); 
				strcat(new_txt,portion);
				len_of_txt = strlen(new_txt);
				/*"Closing" the string with a space followed by a NULL terminator.*/
				new_txt[len_of_txt] = ' ';
				new_txt[len_of_txt + 1] = '\0';
				
			}
		}
		portion = strtok(NULL," \t\n");
		/*"Closing" the line.*/
		len_of_txt = strlen(new_txt);
		new_txt[len_of_txt ] = '\n';
	}
	/*"Closing" the string.*/
	new_txt[len_of_txt] = '\0';
	printf("new_txt is: %s\n", new_txt);
	free(new_txt);
	for(temp = head; temp != NULL; temp = temp->next)
	{
		printf("macro_name is:%s\n", temp->macro_name);
		printf("macro is: %s\n", temp->macro);
		printf("macro_index is: %d\n", temp->macro_index);
	}
	return 0;
}



int is_in_macro_list(struct Macros *head, char portion[]) 
{
	printf("Macro in function is: %s\n", portion);
	struct Macros *current = head;
	while (current != NULL) 
	{
       		if (strcmp(current-> macro_name,portion) == 0) 
      			return current -> macro_index;
   		current = current->next;
	}
   	return -1;
}



char *insert_string(char *new_txt, int macroflag, struct Macros *head) 
{
	size_t len;
	struct Macros *current = head;
	for(current = head;current->macro_index != macroflag; current = current -> next);
	strcat(new_txt, current->macro);
	len = strlen(new_txt);
	new_txt[len] = '\n';
	return new_txt;
}




struct Macros *createMacro(struct Macros *next, int macro_counter, char *macro_name) 
{
	struct Macros *newMacro = (struct Macros*)malloc(sizeof(struct Macros));
	if(newMacro == NULL)
		return NULL;
	newMacro->macro_index = macro_counter;
	strcpy(newMacro->macro_name,macro_name);
    	newMacro->next = next;
   	return newMacro;
}


