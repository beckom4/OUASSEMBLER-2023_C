/* -------------------------------------------- */

/** 
* MAMAN 14, FILE: errors_checks.h
* @authors: Linoy daniel & Omri Beck
* This file is part of the sst module and supports it by checking the text, or some of it, for errors.
**/

/* -------------------------------------------- */


int label_errors(char str[], struct sst *res);

int directive_data_errors(char str[], struct sst *res);

int directive_string_errors(char str[], struct sst *res);

int directive_entext_errors(char str[], struct sst *res);

int no_operands_errors(char str[], struct sst *res);

int operand_label_errors(char str[], struct sst *res);

int is_empty_error(char str[]);





