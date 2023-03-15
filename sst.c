#include "sst.h"

struct sst_token {
    const char tok;
    const char * tok_description;
};

static struct sst_token tokens[NUM_OF_SEPERATORS] = {
    	{',',"seperator"},
   	{':',"end of label seperator"},
   	{'(',"open brackets"},
    	{')',"close brackets"},
};

struct sst_cpu_inst {
    const char *inst_name;
    const enum sst_cpu_inst_tag i_tag;
};

/*Lexicographically sorting the array.*/
static struct sst_cpu_inst sst_cpu_insts[NUM_OF_CMD] = {
    {"add",sst_tag_cpu_i_add},
    {"bne",sst_tag_cpu_i_bne},
    {"clr",sst_tag_cpu_i_clr},
    {"cmp",sst_tag_cpu_i_cmp},
    {"dec",sst_tag_cpu_i_dec},
    {"inc",sst_tag_cpu_i_inc},
    {"jmp",sst_tag_cpu_i_jmp},
    {"jsr",sst_tag_cpu_i_jsr},
    {"lea",sst_tag_cpu_i_lea},
    {"mov",sst_tag_cpu_i_mov},
    {"not",sst_tag_cpu_i_not},
    {"prn",sst_tag_cpu_i_prn},
    {"red",sst_tag_cpu_i_red},
    {"rts",sst_tag_cpu_i_rts},
    {"stop",sst_tag_cpu_i_stop},
    {"sub",sst_tag_cpu_i_sub},
};

struct sst sst_get_stt_from_line(const char * line) 
{	
	char *token;
	char portion[MAX_LINE], last_portion[MAX_LINE];
	int i, j, k, index_of_tok, error_flag, label_flag, command_flag, directive_flag;
	int *index;

	struct sst res;

	i = 0, error_flag = 0, command_flag = -1, directive_flag = -1;
	index = &i;
	/*Checking if this is a comment.*/	
	if(is_comment(line) == 1)
		res.syntax_option = sst_comment;
	/*Checking if this is an empty line*/
	else if(is_empty(line))
		res.syntax_option = sst_white_space;
	/*Checking if this is a command/ directive*/
	else
	{
		/*Checking for ':', meaning for labels:*/
		label_flag = check_label(&res, line);
		if(label_flag == -1)
			error_flag = 1;
		*index += label_flag;
		printf("label_flag is: %d and index is: %d\n", label_flag, *index);
		/*Making sure that we did not find any errors.*/
		if(error_flag != 1)
			command_flag = check_command(line, &index);
		/*Checking if the current portion of the line is a command*/
		if(command_flag != -1)
			res.syntax_option = sst_instruction;
		/*Checking if this is a directive*/
		else if(error_flag != 1)
			directive_flag = is_directive(line, &index);
		/*Checking if we found a directive*/
		if(directive_flag != -1)
			res.syntax_option = sst_directive;
		printf("i is: %d\n", *index);
		j = 0;
		while(line[*index] != '\0')
		{
			printf("char is: %c\t", line[*index]);
			last_portion[j] = line[*index];
			j++;
			++(*index);
		}
		last_portion[j] = '\0';
		printf("last_portion is: %s\n",last_portion);
			
		/*Extracting the rest of the text.*/
		for(j = 0; j < NUM_OF_TOKENS; j++)
		{
			token = strchr(last_portion,tokens[j].tok);
			if(token != NULL)
			{
				index_of_tok = (int)(token - last_portion);
				for(k = 0; k < index_of_tok; k++)
					portion[k] = last_portion[k];
				portion[k] = '\0';
				printf("portion is: %s\n", portion);
			}
										
		}
			
		
						
	}
	

	/*TEMPORARY - FOR CHECKS ONLY*/
	printf("label_flag is: %d\n", label_flag);
	printf("command_flag is: %d\n", command_flag);
	printf("directive_flag is: %d\n", directive_flag);
	/*TEMPORARY - FOR CHECKS ONLY*/
	

	if(error_flag == 1)
		res.syntax_option = sst_syntax_error;

   	return res;
}

int check_command(const char str[], int **index)
{
	int left, right, mid, j, parse_begin, parse_end,temp;
	char cmd[MAX_LINE];
	j = 0, parse_begin = 0, parse_end = 0, temp = **index;

	while (parse_end != 1) 
	{
    		if (!isspace(str[**index + 1])) 
		{
       			parse_begin = 1;
        		cmd[j] = str[**index + 1];
        		++j;
    		} 
		else if (isspace(str[**index + 1]) && parse_begin == 1) {
       			parse_end = 1;
    	}
    		++(**index);
	}
	cmd[j] = '\0';
	left = 0;
	right = NUM_OF_CMD - 1;
	/*Binary search to find the command*/
	while(left <= right)
    	{
		mid = (left+ right)/2;
        	if(strcmp(cmd, sst_cpu_insts[mid].inst_name) > 0)
			left = mid + 1;
		else if(strcmp(cmd, sst_cpu_insts[mid].inst_name) < 0)
			right = mid - 1;
		else 	
           		return sst_cpu_insts[mid].i_tag;
   	}
	**index = temp;
    	return -1;
}


int is_reg(const char str[])
{
	int i;
	const char regs[][REG_SIZE] = {"r0","r1","r2","r3","r4",
				"r5","r6","r7"};

	for(i = 0; i < NUM_OF_REGS; i++)
	{
		if(strcmp(str,regs[i]))
			return i;
	}
	return -1;
}



int is_empty(const char str[])
{
	int i, count;
	i = 0;
	count = 0;
	while (i < strlen(str))
	{
		if (isspace(str[i]))
			count++;
		i++;
	}
	if(count == strlen(str))
		return 1;
	else
		return 0;
}


int is_comment(const char str[])
{
	if(str[0] == ';')
		return 1;
	else
		return 0;

}

int is_directive(const char str[], int **index)
{
	int i, j, parse_begin,parse_end,temp;
	char directive[MAX_LINE];
	const char directives_arr[][WORD_SIZE] = {".data",".string",".entry",".extern"};
	j = 0;
	temp = **index;
	while (parse_end != 1) 
	{
    		if (!isspace(str[**index + 1])) 
		{
       			parse_begin = 1;
        		directive[j] = str[**index + 1];
        		++j;
    		} 
		else if (isspace(str[**index + 1]) && parse_begin == 1)
       			parse_end = 1;
    		++(**index);
	}
	directive[j] = '\0';
	printf("directive is: %s\n", directive);
	for(i = 0; i < NUM_OF_INSTRUCTIONS; i++)
	{
		if(strcmp(directive,directives_arr[i]) == 0)
			return i;
	}
	**index = temp;
	return -1;
}

int is_label(const char str[])
{
	int i;
	size_t len;
	len = strlen(str);
	for(i = 0; i < len; i++)
		if(!isalpha(str[i]))
			return -1;
	return 1;		
}


int check_label(struct sst *res, const char *line)
{
	int i, index_of_tok;
	char first_portion[MAX_LABEL_LENGTH + 1];
	char *token;
	char *final_portion = (char*)malloc(sizeof(char) * MAX_LINE);
	/*Making sure that the system managed to allocate enough memory for first_portion*/
	if(first_portion == NULL)
	{
		printf("The system was not able to allocate enough memory for some of your text.\n");
		exit(1);
	}
	token = strchr(line, tokens[1].tok);
	if(token != NULL)
		index_of_tok = (int)(token - line);
	else
		final_portion = token + 1;
	if (final_portion != NULL)
       	 	token = strchr(final_portion, tokens[1].tok);
	else
        	token = NULL;
	/*Checking if there are more ':' in the line, meaning if there's an error.*/
	if(token != NULL || index_of_tok == 0)
	{
		free(final_portion);
		return -1;
	}
	else
	{
		for(i = 0; i < index_of_tok; i++)
			first_portion[i] = line[i];
		first_portion[index_of_tok] = '\0';
		if(is_label(first_portion) == 1)
		{
			strcpy(res->label,first_portion);
			free(final_portion);
			return index_of_tok;
		}
		else
		{
			free(final_portion);
			return 0;
		}
	}		
}


int main()
{
	char test[] = "MAIN:	mov r1, r3";
	sst_get_stt_from_line(test);
	return 0;
}
