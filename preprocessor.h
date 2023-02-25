/*This file serves as the preprocessor - It consists of the functions that find macros in the files, check them for errors, analyze them and save their content.*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct Macros *Macros;

#define END_PROGRAM 1
#define MAX_MACRO 200
#define MAX_LINE 80


int is_in_macro_list(struct Macros* head, char portion[]); 
char *insert_string(char *new_txt, int macroflag, struct Macros *head); 
