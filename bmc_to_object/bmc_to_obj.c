/* -------------------------------------------- */

/**
* MAMAN 14, FILE: bmc.c
* Authors: Linoy daniel & Omri Beck
**/

/* -------------------------------------------- */


/* --------- INCLUDES: --------- */


#include <stdlib.h>
#include <stdio.h>
#include "bmc_to_obj.h"
#include "../dynarray/dynarray.h"



/* --------- DEFINES: --------- */


#define MEMORY_ADDR_PREFIX '0'
#define PROG_START_ADDR 100
#define WORD_LEN 14

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
* @return returns end address.
**/
static int print_given_list_in_binary_format(int *given_list,FILE * ob,int start_addr);


/* --------- FUNCTIONS IMPLEMENTATION: --------- */


void print_in_binary_format(int *code_list, int *data_list,FILE * ob){
    unsigned int code_len = dynarray_length(code_list);
    unsigned int data_len = dynarray_length(data_list);
   
    /* print the first line in the output: */
    fprintf(ob,"\t\t\t\t");
    fprintf(ob,"%u %u\n", code_len, data_len);
    /* print code list: */
    
    /* print data list: */
    print_given_list_in_binary_format(data_list,ob,print_given_list_in_binary_format(code_list,ob,PROG_START_ADDR));
}



static int print_given_list_in_binary_format(int *given_list,FILE * ob,int start_addr){
    unsigned int length = dynarray_length(given_list);
    int i;
    const char *binary_str;
    /* print the current element in given list to binary "..//./" and print it */
    for (i = 0 ; i < length ; i++){
        /* print format "0[MEMORY_ADDRESS]": */
        fprintf(ob,"%c%d", MEMORY_ADDR_PREFIX, start_addr);
        fprintf(ob,"\t\t");
        /* convert given_list[i] to binary string "./..//." and print the result: */
        binary_str = convert_word_bits_to_binary_format(given_list[i]);
        fprintf(ob,"%s", binary_str);
        free((void*)binary_str);
        fprintf(ob,"\n");
       
        /* premote to the next address: */
        start_addr++;
    }
    return start_addr;
}



static char* convert_word_bits_to_binary_format(int one_memory_word){
    int ndx, bit_mask = 1;
    char* res = (char*)calloc(WORD_LEN + 1, sizeof(char));
    if (res == NULL)
        return NULL;

    for (ndx = WORD_LEN - 1 ; ndx >= 0 ; ndx--) {
        /* if mem_word & bit_mask != 0 => '/', otherwise => '.': */
        res[ndx] = (one_memory_word & bit_mask) ? '/' : '.';
        bit_mask = bit_mask << 1; 
    }
    return res;
}














