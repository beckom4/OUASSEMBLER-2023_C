/*This module analyzes the text */

#include <search.h>
#include <string.h>
#include <ctype.h>

#define MAX_LABEL_LENGTH 32
#define MAX_STRING_DEF_LEN 80
#define MAX_DATA_DEF_LEN 80
#define MAX_SYNTAX_ERR_LEN 120
#define NUM_OF_CMD 16
#define NUM_OF_REGS 8
#define NUM_OF_INSTRUCTIONS 4
#define WORD_SIZE 10
#define REG_SIZE 2
#define NUM_OF_SEPERATORS 6

enum sst_directive_tag {
    sst_tag_dir_entry,
    sst_tag_dir_extern,
    sst_tag_dir_string,
    sst_tag_dir_data
};
/**
 * @param set1, 2 operands;
 * @param set2, 1 operand
 * @param set3, 0 operands
 */
enum sst_cpu_inst_tag {
    sst_tag_cpu_i_mov,
    sst_tag_cpu_i_cmp,
    sst_tag_cpu_i_add,
    sst_tag_cpu_i_sub,
    sst_tag_cpu_i_lea,


    sst_tag_cpu_i_not,
    sst_tag_cpu_i_clr,
    sst_tag_cpu_i_inc,
    sst_tag_cpu_i_dec,
    sst_tag_cpu_i_jmp,
    sst_tag_cpu_i_bne,
    sst_tag_cpu_i_red,
    sst_tag_cpu_i_prn,
    sst_tag_cpu_i_jsr,

    sst_tag_cpu_i_rts,
    sst_tag_cpu_i_stop
};

enum sst_operand_tag {
    sst_tag_op_c_number,
    sst_tag_op_label,
    sst_tag_op_label_with_operands,
    sst_tag_op_register
};

enum sst_option {
    sst_directive,
    sst_instruction,
    sst_white_space,
    sst_comment,
    sst_syntax_error
};
/**
 * syntax struct tree
 * 
 */
struct sst {
    char label[MAX_LABEL_LENGTH +1];
    enum sst_option syntax_option;
    char syntax_error_buffer[MAX_SYNTAX_ERR_LEN + 1];
    union {
        struct {
            enum sst_directive_tag dir_tag;
            union {
                char string[MAX_STRING_DEF_LEN + 1];

                struct  {
                    char data_arr[MAX_DATA_DEF_LEN];
                    char actual_data_size;
                }data_array;

                char label[MAX_LABEL_LENGTH + 1];
            }dir;
        }syntax_directive;
        struct {
            enum sst_cpu_inst_tag cpu_i_tag;
            union {
                struct {
                    enum sst_operand_tag ops_tags[2];
                    union {
                        char reg;
                        char c_number;
                        char label[MAX_LABEL_LENGTH +1];
                    }operands[2];
                }set_2_operands;
                struct {
                    enum sst_operand_tag ops_tag;
                    union {
                        struct {
                            char label[MAX_LABEL_LENGTH +1];
                            enum sst_operand_tag ops_tags[2];
                            union {
                                char reg;
                                char c_number;
                                char label[MAX_LABEL_LENGTH +1];
                            }operands[2];
                        }label_with_ops;

                        char reg;
                        char c_number;
                        char label[MAX_LABEL_LENGTH +1];
                    }operands;
                }set_1_operands;
            }inst_operands;
        }instruction_syntax;
    }asm_directive_and_cpu_instruction;
};

/**
 * creates a SST from a line.
 * 
 * @param line  the line.
 * @return struct sst the sst.
 */
struct sst sst_get_stt_from_line(const char * line);


/*Function declarations:*/

int is_command(const char str[]);

int is_reg(const char str[]);

int is_empty(const char str[]);

int is_instruction(const char str[]);

int is_label(const char str[]);

int is_comment(const char str[]);
