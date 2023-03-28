
/* ----- INCLUDES: ----- */

#include "sst.h"
#include "errors_checks.h"
#include "handle_parse.h"

#include <search.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


/* ----- DEFINEES: ----- */

#define TRUE 1
#define FALSE 0

#define NUM_OF_INSTRUCTIONS 4
#define NUM_OF_CMD 16
#define NUM_OF_REGS 8
#define WORD_SIZE 10
#define NUM_OF_SEPERATORS 6
#define MAX_LINE_LEN 80
#define NUM_OF_TOKENS 5
#define DEC 10
#define FOUND_ERROR -2
#define SET1 2
#define SET2 1
#define SET3 0
#define CMP 1
#define LEA 6
#define JMP 9
#define BNE 10
#define PRN 12
#define JSR 13
#define BIT_RANGE_LOW -128
#define BIT_RANGE_HIGH 127


/* defines - diractive: */
#define DIRACTIVE_DATA 0
#define DIRACTIVE_STRING 1
#define DIRACTIVE_ENTRY 2
#define DIRACTIVE_EXTERN 3


/* ---- STRUCTS: ----- */


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



/* ----- FUNCTIONS IMPORT: ----- */


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


/* ------- FUNCTIONS DECLARETION: ------- */


/* creates a SST Struct from a line. */
struct sst sst_get_stt_from_line(const char * line);

/* Determines if the current portion of the line is a valid command(instruction). 
 * it it is => return the tag of the command (number from {0,1,2,...,15}).
 * otherwie not => return -1. */
int check_command(const char str[], int **index);


/* Determines if the current portion of the line is a valid register.
 * if it is => return the register number.
 * otherwise => return -1. */
long int check_reg(const char str[]);

 /* Determines if a the line is an empty line or not.
 * if it is => return TRUE.
 * otherwise => return FALSE. */
int line_is_empty(const char given_line[]);

/* Determines if a the line is a comment or not. 
 * if it is => return TRUE.
 * otherwise => return FALSE. */
int is_comment(const char str[]);

/* Determines if a the current portion is a valid directive.
 * if it is => return the tag of the diractive (number from {0,1,2,3})
 * if not => return -1. */
int check_directive(const char str[], int **index, struct sst *res);

 /* Determines if a the current portion is a valid label.
 * if it is => return the size of the label. 
 * if not => return -1. */
int check_label(struct sst *res, const char *line);

/*Builds the sst of the directive recursively.*/
int handle_directive(int directive_flag, char last_portion[], struct sst *res, int k);

/*Builds the sst of the command recursively.*/
int handle_command(int command_flag, char last_portion[], struct sst *res, int num_of_operands);

 /* Determines if the string is a number or not.
 * if it is return => TRUE.
 * if not => return FALSE. */
long int is_immediate(char str[]);


/*----- FUNCTIONS IMPLEMANTATIONS: -----*/


/**
 * creates a SST from a line.
 * 
 * @param line - the line to analize.
 * @return struct sst - the sst of the line.
 **/
struct sst sst_get_stt_from_line(const char * line) {	
	char last_portion[MAX_LINE_LEN];
	int i, j, error_flag, label_flag, command_flag, directive_flag;
	int *index;
	struct sst res;
	i = 0, error_flag = 0, command_flag = -1, directive_flag = -1;
	index = &i;
	
    /*Resetting the label array.*/
	memset(res.label,'\0',MAX_LABEL_LENGTH +1);
	
    /*Checking if this is a comment.*/	
    if (is_comment(line) == 1)
		res.syntax_option = sst_comment;
	
    /*Checking if this is an empty line*/
	else if (line_is_empty(line))
		res.syntax_option = sst_white_space;
	
    /*Checking if this is a command/ directive*/
	else {
		/*Checking for ':', meaning for labels:*/
		label_flag = check_label(&res, line);
		if (label_flag == FOUND_ERROR)
			error_flag = FOUND_ERROR;
		else if (label_flag != -1)
			*index += label_flag + 1;
		if (error_flag != FOUND_ERROR)
			command_flag = check_command(line, &index);
		/*Checking if the current portion of the line is a command or not.*/
		if (command_flag != -1)
			res.syntax_option = sst_instruction;
		/*Checking if this is a directive*/
		else if (error_flag != FOUND_ERROR)
			directive_flag = check_directive(line, &index, &res);
		/*Checking if this is neither a command nor a directive*/
		if (command_flag == -1 && directive_flag == -1){
			error_flag = FOUND_ERROR;
			res.syntax_option = sst_syntax_error;
			strcpy(res.syntax_error_buffer,"Illegal label/command/ directive.");
		}
		
        /*Checking if the directive has no operands*/
		if(directive_flag == FOUND_ERROR)
			error_flag = FOUND_ERROR;
		
        if (error_flag == 0){
			j = 0;
			while (line[*index] != '\0'){
			    last_portion[j] = line[*index];
				j++;
				++(*index);
			}
			last_portion[j] = '\0';	
		}

		if (directive_flag != -1 && error_flag == 0) {
			if (directive_flag == 1){
				error_flag = directive_string_errors(last_portion, &res);
				if (error_flag != FOUND_ERROR)
				/*#*/ memset(res.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr, BIT_RANGE_HIGH + 1, MAX_LINE_LEN);
			}
			
            /*Checking if the directive is .extern and the user defined a label anyway.*/
			else if (directive_flag == 2 && label_flag != -1) 
				/*memset(res.label, '\0', sizeof(res.label));*/
				res -> label = {0};		
			
            if (error_flag != FOUND_ERROR) 
				handle_directive(directive_flag, last_portion, &res, 0);
		}

		/*Handling the command.*/
		if (error_flag != FOUND_ERROR && command_flag != -1)
			if (handle_command(command_flag, last_portion, &res, 0) == FOUND_ERROR)
				error_flag = FOUND_ERROR;						
	}
	
	if (error_flag == FOUND_ERROR)
		res.syntax_option = sst_syntax_error;
   	
    return res;
} /* End of sst_get_stt_from_line */


/* -------------------------------------------- */


/**
 * Determines if the current portion of the line is a valid command(instruction). 
 * it it is => return the tag of the command (number from {0,1,2,...,15}).
 * otherwie not => return -1.
 * 
 *  @param str - the portion in the line to be checked.
 *  @param index - a pointer to apointer that holds the address of the location of the beginning of this portion.
 *  @return .i_tag - the tag of the command if it is a command, a number 0 - 15.
 *            -1 if this portion is not a command.
 */
int check_command(const char str[], int **index) {
	int left, right, mid, j, parse_begin, parse_end,temp;
	char cmd[MAX_LINE_LEN];
	j = 0, parse_begin = 0, parse_end = 0, temp = **index;
	while (parse_end != 1) {
    		if (!isspace(str[**index])) {
       			parse_begin = 1;
        		cmd[j] = str[**index];
        		++j;
    		} 
		if (str[**index] == '\0' && parse_begin == 1) 
       			parse_end = 1;
 		if (isspace(str[**index]) && parse_begin == 1) {
			cmd[j] = '\0';
			parse_end = 1;
		}
		if (str[**index] != '\0')
    			++(**index);
	}

	left = 0;
	right = NUM_OF_CMD - 1;
	
    /*Binary search to find the command*/
	while (left <= right) {
		mid = (left+ right)/2;
        if (strcmp(cmd, sst_cpu_insts[mid].inst_name) > 0)
			left = mid + 1;
		else if (strcmp(cmd, sst_cpu_insts[mid].inst_name) < 0)
			right = mid - 1;
		else return sst_cpu_insts[mid].i_tag;
   	}

	**index = temp;
    return -1;
} /*End of check_command*/


/* -------------------------------------------- */


/**
 * Determines if the current portion of the line is a valid register.
 * if it is => return the register number.
 * otherwise => return -1
 * 
 *  @param str - the portion in the line to be checked.
 *  @return a number from {0,1,2,3,4,5,6,7} that represents the register number.
 *	   -1 if this portion is not a valid register.
 **/
long int check_reg(const char str[]) {
    int i, found;
    char copy[MAX_LINE_LEN];
    char *portion;
    const char regs_arr[][WORD_SIZE] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    found = 0;
    strcpy(copy, str);
    portion = strtok(copy, " \t");
   
    if (portion == NULL)
	    return FOUND_ERROR;
    
    for (i = 0 ; i < NUM_OF_REGS ; i++) {
        if (strcmp(portion, regs_arr[i]) == 0) {
            found = 1;
            break;
        }
    }

    if (found == 0) 
        return -1;
    
    portion = strtok(NULL, " \t");
    
    if (portion != NULL) 
        return -1;
    else 
        return i;
    
} /* End of check_reg. */


/* -------------------------------------------- */


/**
 * Determines if a the line is an empty line or not.
 * if it is => return TRUE.
 * otherwise => return FALSE.
 * 
 * @param given_line - the line.
 * @return TRUE if the line is empty. FALSE if it's not.
 */
int line_is_empty(const char given_line[]) {
	int i, count = 0;

    for(i = 0 ; i < strlen(given_line) ; i++){
        if (isspace(given_line[i]))
            count++;
    }

    if (count == strlen(given_line))
		return TRUE;  /* the line is empty */
	else
		return FALSE;
} /* End of is_empty */


/* -------------------------------------------- */


/**
 * Determines if a the line is a comment or not.
 * 
 *  @param given_line - the line.
 *  @return TRUE if the line is a comment. FALSE if it's not.
 **/
int is_comment(const char given_line[]) {
	int i;
	i = 0;

	while (isspace(given_line[i]))
		++i;
	
    if (given_line[i] == ';')
		return TRUE;
	else
		return FALSE;
}/*End of is_comment.*/


/* -------------------------------------------- */


/**
 * Determines if a the current portion is a valid directive.
 * if it is => return the tag of the diractive (number from {0,1,2,3})
 * if not => return -1
 * 
 *  @param str - the portion to be checked.
 *  @param index - a pointer to a pointer to the address of the beginning of the portion.
 *  @param res - pointer to the sst of the line.
 *  @return a number from {0,1,2,3} that represents the tag of the directive.
 *          -1 if it's not a valid directive.
 */
int check_directive(const char str[], int **index, struct sst *res) {
	int i, j, parse_begin,parse_end,temp;
	char directive[MAX_LINE_LEN];
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

 		if (isspace(str[**index]) && parse_begin == 1) {
			directive[j] = '\0';
			parse_end = 1;
		}

		if (str[**index] != '\0')
    			++(**index);
	}

	for(i = 0 ; i < 4 ; i++) {
		if (strcmp(directive,directives_arr[i]) == 0)
			return i;
	}

	**index = temp;
	return -1;
}/*End of check_directive.*/


/* -------------------------------------------- */


/**
 * Determines if a the current portion is a valid label.
 * if it is => return the size of the label. 
 * if not => return -1
 * 
 *  @param line - pointer to the portion to be checked.
 *  @param res - pointer to the sst of the line.
 *  @return the size of the label, if it is a label.
 *          -1 if it's not a valid label.
 **/
int check_label(struct sst *res, const char *line) {
	int i, index_of_tok;
	char first_portion[MAX_LABEL_LENGTH + 1];
	char *token;

	token = strchr(line, tokens[3].tok);
	if (token == NULL)
		return -1;
	else {
		index_of_tok = (int)(token - line);		
		for (i = 0 ; i < index_of_tok ; i++)
			first_portion[i] = line[i];
		first_portion[index_of_tok] = '\0';
		if (label_errors(first_portion, res) != FOUND_ERROR) {
			strcpy(res->label,first_portion);
			return index_of_tok;
		}
		else
			return FOUND_ERROR;	
	}		
} /* End of check_label.*/


/* -------------------------------------------- */


/**
 * Builds the sst of the directive recursively.
 * 
 *  @param directive_flag - the tag of the directive.
 *  @param last_portion - the portion to be checked.
 *  @param res - pointer the sst of the line.
 *  @param k - the number of parameters after the diractive
 *  @return 0
 **/
int handle_directive(int directive_flag, char last_portion[], struct sst *res, int k){
	int index_of_tok;
	long int param;
	char *token;
	char parameter[MAX_LINE_LEN], sub_str[MAX_LINE_LEN];
    index_of_tok = 0;
	res -> syntax_option = sst_directive;
	
    switch(directive_flag){
		
        /* Handling the directive .data*/
		case DIRACTIVE_DATA:
			res -> asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_data;
			token = strchr(last_portion, tokens[1].tok);
            if (token == NULL) {
				strcpy(parameter, last_portion);
				if (directive_data_errors(parameter, res) != FOUND_ERROR){
					param = strtol(parameter, NULL, DEC);
					res -> asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k] = param;
					return 1;
				}
                else return FOUND_ERROR;
			}
			else {
				index_of_tok = (int)(token - last_portion);
				strcpy(sub_str,&last_portion[index_of_tok + 1]);
				strncpy(parameter, last_portion, index_of_tok);
				parameter[index_of_tok] = '\0';
				
                if (directive_data_errors(parameter, res) != FOUND_ERROR){
					param = strtol(parameter, &token, DEC);
					res -> asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[k] = param;
					res -> asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.actual_data_size = k + 1;
					return handle_directive(directive_flag, sub_str, res, ++k);
				}
				else return FOUND_ERROR;
			}
            break;
            
		/* Handling the directive .string*/
		case DIRACTIVE_STRING:	
			res -> asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_string;
			token = strchr(last_portion, tokens[4].tok);
			if (token == NULL)
				return 1;
			else {	
				index_of_tok = (int)(token - last_portion);
				strcpy(sub_str,&last_portion[index_of_tok + 1]);
				strncpy(res->asm_directive_and_cpu_instruction.syntax_directive.dir.string, last_portion, index_of_tok);
				res -> asm_directive_and_cpu_instruction.syntax_directive.dir.string[index_of_tok] = '\0';
				return handle_directive(directive_flag, sub_str, res, 0);	
			}
            break;

        
    	/*Handling the directive .entry*/
		case DIRACTIVE_ENTRY:
			if (directive_entext_errors(last_portion, res) == FOUND_ERROR) 
                return FOUND_ERROR;
            else {    
			    res -> asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_entry;
			    token = strtok(last_portion," \t");
			    strcpy(res -> asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label, token);
            }
			break;
		
        /*Handling the directive .extern*/
		case DIRACTIVE_EXTERN:
			if (directive_entext_errors(last_portion, res) == FOUND_ERROR)
				return FOUND_ERROR;
			else {
                res -> asm_directive_and_cpu_instruction.syntax_directive.dir_tag = sst_tag_dir_extern;
			    token = strtok(last_portion," \t");
			    strcpy(res -> asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label, token);
            }
		    break;	
	}
	return 0;
} /* End of handle_directive. */


/* -------------------------------------------- */


/**
 * @brief Builds the sst of the command recursively.
 *
 *  @param command_flag - the tag of the directive.
 *  @param last_portion - the portion to be checked.
 *  @param res - pointer to the sst of the line.
 *  @param num_of_operands - the current number of operands that the function handled.
 *  @return 0
 **/
int handle_command(int command_flag, char last_portion[], struct sst *res, int num_of_operands) {
	int i, index_of_tok, open_bracket, close_bracket, comma, end;
	char current_portion[MAX_LINE_LEN], sub_str_temp[MAX_LINE_LEN];
	char *token;
	char t;

	open_bracket = close_bracket = comma = end = 0;	
	token = &t;
	res -> syntax_option = sst_instruction;
	res -> asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag = command_flag;
	
    while(end == 0) {
		memset(current_portion, '\0', MAX_LINE_LEN);
		
        for(i = 0 ; i < NUM_OF_TOKENS ; i++) {
			token = strchr(last_portion, tokens[i].tok);	
            if (token != NULL) {
				index_of_tok = (int)(token - last_portion);
				break;
			}
		}

		if (token == NULL) {
			index_of_tok = strlen(last_portion) - 1;
			end = 1;
			token = &t;
		}
		
        switch(*token) {
			case '(':
				if (handle_open_bracket(last_portion, command_flag, index_of_tok,res) != FOUND_ERROR){
					strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
					strcpy(last_portion, sub_str_temp);
					++open_bracket;
					break;
				}
				else return FOUND_ERROR;
			
            case ',':
				if (handle_comma(command_flag, num_of_operands, comma, close_bracket, open_bracket, last_portion, index_of_tok, res) != FOUND_ERROR){
					strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
					strcpy(last_portion, sub_str_temp);
					++comma;
					++num_of_operands;				
					break;
				}
				else return FOUND_ERROR;
			
            case ')':
				if (handle_close_bracket(command_flag, num_of_operands, close_bracket, open_bracket, last_portion, index_of_tok, res) != FOUND_ERROR) {
					strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
					strcpy(last_portion, sub_str_temp);
					++num_of_operands;
					break;
				}
			    else return FOUND_ERROR;
			
            case ':':
				res->syntax_option = sst_syntax_error;
				strcpy(res->syntax_error_buffer,"illegal character.");
				return FOUND_ERROR;
			
            case '"':
				res -> syntax_option = sst_syntax_error;
				strcpy(res -> syntax_error_buffer, "illegal character.");
				return FOUND_ERROR;
			
            /*None of the tokens were found.*/	
			default:
				if (found_no_token(command_flag, num_of_operands,last_portion, res) != FOUND_ERROR){
					strcpy(sub_str_temp,&last_portion[index_of_tok + 1]);
					strcpy(last_portion, sub_str_temp);
        	 		++num_of_operands;
					break;
				}
				else return FOUND_ERROR;
		}/*End of external switch - case*/	
	}/*End of while loop*/
	return 0;
}/*End of handle_command.*/


/* -------------------------------------------- */


/**
 * Determines if the string is a number or not.
 * if it is return => TRUE.
 * if not => return FALSE. 
 * 
 * @param str - the string to be checked.
 * @return TRUE if the string is a valid integer. FALSE if it's not. 
 **/
long int is_immediate(char str[]){
    char *portion , *endptr;
    char copy[MAX_LINE_LEN];
    int i;
    long int imm;
    portion = strtok(str, " \t");

    if (portion == NULL || portion[0] != '#' || strlen(portion) == 1) 
        return LONG_MIN;
    
    for (i = 1 ; i < strlen(portion) - 1 ; i++)
	    copy[i -1] = portion[i];
    
    copy[i - 1] = '\0';
    imm = strtol(copy, &endptr, DEC); 
    
    if (imm < BIT_RANGE_LOW || imm > BIT_RANGE_HIGH)
	    return FOUND_ERROR;
    
    i = 0;

    if (copy[i] == '-' || copy[i] == '+') 
        i++;
    
    while(copy[i] != '\0'){
    	if (!isdigit(copy[i])) {
        	return LONG_MIN;
   	    }
	 i++;
    }
    return imm;
}/*End of is_immediate.*/


