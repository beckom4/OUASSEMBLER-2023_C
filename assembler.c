#include "assembler.h"
#include "../sst/sst.h"
#include <stdio.h>
#include <string.h>
#include "../dynarray/dynarray.h"

#define PROG_START_ADDR 100


#define is_inst_tag_set_A(tag) (tag >= sst_tag_cpu_i_mov && tag <= sst_tag_cpu_i_lea )
#define is_inst_tag_set_B(tag) (tag >= sst_tag_cpu_i_not && tag <= sst_tag_cpu_i_jsr )
#define is_inst_tag_set_C(tag) (tag >= sst_tag_cpu_i_rts && tag <= sst_tag_cpu_i_stop )

struct asm_label {
    char label_str[MAX_LABEL_LENGTH +1 ];
    int address;
    enum {
        asm_label_type_extern,
        asm_label_type_entry,
        asm_label_type_code,
        asm_label_type_data,
        asm_label_type_data_entry,
        asm_label_type_code_entry
    }asm_label_type;
    int line_of_definition;
};

struct CU {
    struct asm_label * symbol_table;
    void * bmc_code_list;
    void * bmc_data_list;
    void * extern_list;
};

static struct asm_label * assembler_search_symbol(struct asm_label * symbol_table,const char * symbol) {
    int i;
    for(i =0; i<dynarray_length(symbol_table);i++) {
        if(strcmp(symbol_table[i].label_str,symbol) == 0) {
            return &symbol_table[i];
        }
    }
    return NULL;
}
/**
 * @brief builds the symbol table...
 * 
 * @param am_file assuming file is open in read mode, and is ok ( not NULL) .
 * @param cu pointer to CU struct.
 * @return returns 0 on success, and on error return any other number.
 */
static int assembler_first_pass(FILE * am_file, struct CU * cu) {
    char line_buff[MAX_LINE_LEN + 1] = {0};
    struct sst syntax_line;
    struct asm_label * find;
    struct asm_label new_label;
    int line_counter = 1;
    int i;
    int IC = PROG_START_ADDR;
    int DC = 0;
    while(fgets(line_buff,MAX_LINE_LEN,am_file)) {
        syntax_line = sst_get_stt_from_line(&line_buff[0]);
        switch (syntax_line.syntax_option)
        {
        case sst_syntax_error:

            break;
        case sst_directive: case sst_instruction:
            if(syntax_line.label[0] !='\0') { /* check if there is a label definition..*/
                /* relative only for string data  or any instruction*/
                if(syntax_line.syntax_option == sst_instruction || syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag == sst_tag_dir_data  
                                            || syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag == sst_tag_dir_string) {
                    find = assembler_search_symbol(cu->symbol_table,syntax_line.label);
                    if(find != NULL) {
                        if(syntax_line.syntax_option == sst_instruction) {
                            switch (find->asm_label_type)
                            {
                            case asm_label_type_entry:
                                find->address        = IC;
                                find->asm_label_type = asm_label_type_code_entry;
                                break;
                            default: 
                                /* all other cases are redefinition !*/
                                /* error */
                                break;
                            }
                        }else {
                            switch (find->asm_label_type)
                            {
                            case asm_label_type_entry:
                                find->address           = DC;
                                find->asm_label_type    = asm_label_type_data_entry;
                                break;
                            
                            default:
                                /* all other cases are redefinition !*/
                                /* error */
                                break;
                            }
                        }
                    }else {
                        strcpy(new_label.label_str,syntax_line.label);
                        new_label.line_of_definition = line_counter;
                        if(syntax_line.syntax_option == sst_instruction) {
                            new_label.asm_label_type = asm_label_type_code;
                            new_label.address        = IC;
                        }
                        else {
                            new_label.asm_label_type = asm_label_type_data;
                            new_label.address        = DC;
                        }
                        dynarray_push(cu->symbol_table,new_label);

                    }
                }
            }
            if(syntax_line.syntax_option == sst_instruction) {
                IC++;
                if(is_inst_tag_set_A(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag)) {
                    if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] == sst_tag_op_register &&
                                syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] == sst_tag_op_register) {
                                    IC++;
                                }else {
                                    IC+=2;
                                }
                    
                }else if(is_inst_tag_set_B(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag)) {
                    if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag == sst_tag_op_label_with_operands) {
                        if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[0] == sst_tag_op_register &&
                            syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[1] == sst_tag_op_register ) {
                        IC+=2; 
                        } else {
                            IC+3;
                        }
                    }else {
                        IC++;
                    }
                }
            } else {
                switch (syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag)
                {
                case sst_tag_dir_string:
                    DC+= strlen(syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.string) + 1;
                    break;
                case sst_tag_dir_data:
                    DC+=syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.actual_data_size;
                    break;
                case sst_tag_dir_extern: case sst_tag_dir_entry:
                    find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.label);
                    if (find) {
                        switch (find->asm_label_type)
                        {
                        case asm_label_type_entry:
                            switch (syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag)
                            {
                            case asm_label_type_extern:
                                /* error */
                                break;
                            case asm_label_type_entry: case asm_label_type_data_entry: case asm_label_type_code_entry:
                                /* warning*/
                            case asm_label_type_data:
                                find->asm_label_type = asm_label_type_data_entry;
                                break;
                            case asm_label_type_code:
                                find->asm_label_type = asm_label_type_code_entry;
                                break;
                            }
                            break;
                        }
                    }else {
                        strcpy(new_label.label_str,syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.label);
                        new_label.asm_label_type = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag;
                        new_label.line_of_definition = line_counter;
                        dynarray_push(cu->symbol_table,new_label);
                    }
                }
            }
            break;
        }
        for(i=0;i<dynarray_length(cu->symbol_table);i++) {
            if(cu->symbol_table[i].asm_label_type == asm_label_type_entry) {
                /* error declaring  entry without  definition */
            }else if(cu->symbol_table[i].asm_label_type == asm_label_type_data || cu->symbol_table[i].asm_label_type == asm_label_type_data_entry ) {
                cu->symbol_table[i].address +=IC;
            }
        }
        line_counter++;
    }
}







int assembler_main(int number_of_files, char **file_names) {
    
}
