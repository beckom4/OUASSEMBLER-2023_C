#include sst.h"

/*Gets the label that the user defined.*/
char *get_label(struct sst res){
      char *label_copy = malloc(strlen(res.label) + 1); 
      strcpy(label_copy, res.label); 
      return label_copy; 
}

/*Gets the syntax option.*/
int get_syntax_option(struct sst res){
	return res.syntax_option;
}

/*Gets the sst_directive -  Tells you which directive it is.*/
int get_sst_directive(struct sst res){
	 return res.asm_directive_and_cpu_instruction.syntax_directive.dir_tag;
}


/*For directive .data - gets the array of ints.*/
int *get_data_array(struct sst res){
       int *data_array_copy = malloc(res.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.actual_data_size * sizeof(int)); 
       memcpy(data_array_copy, res.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.data_arr, 							      	     res.asm_directive_and_cpu_instruction.syntax_directive.dir.data_array.actual_data_size * sizeof(int)); 
       return data_array_copy; 
}

/*For the directive .string - gets the string.*/
char *get_string(struct sst res){
      char *string_copy = malloc(strlen(res.asm_directive_and_cpu_instruction.syntax_directive.dir.string) + 1); 
      strcpy(string_copy, res.asm_directive_and_cpu_instruction.syntax_directive.dir.string);
      return string_copy; 
}

/*For the directives extern and entry - gets the label.*/
char *get_label_extern_entry(struct sst res){
      char *label_copy = malloc(strlen(res.asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label) + 1); 
      strcpy(label_copy, res.asm_directive_and_cpu_instruction.syntax_directive.dir.label_array.label); 
      return label_copy; 
}


/*Gets the instruction tag.*/
int get_i_tag(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.cpu_i_tag;
}
		

/*For instructions with 2 operands - gets the tag of the 1st operand.*/
int get_ops_tag1(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[0];
}
            

/*For instructions with 2 operands - If the 1st operand is aregister- Gets the register number.*/   
long int get_reg1(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[0].reg;
}
           
/*For instructions with 2 operands - If the 1st operand is a constant number - Gets the number.*/   
long int get_c_number1(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[0].c_number;
}
        
/*For instructions with 2 operands - If the 1st operand is a label - Gets the label.*/   
char *get_label_operand1(struct sst res){
       char *label_copy = malloc(strlen(res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[0].label) + 1); 
       strcpy(label_copy, res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[0].label); 
       return label_copy; 
}
            

/*For instructions with 2 operands - gets the tag of the 2nd operand.*/
int get_ops_tag2(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.ops_tags[1];
}
 	    
          
/*For instructions with 2 operands - If the 2nd operand is aregister- Gets the register number.*/   
long int get_reg2(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[1].reg;
}
           
/*For instructions with 2 operands - If the 2nd operand is a constant number - Gets the number.*/   
long int get_c_number2(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[1].c_number;
}
        
char *get_label_operand2(struct sst res){
       char *label_copy = malloc(strlen(res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[1].label) + 1);
       strcpy(label_copy, res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_2_operands.operands[1].label);
       return label_copy;
}


/*For instructions with 1 operand - Gets the operand tag*/
int get_ops_tag_one_op(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.ops_tag;
}

  
/*For instructions with 1 operand - If the operand is a register, gets the register number.*/		
int get_reg_one_op(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.reg;
}		
	   

/*For instructions with 1 operand - If the operand is a constant number, gets the number.*/		
int get_c_num_one_op(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.c_number;
}           
          

/*For instructions with 1 operand - If the operand is a label, gets the label.*/		
char *get_label_one_op(struct sst res){
       char *label_copy = malloc(strlen(res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label) + 1);
       strcpy(label_copy, res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label);
       return label_copy;
}
           
		
/*For instructions with 1 operand that's a label with 2 operands- gets the label.*/		
char *get_label_one_op_label_with_ops(struct sst res){
       char *label_copy = malloc(strlen(res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.label) + 1);
       strcpy(label_copy, res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.label);
       return label_copy;
}



/*For instructions with 1 operand that's a label with 2 operands- If the 1st operand is a register, gets its number.*/		
long int get_reg1_one_op_label_with_ops(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[0].reg;
}

		
	    
/*For instructions with 1 operand that's a label with 2 operands- If the 1st operand is a constant number, gets the number.*/		
long int get_c_number1_one_op_label_with_ops(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[0].c_number;
}


/*For instructions with 1 operand that's a label with 2 operands- If the 1st operand is a label, gets the label.*/		
char *get_label1_one_op_label_with_ops(struct sst res){
        char *label_copy = malloc(strlen(res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[0].
	label)+ 1);
        strcpy(label_copy, res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[0].label);
        return label_copy;
}
	   
	   
	    
/*For instructions with 1 operand that's a label with 2 operands- If the 2nd operand is a register, gets its number.*/		
long int get_reg2_one_op_label_with_ops(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[1].reg;
}

		
	    
/*For instructions with 1 operand that's a label with 2 operands- If the 2nd operand is a constant number, gets the number.*/		
long int get_c_number2_one_op_label_with_ops(struct sst res){
	return res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[1].c_number;
}


/*For instructions with 1 operand that's a label with 2 operands- If the 2nd operand is a label, gets the label.*/		
char *get_label2_one_op_label_with_ops(struct sst res){
        char *label_copy = malloc(strlen(res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.
	operands[1].label) + 1);
   	strcpy(label_copy, res.asm_directive_and_cpu_instruction.instruction_syntax.inst_operands.set_1_operands.operands.label_with_ops.operands[1].label);
  	return label_copy;
}


/*If the line has an error, gets the error*/
char *get_error(struct sst res){
       char *error_copy = malloc(strlen(res.syntax_error_buffer) + 1);
       strcpy(error_copy, res.syntax_error_buffer);
       return error_copy;
}
