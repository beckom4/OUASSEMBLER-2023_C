/*-----INCLUDES AND DEFINES-----*/

#include "sst.h"
#include "errors_checks.h"
#include "handle_parse.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <limits.h>

#define NUM_OF_INSTRUCTIONS 4
#define NUM_OF_CMD 16
#define NUM_OF_REGS 8
#define WORD_SIZE 10
#define NUM_OF_SEPERATORS 6
#define MAX_LINE 80
#define NUM_OF_TOKENS 5
#define DEC 10
#define FOUND_ERROR 129
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


/*-----FUNCTIOON IMPORT------*/
extern int directive_data_errors(char str[], struct sst *res);
extern int directive_string_errors(char str[], struct sst *res);
extern int directive_entext_errors(char str[], struct sst *res);
extern int label_errors(char str[], struct sst *res);
extern int no_operands_errors(char str[], struct sst *res);
extern int operand_label_errors(char str[], struct sst *res);
extern long int check_reg(const char str[]);
extern long int is_immediate(char str[]);
extern int is_empty(const char str[]);

/*-----FUNCTION DECLARATIONS------*/

/**
 * Handles the parses that were carved up using the token '('.
 * 
 * @param last_portion - The strinng to check.
 * 	  command_flag - The nummerical value( according to the command table) of the command.
 *        index_of_tok - The index in which the token has been discovered.
 *        res - The sst.
 * @return 1 if the process went smoothly.
 *         FOUND_ERROR if an error was found.
 */
int handle_open_bracket(char last_portion[], int command_flag, int index_of_tok, struct sst *res);


/**
 * Handles the parses that were carved up using the token ','.
 * 
 * @param last_portion - The strinng to check.
 * 	  command_flag - The nummerical value( according to the command table) of the command.
 *        num_of_operands - The number of operands that were discovered in the line so far.
 *        comma - The number of commas that were discovered so far.
 *        open_bracket - The number of open brackets that were discovered so far.
 *        close_bracket - The number of close brackets that were discovered so far.
 *        index_of_tok - The index in which the token has been discovered.
 *        res - The sst.
 * @return 1 if the process went smoothly.
 *         FOUND_ERROR if an error was found.
 */
int handle_comma(int command_flag, int num_of_operand, int comma, int close_bracket, int open_bracket, char last_portion[], int index_of_tok, struct sst *res);


/**
 * Handles the parses that were carved up using the token ')'.
 * 
 * @param last_portion - The strinng to check.
 * 	  command_flag - The nummerical value( according to the command table) of the command.
 *        num_of_operands - The number of operands that were discovered in the line so far.
 *        open_bracket - The number of open brackets that were discovered so far.
 *        close_bracket - The number of close brackets that were discovered so far.
 *        index_of_tok - The index in which the token has been discovered.
 *        res - The sst.
 * @return 1 if the process went smoothly.
 *         FOUND_ERROR if an error was found.
 */
int handle_close_bracket(int command_flag, int num_of_operands, int close_bracket, int open_bracket, char last_portion[], int index_of_tok, struct sst *res);


/**
 * Handles the case where we were not able to parse the string or parts of it.
 * 
 * @param last_portion - The strinng to check.
 * 	  command_flag - The nummerical value( according to the command table) of the command.
 *        num_of_operands - The number of operands that were discovered in the line so far.
 *        res - The sst.
 * @return 1 if the process went smoothly.
 *         FOUND_ERROR if an error was found.
 */
int found_no_token(int command_flag, int num_of_operands, char last_portion[], struct sst *res);


/**
 * Determines how many operands are expected per the set definition in the heeader file.
 * 
 * @param command_flag - the tag of the directive.
 * @return SET1 for 2 operands
	   SET2 for 1 operand
	   SET3 for 0 operands
 */
int determine_set(int command_flag);

/*-----FUNCTIONS-----*/

int handle_open_bracket(char last_portion[], int command_flag, int index_of_tok, struct sst *res){
	int i;
	char current_portion[MAX_LINE];
	char *label;
	if(determine_set(command_flag) != SET2){
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"Extra comma/s.");
		return FOUND_ERROR;
	}
	/*1 operand with open bracket - Must be label with 2 ops*/
	res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag = sst_tag_op_label_with_operands;	
	for(i = 0; i < index_of_tok; i++)
		current_portion[i] = last_portion[i];
	current_portion[i] = '\0';
	label = strtok(current_portion, " \t");
	if(operand_label_errors(current_portion, res) != FOUND_ERROR) {
		strcpy(res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.label, label);
		return 1;
	}
	else
		return FOUND_ERROR;
}/*End of handle_open_bracket.*/

int handle_comma(int command_flag, int num_of_operands, int comma, int close_bracket, int open_bracket, char last_portion[], int index_of_tok, struct sst *res){
	int i;
	long int operand;
	char current_portion[MAX_LINE];
	if(comma > 1) {
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"Extra comma/s.");
		return FOUND_ERROR;
	}
	for(i = 0; i < index_of_tok; i++)
		current_portion[i] = last_portion[i];
	current_portion[i] = '\0';
	if(is_empty(current_portion)){
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"Consecutive/ missing comma/s.");
		return FOUND_ERROR;
	}
	operand = check_reg(current_portion);
	/*Checking if the operand is a register.*/
	if(operand != -1) {
		switch(determine_set(command_flag)) {
			/*2 operands with comma - 1st op is reg*/
			case SET1:
				/*Making sure that the command is not lea and the operand is not so*/
				if(command_flag == LEA) {
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"illegal source operand for lea.");
					return FOUND_ERROR;
				}
				/*Commands with 2 operands should not have brackets.*/
				if(open_bracket != 0 || close_bracket != 0) {
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"illegal bracket/s.");
					return FOUND_ERROR;
				}
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[num_of_operands] = sst_tag_op_register;
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[num_of_operands].reg = operand;	
				break;
			/*1 operand with comma - Must be label with 2 operands: 1st op is reg*/	
			case SET2:
				/*The only set 2 commands that have a comma are ones with a label with operands.*/
				if(res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag != sst_tag_op_label_with_operands){
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"illegal operand/s.");
					return FOUND_ERROR;
				}
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[0] = 
				sst_tag_op_register;
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.
				operands.label_with_ops.operands[num_of_operands].reg = operand;
				break;
				/*No operands - error.*/	
				case SET3:
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"register after stop/ rts is not allowed.");
					return FOUND_ERROR;
			}		
		}
		/*Checking if the operand is a valid long integer*/
		else if(is_immediate(current_portion) != LONG_MIN) {
			operand = is_immediate(current_portion);
			if(operand == FOUND_ERROR){
				res->syntax_option = sst_syntax_error;
				strcpy(res->syntax_error_buffer,"Constant number is out of 8 bit range.");
				return FOUND_ERROR;
			}
			switch(determine_set(command_flag)) {
				case SET1:
					/*Making sure that the command is not lea and the operand is not so*/
					if(command_flag == LEA) {
						res->syntax_option = sst_syntax_error;
						strcpy(res->syntax_error_buffer,"illegal source operand for lea.");
						return FOUND_ERROR;
					}
					res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[num_of_operands] = 						sst_tag_op_c_number;
					res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[num_of_operands].c_number = 						operand;
					break;	
				case SET2:
					/*The only set 2 commands that have a comma are ones with a label with operands.*/
					if(res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag != 						sst_tag_op_label_with_operands){
						res->syntax_option = sst_syntax_error;
						strcpy(res->syntax_error_buffer,"illegal operand/s.");
						return FOUND_ERROR;
					}
					res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[0] = 
					sst_tag_op_c_number;
					res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.
					operands.label_with_ops.operands[num_of_operands].c_number = operand;
					break;	
				case SET3:
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"operand after stop/ rts is not allowed.");
					return FOUND_ERROR;
			}
		}
		/*Checking if the operand is a label.*/	
		else if(operand_label_errors(current_portion, res) != FOUND_ERROR) {
			switch(determine_set(command_flag)) {
				/*2 operands.*/
				case SET1:
					res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[num_of_operands] = 						sst_tag_op_label;
					strcpy(res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
					operands[num_of_operands].label, current_portion);
					break;	
				/*1 operand.*/
				case SET2:
					/*The only set 2 commands that have a comma are ones with a label with operands.*/
					if(res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag != 						sst_tag_op_label_with_operands){
						res->syntax_option = sst_syntax_error;
						strcpy(res->syntax_error_buffer,"illegal operand/s.");
						return FOUND_ERROR;
					}
					res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[0] = 
					sst_tag_op_label;							  
					strcpy(res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.
					operands.label_with_ops.operands[num_of_operands].label, current_portion);
					break;
				/*No operands.*/
				case SET3:
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"operand after stop/ rts is not allowed.");
					return FOUND_ERROR;
			}
		}
		/*The opperaand is not immediate, register or label*/
		else {
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"illegal operand.");
			return FOUND_ERROR;
		}
		return 1;
}/*End of handle_comma.*/


int handle_close_bracket(int command_flag, int num_of_operands, int close_bracket, int open_bracket, char last_portion[], int index_of_tok, struct sst *res) {
	int i;
	long int operand;
	char current_portion[MAX_LINE];
	char *label;

	/*Only commands with one operand whose a label with 2 ops should have brackets.*/
	if(determine_set(command_flag) != SET2 || res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag != 				sst_tag_op_label_with_operands){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"Extra comma/s.");
			return FOUND_ERROR;
	}
	/*Close bracket with no open bracket.*/
	else if(open_bracket - close_bracket != 1) {
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"close bracket with no open bracket.");
		return FOUND_ERROR;
	}
	++close_bracket;
	for(i = 0; i < index_of_tok; i++)
		current_portion[i] = last_portion[i];
	current_portion[i] = '\0';
	operand = check_reg(current_portion);
	/*Checking if the operand is a register.*/
	if(operand != -1){
		res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[1] = sst_tag_op_register;
		res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[num_of_operands].reg = operand;
	}
	/*Checking if the operand is a valid integer*/
	else if(is_immediate(current_portion) != LONG_MIN) {
		operand = is_immediate(current_portion);
		if(operand == FOUND_ERROR){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"Constant number is out of 8 bit range.");
			return FOUND_ERROR;
		}
		res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[1] = sst_tag_op_c_number;
		res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[num_of_operands].c_number = 			operand;	
	}
	/*Checking if the operand is a label.*/	
	else if(operand_label_errors(current_portion, res) != FOUND_ERROR) {
		res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[1] = sst_tag_op_label;
		/*Eliminating the closing bracket.*/
		label = strtok(last_portion," \t\n");
		strcpy(res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[num_of_operands].label, 			label);
	}
	/*The operand is not a register, an integer or a label*/
	else {
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"illegal operand.");
		return FOUND_ERROR;
	}
	return 1;
}/*End of handle_close_bracket.*/


int found_no_token(int command_flag, int num_of_operands, char last_portion[], struct sst *res){
	long int operand; 	
	char *label;
																	
	switch(determine_set(command_flag)) {
	   case SET1:
		if(num_of_operands > 1){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"Too many operands");
			return FOUND_ERROR;
		}
		/*commands with 2 operands must have a comma*/
		if(num_of_operands == 0) {
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"MIssing comma/s and/or operand/s");
			return FOUND_ERROR;
		}
		/*This is the second operand.*/
		else if(num_of_operands == 1) {
			operand = check_reg(last_portion);
			/*Checking if the 2nd operand is a register.*/
			if(operand != -1) {
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[num_of_operands] = sst_tag_op_register;
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[num_of_operands].reg = operand;	
			}
			/*The second operand is an integer.*/
			else if(is_immediate(last_portion) != LONG_MIN) {
				operand = is_immediate(last_portion);
	
				if(operand == FOUND_ERROR){
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"Constant number is out of 8 bit range.");
					return FOUND_ERROR;
				}
				/*Making sure that the operand is not an illegal dest operand*/
				if(command_flag != CMP && num_of_operands == 1) {
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"illegal operand/s.");
					return FOUND_ERROR;
				}
				res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[num_of_operands] = sst_tag_op_c_number;
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[num_of_operands].c_number = operand;
			}
			/*The second operand is a label.*/
			else if(operand_label_errors(last_portion, res) != FOUND_ERROR){
				res-> asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[num_of_operands] = sst_tag_op_label;
				label = strtok(last_portion," \t\n");
				strcpy(res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.
				operands[num_of_operands].label,label);
			}
			/*2nd operand is not a register, integer or potential label - error*/
			else {
				res->syntax_option = sst_syntax_error;
				strcpy(res->syntax_error_buffer,"illegal 2nd operand.");
				return FOUND_ERROR;
			}
		}	
		break;
	   case SET2:
		if((num_of_operands > 2 && res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag == 
		   sst_tag_op_label_with_operands) || (num_of_operands > 1 && res->asm_directive_and_cpu_instruction.instruction_syntax.
		    inst_operands.set_1_operands.ops_tag != sst_tag_op_label_with_operands)){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"Too many operands");
			return FOUND_ERROR;
		}
		/*Extracting the operand for the rest of the set2 commands.*/
		if(num_of_operands == 0) {
			operand = check_reg(last_portion);
			if(operand == FOUND_ERROR) {
				res->syntax_option = sst_syntax_error;
				strcpy(res->syntax_error_buffer,"Missing operand.");
				return FOUND_ERROR;
			}
			/*Operand is a register.*/
			if(operand != -1) {
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag = sst_tag_op_register;
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.reg = operand;
									
			}
			/*Checking if the operand is a valid integer*/
			else if(is_immediate(last_portion) != LONG_MIN) {
				operand = is_immediate(last_portion);
				/*From set2, only prn can have immediate as an operand.*/
				if(command_flag == PRN) {
					if(operand == FOUND_ERROR){
						res->syntax_option = sst_syntax_error;
						strcpy(res->syntax_error_buffer,"Constant number is out of 8 bit range.");
						return FOUND_ERROR;
					}
					res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag = sst_tag_op_c_number;
					res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.c_number = operand;
				}
				/*The command is not PRN.*/
				else {
					res->syntax_option = sst_syntax_error;
					strcpy(res->syntax_error_buffer,"Illegal operand/s");
					return FOUND_ERROR;
				}
			}
			/*Operand is a label*/
			else if(operand_label_errors(last_portion, res) != FOUND_ERROR) {
				res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag = sst_tag_op_label;
				label = strtok(last_portion," \t\n");
				strcpy(res->asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label, label);	
			}
			/*Operand is not a register, integer or label*/
			else {
				res->syntax_option = sst_syntax_error;
				strcpy(res->syntax_error_buffer,"illegal operand/s.");
				return FOUND_ERROR;
			}
		}
		break;
	   case SET3:
		if(num_of_operands > 0){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"Too many operands");
			return FOUND_ERROR;
		}
		if(!is_empty(last_portion)) {
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"illegal characters/ operands after stop/ rts.");
			return FOUND_ERROR;
		}
		else
		break;	
	}
	return 1;
}/*End of found_no_token.*/


int determine_set(int command_flag) {
	if(command_flag == sst_tag_cpu_i_rts || command_flag == sst_tag_cpu_i_stop)
			return SET3;					
	else if(command_flag == sst_tag_cpu_i_not || command_flag == sst_tag_cpu_i_clr || command_flag == sst_tag_cpu_i_inc ||
		command_flag == sst_tag_cpu_i_dec || command_flag == sst_tag_cpu_i_jmp || command_flag == sst_tag_cpu_i_bne || 
		command_flag == sst_tag_cpu_i_red || command_flag == sst_tag_cpu_i_prn || command_flag == sst_tag_cpu_i_jsr)
			return SET2;
	else
		return SET1;
}/*End of determine_set.*/
