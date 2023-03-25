#include "sst.h"
#include "errors_checks.h"
#include "handle_parse.h"

#include <search.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/*inclusion of functions from errors_checks.c*/
extern int directive_data_errors(char str[], struct sst *res);
extern int directive_string_errors(char str[], struct sst *res);
extern int directive_entext_errors(char str[], struct sst *res);
extern int label_errors(char str[], struct sst *res);
extern int no_operands_errors(char str[], struct sst *res);
extern int operand_label_errors(char str[], struct sst *res);
extern int handle_open_bracket(char last_portion[], int command_flag, int index_of_tok, struct sst *res);
extern int handle_comma(int command_flag, int num_of_operand, int comma, int close_bracket, int open_bracket, char last_portion[], int index_of_tok, struct sst *res);
extern int handle_close_bracket(int command_flag, int num_of_operands, int close_bracket, int open_bracket, char last_portion[], int index_of_tok, struct sst *res);
extern int found_no_token(int command_flag, int num_of_operands, char last_portion[], struct sst *res);

/*Function declarations:*/

/*TEMPORARY*/
void print_sst(struct sst my_sst);

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
long int check_reg(const char str[]);

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
 * Determines if the string is a number or not.
 * 
 * @param str - the string to be checked.
 * @return 1 if the string is a valid integer.
	   0 if it's not. 
 */
long int is_immediate(char str[]);



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

struct sst sst_get_stt_from_line(const char * line) {	
	char last_portion[MAX_LINE];
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
	else {
		/*Checking for ':', meaning for labels:*/
		label_flag = check_label(&res, line);
		if(label_flag == FOUND_ERROR)
			error_flag = FOUND_ERROR;
		else if(label_flag != -1)
			*index += label_flag + 1;
		if(error_flag != FOUND_ERROR)
			command_flag = check_command(line, &index);
		/*Checking if the current portion of the line is a command or not.*/
		if(command_flag != -1)
			res.syntax_option = sst_instruction;
		/*Checking if this is a directive*/
		else if(error_flag != FOUND_ERROR)
			directive_flag = check_directive(line, &index, &res);
		/*Checking if this is neither a command nor a directive*/
		if(command_flag == -1 && directive_flag == -1){
			error_flag = FOUND_ERROR;
			res.syntax_option = sst_syntax_error;
			strcpy(res.syntax_error_buffer,"Illegal label/command/ directive.");
		}
		/*Checking if the directive has no operands*/
		if(directive_flag == FOUND_ERROR)
			error_flag = FOUND_ERROR;
		if(error_flag == 0){
			j = 0;
			while(line[*index] != '\0'){
				last_portion[j] = line[*index];
				j++;
				++(*index);
			}
			last_portion[j] = '\0';	
		}	
		if(directive_flag != -1 && error_flag == 0) {
			if(directive_flag == 1){
				error_flag = directive_string_errors(last_portion, &res);
				if(error_flag != FOUND_ERROR){
					memset(res.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr, BIT_RANGE_HIGH + 1, MAX_LINE);
				}
			}
			/*Checking if the directive is .extern and the user defined a label anyway.*/
			else if(directive_flag == 2 && label_flag != -1) 
				memset(res.label, '\0', sizeof(res.label));		
			if(error_flag != FOUND_ERROR) 
				handle_directive(directive_flag, last_portion, &res, 0);
		}
		/*Handling the command.*/
		if(error_flag != FOUND_ERROR && command_flag != -1)
			if(handle_command(command_flag, last_portion, &res, 0) == FOUND_ERROR)
				error_flag = FOUND_ERROR;						
	}

	/*TEMPORARY - FOR CHECKS ONLY*/
	printf("label_flag is: %d\n", label_flag);
	printf("command_flag is: %d\n", command_flag);
	printf("directive_flag is: %d\n", directive_flag);
		
	printf("tree is: \n");
	print_sst(res);
	/*TEMPORARY - FOR CHECKS ONLY*/
	

	if(error_flag == FOUND_ERROR)
		res.syntax_option = sst_syntax_error;

   	return res;
}

int check_command(const char str[], int **index) {
	int left, right, mid, j, parse_begin, parse_end,temp;
	char cmd[MAX_LINE];
	j = 0, parse_begin = 0, parse_end = 0, temp = **index;
	while (parse_end != 1) {
    		if (!isspace(str[**index])) {
       			parse_begin = 1;
        		cmd[j] = str[**index];
        		++j;
    		} 
		if (str[**index] == '\0' && parse_begin == 1) 
       			parse_end = 1;
 		if(isspace(str[**index]) && parse_begin == 1) {
			cmd[j] = '\0';
			parse_end = 1;
		}
		if(str[**index] != '\0')
    			++(**index);
	}
	left = 0;
	right = NUM_OF_CMD - 1;
	/*Binary search to find the command*/
	while(left <= right) {
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


long int check_reg(const char str[]) {
    int i, found;
    char copy[MAX_LINE];
    char *portion;
    const char regs_arr[][WORD_SIZE] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    found = 0;
    strcpy(copy, str);
    portion = strtok(copy, " \t");
    if(portion == NULL)
	return FOUND_ERROR;
    for (i = 0; i < NUM_OF_REGS; i++) {
        if (strcmp(portion, regs_arr[i]) == 0) {
            found = 1;
            break;
        }
    }
    if (found == 0) {
        return -1;
    }
    portion = strtok(NULL, " \t");
    if (portion != NULL) {
        return -1;
    } 
    else {
        return i;
    }
}


int is_empty(const char str[]) {
	int i, count;
	i = 0;
	count = 0;
	while (i < strlen(str)) {
		if (isspace(str[i]))
			count++;
		i++;
	}
	if(count == strlen(str))
		return 1;
	else
		return 0;
}


int is_comment(const char str[]) {
	int i;
	i = 0;
	while(isspace(str[i]))
		++i;
	if(str[i] == ';')
		return 1;
	else
		return 0;

}

int check_directive(const char str[], int **index, struct sst *res) {
	int i, j, parse_begin,parse_end,temp;
	char directive[MAX_LINE];
	const char directives_arr[][WORD_SIZE] = {".data",".string",".entry",".extern"};
	j = 0;
	temp = **index;
	while (parse_end != 1) {
    		if (!isspace(str[**index])) {
       			parse_begin = 1;
        		directive[j] = str[**index];
        		++j;
		}
    		if (str[**index] == '\0' && parse_begin == 1)
			return -1;
 		if(isspace(str[**index]) && parse_begin == 1) {
			directive[j] = '\0';
			parse_end = 1;
		}
		if(str[**index] != '\0')
    			++(**index);
	}
	for(i = 0; i < NUM_OF_INSTRUCTIONS; i++) {
		if(strcmp(directive,directives_arr[i]) == 0)
			return i;
	}
	**index = temp;
	return -1;
}


int check_label(struct sst *res, const char *line) {
	int i, index_of_tok;
	char first_portion[MAX_LABEL_LENGTH + 1];
	char *token;

	token = strchr(line, tokens[3].tok);
	if(token == NULL)
		return -1;
	else {
		index_of_tok = (int)(token - line);		
		for(i = 0; i < index_of_tok; i++)
			first_portion[i] = line[i];
		first_portion[index_of_tok] = '\0';
		if(label_errors(first_portion, res) != FOUND_ERROR) {
			strcpy(res->label,first_portion);
			return index_of_tok;
		}
		else
			return FOUND_ERROR;	
	}		
}

int handle_directive(int directive_flag, char last_portion[], struct sst *res, int k)
{
	
	int index_of_tok;
	long int param;
	char *token;
	char parameter[MAX_LINE], sub_str[MAX_LINE];
        index_of_tok = 0;
	res->syntax_option = sst_directive;
	switch(directive_flag){
		/*Handling the directive .data*/
		case 0:
			res->asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_data;
			token = strchr(last_portion, tokens[1].tok);
			if(token == NULL) {
				strcpy(parameter, last_portion);
				if (directive_data_errors(parameter, res) != FOUND_ERROR){
					param = strtol(parameter, NULL, DEC);
					printf("param is: %ld\n", param);
					res->asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k] = param;
					printf("the number is: %ld\n", res->asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k]);
					return 1;
				}
				else
					return FOUND_ERROR;
			}
			else {
				index_of_tok = (int)(token - last_portion);
				strcpy(sub_str,&last_portion[index_of_tok + 1]);
				strncpy(parameter, last_portion, index_of_tok);
				parameter[index_of_tok] = '\0';
				if (directive_data_errors(parameter, res) != FOUND_ERROR){
					printf("parameter is: %s\n", parameter);
					param = strtol(parameter, &token, DEC);
					printf("param is: %ld\n", param);
					res->asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k] = param;
					printf("the number is: %ld\n", res->asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k]);
					return handle_directive(directive_flag, sub_str, res, ++k);
				}
				else
					return FOUND_ERROR;
			}
		/*Handling the directive .string*/
		case 1:	
			res->asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_string;
			token = strchr(last_portion, tokens[4].tok);
			if(token == NULL)
				return 1;
			else {	
				index_of_tok = (int)(token - last_portion);
				strcpy(sub_str,&last_portion[index_of_tok + 1]);
				strncpy(res->asm_directive_and_cpu_instruction.syntax_directive.dir.string, last_portion, index_of_tok);
				res->asm_directive_and_cpu_instruction.syntax_directive.dir.string[index_of_tok] = '\0';
				return handle_directive(directive_flag, sub_str, res, 0);	
			}
		/*Handling the directive .entry*/
		case 2:
			if(directive_entext_errors(last_portion, res) == FOUND_ERROR)
				return FOUND_ERROR;
			res->asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_entry;
			token = strtok(last_portion," \t");
			strcpy(res->asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label, token);
			break;
		/*Handling the directive .extern*/
		case 3:
			if(directive_entext_errors(last_portion, res) == FOUND_ERROR)
				return FOUND_ERROR;
			res->asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_extern;
			token = strtok(last_portion," \t");
			strcpy(res->asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label, token);
			break;	
	}
	return 0;
}


int handle_command(int command_flag, char last_portion[], struct sst *res, int num_of_operands) {
	int i, index_of_tok, open_bracket, close_bracket, comma, end;
	char current_portion[MAX_LINE], sub_str_temp[MAX_LINE];
	char *token;
	char t;

	open_bracket = 0, close_bracket = 0, comma = 0, end = 0;	
	token = &t;
	res->syntax_option = sst_instruction;

	res->asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag = command_flag;
	while(end == 0) {
		memset(current_portion, '\0', MAX_LINE);
		for(i = 0; i < NUM_OF_TOKENS; i++) {
			token = strchr(last_portion, tokens[i].tok);
			if(token != NULL) {
				index_of_tok = (int)(token - last_portion);
				break;
			}
		}
		if(token == NULL) {
			end = 1;
			token = &t;
		}
		switch(*token) {
			case '(':
				if(handle_open_bracket(last_portion, command_flag, index_of_tok,res) != FOUND_ERROR){
					strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
					strcpy(last_portion, sub_str_temp);
					++open_bracket;
					break;
				}
				else	
					return FOUND_ERROR;
			case ',':
				if(handle_comma(command_flag, num_of_operands, comma, close_bracket, open_bracket, last_portion, index_of_tok, res) != FOUND_ERROR){
					strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
					strcpy(last_portion, sub_str_temp);
					++comma;
					++num_of_operands;				
					break;
				}
				else
					return FOUND_ERROR;
			case ')':
				if(handle_close_bracket(command_flag, num_of_operands, close_bracket, open_bracket, last_portion, index_of_tok, res) != FOUND_ERROR) {
					/*if(last_portion[index_of_tok + 1] != '\0')*/
					strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
					strcpy(last_portion, sub_str_temp);
					++num_of_operands;
					break;
				}
				else
					return FOUND_ERROR;
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
				if(found_no_token(command_flag, num_of_operands,last_portion, res) != FOUND_ERROR){
					strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
					strcpy(last_portion, sub_str_temp);
        	 			++num_of_operands;
					break;
				}
				else
					return FOUND_ERROR;
		}/*End of external switch - case*/	
	}/*End of while loop*/
	return 0;
}



long int is_immediate(char str[]){
    char *portion , *endptr;
    char copy[MAX_LINE];
    int i;
    long int imm;
    portion = strtok(str, " \t"); 
    if (portion == NULL || portion[0] != '#' || strlen(portion) == 1) 
        return LONG_MIN;
    for(i = 1; i < strlen(portion); i++)
	copy[i -1] = portion[i];
    copy[i - 1] = '\0';
    imm = strtol(copy, &endptr, DEC); 
    if(imm < BIT_RANGE_LOW || imm > BIT_RANGE_HIGH)
	return FOUND_ERROR;
    i = 1;
    if (portion[i] == '-' || portion[i] == '+') {
        i++;
    }
    while(portion[i] != '\0'){
    	if (!isdigit(portion[i])) {
        	return LONG_MIN;
   	 }
	 i++;
    }
    return imm;
}















