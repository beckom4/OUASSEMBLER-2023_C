/* -------------------------------------------- */

/**
* MAMAN 14, FILE: assembler.c
* authors: Linoy daniel & Omri Beck
**/

/* -------------------------------------------- */


/* -------- DESCRIPTION: -------- */


/* --------- INCLUDES: --------- */


#include "assembler.h"
#include "sst.h"
#include <stdio.h>
#include <string.h>
#include "dynarray.h"
#include "bmc.h"


/* --------- DEFINES: --------- */


#define is_inst_tag_set_A(tag) (tag >= sst_tag_cpu_i_mov && tag <= sst_tag_cpu_i_lea )
#define is_inst_tag_set_B(tag) (tag >= sst_tag_cpu_i_not && tag <= sst_tag_cpu_i_jsr )
#define is_inst_tag_set_C(tag) (tag >= sst_tag_cpu_i_rts && tag <= sst_tag_cpu_i_stop )


/* --------- STRUCTURES: --------- */


struct asm_label {
    char label_str[MAX_LABEL_LENGTH + 1];
    int address;
    enum {
        asm_label_type_extern,
        asm_label_type_entry,
        asm_label_type_code,
        asm_label_type_data,
        asm_label_type_data_entry,
        asm_label_type_code_entrysst_fianl_test: sst.o errors_checks.o handle_parse.o preprocessor.o dynarray.o bmc.o assembler.o
	gcc -ansi -Wall -pedantic sst.o errors_checks.o handle_parse.o preprocessor.o dynarray.o bmc.o assembler.o -o sst_final_test
sst.o: sst.c sst.h errors_checks.h handle_parse.h
	gcc -c -ansi -Wall -pedantic sst.c -o sst.o
errors_checks.o: errors_checks.c errors_checks.h sst.h handle_parse.h
	gcc -c -ansi -Wall -pedantic errors_checks.c -o errors_checks.o
handle_parse.o: handle_parse.c errors_checks.h sst.h
	gcc -c -ansi -Wall -pedantic handle_parse.c -o handle_parse.o
preprocessor.o: preprocessor.c preprocessor.h
	gcc -c -ansi -Wall -pedantic preprocessor.c -o preprocessor.o
dynarray.o: dynarray.c dynarray.h
	gcc -c -ansi -Wall -pedantic dynarray.c -o dynarray.o
bmc.o: bmc.c bmc.h dynarray.h
	gcc -c -ansi -Wall -pedantic bmc.c -o bmc.o
assembler.o: assembler.c dynarray.h bmc.h sst.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o
    } asm_label_type;

    int line_of_definition;
};


struct CU {
    struct asm_label *symbol_table;
    int * bmc_code_list;
    int * bmc_data_list;
    void * extern_list;

};






















/* --------- FUNCTIONS DECLARATIONS: --------- */


/**
 * @brief search given symbol in the symbol table.
 * 
 * @param symbol_table - pointer to the symbol table.
 * @param symbol - pointer to the given symbol needed to find.
 * @return return pointer to the symbol if found, otherwise return NULL.
 */
static struct asm_label * assembler_search_symbol(struct asm_label * symbol_table, const char * symbol);


/**
 * @brief builds the symbol table...
 * 
 * @param am_file assuming file is open in read mode, and is ok (not NULL).
 * @param cu pointer to CU struct.
 * @return returns 0 on success, and on error return any other number.
 */
static int assembler_first_pass(FILE* am_file, struct CU* cu);


/**
 * @brief encode given file into array
 * 
 * @param am_file assuming file is open in read mode, and is ok (not NULL).
 * @param cu pointer to CU struct.
 * @return returns 0 on success, and on error return any other number.
 */
static int assembler_second_pass(FILE* am_file, struct CU* cu);



/* --------- FUNCTIONS IMPLEMENTATION: --------- */


static struct asm_label * assembler_search_symbol(struct asm_label * symbol_table, const char * symbol) {
    int i;
    for(i = 0 ; i < dynarray_length(symbol_table) ; i++) {
        if(strcmp(symbol_table[i].label_str,symbol) == 0) {
            return &symbol_table[i];
        }
    }
    return NULL;
}


/* -------------------------------------------- */


static int assembler_first_pass(FILE * am_file, struct CU * cu) {
    char line_buff[MAX_LINE_LEN + 1] = {0};
    struct sst syntax_line;
    struct asm_label * find;
    struct asm_label new_label;
    int line_counter = 1, IC = PROG_START_ADDR, DC = 0;
    int i;
    /* reads line by line in the given am_file, and store it in line_buff.
       reads each line until MAX_LINE_LEN or new line character:  */
    while(fgets(line_buff, MAX_LINE_LEN, am_file) != NULL) {
	printf("flag1\n");
        /* construct sst struct of the current line */
        syntax_line = sst_get_stt_from_line(&line_buff[0]);
	printf("flag2\n");
        /* handles each line {syntax error, instruction, directive, comment, empty} */
        switch (syntax_line.syntax_option){

            case sst_syntax_error: 
                /* print error */
            break;

            case sst_directive: case sst_instruction: 
                /* check if there is label defination in the line: */
                if (syntax_line.label[0] != '\0') { 
                    /* relative only for string data or any instruction */
                    if (syntax_line.syntax_option == sst_instruction 
                        || syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag == sst_tag_dir_data  
                        || syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag == sst_tag_dir_string) {
                        find = assembler_search_symbol(cu->symbol_table,syntax_line.label);
                        if (find != NULL) {
                            if(syntax_line.syntax_option == sst_instruction) {
                                switch (find->asm_label_type){
                                    case asm_label_type_entry:
                                        find->address = IC;
                                        find->asm_label_type = asm_label_type_code_entry;
                                    break;
                            default: 
                                /* all other cases are redefinition !*/
                                /* error */
                                break;
                            }
                        }
                        else {
                            switch (find->asm_label_type){
                            case asm_label_type_entry:
                                find->address = DC;
                                find->asm_label_type = asm_label_type_data_entry;
                                break;
                            
                            default:
                                /* all other cases are redefinition !*/
                                /* error */
                                break;
                            }
                        }
                    }
                    else {
                        strcpy(new_label.label_str,syntax_line.label);
                        new_label.line_of_definition = line_counter;
                        if (syntax_line.syntax_option == sst_instruction) {
                            new_label.asm_label_type = asm_label_type_code;
                            new_label.address = IC;
                        }
                        else {
                            new_label.asm_label_type = asm_label_type_data;
                            new_label.address = DC;
                        }
                        dynarray_push(cu->symbol_table,new_label);

                    }
                }
	    case sst_white_space:
		break;
	    case sst_comment:
		break;
            }

            if(syntax_line.syntax_option == sst_instruction) {
                IC++;
                if(is_inst_tag_set_A(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag)) {
                    if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] == sst_tag_op_register &&
                                syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1] == sst_tag_op_register) {
                                    IC++;
                                }
                                else {
                                    IC += 2;
                                }
                    
                }
                else if(is_inst_tag_set_B(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag)) {
                    if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag == sst_tag_op_label_with_operands) {
                        if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[0] == sst_tag_op_register &&
                            syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.ops_tags[1] == sst_tag_op_register ) {
                            IC += 2; 
                        } 
                        else 
                            IC += 3;
                    }
                    else 
                        IC++;
                }
            } 
            else {
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
			/*IMPORTANT - this switch - case is the cause of multiple warnings.*/
                        switch (find->asm_label_type)
                        {
                       /* case asm_label_type_entry:
                            switch (syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag){*/
                            case asm_label_type_extern:
                                /* error */
                               break;
                            case asm_label_type_entry: 
				break;
			    case asm_label_type_data_entry: 
				break;
			    case asm_label_type_code_entry:
                                /* warning*/
				break;
                            case asm_label_type_data:
                                find->asm_label_type = asm_label_type_data_entry;
                                break;
                            case asm_label_type_code:
                                find->asm_label_type = asm_label_type_code_entry;
                                break;
                            }
                            break;
                     /*   }*/
                    }else {
                        strcpy(new_label.label_str,syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label);
                        new_label.asm_label_type = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir_tag;
                        new_label.line_of_definition = line_counter;
                        dynarray_push(cu->symbol_table,new_label);
                    }
                }
            }
            break;
        }
        for(i = 0 ; i < dynarray_length(cu->symbol_table) ; i++) {
            if(cu->symbol_table[i].asm_label_type == asm_label_type_entry) {
                /* error declaring  entry without  definition */
            }else if(cu->symbol_table[i].asm_label_type == asm_label_type_data || cu->symbol_table[i].asm_label_type == asm_label_type_data_entry ) {
                cu->symbol_table[i].address +=IC;
            }
        }
        line_counter++;
    }

    return 1;
}


/* -------------------------------------------- */


static int assembler_second_pass(FILE * am_file, struct CU * cu) {
    char line_buff[MAX_LINE_LEN + 1] = {0};
    struct sst syntax_line;
    struct asm_label * sym_find;
    int mem_word = 0;
    int i;
    int line_counter = 0;
    rewind(am_file);
    while(fgets(line_buff,MAX_LINE_LEN,am_file) != NULL) {
        syntax_line = sst_get_stt_from_line(&line_buff[0]);
        switch (syntax_line.syntax_option)
        {
        case sst_instruction: {
            /* first machine word*/
            mem_word = syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag << 8; /* op code*/
            if (is_inst_tag_set_A(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag)) {
                mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] << 4;/* src*/
                mem_word |= syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1] << 2;/* dest*/
                dynarray_push(cu->bmc_code_list, mem_word);
                
                /* next machine words are arguments..*/
                /* if both operands are registers*/
                if(syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0] == sst_tag_op_register &&
                                syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1] == sst_tag_op_register) {
                           mem_word = (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[0].reg  << 6) | /* src*/
                                        (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[1].reg   << 2); /* dest*/
                                        dynarray_push(cu->bmc_code_list, mem_word);
                }else {
                /* if the operands are in any combination of register, label, and constant number then..*/
                    for(i = 0 ; i < 2 ; i++) {
                        switch (syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[i]){
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
                                }
                                else {
                                    mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                                }
                            }
                            else {
                                /* error .... undefined symbol*/
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
                    dynarray_push(cu->symbol_table,mem_word);
                    /* must  be BNE ,JSR or JMP*/
                    sym_find = assembler_search_symbol(cu->symbol_table,syntax_line.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.label);
                    if(sym_find != NULL) { /* if we found the symbol*/
                        if(sym_find->asm_label_type == asm_label_type_extern) {
                            mem_word = 1; /* A.R.E = 01*/
                        }else {
                            mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                        }
                        dynarray_push(cu->bmc_code_list,mem_word);
                    }else {
                            /* error .... undefined symbol*/
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
                            }else {
                                mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                            }
                            }else {
                                /* error .... undefined symbol*/
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
                            }else {
                                mem_word = (2 | (sym_find->address << 2)); /* A.R.E = 10 */ 
                            }
                            }else {
                                /* error .... undefined symbol*/
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
                    for(i = 0 ; i < syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.actual_data_size ; i++) {
                        mem_word = syntax_line.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr[i];
                        dynarray_push(cu->bmc_data_list,mem_word);
                    }
                }
           break; 
        }   
        case sst_syntax_error:
		break; 
	case sst_comment: 
		break;
	case sst_white_space:
            break;
        }
        line_counter++;
    }
    
    return 1;
}


/* -------------------------------------------- */


int main() {
    char* file_names[] = {"file1.txt", "file2.txt"};
    int num_files = sizeof(file_names) / sizeof(file_names[0]);
    assembler_main(num_files, file_names);
    return 0;
}

int assembler_main(int number_of_files, char **file_names) {
    int i;
    struct CU current_cu;
    current_cu.bmc_code_list = dynarray_create(int); 
    current_cu.bmc_data_list = dynarray_create(int);
    current_cu.symbol_table  = dynarray_create(struct asm_label);

    for ( i = 0; i < number_of_files; i++) {
        FILE *file_ptr = fopen(file_names[i], "r"); 
        if (file_ptr == NULL) {
            fprintf(stderr, "Error: could not open file %s\n", file_names[i]);
            continue;
        }

        assembler_first_pass(file_ptr, &current_cu);
        assembler_second_pass(file_ptr, &current_cu);

        fclose(file_ptr); 
    }

    return 0;
}




