/* -------------------------------------------- */

/** 
* MAMAN 14, FILE: assembler.h
* @authors: Linoy daniel & Omri Beck
* This module reads files given to us by the user and converts each line in their content into regular binary code.
* In addition, before cooding the text, the assembler goes over the text, picks up label definitions and inserts them into the label table.
**/

/* -------------------------------------------- */


#ifndef ASSEMBLER
#define ASSEMBLER

#define MAX_LINE_LEN 80
/**
 * @brief the main assembler function(API).
 * 
 * @param number_of_files - number of files
 * @param file_names - the files name.
 * @return returns the number of files that were sucessfully assembled.
 */
int assembler_main(int number_of_files, char **file_names);



#endif
