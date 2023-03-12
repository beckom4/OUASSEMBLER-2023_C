/*This file serves as the preprocessor - It consists of the functions that find macros in the files, check them for errors, analyze them and save their content.*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct Macros *Macros;

#define END_PROGRAM 1
#define MAX_MACRO 200
#define MAX_LINE 80
#define MAX_CMD_ARR 4

/*
	The main function of the preprocessor. It produces a file whose name is the same as the one it got, only with extended_ attached to it.
	The file produced will replace each macro with its content.
	@param: 
		fptr is a pointer to the file.
		file name is a pointer to the name of the file
	@return:
		0 if the orocess ran smoothly.
		1 if severe issues emerged that required the program to terminate.
*/
int preprocessor(FILE *fptr, char *file_name);
/*
	This function checks whether the word we're currently checking should be replaced with a macro or not. 
	@param: 
		head is a pointer to the head of the linked list of the macros.
		original line is a string that has the content of the current line of the text we're checking.
	@return:
		The index of the macro if it is in the list.
		-1 if the line contains no macro names that should be replaced.
*/
int is_in_macro_list(struct Macros *head, char original_line[]);
/*
	This function creates a new node and adds it tot head of the linked list.
	@param: 
		next is a pointer to the next node.
		macro_counter is an integer that indicates the index of the macro in   the list.
		macro_name is a pointer to the name of the macro.
	@return:
		A new node which will now bw the new head of the list.
*/
struct Macros *create_macro(struct Macros *next, int macro_counter, char *macro_name);
/*
	This function replaces the macro's name with its content.
	@param: 
		new_txt is a pointer to the pointer to new_txt.
		original_line is the original current line in the text to which the macro's content needs to be inserted.
		head is a pointer to the head of the linked list of macros.
		macro_index is the index of the macro whose content needs to be inserted to the text.
*/
void insert_string(char **new_txt, char original_line[], struct Macros *head, int macro_index) ; 
/*
	This function frees the memory that we used in order to create and maintain the linked list. 
	@param: 
		head is a pointer to the head of the linked list of macros.
*/
void free_list(struct Macros *head) ;
/*
	This function produces the files that has the extended text including the content of the macros.
	@param: 
		new_txt is a pointer to the newtext that will be printed to the file. 
		old_file_name is a pointer to the old name of the file. 
	@return:
		0 if it went smoothly.
		1 if any issues emerged that required the program to be terminated.
*/
int produce_file(char *new_txt, char *old_file_name);
/*
	This function checks whhether or not a macro's name is a command's name which is an error.
	@param: 
		macro_name is a pointer to the macro's name.
	@return:
		0 if the macro's name is not a command's name.
		1 if it is a command's name.
*/
int is_command(char *macro_name);

