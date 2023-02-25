#include "preprocessor.h"

/*Defining a linked list that will contain the macros. The list is declared in the h file and defined here in order to keep the encapsulation principle.*/
struct Macros {
	int macro_flag_in_list;
	char macro[MAX_MACRO];
	struct Macros* next;
};


int main_pre_processor(FILE *fptr)
{	
	int i,macroflag;

	size_t len_of_line, len_of_txt;
	
	char c;
	char mcr[] = "mcr\0";
	char endmcr[] = "endmcr\0";
	char line[MAX_LINE];
	char *portion, *new_txt,*original_txt;

	struct Macros* head;

	new_txt = (char*)malloc(sizeof(char));
	original_txt = (char*)malloc(sizeof(char));

	head = NULL;
	
	i = 0;
	
	while (fgets(original_txt, MAX_LINE, fptr)) 
    	{
		original_txt = (char*)realloc(original_txt, sizeof(char) * (MAX_LINE));
		portion = strtok(line," \t");
	
		while(portion != NULL)
		{
			macroflag = is_in_macro_list(head, portion);
			if(macroflag != 0)
			{
				new_txt = insert_string(new_txt,macroflag,head);
				if(new_txt == NULL)
				{
				printf("The system could not allocate enough memory for some of your text.\n");
				return END_PROGRAM;
				}	
			}

		}
	}
	return 0;
}



int is_in_macro_list(struct Macros *head, char portion[]) 
{
	int macroflag;
	struct Macros *current = head;
	macroflag = 0;
	while (current != NULL) 
	{
		++macroflag;
       		if (strcmp(current-> macro,portion)) 
      			return macroflag;
   		current = current->next;
	}
   	return 0;
}



char *insert_string(char *new_txt, int macroflag, struct Macros *head) 
{
	size_t insert_len;

	struct Macros *current = head;
	while (current != NULL) 
	{
		
       		if (macroflag == current->macro_flag_in_list)
		{
			insert_len = strlen(current->macro);
			new_txt = (char*)realloc(new_txt, sizeof(char) * (insert_len+1)); /*+1 for NULL terminator.*/
			if(new_txt == NULL)
			{
				printf("The system could not allocate enough memory for some of your text.\n");
				return NULL;
			}
			strcat(new_txt, current->macro);
		}
      		current = current->next;
	}
	return new_txt;
}
