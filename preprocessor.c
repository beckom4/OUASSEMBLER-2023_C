/*IMPORTANT- IN ORDERR TO USE THE FUNCTION ISCOMMAND WE WILL NEED TO INCLUDE THE H FILE OF THE INPUT MANAGER(SST MUDULE), ONCE WRITTEN. 
THE RETURN VALUE MAY ALSO NEED TO CHANGE. THIS LINE WILL BE MARKED AS A REMARK IN THE MEAN TIME. OTHER THAN THAT, THIS MODULE SEEMS TO BE RUNNING WELL AND HAS BEEN TESTED.*/

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
	int macro_counter,macroflag,macro_in_list,size, rows_counter, macro_index, macro_name_flag, end_of_macro_flag;

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
	end_of_macro_flag = 0;
	size = 0;
	
	while ((fgets(original_line, MAX_LINE, fptr)) != NULL) 
    	{
		/*Making sur there's no double \n in the output due to a macro or the end of it.*/
		if(macroflag == 0 && end_of_macro_flag == 0)
		{
			len_of_txt = strlen(new_txt);
			new_txt[len_of_txt ] = '\n';
		}
		end_of_macro_flag = 0;
		size += MAX_LINE;
		++rows_counter;
		portion = strtok(original_line," \t\n");
	
		while(portion != NULL)
		{
			macro_in_list = 0;
			macro_in_list = is_in_macro_list(head, portion);
			if(macro_in_list != -1)
			{
				char *temp = realloc(new_txt, size + sizeof(char));
				if (temp == NULL) 
				{
   					printf("The system could not allocate enough memory for some of your text.\n");
    					return END_PROGRAM;
				}
				new_txt = temp;
				insert_string(&new_txt,macro_in_list,head);
			}
			/*Checking if this portion is the name of the macro*/
			else if(macroflag == 1 && macro_name_flag == 1)
			{
				/*checking if mcr is the last word in the line*/
				if(portion == NULL)
					continue;
				/*Checking for an empty macro*/
				else if(strcmp(portion,endmcr) == 0)
 					printf("The macro that should end in row %d is empty.\n", rows_counter);
				/*Checking if the macro name is one of the commands*/
				/*else if(isCommand(portion) != -1)
					printf("The macro in row %d has an illegal macro name.\n", rows_counter);*/	
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
			{
				end_of_macro_flag = 1;
				macroflag = 0;	
			}	
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
			portion = strtok(NULL," \t\n");
		}
		
	}
	printf("new_txt is: %s\n", new_txt);
	free_list(head);
	free(new_txt);
	if(produce_file(new_txt) != 0)
		return END_PROGRAM;
	/*TEMPORARY - DON'T FORGET TO DELETE*/
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
	struct Macros *current = head;
	while (current != NULL) 
	{
       		if (strcmp(current-> macro_name,portion) == 0) 
      			return current -> macro_index;
   		current = current->next;
	}
   	return -1;
}



void insert_string(char **new_txt, int macroflag, struct Macros *head) 
{
	size_t len;
	struct Macros *current = head;
	for(current = head;current->macro_index != macroflag; current = current -> next);
	strcat(*new_txt, current->macro);
	len = strlen(*new_txt);
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


