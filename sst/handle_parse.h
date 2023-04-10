/* -------------------------------------------- */

/** 
* MAMAN 14, FILE: handle_parse.h
* @authors: Linoy daniel & Omri Beck
* This file is part of the sst module. It's responsible for analyzing and handling the different parses that were carved up from the original line.
**/

/* -------------------------------------------- */


int handle_open_bracket(char last_portion[], int command_flag, int index_of_tok, struct sst *res);

int handle_comma(int command_flag, int num_of_operand, int comma, int close_bracket, int open_bracket, char last_portion[], int index_of_tok, struct sst *res);

int handle_close_bracket(int command_flag, int num_of_operands, int close_bracket, int open_bracket, char last_portion[], int index_of_tok, struct sst *res);

int found_no_token(int command_flag, int num_of_operands, char last_portion[], struct sst *res);

int determine_set(int command_flag);
