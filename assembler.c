#include "assembler.h"
#include "preprocessor.h"
#include "sst.h"
#include <stdio.h>
#include <string.h>
#include "dynarray.h"
#include "bmc.h"
#include <stdarg.h>
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
static const char *asm_label_type_strings[6] = {
    "external symbol",
    "entry symbol",
    "code symbol",
    "data symbol",
    "entry data symbol",
    "entry code symbol"
};

struct extern_use {
	char extern_name[MAX_LABEL_LENGTH +1];
	int * addresses;
};

struct CU {
    struct asm_label * symbol_table;
    int *   bmc_code_list;#include "assembler.h"
#include "../sst/sst.h"
#include <stdio.h>
#include <string.h>
#include "../dynarray/dynarray.h"
#include "../bmc_to_object/bmc_to_obj.h"
#include "../preprocessor/preprocessor.h"
#include <stdarg.h>
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
static const char *asm_label_type_strings[6] = {
    "external symbol",
    "entry symbol",
    "code symbol",
    "data symbol",
    "entry data symbol",
    "entry code symbol"
};

struct extern_use {
	char extern_name[MAX_LABEL_LENGTH +1];
	int * addresses;
};

struct CU {
    struct asm_label * symbol_table;
    int *   bmc_code_list;
    int *   bmc_data_list;
    int *   ref_to_entry_symbols;
    struct extern_use *  extern_list;
};
#define assembler_error_color           "\u001b[31m"
#define assembler_warning_color         "\u001b[33m"
#define assembler_error_color_reset     "\u001b[0m"
static void assembler_print_error(const char * file_name,int line_number,const char *arg_format,...) {
    va_list arg;
    va_start(arg,arg_format);
    printf("%s:%d \x1b[31m  error: \x1b[0m", file_name, line_number);
    vprintf(arg_format,arg);
    va_end(arg);
}
static void assembler_print_warning(const char * file_name,int line_number,const char *arg_format,...) {
    va_list arg;
    va_start(arg,arg_format);
    printf("%s:%d \x1b[33m  error:  \x1b[0m", file_name, line_number);
    vprintf(arg_format,arg);
    va_end(arg);
}



struct extern_use * assembler_extern_use_search(struct extern_use * extern_use_arr, const void * label) {
    size_t i;
    for(i=0;i<dynarray_length(extern_use_arr);i++) {
        if(strcmp(extern_use_arr[i].extern_name,label) == 0) {
            return &extern_use_arr[i];
        }
    }
    return NULL;
}


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
static int assembler_first_pass(const char *am_file_name,FILE * am_file, struct CU * cu) {
    char line_buff[MAX_LINE_LEN + 1] = {0};
    struct sst syntax_line;
    struct asm_label * find;
    struct asm_label new_label;
    int first_pass_err = 0;
    int line_counter = 1;
    int i;
    int IC = PROG_START_ADDR;
    int DC = 0;
    while(fgets(line_buff,MAX_LINE_LEN,am_file)) {
        syntax_line = sst_get_stt_from_line(&line_buff[0]);
        switch (syntax_line.syntax_option)
        {
        case sst_white_space: case sst_comment:
            break;
        case sst_syntax_error:
        /* syntax error*/
            assembler_print_error(am_file_name,line_counter," syntax : %s\n",syntax_line.syntax_error_buffer);
            break;
        case sst_directive: case sst_instruction:
	    if(syntax_line.label[0] !='\0')
		printf("label is: %s\n", syntax_line.label);
	    else 
		printf("We're good.\n");
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
                                assembler_print_error(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[find->asm_label_type]);
                                 first_pass_err=1;
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
                                assembler_print_error(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[find->asm_label_type]);
                                first_pass_err = 1;
                                break;
                            }
                        }
                    }else {
                        strcpy(new_label.label_str,syntax_line.label);
                        new_label.line_of_definition        = line_counter;
                        if(syntax_line.syntax_option        == sst_instruction) {
                            new_label.asm_label_type        = asm_label_type_code;
                            new_label.address               = IC;
                            new_label.line_of_definition    = line_counter;
                        }
                        else {
                            new_label.asm_label_type        = asm_label_type_data;
                            new_label.address               = DC;
                            new_label.line_of_definition    = line_counter;
                        }
                        dynarray_push(cu->symbol_table,new_label);
                    }
                }
            }
            if(syntax_line.syntax_option == sst_instruction) {
                IC++;
                if(is_inst_tag_set_A(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag)) {
                    if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] == sst_tag_op_register &&
                                syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1] == sst_tag_op_register) {
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
                            IC+=3;
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
                    find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label);
                    if (find) {
                        switch (find->asm_label_type)
                        {
                            case asm_label_type_extern:
                                if(syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag ==sst_tag_dir_entry ) {
                                    assembler_print_error(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[asm_label_type_entry]);
                                    first_pass_err =1;
                                    /* error */
                                }else {
                                    /* warning*/
                                    assembler_print_warning(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[asm_label_type_extern]);
                                }
                                break;
                            case asm_label_type_entry: case asm_label_type_data_entry: case asm_label_type_code_entry: 
                                if(syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag ==sst_tag_dir_entry ) {
                                /* warning*/
                                assembler_print_warning(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[asm_label_type_entry]);
                                }else {
                                    /* error */
                                    assembler_print_error(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[asm_label_type_extern]);
                                    first_pass_err =1;
                                }
                                break;
                            case asm_label_type_data:
                                find->asm_label_type = asm_label_type_data_entry;
                                break;
                            case asm_label_type_code:
                                find->asm_label_type = asm_label_type_code_entry;
                                break;
                        }
                    }else {
                        strcpy(new_label.label_str,syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label);
                        new_label.asm_label_type = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag;
                        new_label.line_of_definition = line_counter;
                        dynarray_push(cu->symbol_table,new_label);
                    }
                    break;
                }
                
            }
            
        }
        for(i=0;i<dynarray_length(cu->symbol_table);i++) {
            if(cu->symbol_table[i].asm_label_type == asm_label_type_entry) {
                /* error declaring  entry without  definition */
                assembler_print_error(am_file_name,line_counter,"symbol '%s' was declared in line %d as '%s' but was never defined in this file.\n",cu->symbol_table[i].label_str,cu->symbol_table[i].address,asm_label_type_strings[asm_label_type_entry]);
            }else if(cu->symbol_table[i].asm_label_type == asm_label_type_data || cu->symbol_table[i].asm_label_type == asm_label_type_data_entry ) {
                cu->symbol_table[i].address +=IC;
            }else if(cu->symbol_table[i].asm_label_type == asm_label_type_data_entry || cu->symbol_table[i].asm_label_type == asm_label_type_code_entry) {
                dynarray_push(cu->ref_to_entry_symbols,i);
            }
        }
        line_counter++;
    }
    return first_pass_err;
}

static int assembler_second_pass(const char *am_file_name,FILE * am_file, struct CU * cu) {
    char line_buff[MAX_LINE_LEN + 1] = {0};
    struct sst syntax_line;
    struct asm_label * sym_find;
    struct extern_use * extern_find;
    struct extern_use new_extern_use = {0};
    int mem_word = 0;
    int i;
    int line_counter = 0;
    int second_pass_error = 0;
    while(fgets(line_buff,MAX_LINE_LEN,am_file)) {
        syntax_line = sst_get_stt_from_line(&line_buff[0]);
        switch (syntax_line.syntax_option)
        {
        case sst_instruction: {
            /* first machine word*/
            mem_word  = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag << 8; /* op code*/
            if(is_inst_tag_set_A(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag)) {
                mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] << 4;/* src*/
                mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1] << 2;/* dest*/
                dynarray_push(cu->bmc_code_list,mem_word);
                /* next machine words are arguments..*/
                /* if both operands are registers*/
                if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] == sst_tag_op_register &&
                                syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1] == sst_tag_op_register) {
                           mem_word = (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[0].reg  << 6) | /* src*/
                                        (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[1].reg   << 2); /* dest*/
                                        dynarray_push(cu->bmc_code_list,mem_word);
                }else {
                /* if the operands are in any combination of register, label, and constant number then..*/
                    for(i=0;i<2;i++) {
                        switch (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[i])
                        {
                            /* #*/
                        case sst_tag_op_register: /* register */
                                /* if it's a register depends if it's source or dest put it in the right bits according to i src(8 - (0 * 6)) dest(8 - (1 * 6)) */
                                mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].reg << (8 - (i *6));
                            break;
                        case sst_tag_op_label: /* label */
                            sym_find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                            if(sym_find != NULL) { /* if we found the symbol*/
                            if(sym_find->asm_label_type == asm_label_type_extern) {
                                mem_word = 1; /* A.R.E = 01*/
                                extern_find = assembler_extern_use_search(cu->extern_list,sym_find->label_str);
                                if(extern_find) {
                                    i = dynarray_length(cu->bmc_code_list);
                                    dynarray_push(extern_find->addresses,i);
                                }else {
                                    strcpy(new_extern_use.extern_name,sym_find->label_str);
                                    new_extern_use.addresses = dynarray_create(int);
                                    new_extern_use.addresses[0] = dynarray_length(cu->bmc_code_list);
                                }
                            }else {
                                mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                            }
                            }else {
                                /* error .... undefined symbol*/
                                assembler_print_error(am_file_name,line_counter,"undefined symbol '%s'.\n",syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                                second_pass_error=1;
                                break;
                            }
                            break;
                        case sst_tag_op_c_number: /* constant number*/
                            mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].c_number << 2;
                            break;
                        default:
                            break;
                        }
                        dynarray_push(cu->bmc_code_list,mem_word);
                        /* #*/
                    }
            }
            }else if is_inst_tag_set_B(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag) {
                /* #*/
                if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag == sst_tag_op_label_with_operands) { /*  BNE,JSR,JMP case L(p1,p2)*/
                    mem_word = 2 << 2; /* 10 in dest*/
                    mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[0] << 12; /* param1*/ 
                    mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[1] << 10; /* param2*/
                    dynarray_push(cu->bmc_code_list,mem_word);
                    /* must  be BNE ,JSR or JMP*/
                    sym_find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.label);
                    if(sym_find != NULL) { /* if we found the symbol*/
                        if(sym_find->asm_label_type == asm_label_type_extern) {
                            mem_word = 1; /* A.R.E = 01*/
                            extern_find = assembler_extern_use_search(cu->extern_list,sym_find->label_str);
                            if(extern_find) {
                                i = dynarray_length(cu->bmc_code_list);
                                dynarray_push(extern_find->addresses,i);
                            }else {
                                strcpy(new_extern_use.extern_name,sym_find->label_str);
                                new_extern_use.addresses = dynarray_create(int);
                                new_extern_use.addresses[0] = dynarray_length(cu->bmc_code_list);
                            }
                        }else {
                            mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                        }
                        dynarray_push(cu->bmc_code_list,mem_word);
                    }else {
                            /* error .... undefined symbol*/
                            assembler_print_error(am_file_name,line_counter,"undefined symbol '%s'.\n",syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                            second_pass_error=1;
                    }
                    for(i=0;i<2;i++) {
                        switch (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[i])
                        {
                        case sst_tag_op_register: /* register */
                                /* if it's a register depends if it's source or dest put it in the right bits according to i src(8 - (0 * 6)) dest(8 - (1 * 6)) */
                                mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].reg << (8 - (i *6));
                            break;
                        case sst_tag_op_label: /* label */
                            sym_find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                            if(sym_find != NULL) { /* if we found the symbol*/
                            if(sym_find->asm_label_type == asm_label_type_extern) {
                                mem_word = 1; /* A.R.E = 01*/
                                extern_find = assembler_extern_use_search(cu->extern_list,sym_find->label_str);
                                if(extern_find) {
                                    i = dynarray_length(cu->bmc_code_list);
                                    dynarray_push(extern_find->addresses,i);
                                }else {
                                    strcpy(new_extern_use.extern_name,sym_find->label_str);
                                    new_extern_use.addresses = dynarray_create(int);
                                    new_extern_use.addresses[0] = dynarray_length(cu->bmc_code_list);
                                }
                            }else {
                                mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                            }
                            }else {
                                /* error .... undefined symbol*/
                                assembler_print_error(am_file_name,line_counter,"undefined symbol '%s'.\n",syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                                second_pass_error=1;
                            }
                            break;
                        case sst_tag_op_c_number: /* constant number*/
                            mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].c_number << 2;
                            break;
                        default:
                            break;
                        }
                        dynarray_push(cu->bmc_code_list,mem_word);
                }
                /* #*/
                }else { /* all other cases in B group including jmp , jsr , bne with 1 arg */
                    switch (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag)
                        {
                        case sst_tag_op_register: /* register */
                                /* if it's a register depends if it's source or dest put it in the right bits according to i src(8 - (0 * 6)) dest(8 - (1 * 6)) */
                                mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.reg << (8 - (i *6));
                            break;
                        case sst_tag_op_label: /* label */
                            sym_find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label);
                            if(sym_find != NULL) { /* if we found the symbol*/
                            if(sym_find->asm_label_type == asm_label_type_extern) {
                                mem_word = 1; /* A.R.E = 01*/
                                extern_find = assembler_extern_use_search(cu->extern_list,sym_find->label_str);
                                if(extern_find) {
                                    i = dynarray_length(cu->bmc_code_list);
                                    dynarray_push(extern_find->addresses,i);
                                }else {
                                    strcpy(new_extern_use.extern_name,sym_find->label_str);
                                    new_extern_use.addresses = dynarray_create(int);
                                    new_extern_use.addresses[0] = dynarray_length(cu->bmc_code_list);
                                }
                            }else {
                                mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                            }
                            }else {
                                /* error .... undefined symbol*/
                                assembler_print_error(am_file_name,line_counter,"undefined symbol '%s'.\n",syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                                second_pass_error=1;
                            }
                            break;
                        case sst_tag_op_c_number: /* constant number*/
                            mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.c_number << 2;
                            break;
                        default:
                            break;
                        }
                }
            }
            else { /* group C , no args*/
                dynarray_push(cu->bmc_code_list,mem_word);
            }
            break;
        }
        case sst_directive: {
                if(syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag == sst_tag_dir_string) { /* if it's string directive*/
                    for(i=0;syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.string[i] !='\0';i++) {
                        mem_word = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.string[i];
                        dynarray_push(cu->bmc_data_list,mem_word);
                    }
                    mem_word = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.string[i];
                    dynarray_push(cu->bmc_data_list,mem_word);
                }else if(syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag == sst_tag_dir_data) {
                    for(i = 0; i<syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.actual_data_size;i++) {
                        dynarray_push(cu->bmc_data_list,mem_word);
                    }
                }
           break; 
        }
            
        default:
            break;
        }
        line_counter++;
    }
    return second_pass_error;
}



static void assembler_print_ent_file(FILE * ent,const int *ref_to_entry_symbols, const struct asm_label * sym_table) {
    int i;
    for(i=0;i<dynarray_length((void*)ref_to_entry_symbols);i++) {
        fprintf(ent,"%s\t\t%d\n",sym_table[ref_to_entry_symbols[i]].label_str,sym_table[ref_to_entry_symbols[i]].address);
    }
}

static void assembler_print_ext_file(FILE *ext,const struct extern_use * eu) {
    int i,j;
    for(i=0;i<dynarray_length((void*)eu);i++) {
        for(j=0;j<dynarray_length(eu->addresses);j++) {
            fprintf(ext,"%s\t\t%d",eu->extern_name,eu->addresses[j]);
        }
    }
}

int assembler_main(int number_of_files, char **file_names) {
    struct CU current_cu;
    int i;
    int j;
    char *am_file_name, *ob_file_name, * ent_file_name, *ext_file_name;
    FILE *am_file, *ob, *ent, *ext;
    for(i=0;i<number_of_files;i++) {
        if(preprocessor(file_names[i])) {
                am_file_name = malloc(strlen(file_names[i]) +3 +1);
                /*strcat(strcpy(am_file_name,file_names[i]),".am");*/
		strcpy(am_file_name,file_names[i]);
		am_file = fopen(am_file_name,"r");
                if(am_file != NULL) {
                    current_cu.bmc_code_list        = dynarray_create(int);
                    current_cu.bmc_data_list        = dynarray_create(int);
                    current_cu.symbol_table         = dynarray_create(struct asm_label);
                    current_cu.extern_list          = dynarray_create(struct extern_use);
                    current_cu.ref_to_entry_symbols = dynarray_create(int);
                    if(assembler_first_pass(am_file_name,am_file,&current_cu) == 0) {
                        rewind(am_file);
                        if(assembler_second_pass(am_file_name,am_file,&current_cu) == 0) {
                            free(am_file_name);
                            fclose(am_file);
                            if(dynarray_length(current_cu.bmc_code_list) > 0 || dynarray_length(current_cu.bmc_data_list)) {
                                ob_file_name = malloc(strlen(file_names[i]) +3 +1);
                                strcat(strcpy(ob_file_name,file_names[i]),".am");
                                ob = fopen(ob_file_name,"w");
                                if(ob){
                                    print_in_binary_format(current_cu.bmc_code_list,current_cu.bmc_data_list,ob);
                                    fclose(ob);
                                }
				else
					printf("Unable to open ob file w mode.\n");
                                free(ob_file_name);
                            }
                            if(dynarray_length(current_cu.extern_list) > 0) {
                                ext_file_name = malloc(strlen(file_names[i]) +4 +1);
                                strcat(strcpy(ent_file_name,file_names[i]),".ext");
                                ext = fopen(ext_file_name,"w");
                                if(ext) {
                                    assembler_print_ext_file(ext,current_cu.extern_list);
                                    fclose(ext);
                                }
                                free(ext_file_name);
                            }
                            if(dynarray_length(current_cu.ref_to_entry_symbols) > 0) {
                                ent_file_name = malloc(strlen(file_names[i]) +4 +1);
                                strcat(strcpy(ent_file_name,file_names[i]),".ent");
                                ent = fopen(ent_file_name,"w");
                                if(ent) {
                                    assembler_print_ent_file(ent,current_cu.ref_to_entry_symbols,current_cu.symbol_table);
                                    fclose(ent);
                                }
                                free(ent_file_name);
                            }                            
                        }
                    }
                    dynarray_destroy(current_cu.bmc_code_list);
                    dynarray_destroy(current_cu.bmc_data_list);
                    dynarray_destroy(current_cu.symbol_table);
                    for(j=0;j<dynarray_length(current_cu.extern_list);j++) {
                        dynarray_destroy(current_cu.extern_list->addresses);
                    }
                    dynarray_destroy(current_cu.extern_list);
                    dynarray_destroy(current_cu.ref_to_entry_symbols);
                }
		else
			printf("Unable to open .am file\n");
        }
    }
    return 0;
}

    int *   bmc_data_list;
    int *   ref_to_entry_symbols;
    struct extern_use *  extern_list;
};
#define assembler_error_color           "\u001b[31m"
#define assembler_warning_color         "\u001b[33m"
#define assembler_error_color_reset     "\u001b[0m"
static void assembler_print_error(const char * file_name,int line_number,const char *arg_format,...) {
    va_list arg;
    va_start(arg,arg_format);
    printf("%s:%d \x1b[31m  error: \x1b[0m", file_name, line_number);
    vprintf(arg_format,arg);
    va_end(arg);
}
static void assembler_print_warning(const char * file_name,int line_number,const char *arg_format,...) {
    va_list arg;
    va_start(arg,arg_format);
    printf("%s:%d \x1b[33m  error:  \x1b[0m", file_name, line_number);
    vprintf(arg_format,arg);
    va_end(arg);
}



struct extern_use * assembler_extern_use_search(struct extern_use * extern_use_arr, const void * label) {
    size_t i;
    for(i=0;i<dynarray_length(extern_use_arr);i++) {
        if(strcmp(extern_use_arr[i].extern_name,label) == 0) {
            return &extern_use_arr[i];
        }
    }
    return NULL;
}


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
static int assembler_first_pass(const char *am_file_name,FILE * am_file, struct CU * cu) {
    char line_buff[MAX_LINE_LEN + 1] = {0};
    struct sst syntax_line;
    struct asm_label * find;
    struct asm_label new_label;
    int first_pass_err = 0;
    int line_counter = 1;
    int i;
    int IC = PROG_START_ADDR;
    int DC = 0;
    while(fgets(line_buff,MAX_LINE_LEN,am_file)) {
        syntax_line = sst_get_stt_from_line(&line_buff[0]);
        switch (syntax_line.syntax_option)
        {
        case sst_white_space: case sst_comment:
            break;
        case sst_syntax_error:
        /* syntax error*/
            assembler_print_error(am_file_name,line_counter," syntax : %s\n",syntax_line.syntax_error_buffer);
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
                                assembler_print_error(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[find->asm_label_type]);
                                 first_pass_err=1;
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
                                assembler_print_error(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[find->asm_label_type]);
                                first_pass_err = 1;
                                break;
                            }
                        }
                    }else {
                        strcpy(new_label.label_str,syntax_line.label);
                        new_label.line_of_definition        = line_counter;
                        if(syntax_line.syntax_option        == sst_instruction) {
                            new_label.asm_label_type        = asm_label_type_code;
                            new_label.address               = IC;
                            new_label.line_of_definition    = line_counter;
                        }
                        else {
                            new_label.asm_label_type        = asm_label_type_data;
                            new_label.address               = DC;
                            new_label.line_of_definition    = line_counter;
                        }
                        dynarray_push(cu->symbol_table,new_label);
                    }
                }
            }
            if(syntax_line.syntax_option == sst_instruction) {
                IC++;
                if(is_inst_tag_set_A(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag)) {
                    if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] == sst_tag_op_register &&
                                syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1] == sst_tag_op_register) {
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
                            IC+=3;
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
                    find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label);
                    if (find) {
                        switch (find->asm_label_type)
                        {
                            case asm_label_type_extern:
                                if(syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag ==sst_tag_dir_entry ) {
                                    assembler_print_error(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[asm_label_type_entry]);
                                    first_pass_err =1;
                                    /* error */
                                }else {
                                    /* warning*/
                                    assembler_print_warning(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[asm_label_type_extern]);
                                }
                                break;
                            case asm_label_type_entry: case asm_label_type_data_entry: case asm_label_type_code_entry: 
                                if(syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag ==sst_tag_dir_entry ) {
                                /* warning*/
                                assembler_print_warning(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[asm_label_type_entry]);
                                }else {
                                    /* error */
                                    assembler_print_error(am_file_name,line_counter,"symbol '%s' was defined in line %d as '%s' and now is being redefined as '%s'.\n",find->label_str,find->address,asm_label_type_strings[find->asm_label_type],asm_label_type_strings[asm_label_type_extern]);
                                    first_pass_err =1;
                                }
                                break;
                            case asm_label_type_data:
                                find->asm_label_type = asm_label_type_data_entry;
                                break;
                            case asm_label_type_code:
                                find->asm_label_type = asm_label_type_code_entry;
                                break;
                        }
                    }else {
                        strcpy(new_label.label_str,syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label);
                        new_label.asm_label_type = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag;
                        new_label.line_of_definition = line_counter;
                        dynarray_push(cu->symbol_table,new_label);
                    }
                    break;
                }
                
            }
            
        }
        for(i=0;i<dynarray_length(cu->symbol_table);i++) {
            if(cu->symbol_table[i].asm_label_type == asm_label_type_entry) {
                /* error declaring  entry without  definition */
                assembler_print_error(am_file_name,line_counter,"symbol '%s' was declared in line %d as '%s' but was never defined in this file.\n",cu->symbol_table[i].label_str,cu->symbol_table[i].address,asm_label_type_strings[asm_label_type_entry]);
            }else if(cu->symbol_table[i].asm_label_type == asm_label_type_data || cu->symbol_table[i].asm_label_type == asm_label_type_data_entry ) {
                cu->symbol_table[i].address +=IC;
            }else if(cu->symbol_table[i].asm_label_type == asm_label_type_data_entry || cu->symbol_table[i].asm_label_type == asm_label_type_code_entry) {
                dynarray_push(cu->ref_to_entry_symbols,i);
            }
        }
        line_counter++;
    }
    return first_pass_err;
}

static int assembler_second_pass(const char *am_file_name,FILE * am_file, struct CU * cu) {
    char line_buff[MAX_LINE_LEN + 1] = {0};
    struct sst syntax_line;
    struct asm_label * sym_find;
    struct extern_use * extern_find;
    struct extern_use new_extern_use = {0};
    int mem_word = 0;
    int i;
    int line_counter = 0;
    int second_pass_error = 0;
    while(fgets(line_buff,MAX_LINE_LEN,am_file)) {
        syntax_line = sst_get_stt_from_line(&line_buff[0]);
        switch (syntax_line.syntax_option)
        {
        case sst_instruction: {
            /* first machine word*/
            mem_word  = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag << 8; /* op code*/
            if(is_inst_tag_set_A(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag)) {
                mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] << 4;/* src*/
                mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1] << 2;/* dest*/
                dynarray_push(cu->bmc_code_list,mem_word);
                /* next machine words are arguments..*/
                /* if both operands are registers*/
                if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] == sst_tag_op_register &&
                                syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1] == sst_tag_op_register) {
                           mem_word = (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[0].reg  << 6) | /* src*/
                                        (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[1].reg   << 2); /* dest*/
                                        dynarray_push(cu->bmc_code_list,mem_word);
                }else {
                /* if the operands are in any combination of register, label, and constant number then..*/
                    for(i=0;i<2;i++) {
                        switch (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[i])
                        {
                            /* #*/
                        case sst_tag_op_register: /* register */
                                /* if it's a register depends if it's source or dest put it in the right bits according to i src(8 - (0 * 6)) dest(8 - (1 * 6)) */
                                mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].reg << (8 - (i *6));
                            break;
                        case sst_tag_op_label: /* label */
                            sym_find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                            if(sym_find != NULL) { /* if we found the symbol*/
                            if(sym_find->asm_label_type == asm_label_type_extern) {
                                mem_word = 1; /* A.R.E = 01*/
                                extern_find = assembler_extern_use_search(cu->extern_list,sym_find->label_str);
                                if(extern_find) {
                                    i = dynarray_length(cu->bmc_code_list);
                                    dynarray_push(extern_find->addresses,i);
                                }else {
                                    strcpy(new_extern_use.extern_name,sym_find->label_str);
                                    new_extern_use.addresses = dynarray_create(int);
                                    new_extern_use.addresses[0] = dynarray_length(cu->bmc_code_list);
                                }
                            }else {
                                mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                            }
                            }else {
                                /* error .... undefined symbol*/
                                assembler_print_error(am_file_name,line_counter,"undefined symbol '%s'.\n",syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                                second_pass_error=1;
                                break;
                            }
                            break;
                        case sst_tag_op_c_number: /* constant number*/
                            mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].c_number << 2;
                            break;
                        default:
                            break;
                        }
                        dynarray_push(cu->bmc_code_list,mem_word);
                        /* #*/
                    }
            }
            }else if is_inst_tag_set_B(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag) {
                /* #*/
                if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag == sst_tag_op_label_with_operands) { /*  BNE,JSR,JMP case L(p1,p2)*/
                    mem_word = 2 << 2; /* 10 in dest*/
                    mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[0] << 12; /* param1*/ 
                    mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[1] << 10; /* param2*/
                    dynarray_push(cu->bmc_code_list,mem_word);
                    /* must  be BNE ,JSR or JMP*/
                    sym_find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.label);
                    if(sym_find != NULL) { /* if we found the symbol*/
                        if(sym_find->asm_label_type == asm_label_type_extern) {
                            mem_word = 1; /* A.R.E = 01*/
                            extern_find = assembler_extern_use_search(cu->extern_list,sym_find->label_str);
                            if(extern_find) {
                                i = dynarray_length(cu->bmc_code_list);
                                dynarray_push(extern_find->addresses,i);
                            }else {
                                strcpy(new_extern_use.extern_name,sym_find->label_str);
                                new_extern_use.addresses = dynarray_create(int);
                                new_extern_use.addresses[0] = dynarray_length(cu->bmc_code_list);
                            }
                        }else {
                            mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                        }
                        dynarray_push(cu->bmc_code_list,mem_word);
                    }else {
                            /* error .... undefined symbol*/
                            assembler_print_error(am_file_name,line_counter,"undefined symbol '%s'.\n",syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                            second_pass_error=1;
                    }
                    for(i=0;i<2;i++) {
                        switch (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[i])
                        {
                        case sst_tag_op_register: /* register */
                                /* if it's a register depends if it's source or dest put it in the right bits according to i src(8 - (0 * 6)) dest(8 - (1 * 6)) */
                                mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].reg << (8 - (i *6));
                            break;
                        case sst_tag_op_label: /* label */
                            sym_find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                            if(sym_find != NULL) { /* if we found the symbol*/
                            if(sym_find->asm_label_type == asm_label_type_extern) {
                                mem_word = 1; /* A.R.E = 01*/
                                extern_find = assembler_extern_use_search(cu->extern_list,sym_find->label_str);
                                if(extern_find) {
                                    i = dynarray_length(cu->bmc_code_list);
                                    dynarray_push(extern_find->addresses,i);
                                }else {
                                    strcpy(new_extern_use.extern_name,sym_find->label_str);
                                    new_extern_use.addresses = dynarray_create(int);
                                    new_extern_use.addresses[0] = dynarray_length(cu->bmc_code_list);
                                }
                            }else {
                                mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                            }
                            }else {
                                /* error .... undefined symbol*/
                                assembler_print_error(am_file_name,line_counter,"undefined symbol '%s'.\n",syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                                second_pass_error=1;
                            }
                            break;
                        case sst_tag_op_c_number: /* constant number*/
                            mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].c_number << 2;
                            break;
                        default:
                            break;
                        }
                        dynarray_push(cu->bmc_code_list,mem_word);
                }
                /* #*/
                }else { /* all other cases in B group including jmp , jsr , bne with 1 arg */
                    switch (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag)
                        {
                        case sst_tag_op_register: /* register */
                                /* if it's a register depends if it's source or dest put it in the right bits according to i src(8 - (0 * 6)) dest(8 - (1 * 6)) */
                                mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.reg << (8 - (i *6));
                            break;
                        case sst_tag_op_label: /* label */
                            sym_find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label);
                            if(sym_find != NULL) { /* if we found the symbol*/
                            if(sym_find->asm_label_type == asm_label_type_extern) {
                                mem_word = 1; /* A.R.E = 01*/
                                extern_find = assembler_extern_use_search(cu->extern_list,sym_find->label_str);
                                if(extern_find) {
                                    i = dynarray_length(cu->bmc_code_list);
                                    dynarray_push(extern_find->addresses,i);
                                }else {
                                    strcpy(new_extern_use.extern_name,sym_find->label_str);
                                    new_extern_use.addresses = dynarray_create(int);
                                    new_extern_use.addresses[0] = dynarray_length(cu->bmc_code_list);
                                }
                            }else {
                                mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                            }
                            }else {
                                /* error .... undefined symbol*/
                                assembler_print_error(am_file_name,line_counter,"undefined symbol '%s'.\n",syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[i].label);
                                second_pass_error=1;
                            }
                            break;
                        case sst_tag_op_c_number: /* constant number*/
                            mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.c_number << 2;
                            break;
                        default:
                            break;
                        }
                }
            }
            else { /* group C , no args*/
                dynarray_push(cu->bmc_code_list,mem_word);
            }
            break;
        }
        case sst_directive: {
                if(syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag == sst_tag_dir_string) { /* if it's string directive*/
                    for(i=0;syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.string[i] !='\0';i++) {
                        mem_word = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.string[i];
                        dynarray_push(cu->bmc_data_list,mem_word);
                    }
                    mem_word = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.string[i];
                    dynarray_push(cu->bmc_data_list,mem_word);
                }else if(syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag == sst_tag_dir_data) {
                    for(i = 0; i<syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.actual_data_size;i++) {
                        mem_word = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[i];
                        dynarray_push(cu->bmc_data_list,mem_word);
                    }
                }
           break; 
        }
            
        default:
            break;
        }
        line_counter++;
    }
    return second_pass_error;
}



static void assembler_print_ent_file(FILE * ent,const int *ref_to_entry_symbols, const struct asm_label * sym_table) {
    int i;
    for(i=0;i<dynarray_length((void*)ref_to_entry_symbols);i++) {
        fprintf(ent,"%s\t\t%d\n",sym_table[ref_to_entry_symbols[i]].label_str,sym_table[ref_to_entry_symbols[i]].address);
    }
}

static void assembler_print_ext_file(FILE *ext,const struct extern_use * eu) {
    int i,j;
    for(i=0;i<dynarray_length((void*)eu);i++) {
        for(j=0;j<dynarray_length(eu->addresses);j++) {
            fprintf(ext,"%s\t\t%d",eu->extern_name,eu->addresses[j]);
        }
    }
}
int assembler_main(int number_of_files, char **file_names) {
    struct CU current_cu;
    int i;
    int j;
    char *am_file_name, *ob_file_name, * ent_file_name, *ext_file_name;
    FILE *am_file, *ob, *ent, *ext;
    for(i=0;i<number_of_files;i++) {
        if(preprocessor(file_names[i])) {
                am_file_name = malloc(strlen(file_names[i]) +3 +1);
                strcat(strcpy(am_file_name,file_names[i]),".am");
		am_file = fopen(am_file_name,"r");
                if(am_file != NULL) {
                    current_cu.bmc_code_list        = dynarray_create(int);
                    current_cu.bmc_data_list        = dynarray_create(int);
                    current_cu.symbol_table         = dynarray_create(struct asm_label);
                    current_cu.extern_list          = dynarray_create(struct extern_use);
                    current_cu.ref_to_entry_symbols = dynarray_create(int);
                    if(assembler_first_pass(am_file_name,am_file,&current_cu) == 0) {
                        rewind(am_file);
                        if(assembler_second_pass(am_file_name,am_file,&current_cu) == 0) {
                            free(am_file_name);
                            fclose(am_file);
                            if(dynarray_length(current_cu.bmc_code_list) > 0 || dynarray_length(current_cu.bmc_data_list)) {
                                ob_file_name = malloc(strlen(file_names[i]) +3 +1);
                                strcat(strcpy(ob_file_name,file_names[i]),".am");
                                ob = fopen(ob_file_name,"w");
                                if(ob){
                                    print_in_binary_format(current_cu.bmc_code_list,current_cu.bmc_data_list,ob);
                                    fclose(ob);
                                }
                                free(ob_file_name);
                            }
                            if(dynarray_length(current_cu.extern_list) > 0) {
                                ext_file_name = malloc(strlen(file_names[i]) +4 +1);
                                strcat(strcpy(ent_file_name,file_names[i]),".ext");
                                ext = fopen(ext_file_name,"w");
                                if(ext) {
                                    assembler_print_ext_file(ext,current_cu.extern_list);
                                    fclose(ext);
                                }
                                free(ext_file_name);
                            }
                            if(dynarray_length(current_cu.ref_to_entry_symbols) > 0) {
                                ent_file_name = malloc(strlen(file_names[i]) +4 +1);
                                strcat(strcpy(ent_file_name,file_names[i]),".ent");
                                ent = fopen(ent_file_name,"w");
                                if(ent) {
                                    assembler_print_ent_file(ent,current_cu.ref_to_entry_symbols,current_cu.symbol_table);
                                    fclose(ent);
                                }
                                free(ent_file_name);
                            }                            
                        }
                    }
                    dynarray_destroy(current_cu.bmc_code_list);
                    dynarray_destroy(current_cu.bmc_data_list);
                    dynarray_destroy(current_cu.symbol_table);
                    for(j=0;j<dynarray_length(current_cu.extern_list);j++) {
                        dynarray_destroy(current_cu.extern_list->addresses);
                    }
                    dynarray_destroy(current_cu.extern_list);
                    dynarray_destroy(current_cu.ref_to_entry_symbols);
                }
        }
    }
    return 0;
}



int main(int argc, char* argv[]) {
    int i;
    for (i = 2; i < argc; i++) 
   	 assembler_main(i - 1 , &argv[i]);
    return 1;
}
