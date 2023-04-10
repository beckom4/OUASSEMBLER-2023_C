/*-----INCLUDES AND DEFINES-----*/

#include "sst.h"
#include "errors_checks.h"
#include "handle_parse.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


#define NUM_OF_INSTRUCTIONS 4
#define NUM_OF_CMD 16
#define NUM_OF_REGS 8
#define WORD_SIZE 10
#define MAX_LINE 80
#define DEC 10
#define FOUND_ERROR 129
#define MAX_ASCII 127
#define SET1 2
#define SET2 1
#define SET3 0
#define CMP 1
#define LEA 6
#define JMP 9
#define BNE 10
#define PRN 12
#define JSR 13
#define BIT_RANGE_LOW -16384
#define BIT_RANGE_HIGH 16383
#define REG_SIZE 2


/*-----LOCAL FUNCTION DECLARATION-----*/
/**
 * Checks if a string is a directive or not.
 * 
 * @param str - the string to check.
 * @return 1 - the string is a directive.
 *         0 - the string is not a directive.
 */
int is_directive(char str[]);

/**
 * Checks if a string is a register or not.
 * 
 * @param str - the string to check.
 * @return 1 - The string is a register.
 *         0 - The string is not a register.
 */
int is_reg(char str[]);

/**
 * Checks if a string is a command or not.
 * 
 * @param str - the string to check.
 * @return 1 - The string is a coammnd.
 *         0 - The string is not a command.
 */
int is_command(char str[]);

/**
 * Checks if a char is ascii or not.
 * 
 * @param str - the char to check.
 * @return 1 - The char is ascii.
 *         0 - The char is not ascii.
 */
int is_ascii(char c);

/*-----FUNCTIONS-----*/

/**
 * Checks the label for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int label_errors(char str[], struct sst *res){
	int i, word_begin;
	char *label;
	word_begin = 0;
	/*Checking if the first char is ':', meaning if the label is empty*/
	if(str[0] == '\0' || is_empty_error(str)){
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"label is empty");
		return FOUND_ERROR;
	}
	for(i = 0; i < strlen(str);i++){
		if(!isspace(str[i]) && word_begin == 0)
			word_begin = 1;
		if(isspace(str[i]) && word_begin == 1){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"spaces in or after label name.");
			return FOUND_ERROR;
		}
	}		
	label = strtok(str, " \t");
	/*Checking if the label is a command/ register/ directive*/
	if(is_command(label) ==  1 || is_reg(label) == 1 || is_directive(label) == 1){
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"label name is a command/ directive/ register");
		return FOUND_ERROR;
	}	
	for(i = 0; i < strlen(label) - 1; i++)
		if(!(isalpha(label[i]) || isdigit(label[i]))){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"illegal label.");
			return FOUND_ERROR;
		}
	label = strtok(NULL, " \t");
	if(label != NULL){

		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"illegal label.");
		return FOUND_ERROR;
	}
	return 1;		
}/*End of label_errors.*/


/**
 * Checks the directive data for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int directive_data_errors(char str[], struct sst *res)
{
	int i, num, sign;
	char *portion;
	/*Checking if there are too many commas in the text*/
	if(is_empty_error(str) == 1 || str[0] == '\0'){
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"Too few/ too many operands.");
		return FOUND_ERROR;
	}
	portion = strtok(str, " \t\n");
	/*Checking if the this portion is empty*/
	if(portion == NULL){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"illegal parameter/s after .data.");
			return FOUND_ERROR;
	}
	if(portion[0] == '+' || portion[0] == '-')
			sign = 1;
	for(i = sign; i < strlen(portion); i++){
		if(!isdigit(portion[i])){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"illegal char after .data - char is not a digit.");
			return FOUND_ERROR;
		}	 
	}	
	num = atoi(portion);
	/*Checking that the parameter is within the 14 bits range.*/
	if(num < BIT_RANGE_LOW || num > BIT_RANGE_HIGH){
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"illegal char after .data - The integer is not within the 14 bit range");
		return FOUND_ERROR;
	}
	portion = strtok(NULL, " \t\n");
	/*Checking if there are multiple parts in that portion which is an error.*/
	if(portion != NULL){
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"parameter/s after .data must be seperated with a comma.");
			return FOUND_ERROR;
	}
	return 1;	
}/*End of directive_data_errors.*/

/**
 * Checks the directive string for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int directive_string_errors(char str[], struct sst *res)
{
	int i, apostrophes_flag;	
	apostrophes_flag = 0;
	if(is_empty_error(str) == 1 || str[0] == '\0')
	{
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"No operands after directive");
		return FOUND_ERROR;
	}	
	/*Checking if the this portion is empty, meaning 2 consecutive commas*/
	for(i = 0; i < strlen(str); i++)
	{
		if(apostrophes_flag == 1 && str[i] == '\"')
			apostrophes_flag = 0;
		else if(isspace(str[i]))
			continue;
		/*Checking for characters before/ after the postrophesor if there are no apostrophes at all.*/
		else if(apostrophes_flag == 0 && str[i] != '\"')
		{
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer," illegal character/s after .string");
			return FOUND_ERROR;	
		}
		else if(str[i] == '\"')
			apostrophes_flag = 1;
		/*Making sure that the string's content is ascii*/
		if(!is_ascii(str[i]))
		{
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"A character after .string is not ascii.");
			return FOUND_ERROR;
		}
	}
	if(apostrophes_flag == 1)
	{
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"Missing apostrophes");
		return FOUND_ERROR;
	}
	return 1;	
}/*End of directive_string_errors.*/

/**
 * Checks the directives entry and extern for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int directive_entext_errors(char str[], struct sst *res)
{
	int i;
	char copy[MAX_LINE];
	char *portion;

	if(is_empty_error(str) == 1 || str[0] == '\0') {
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"No operands after directive");
			return FOUND_ERROR;
	}
	strcpy(copy, str);
	portion = strtok(copy, " \t");
	for(i = 0; i < strlen(portion) - 1; i++){
		if(!(isalpha(str[i]) || isdigit(str[i]))) {
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"illegal label name.");
			return FOUND_ERROR;
		}
	}
	portion = strtok(NULL, " \t");
	if(portion != NULL){
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"Multiple parameters after directive");
		return FOUND_ERROR;
	}
	return 1;
	
}/*End of directive_ent_ext_errors.*/

/**
 * Checks a command with no operands for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int no_operands_errors(char str[], struct sst *res)
{
	int i;
	for(i = 0; i < strlen(str); i++)
	{
		if(is_empty_error(str) == 0)
		{
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"Extra characters/ operands after stop/ rts.");
			return FOUND_ERROR;
		}	
	}
	return 1;
}/*End of no_operands_errors.*/


/**
 * Checks a command with with a label as operand for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int operand_label_errors(char str[], struct sst *res) {
	int i;
	char copy[MAX_LINE];
	char *portion;

	strcpy(copy, str);
	portion = strtok(copy, " \t");
	for(i = 0; i < strlen(portion) - 1; i++){
		if(!(isalpha(portion[i]) || isdigit(portion[i]))) {
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"illegal label as operand.");
			return FOUND_ERROR;
		}
	}
	portion = strtok(NULL, " \t");
	if(portion != NULL){
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"Multiple parameters before open bracket.");
		return FOUND_ERROR;
	}
	return 1;	
}/*End of operand_label_errors.*/

/**
 * Checks if a string is empty or not.
 * 
 * @param str - the string to check.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int is_empty_error(char str[])
{
	int i;
	for(i = 0; i < strlen(str);i++)
		if(!isspace(str[i]))
			return 0;
	return 1;
}/*End of is_empty_errors.*/

int is_ascii(char c) 
{
	return (c >= 0 && c <= MAX_ASCII);
}/*End of is_ascii.*/


int is_command(char str[])
{
	const char commands[][WORD_SIZE] = {"mov","cmp","add","sub",
				"not","clr","lea","inc",
				"dec","jmp","bne","red",
				"prn","jsr","rts","stop"};	
	int i; 

	for(i = 0; i < NUM_OF_CMD; i++)
	{
		if(strcmp(str,commands[i]) == 0)
			return 1;
	}
	return 0;

}/*End of is_command.*/

int is_reg(char str[])
{
	int i;
	const char regs[][REG_SIZE] = {"r0","r1","r2","r3","r4",
				"r5","r6","r7"};

	for(i = 0; i < NUM_OF_REGS; i++)
	{
		if(strcmp(str,regs[i]) == 0)
			return 1;
	}
	return 0;
}/*End of is_reg.*/


int is_directive(char str[])
{
	int i;
	const char instructions[][WORD_SIZE] = {".data",".string",".entry",".extern"};

	for(i = 0; i < NUM_OF_INSTRUCTIONS; i++)
	{
		if(strcmp(str,instructions[i]) == 0)
			return 1;
	}
	return 0;
}/*End of is_directive.*/
