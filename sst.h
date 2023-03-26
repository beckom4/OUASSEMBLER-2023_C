/*This module analyzes the text and builds a syntax tree out of a given line. */

#define MAX_LABEL_LENGTH 30
#define MAX_STRING_DEF_LEN 80
#define MAX_DATA_DEF_LEN 80
#define MAX_SYNTAX_ERR_LEN 120
#define NUM_OF_CMD 16
#define NUM_OF_REGS 8
#define NUM_OF_INSTRUCTIONS 4
#define WORD_SIZE 10
#define NUM_OF_SEPERATORS 6
#define MAX_LINE 80
#define NUM_OF_TOKENS 5
#define DEC 10
#define FOUND_ERROR -2
#define SET1 2
#define SET2 1
#define SET3 0
#define CMP 1
#define LEA 6
#define JMP 9
#define BNE 10
#define PRN 12
#define JSR 13
#define BIT_RANGE_LOW -128
#define BIT_RANGE_HIGH 127

enum sst_directive_tag {
    sst_tag_dir_data,
    sst_tag_dir_string,
    sst_tag_dir_entry,
    sst_tag_dir_extern
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
/* -------------------------------------------- */

/** 
* MAMAN 14, FILE: sst.h
* @authors: Linoy daniel & Omri Beck
*This module is the sst(syntax struct tree) - It analyzes each text line, breaks it down and stores each part of the line(or all of it).
* in its designated spot in the syntax tree.
*/

/* -------------------------------------------- */



#define MAX_LABEL_LENGTH 30
#define MAX_STRING_DEF_LEN 80
#define MAX_DATA_DEF_LEN 80
#define MAX_SYNTAX_ERR_LEN 120
#define NUM_OF_CMD 16
#define NUM_OF_REGS 8
#define NUM_OF_INSTRUCTIONS 4
#define WORD_SIZE 10
#define NUM_OF_SEPERATORS 6
#define MAX_LINE 80
#define NUM_OF_TOKENS 5
#define DEC 10
#define FOUND_ERROR -2
#define SET1 2
#define SET2 1
#define SET3 0
#define CMP 1
#define LEA 6
#define JMP 9
#define BNE 10
#define PRN 12
#define JSR 13
#define BIT_RANGE_LOW -128
#define BIT_RANGE_HIGH 127

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



/**
 * creates a SST from a line.
 * 
 * @param line - the line to analize.
 * @return struct sst - the sst of the line.
 */
struct sst sst_get_stt_from_line(const char * line);

/**
 * Determines if the current portion of the line is a valid command(instruction).
 * 
 * @param str - the portion in the line to be checked.
	  index - a pointer to apointer that holds the address of the location of the beginning of this portion.
 * @return .i_tag - the tag of the command if it is a command, a number 0 - 15.
	   -1 if this portion is not a command.
 */
int check_command(const char str[], int **index);

/**
 * Determines if a the current portion of the line is a valid register.
 * 
 * @param str - the portion in the line to be checked.
 * @return .i: a number 0 - 7 that represents the register number.
	   -1 if this portion is not a valid register.
 */
long int check_reg(const char str[]);

/**
 * Determines if a the line is an empty line or not.
 * 
 * @param str - the line.
 * @return 1 if the line is empty.
	   0 if it's not.
 */
int is_empty(const char str[]);

/**
 * Determines if a the line is a comment or not.
 * 
 * @param str - the line.
 * @return 1 if the line is a comment.
	   0 if it's not.
 */
int is_comment(const char str[]);


/**
 * Determines if a the current portion is a valid directive.
 * 
 * @param str - the portion to be checked.
	  index - a pointer to a pointer to the address of the beginning of the portion.
	  struct sst - the sst of the line.
 * @return i - A number 0-3 that represents the tag of the directive.
	   -1 if it's not a valid directive.
 */
int check_directive(const char str[], int **index, struct sst *res);

/**
 * Determines if a the current portion is a valid label.
 * 
 * @param str - the portion to be checked.
	  struct sst - the sst of the line.
 * @return label_flag - the size of the label, if it is a label.
	   -1 if it's not a valid label.
 */
int check_label(struct sst *res, const char *line);

/**
 * Builds the sst of the directive recursively.
 * 
 * @param directive_flag - the tag of the directive.
	  last_portion - the portion to be checked.
	  struct sst - the sst of the line.
 * @return 0
 */
int handle_directive(int directive_flag, char last_portion[], struct sst *res, int k);

/**
 * Builds the sst of the command recursively.
 * 
 * @param command_flag - the tag of the directive.
	  last_portion - the portion to be checked.
	  struct sst - the sst of the line.
	  num_of_operands - the current number of operands that the function handled.
 * @return 0
 */
int handle_command(int command_flag, char last_portion[], struct sst *res, int num_of_operands);

/**
 * Determines if the string is a number or not.
 * 
 * @param str - the string to be checked.
 * @return 1 if the string is a valid integer.
	   0 if it's not. 
 */
long int is_immediate(char str[]);

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
		struct  {
                    long int data_arr[MAX_DATA_DEF_LEN];
                    char actual_data_size;
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



