/* --------- INCLUDES: --------- */


#include <stdlib.h>
#include <stdio.h>
#include "bmc.h"
#include "dynarray.h"


/* --------- DEFINES: --------- */


#define MEMORY_ADDR_PREFIX '0'


/* --------- STATIC FUNCTIONS DECLARATIONS: --------- */


/** 
* @brief Gets one memory word (int value) and convert its bits to binary format ".//../././" 
*
* @param int one_memory_word - the memory word to convert 
* @return pointer to string result after convert
* */
static char* convert_word_bits_to_binary_format(int one_memory_word);


/**
* @brief Gets int* pointer to given_list (dynarray) of memory words,
* and print its output binary format ".///../." 
*
* @param *given_list - the pointer to list of memory words 
**/
static void print_given_list_in_binary_format(int *given_list);


/* --------- FUNCTIONS IMPLEMENTATION: --------- */


/**
* Gets 2 lists of memory words: list of code words & list of data words, 
* and print its content in binary format ".//..././" 
**/
void print_in_binary_format(int *code_list, int *data_list){
    unsigned int code_len = dynarray_length(code_list);
    unsigned int data_len = dynarray_length(data_list);
   
    /* print the first line in the output: */
    printf("\t\t\t\t");
    printf("%ud, %ud", code_len, data_len);
    /* print code list: */
    print_given_list_in_binary_format(code_list);
    /* print data list: */
    print_given_list_in_binary_format(data_list);
}/*End of print_in_binary_format*/



static void print_given_list_in_binary_format(int *given_list){
    unsigned int length = dynarray_length(given_list);
    int current_addres = PROG_START_ADDR;
    int i;

    /* print the current element in given list to binary "..//./" and print it */
    for (i = 0 ; i < length ; i++){
        /* print format "0[MEMORY_ADDRESS]": */
        printf("%c%d", MEMORY_ADDR_PREFIX, current_addres);
        printf("\t\t");
        /* convert given_list[i] to binary string "./..//." and print the result: */
        printf("%s", convert_word_bits_to_binary_format(given_list[i]));
        printf("\n");
       
        /* premote to the next address: */
        current_addres++;
    }
}/*End of print_given_list_in_binary_format*/



static char* convert_word_bits_to_binary_format(int one_memory_word){
    int ndx, bit_mask = 1;
    char* res = (char*)calloc(WORD_LEN, sizeof(char));
    if (res == NULL) { 
         	printf("Memory allocation error.\n"); 
		return NULL;
   	 }

    for (ndx = WORD_LEN - 1 ; ndx >= 0 ; ndx--) {
        /* if mem_word & bit_mask != 0 => '/', otherwise => '.': */
        res[ndx] = (one_memory_word & bit_mask) ? '/' : '.';
        bit_mask = bit_mask << 1; 
    }
    return res;
}/*End of convert_word_bits_to_binary_format*/

