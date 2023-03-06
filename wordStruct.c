#include "wordStruct.h"

struct wordStruct{
    unsigned int era : 2; /* 0,1 */
    unsigned int dest_operand : 2; /* 2,3 */
    unsigned int source_operand : 2; /* 4,5 */
    unsigned int opcode : 4; /* 6,7,8,9 */
    unsigned int param2 : 2; /* 10,11 */
    unsigned int param1 : 2; /* 12,13 */
};


/********************************************************/


wordStruct *initialize_word_struct(void) {
    wordStruct *word_struct = malloc(sizeof(wordStruct));
    if (word_struct)
        return word_struct;
    else {
        /*
         * give some exception
         */
        return NULL;
    }
}


/********************************************************/


int get_era(wordStruct *word) {
    /*
     * == (*word).era
     */
    return word -> era;
}


/********************************************************/


int get_dest_operand(wordStruct *word) {
    return word -> dest_operand;
}


/********************************************************/


int get_source_operand(wordStruct *word) {
    return word -> source_operand;
}


/********************************************************/


int get_opcode(wordStruct *word) {
    return word -> opcode;
}


/********************************************************/


int get_param2(wordStruct *word) {
    return word -> param2;
}


/********************************************************/


int get_param1(wordStruct *word) {
    return word -> param1;
}


/********************************************************/


void set_dest_operand(unsigned int destOp) {
  /* should we check? */
  dest_operand = destOp;
}


/********************************************************/


void set_source_operand(unsigned int sourceOp) {
  source_operand = sourceOp;
}

