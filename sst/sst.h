/* -------------------------------------------- */

/** 
* MAMAN 14, FILE: sst.h
* @authors: Linoy daniel & Omri Beck
*This module is the sst(syntax struct tree) - It analyzes each text line, breaks it down and stores each part of the line(or all of it).
* in its designated spot in the syntax tree.
*/

/* -------------------------------------------- */

#define MAX_LABEL_LENGTH 32
#define MAX_STRING_DEF_LEN 80
#define MAX_DATA_DEF_LEN 80
#define MAX_SYNTAX_ERR_LEN 120

enum sst_directive_tag {
    sst_tag_dir_data,
    sst_tag_dir_string,
    sst_tag_dir_entry,
    sst_tag_dir_extern
};

enum sst_cpu_inst_tag {
    sst_tag_cpu_i_mov,
    sst_tag_cpu_i_cmp,
    sst_tag_cpu_i_add,
    sst_tag_cpu_i_sub,


    sst_tag_cpu_i_not,
    sst_tag_cpu_i_clr,

    sst_tag_cpu_i_lea,

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
 * syntax struct tree - 
 * Defining the tree and its fields using unions.
 */

struct sst {
    char label[MAX_LABEL_LENGTH +1];
    enum sst_option syntax_option;
    char syntax_error_buffer[MAX_SYNTAX_ERR_LEN + 1];
    union {
        struct {
            enum sst_directive_tag dir_tag;
            union {
		struct  {
                    long int data_arr[MAX_DATA_DEF_LEN];
                    unsigned int actual_data_size;
                }data_array;

                char string[MAX_STRING_DEF_LEN + 1];
		struct  {
                    char label[MAX_LABEL_LENGTH + 1];
		}label_array; 
            }dir;
        }syntax_directive;
        struct {
            enum sst_cpu_inst_tag cpu_i_tag;
            union {
                struct {
                    enum sst_operand_tag ops_tags[2];
                    union {
                        long int reg;
                        long int c_number;
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
                                long int reg;
                                long int c_number;
                                char label[MAX_LABEL_LENGTH +1];
                            }operands[2];
                        }label_with_ops;

                        long int reg;
                        long int c_number;
                        char label[MAX_LABEL_LENGTH +1];
                    }operands;
                }set_1_operands;
            }inst_operands;
        }instruction_syntax;
    }asm_directive_and_cpu_instruction;
};




struct sst sst_get_stt_from_line(const char * line);

