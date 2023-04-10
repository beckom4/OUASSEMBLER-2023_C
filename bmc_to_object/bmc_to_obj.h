/* -------------------------------------------- */

/** 
* MAMAN 14, FILE: bmc.h
* @authors: Linoy daniel & Omri Beck
* This module converts the regular binary code into '.' and '/' binary, and outputs it to a file.
**/

/* -------------------------------------------- */


#ifndef BMC_H
#define BMC_H


/* --------- DEFINES: --------- */



/*#define WORD_LEN 14*/


/* --------- FUNCTIONS DECLARATIONS: --------- */





/**
* @brief Gets 2 lists of memory words: list of code words & list of data words, 
* and print its content in binary format ".//..././"  
*
* @param int *code_list - pointer to list of code words.
* @param int *data_list - pointer to list of data words. 
*/
void  print_in_binary_format(int *code_list, int *data_list,FILE * ob);

#endif
