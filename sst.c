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
extern int no_operands_errors(char str[], struct sst *res);



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
	  struct sst - the sst of the line.
 * @return i - A number 0-3 that represents the tag of the directive.
	   -1 if it's not a valid directive.
 */
int check_directive(const char str[], int **index, struct sst *res);

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

/**
 * Determines how many operands are expected per the set definition in the heeader file.
 * 
 * @param command_flag - the tag of the directive.
 * @return SET1 for 2 operands
	   SET2 for 1 operand
	   SET3 for 0 operands
 */
int determine_set(int command_flag);

/**
 * Determines if the string is a number or not.
 * 
 * @param str - the string to be checked.
 * @return 1 if the string is a valid integer.
	   0 if it's not. 
 */
int is_immediate(char str[]);

struct sst_token {
	const char tok;
	const char * tok_description;
};

static struct sst_token tokens[NUM_OF_SEPERATORS] = {
    	{'(',"open brackets"},
	{',',"seperator"},
    	{')',"close brackets"},
	{':',"end of label seperator"},
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
		if(label_flag == FOUND_ERROR)
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
			directive_flag = check_directive(line, &index, &res);
		/*Checking if this is neither a command nor a directive*/
		if(command_flag == -1 && directive_flag == -1)
		{
			error_flag = 1;
			res.syntax_option = sst_syntax_error;
			strcpy(res.syntax_error_buffer,"Illegal command/ directive.");
		}
		/*Checking if the directive has no operands*/
		if(directive_flag == FOUND_ERROR)
			error_flag = 1;
		if(error_flag == 0)
		{
			j = 0;
			while(line[*index] != '\0')
			{
				last_portion[j] = line[*index];
				j++;
				++(*index);
			}
			last_portion[j] = '\0';	
		}	
		if(directive_flag != -1 && error_flag == 0)
		{
			if(directive_flag == 0)
			{
				token = strchr(last_portion, tokens[1].tok);
				if(token == NULL)
				{
					error_flag = 1;
					res.syntax_option = sst_syntax_error;
					strcpy(res.syntax_error_buffer,"There are no commas after .data.");
				}
			}
			else if(directive_flag == 1)
				error_flag = directive_string_errors(last_portion, &res);
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
		/*Handling the command.*/
		if(error_flag == 0 && command_flag != -1)
			if(handle_command(command_flag, last_portion, &res, 0) == FOUND_ERROR)
				error_flag = 1;						
	}

	/*TEMPORARY - FOR CHECKS ONLY*/
	printf("label_flag is: %d\n", label_flag);
	printf("command_flag is: %d\n", command_flag);
	printf("directive_flag is: %d\n", directive_flag);
	if(error_flag == 1)
		printf("error is: %s\n",res.syntax_error_buffer);
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
    		if (!isspace(str[**index])) 
		{
       			parse_begin = 1;
        		cmd[j] = str[**index];
        		++j;
    		} 
		if (str[**index] == '\0' && parse_begin == 1) 
       			parse_end = 1;
 		if(isspace(str[**index]) && parse_begin == 1)
		{
			cmd[j] = '\0';
			parse_end = 1;
		}
		if(str[**index] != '\0')
    			++(**index);
	}
	printf("cmd is: %s\n", cmd);
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
	
	int i, j, word_begin, word_end, reg;
	reg = -1;
	word_begin = 0, word_end = 0;
	printf("str is %s\n", str);
	for(i = 0; i < strlen(str); i++)
	{
		if(word_end == 1 && !isspace(str[i]))
		{
			printf("flag\n");
			return -1;
		}
		if(word_begin == 1)
		{
			for(j = 0; j < NUM_OF_REGS; j++)
			{
				if(str[i] -'0' == j)
					reg = str[i] - '0';
				word_end = 1;
			}
			
		}
		if(isspace(str[i]) && word_begin == 0)
			continue;
		if(word_begin == 0 && str[i] == 'r')
			word_begin = 1;		
	}
	return reg;
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

int check_directive(const char str[], int **index, struct sst *res)
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
		if (str[**index] == '\0' && parse_begin == 1)
		{ 
       			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"No operands after directive.");
			return FOUND_ERROR;
		}
 		if(isspace(str[**index]) && parse_begin == 1)
		{
			directive[j] = '\0';
			parse_end = 1;
		}
		if(str[**index] != '\0')
    			++(**index);
	}
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

	token = strchr(line, tokens[3].tok);
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
			return FOUND_ERROR;
		
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
		token = strchr(last_portion, tokens[1].tok);
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


int handle_command(int command_flag, char last_portion[], struct sst *res, int num_of_operands)
{
	int i, index_of_tok, open_bracket, close_bracket, comma, end;
	long int operand;
	char current_portion[MAX_LINE], sub_str_temp[MAX_LINE];
	char *token;
	char t;

	open_bracket = 0, close_bracket = 0, comma = 0, end = 0;	
	token = &t;
	res->syntax_option = sst_instruction;

	printf("last portion is: %s\n", last_portion);
	res->asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag = command_flag;

	while(end == 0)
	{
		printf("flag1\n");
		memset(current_portion, '\0', MAX_LINE);
		for(i = 0; i < NUM_OF_TOKENS; i++)
		{
			token = strchr(last_portion, tokens[i].tok);
			if(token != NULL)
			{
				index_of_tok = (int)(token - last_portion);
				break;
			}
		}
		if(token == NULL)
		{
			end = 1;
			token = &t;
		}
		printf("token is: %c\n", *token);
		switch(*token)
		{
			case '(':
				++open_bracket;
				for(i = 0; i < index_of_tok; i++)
					current_portion[i] = last_portion[i];
				/*Checking if there characters before '('*/
				/***/
					/*IMPORTANT - the if statement below is missing a condition. 
					We need to add the condition - if current_portion is a label that's ok*/
				/***/
				if(current_portion[0] != '\0' && !is_empty(current_portion))
				{
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"illegal characters before '('.");
					return FOUND_ERROR;	
				}
				else
				{
					strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
					strcpy(last_portion, sub_str_temp);
					printf("last_portion is now: %s\n", last_portion);
				}
				break;
			case ',':
				++comma;
				printf("comma is: %d\n", comma);
				if(comma > 1)
				{
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"Extra comma/s.");
					return FOUND_ERROR;
				}
				for(i = 0; i < index_of_tok; i++)
					current_portion[i] = last_portion[i];
				operand = check_reg(current_portion);
				/*Checking if the operand is a register.*/
				if(operand != -1)
				{
					switch(determine_set(command_flag))
					{
						case SET1:
							/*Making sure that the command is not lea and the operand is not so*/
							if(command_flag == LEA && num_of_operands == 0)
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"illegal source operand for lea.");
								return FOUND_ERROR;
							}
							if(open_bracket != 0)
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"illegal bracket/s.");
								return FOUND_ERROR;
							}
							res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
							ops_tags[num_of_operands] = sst_tag_op_register;
							res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.
							set_2_operands.operands[num_of_operands].reg = operand;
							++num_of_operands;
							strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
							strcpy(last_portion, sub_str_temp);
							printf("last portion 2 is: %s\n", last_portion);
							break;	
						case SET2:
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag = 								sst_tag_op_register;
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.reg = 								operand;
							++num_of_operands;
							strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
							strcpy(last_portion, sub_str_temp);
							printf("last portion 3 is: %s\n", last_portion);
							break;	
						case SET3:
							res->syntax_option = sst_syntax_error;
							strcpy(res->syntax_error_buffer,"register after stop/ rts is not allowed.");
							return FOUND_ERROR;
					}		
				}
				/*Checking if the operand is a valid integer*/
				else if(is_immediate(current_portion))
				{
					operand = strtol(current_portion, &token, DEC);
					switch(determine_set(command_flag))
					{
						case SET1:
						/*Making sure that the command is not lea and the operand is not so*/
						if(command_flag == LEA && num_of_operands == 0)
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"illegal source operand for lea.");
								return FOUND_ERROR;
							}
							res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
							ops_tags[num_of_operands] = sst_tag_op_c_number;
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
							operands[num_of_operands].reg = operand;
							++num_of_operands;
							strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
							strcpy(last_portion, sub_str_temp);
							printf("last portion 2 is: %s\n", last_portion);
							break;	
						case SET2:
							/*Making sure that the operand is not an illegal dest operand*/
							if(determine_set(command_flag) == SET2 && command_flag != PRN && num_of_operands == 1)
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"illegal operand.");
								return FOUND_ERROR;
							}
							/*IMPORTANT - 
							This is just a base for what we need to do for set2 -
							If the command is jmp, bne, jsr we need to extract the label's name and only then continue.
							For these commands, no spaces are allowed between the operands.
							The rest of the commands from set1 SHOULD NOT have brackets at all.*/
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag = 								sst_tag_op_c_number;
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.
							operands.c_number = operand;
							++num_of_operands;
							strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
							strcpy(last_portion, sub_str_temp);
							printf("last portion 2 is: %s\n", last_portion);
							break;	
						case SET3:
							res->syntax_option = sst_syntax_error;
							strcpy(res->syntax_error_buffer,"operand after stop/ rts is not allowed.");
							return FOUND_ERROR;
					}
				}
				/***/
				/*this is the part where we need to check if the operand is a label. This part will be completed after the symbol table is 					ready.*/
				/***/
				/*The opperaand is not immediate, register or label*/
				else
				{
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"illegal operand.");
					return FOUND_ERROR;
				}
				break;
			case ')':
				if(open_bracket - close_bracket != 1)
				{
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"close bracket with no open bracket.");
					return FOUND_ERROR;
				}
				++close_bracket;
				for(i = 0; i < index_of_tok; i++)
					current_portion[i] = last_portion[i];
				operand = check_reg(current_portion);
				/*Checking if the operand is a register.*/
				if(operand != -1)
				{
					switch(determine_set(command_flag))
					{
						case SET1:
							/*Making sure that the command is not lea and the operand is not so*/
							if(command_flag == LEA && num_of_operands == 0)
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"illegal source operand for lea.");
								return FOUND_ERROR;
							}
							res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
							ops_tags[num_of_operands] = sst_tag_op_register;
							res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.
							set_2_operands.operands[num_of_operands].reg = operand;
							++num_of_operands;
							strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
							strcpy(last_portion, sub_str_temp);
							printf("last portion 2 is: %s\n", last_portion);
							break;	
						case SET2:
							/*IMPORTANT - 
							This is just a base for what we need to do for set2 -
							If the command is jmp, bne, jsr we need to extract the label's name and only then continue.
							For these commands, no spaces are allowed between the operands.
							The rest of the commands from set1 SHOULD NOT have brackets at all.*/
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag = 								sst_tag_op_register;
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.reg = 								operand;
							++num_of_operands;
							
							strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
							strcpy(last_portion, sub_str_temp);
							printf("last portion 2 is: %s\n", last_portion);
							break;	
						case SET3:
							res->syntax_option = sst_syntax_error;
							strcpy(res->syntax_error_buffer,"register after stop/ rts is not allowed.");
							return FOUND_ERROR;
					}		
				}
				/*Checking if the operand is a valid integer*/
				else if(is_immediate(current_portion))
				{
					operand = strtol(current_portion, &token, DEC);
					switch(determine_set(command_flag))
					{
						case SET1:
						/*Making sure that the command is not lea and the operand is not so*/
						if(command_flag == LEA && num_of_operands == 0)
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"illegal source operand for lea.");
								return FOUND_ERROR;
							}
							/*Making sure that the operand is not an illegal dest operand*/
							if(determine_set(command_flag) == SET1 && command_flag != CMP && num_of_operands == 1)
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"illegal operand.");
								return FOUND_ERROR;
							}
							res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
							ops_tags[num_of_operands] = sst_tag_op_c_number;
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
							operands[num_of_operands].reg = operand;
							++num_of_operands;
							strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
							strcpy(last_portion, sub_str_temp);
							printf("last portion 2 is: %s\n", last_portion);
							break;	
						case SET2:
							/*Making sure that the operand is not an illegal dest operand*/
							if(determine_set(command_flag) == SET2 && command_flag != PRN && num_of_operands == 1)
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"illegal operand.");
								return FOUND_ERROR;
							}
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag = 								sst_tag_op_c_number;
							res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.
							operands.c_number = operand;
							++num_of_operands;
							strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
							strcpy(last_portion, sub_str_temp);
							printf("last portion 2 is: %s\n", last_portion);
							break;	
						case SET3:
							res->syntax_option = sst_syntax_error;
							strcpy(res->syntax_error_buffer,"operand after stop/ rts is not allowed.");
							return FOUND_ERROR;
					}
				}
				/***/
				/*this is the part where we need to check if the operand is a label. This part will be completed after the symbol table is 					ready.*/
				/***/
				/*The opperaand is not immediate, register or label*/
				else
				{
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"illegal operand.");
					return FOUND_ERROR;
				}
				break;
			case ':':
				res->syntax_option = sst_syntax_error;
				strcpy(res->syntax_error_buffer,"illegal character.");
				return FOUND_ERROR;
			case'"':
				res->syntax_option = sst_syntax_error;
				strcpy(res->syntax_error_buffer,"illegal character.");
				return FOUND_ERROR;
			/*None of the tokens were found.*/	
			default:
        	 		switch(determine_set(command_flag))
				{
						case SET1:
							if(num_of_operands == 0)
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"MIssing comma/s");
								return FOUND_ERROR;
							}
							else if(num_of_operands == 1)
							{
								operand = check_reg(last_portion);
								/*Checking if the 2nd operand is a register.*/
								if(operand != -1)
								{
									printf("last_portion now is: %s\n",last_portion);
									res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
									ops_tags[num_of_operands] = sst_tag_op_c_number;
									res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
									operands[num_of_operands].reg = operand;
									++num_of_operands;
								}
								else if(is_immediate(last_portion))
								{
									/*Making sure that the operand is not an illegal dest operand*/
									if(command_flag != CMP && num_of_operands == 1)
									{
										res->syntax_option = sst_syntax_error;
										strcpy(res->syntax_error_buffer,"illegal operand.");
										return FOUND_ERROR;
									}
									res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
									ops_tags[num_of_operands] = sst_tag_op_c_number;
									res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
									operands[num_of_operands].reg = operand;
									++num_of_operands;
								}	
								
							}
							break;
						case SET2:
							/*Checking if jmp/ jsr/ bne are missing essential characters.*/
							if(num_of_operands == 0 && (command_flag == BNE || command_flag == JSR || command_flag == JMP))
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"Missing characters for jmp/ jsr/ bne.");
								return FOUND_ERROR;
							}
							/*Extracting the operand for the rest of the set2 commands.*/
							else if(num_of_operands == 0)
							{
								operand = check_reg(current_portion);
								/*Operand is a register.*/
								if(operand != -1)
								{
									res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.
									ops_tag = sst_tag_op_register;
									res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.
									reg = operand;
									++num_of_operands;
									strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
									strcpy(last_portion, sub_str_temp);
									printf("last portion 2 is: %s\n", last_portion);	
								}
								/*Checking if the operand is a valid integer*/
								else if(is_immediate(current_portion))
								{
									/*From set2, only prn can have immediate as an operand.*/
									if(command_flag == PRN)
									{
										res->asm_directive_and_cpu_instruction.instruction_syntax.
										inst_operands.set_1_operands.ops_tag = sst_tag_op_c_number;
										res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.
										operands.c_number = operand;
										++num_of_operands;
										strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
										strcpy(last_portion, sub_str_temp);
										printf("last portion 2 is: %s\n", last_portion);
									}
									else
									{
										res->syntax_option = sst_syntax_error;
										strcpy(res->syntax_error_buffer,"Illegal operand/s");
										return FOUND_ERROR;
									}
								}
								/***/
								/*this is the part where we need to check if the operand is a label. This part will be 									completed after the symbol table is ready.*/
								/***/
								/*The opperaand is not immediate, register or label*/
								else
								{
									res->syntax_option = sst_syntax_error;
									strcpy(res->syntax_error_buffer,"illegal operand.");
									return FOUND_ERROR;
								}
							}
							break;
						case SET3:
							if(!is_empty(last_portion))
							{
								res->syntax_option = sst_syntax_error;
								strcpy(res->syntax_error_buffer,"illegal characters after stop/ rts.");
								return FOUND_ERROR;
							}
							else
								break;	
				}
				break;
		}/*End of external switch - case*/
		printf("end is: %d\n", end);	
	}/*End of while loop*/
	return 0;
}


int determine_set(int command_flag)
{
	if(command_flag == sst_tag_cpu_i_rts || command_flag == sst_tag_cpu_i_stop)
			return SET3;					
	else if(command_flag == sst_tag_cpu_i_not || command_flag == sst_tag_cpu_i_clr || command_flag == sst_tag_cpu_i_inc ||
		command_flag == sst_tag_cpu_i_dec || command_flag == sst_tag_cpu_i_jmp || command_flag == sst_tag_cpu_i_bne || 
		command_flag == sst_tag_cpu_i_red || command_flag == sst_tag_cpu_i_prn || command_flag == sst_tag_cpu_i_jsr)
			return SET2;
	else
		return SET1;
}

int is_immediate(char str[])
{
	int i, nr_begin, nr_end, nr_sign;
	nr_begin = 0, nr_end = 0;
	for(i = 0; i < strlen(str); i++)
	{
		if(isspace(str[i]) && nr_begin == 0)
			continue;
		if(str[i] == '#' && nr_sign == 0)
			nr_sign = 1;
		if(!isspace(str[i]) && nr_begin == 0 && nr_sign == 1)
		{
			nr_begin = 1;
			continue;
		}
		if(nr_begin == 0 && (str[i] == '+' || str[i] == '-'))
		{
			nr_begin = 1;
			continue;
		}
		if(!(isdigit(str[i])) && nr_begin == 1 && nr_end == 0)
			return 0;
		if(isspace(str[i]) && nr_begin == 1 && nr_end == 0)
			nr_end = 1;
		if(!isspace(str[i]) && nr_end == 1)
			return 0;
	}
	return 1;
}

/*TEMPORARY - FOR TESTING ONLY*/
int main()
{
	char test[] = "		 add  4, r4,";
	sst_get_stt_from_line(test);
	return 0;
}
/*TEMPORARY - FOR TESTING ONLY*/
