#include "sst.h"
#include "errors_checks.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

int directive_data_errors(char str[], struct sst *res);

int is_empty_error(char str[]);

int is_directive(char str[]);

int is_reg(char str[]);

int is_command(char str[]);

int label_errors(char str[], struct sst *res)
{
	int i;
	/*Checking if the first char is ':', meaning if the label is empty*/
	if(str[0] == '\0')
	{
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"label is empty");
		return 1;
	}
	/*Checking if the label is a command/ register/ directive*/
	if(is_command(str) ==  1 || is_reg(str) == 1 || is_directive(str) == 1)
	{
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"label name is a command/ directive/ register");
		return 1;
	}	
	for(i = 0; i < strlen(str); i++)
		if(!isalpha(str[i]))
		{
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"label has non-alphabetical characters.");
			return 1;
		}
	return 0;		
}

int directive_data_errors(char str[], struct sst *res)
{
	int i, first_digit_flag;
	first_digit_flag = 0;
	/*Checking if there are too many commas n tthe text*/
	if(is_empty_error(str) == 1 || str[0] == '\0')
	{
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"Extra comma after .data.");
		return 1;
	}		
	/*Checking if the this portion is empty, meaning 2 consecutive commas*/
	for(i = 0; i < strlen(str); i++)
	{
		if(!isdigit(str[i]) && !isspace(str[i]) && str[i] != '+' && str[i] != '-')
		{
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"illegal char after .data - char is not a digit.");
			return 1;
		}
		else if(isdigit(str[i]))
			first_digit_flag = 1;
		if(first_digit_flag == 1 && !isdigit(str[i]))
		{
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"Extra characters after parameter.");
			return 1;
		}
	}
	return 0;	
}

int directive_string_errors(char str[], struct sst *res)
{
	int i, apostrophes_flag;	
	apostrophes_flag = 0;	
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
			return 1;	
		}
		else if(str[i] == '\"')
			apostrophes_flag = 1;
		/*Making sure that the string's content is ascii*/
		if(!is_ascii(str[i]))
		{
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"A character after .string is not ascii.");
			return 1;
		}
	}
	if(apostrophes_flag == 1)
	{
		res->syntax_option = sst_syntax_error;
		strcpy(res->syntax_error_buffer,"Missing apostrophes");
		return 1;
	}
	return 0;	
}

int directive_entext_errors(char str[], struct sst *res)
{
	int i,first_word_begin,first_word_end;
	first_word_begin = 0, first_word_end = 0;
	/*Checking for multiple words.*/
	for(i = 0; i < strlen(str); i++)
	{
		if(isspace(str[i]) && first_word_begin == 0)
			continue;
		if(!isspace(str[i]) && first_word_begin == 0)
			first_word_begin = 1;
		if(first_word_begin == 1 && isspace(str[i]))
			first_word_end = 1;
		if(first_word_end == 1 && !isspace(str[i]))
		{
			res->syntax_option = sst_syntax_error;
			strcpy(res->syntax_error_buffer,"Multiple parameters after directive");
			return 1;
		}	
	}
	return 0;
}


int is_empty_error(char str[])
{
	int i;
	for(i = 0; i < strlen(str);i++)
		if(!isspace(str[i]))
			return 0;
	return 1;
}

int is_ascii(char c) 
{
	return (c >= 0 && c <= 127);
}


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

}

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
}


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
}
