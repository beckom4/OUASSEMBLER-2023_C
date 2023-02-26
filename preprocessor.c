#include "preprocessor.h"

/*Defining a linked list that will contain the macros. The list is declared in the h file and defined here in order to keep the encapsulation principle.*/
struct Macros {
	int macro_flag_in_list;
	char macro[MAX_LINE];
	char macro_name[MAX_LINE];
	struct Macros* next;
};


int main_pre_processor(FILE *fptr)
{	
	int macro_counter,macroflag,macro_in_list,size, x;

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
	size = 0;
	
	while ((fgets(original_line, MAX_LINE, fptr)) != NULL) 
    	{
		portion = strtok(original_line," \t");
	
		while(portion != NULL)
		{
			size += MAX_LINE;
			macro_in_list = is_in_macro_list(head, portion);
			if(macro_in_list != 0)
			{
				new_txt = (char*)realloc(new_txt, size + sizeof(char)); 
				new_txt = insert_string(new_txt,macro_in_list,head);
				if(new_txt == NULL)
				{
					printf("The system could not allocate enough memory for some of your text.\n");
					return END_PROGRAM;
				}
					
			}
			/*Checking if this portion a part of a macro.*/
			else if(macroflag != 0 && strcmp(portion,endmcr) != 0)
			{
				len_of_txt = strlen(portion);
				/*portion[len_of_txt] = ' ';*/
				for(temp = head;temp->macro_flag_in_list != macro_in_list; temp = temp->next);
				strcat(temp->macro,portion);		
			}		
			/*Checking if this portion is the end of the macro.*/
			else if(macroflag != 0 && strcmp(portion,endmcr) == 0)
				macroflag = 0;
			else if(strcmp(portion,mcr) == 0)
			{
				macroflag = 1;
				portion = strtok(NULL," \t");
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
			}
			else
			{
				printf("portion is: %s\n", portion);
				new_txt = (char*)realloc(new_txt, size + sizeof(char)); 
				printf("SIZE is: %d\n", size);
				strcat(new_txt,portion);
				printf("new_txt is: %s\n", new_txt);
				len_of_txt = strlen(new_txt);
				/*"Closing" the string with a space followed by a NULL terminator.*/
				new_txt[len_of_txt] = ' ';
				new_txt[len_of_txt + 1] = '\0';
				
			}
			portion = strtok(NULL," \t");
		}
		/*"Closing" the line.*/
		/*new_txt[len_of_txt - 1] = '\n';*/
	}
	/*"Closing" the string.*/
	new_txt[len_of_txt - 1] = '\0';
	free(new_txt);
	printf("new_txt is: %s", new_txt);
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
       		if (strcmp(current-> macro_name,portion)) 
      			return macroflag;
   		current = current->next;
	}
   	return 0;
}



char *insert_string(char *new_txt, int macroflag, struct Macros *head) 
{

	struct Macros *current = head;
	while (current != NULL) 
	{
		
       		if (macroflag == current->macro_flag_in_list)
		{
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


struct Macros *createMacro(struct Macros *next, int counter, char *macro_name) 
{
	struct Macros *newMacro = (struct Macros*)malloc(sizeof(struct Macros));
	if(newMacro == NULL)
		return NULL;
	newMacro->macro_flag_in_list = counter;
	strcpy(newMacro->macro_name,macro_name);
    	newMacro->next = next;
   	return newMacro;
}

