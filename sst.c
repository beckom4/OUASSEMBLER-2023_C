#include "sst.h"
#include "errors_checks.h"

#include <search.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/*inclusion of functions from errors_checks.c*/
extern int directive_data_errors(char str[], struct sst *res);
extern int directive_string_errors(char str[], struct sst *res);
extern int directive_entext_errors(char str[], struct sst *res);
extern int label_errors(char str[], struct sst *res);



/*Function declarations:*/


/**
 * creates a SST from a line.
 * 
 * @param line - the line to analize.
 * @return struct sst - the sst of the line.
 */
struct sst sst_get_stt_from_line(const char * line);

/**
 * Determines if the current portion of the line is a valid command(instruction).
 * 
 * @param str - the portion in the line to be checked.
	  index - a pointer to apointer that holds the address of the location of the beginning of this portion.
 * @return .i_tag - the tag of the command if it is a command, a number 0 - 15.
	   -1 if this portion is not a command.
 */
int check_command(const char str[], int **index);

/**
 * Determines if a the current portion of the line is a valid register.
 * 
 * @param str - the portion in the line to be checked.
 * @return .i: a number 0 - 7 that represents the register number.
	   -1 if this portion is not a valid register.
 */
int check_reg(const char str[]);

/**
 * Determines if a the line is an empty line or not.
 * 
 * @param str - the line.
 * @return 1 if the line is empty.
	   0 if it's not.
 */
int is_empty(const char str[]);

/**
 * Determines if a the line is a comment or not.
 * 
 * @param str - the line.
 * @return 1 if the line is a comment.
	   0 if it's not.
 */
int is_comment(const char str[]);


/**
 * Determines if a the current portion is a valid directive.
 * 
 * @param str - the portion to be checked.
	  index - a pointer to a pointer to the address of the beginning of the portion.
 * @return i - A number 0-3 that represents the tag of the directive.
	   -1 if it's not a valid directive.
 */
int check_directive(const char str[], int **index);

/**
 * Determines if a the current portion is a valid label.
 * 
 * @param str - the portion to be checked.
	  struct sst - the sst of the line.
 * @return label_flag - the size of the label, if it is a label.
	   -1 if it's not a valid label.
 */
int check_label(struct sst *res, const char *line);

/**
 * Builds the sst of the directive recursively.
 * 
 * @param directive_flag - the tag of the directive.
	  last_portion - the portion to be checked.
	  struct sst - the sst of the line.
 * @return 0
 */
int handle_directive(int directive_flag, char last_portion[], struct sst *res, int k);

/**
 * Builds the sst of the command recursively.
 * 
 * @param command_flag - the tag of the directive.
	  last_portion - the portion to be checked.
	  struct sst - the sst of the line.
	  num_of_operands - the current number of operands that the function handled.
 * @return 0
 */
int handle_command(int command_flag, char last_portion[], struct sst *res, int num_of_operands);

struct sst_token {
	const char tok;
	const char * tok_description;
};

static struct sst_token tokens[NUM_OF_SEPERATORS] = {
    	{',',"seperator"},
   	{':',"end of label seperator"},
   	{'(',"open brackets"},
    	{')',"close brackets"},
	{'"',"close brackets"}
};

struct sst_cpu_inst {
    const char *inst_name;
    const enum sst_cpu_inst_tag i_tag;
};

/*Lexicographically sorting the array.*/
static struct sst_cpu_inst sst_cpu_insts[NUM_OF_CMD] = {
    {"add",sst_tag_cpu_i_add},
    {"bne",sst_tag_cpu_i_bne},
    {"clr",sst_tag_cpu_i_clr},
    {"cmp",sst_tag_cpu_i_cmp},
    {"dec",sst_tag_cpu_i_dec},
    {"inc",sst_tag_cpu_i_inc},
    {"jmp",sst_tag_cpu_i_jmp},
    {"jsr",sst_tag_cpu_i_jsr},
    {"lea",sst_tag_cpu_i_lea},
    {"mov",sst_tag_cpu_i_mov},
    {"not",sst_tag_cpu_i_not},
    {"prn",sst_tag_cpu_i_prn},
    {"red",sst_tag_cpu_i_red},
    {"rts",sst_tag_cpu_i_rts},
    {"stop",sst_tag_cpu_i_stop},
    {"sub",sst_tag_cpu_i_sub},
};

struct sst sst_get_stt_from_line(const char * line) 
{	
	char last_portion[MAX_LINE];
	char *token;
	int i, j, error_flag, label_flag, command_flag, directive_flag;
	int *index;

	struct sst res;

	i = 0, error_flag = 0, command_flag = -1, directive_flag = -1;
	index = &i;
	/*Checking if this is a comment.*/	
	if(is_comment(line) == 1)
		res.syntax_option = sst_comment;
	/*Checking if this is an empty line*/
	else if(is_empty(line))
		res.syntax_option = sst_white_space;
	/*Checking if this is a command/ directive*/
	else
	{
		/*Checking for ':', meaning for labels:*/
		label_flag = check_label(&res, line);
		if(label_flag == -2)
			error_flag = 1;
		else if(label_flag != -1)
			*index += label_flag + 1;
		if(error_flag != 1)
			command_flag = check_command(line, &index);
		/*Checking if the current portion of the line is a command or not.*/
		if(command_flag != -1)
			res.syntax_option = sst_instruction;
		/*Checking if this is a directive*/
		else if(error_flag != 1)
			directive_flag = check_directive(line, &index);			
		j = 0;
		while(line[*index] != '\0')
		{
			last_portion[j] = line[*index];
			j++;
			++(*index);
		}
		last_portion[j] = '\0';		
		/****/
		/*A CALL FOR A FUNCTION THAT CHECKS ERROR WILL COME HERE.*/
		if(directive_flag != -1)
		{
			if(directive_flag == 0)
			{
				token = strchr(last_portion, tokens[0].tok);
				if(token == NULL)
				{
					error_flag = 1;
					res.syntax_option = sst_syntax_error;
					strcpy(res.syntax_error_buffer,"There are no commas after .data.");
				}
			}
			else if(directive_flag == 1)
			{
				error_flag = directive_string_errors(last_portion, &res);
				if(error_flag == 1)
					printf("%s\n",res.syntax_error_buffer);
			}
			/*Checking if the directive is .extern and the user defined a label anyway.*/
			else if(directive_flag == 2 && label_flag != -1)
			{
				memset(res.label, '\0', sizeof(res.label));
				strcpy(res.asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.warning,"Warning: label defining before .extern is 					forbiden");
			}
					
			if(error_flag == 0)
			{
				handle_directive(directive_flag, last_portion, &res, 0);
				printf("the parameter is: %s\n", res.asm_directive_and_cpu_instruction.syntax_directive.dir.string);
				printf("warning is: %s\n", res.asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.warning);
			}
		}
			
		
						
	}

	/*TEMPORARY - FOR CHECKS ONLY*/
	printf("label_flag is: %d\n", label_flag);
	printf("command_flag is: %d\n", command_flag);
	printf("directive_flag is: %d\n", directive_flag);
	/*TEMPORARY - FOR CHECKS ONLY*/
	

	if(error_flag == 1)
		res.syntax_option = sst_syntax_error;

   	return res;
}

int check_command(const char str[], int **index)
{
	int left, right, mid, j, parse_begin, parse_end,temp;
	char cmd[MAX_LINE];
	j = 0, parse_begin = 0, parse_end = 0, temp = **index;

	while (parse_end != 1) 
	{
    		if (!isspace(str[**index + 1])) 
		{
       			parse_begin = 1;
        		cmd[j] = str[**index + 1];
        		++j;
    		} 
		else if (isspace(str[**index + 1]) && parse_begin == 1) {
       			parse_end = 1;
    	}
    		++(**index);
	}
	cmd[j] = '\0';
	left = 0;
	right = NUM_OF_CMD - 1;
	/*Binary search to find the command*/
	while(left <= right)
    	{
		mid = (left+ right)/2;
        	if(strcmp(cmd, sst_cpu_insts[mid].inst_name) > 0)
			left = mid + 1;
		else if(strcmp(cmd, sst_cpu_insts[mid].inst_name) < 0)
			right = mid - 1;
		else 	
           		return sst_cpu_insts[mid].i_tag;
   	}
	**index = temp;
    	return -1;
}


int check_reg(const char str[])
{
	int i;
	const char regs[][REG_SIZE] = {"r0","r1","r2","r3","r4",
				"r5","r6","r7"};

	for(i = 0; i < NUM_OF_REGS; i++)
	{
		if(strcmp(str,regs[i]))
			return i;
	}
	return -1;
}



int is_empty(const char str[])
{
	int i, count;
	i = 0;
	count = 0;
	while (i < strlen(str))
	{
		if (isspace(str[i]))
			count++;
		i++;
	}
	if(count == strlen(str))
		return 1;
	else
		return 0;
}


int is_comment(const char str[])
{
	if(str[0] == ';')
		return 1;
	else
		return 0;

}

int check_directive(const char str[], int **index)
{
	int i, j, parse_begin,parse_end,temp;
	char directive[MAX_LINE];
	const char directives_arr[][WORD_SIZE] = {".data",".string",".entry",".extern"};
	j = 0;
	printf("string to check is: %s\n", str);
	temp = **index;
	while (parse_end != 1) 
	{
    		if (!isspace(str[**index])) 
		{
       			parse_begin = 1;
        		directive[j] = str[**index];
        		++j;
    		} 
		else if (isspace(str[**index]) && parse_begin == 1)
       			parse_end = 1;
    		++(**index);
	}
	directive[j] = '\0';
	printf("directive is: %s\n", directive);
	for(i = 0; i < NUM_OF_INSTRUCTIONS; i++)
	{
		if(strcmp(directive,directives_arr[i]) == 0)
			return i;
	}
	**index = temp;
	return -1;
}


int check_label(struct sst *res, const char *line)
{
	int i, index_of_tok;
	char first_portion[MAX_LABEL_LENGTH + 1];
	char *token;

	token = strchr(line, tokens[1].tok);
	if(token == NULL)
		return -1;
	else
	{
		index_of_tok = (int)(token - line);		
		for(i = 0; i < index_of_tok; i++)
			first_portion[i] = line[i];
		first_portion[index_of_tok] = '\0';
		printf("First portion is: %s\n ", first_portion);
		if(label_errors(first_portion, res) == 0)
		{
			strcpy(res->label,first_portion);
			return index_of_tok;
		}
		else
			return -2;
		
	}		
}

int handle_directive(int directive_flag, char last_portion[], struct sst *res, int k)
{
	
	int error_flag, index_of_tok;
	long int param;
	char *token;
	char parameter[MAX_LINE], sub_str[MAX_LINE];
	error_flag = 0, index_of_tok = 0;
	res->syntax_option = sst_directive;
	printf("last portion is: %s\n", last_portion);
	/*Handling the directive .data*/
	if (directive_flag == 0)
	{
		res->asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_data;
		token = strchr(last_portion, tokens[0].tok);
		if(token == NULL)
		{
			strcpy(parameter, last_portion);
			error_flag = directive_data_errors(parameter, res);
			if(error_flag == 1)
				printf("%s\n",res->syntax_error_buffer);
			param = strtol(parameter, NULL, DEC);
			printf("param is: %ld\n", param);
			res->asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k] = param;
			printf("the number is: %ld\n", res->asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k]);
			return 0;
		}
		else
		{
			index_of_tok = (int)(token - last_portion);
			strcpy(sub_str,&last_portion[index_of_tok + 1]);
			strncpy(parameter, last_portion, index_of_tok);
			parameter[index_of_tok] = '\0';
			error_flag = directive_data_errors(parameter, res);
			if(error_flag == 1)
				printf("%s\n",res->syntax_error_buffer);
			printf("parameter is: %s\n", parameter);
			param = strtol(parameter, &token, DEC);
			printf("param is: %ld\n", param);
			res->asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k] = param;
			printf("the number is: %ld\n", res->asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k]);
			return handle_directive(directive_flag, sub_str, res, ++k);	
		}	

	}
	/*Handling the directive .string*/
	else if (directive_flag == 1)
	{
		res->asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_string;
		token = strchr(last_portion, tokens[4].tok);
		if(token == NULL)
			return 0;
		else
		{	
			index_of_tok = (int)(token - last_portion);
			strcpy(sub_str,&last_portion[index_of_tok + 1]);
			printf("sub_str is: %s\n", sub_str);
			printf("last_portion is: %s and index is: %d\n", last_portion, index_of_tok);
			strncpy(res->asm_directive_and_cpu_instruction.syntax_directive.dir.string, last_portion, index_of_tok);
			res->asm_directive_and_cpu_instruction.syntax_directive.dir.string[index_of_tok] = '\0';
			return handle_directive(directive_flag, sub_str, res, 0);	
		}	

	}
	/*Handling the directives .entry and .extern*/
	else if (directive_flag == 2 || directive_flag == 3)
	{
		error_flag = directive_entext_errors(last_portion, res);
		if(error_flag == 0)
		{
			token = strtok(last_portion,"' ','\t'");
			/***/
			/*
			In this section we need to itterate over the label list and compare our string to the labels.
			If it fits one of the labels, great, otherwise - error.
			Since we don't have a label table yet, I'm writing the base of it in a comment and will fix it later on.
		
			for(i = 0; i < size of table list; i++)
			{
				if (strcmp(table_list[i],token))
				{
					res->asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_entry;
					strcpy(res->asm_directive_and_cpu_instruction.syntax_directive.label,token);
					return 0;
				}
			}
			/*The string after .extern does not fit any of the labels in the label table.*/
			/*
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"Missing apostrophes");
			*/
			/***/
		}
	}
	return 0;
}


/*TEMPORARY - FOR TESTING ONLY*/
int main()
{
	char test[] = "LOOP: .string \"This is the way\"";
	sst_get_stt_from_line(test);
	return 0;
}
/*TEMPORARY - FOR TESTING ONLY*/
