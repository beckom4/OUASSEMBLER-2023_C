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
	char *first_portion;
	int i, index_of_tok, error_flag, label_flag, command_flag;
	char command;

	struct sst res;

	error_flag = 0;
	
	if(is_comment(line) == 1)
		res.syntax_option = sst_comment;
	else if(is_empty(line))
		res.syntax_option = sst_white_space;
	else
	{
		/*Checking for ':', meaning for labels:*/
		label_flag = check_label(&res, line);
		if(label_flag == -1)
			error_flag == 1;
		if(error_flag != 1)
			command_flag = check_command(line, label_flag);
		
				
	}
	

	/*TEMPORARY - FOR CHECKS ONLY*/
	printf("label_flag is: %d\n", label_flag);
	printf("command_flag is: %d\n", command_flag);
	/*TEMPORARY - FOR CHECKS ONLY*/
	

	if(error_flag == 1)
		res.syntax_option = sst_syntax_error;

   	return res;
}

int check_command(const char str[], int index)
{
   	int left, right, mid,i,j,pars_begin,pars_end;
	char cmd[MAX_LINE];
	i = index + 1, j = 0, pars_begin = 0, pars_end = 0;
	printf("line is: %s\n", str);
	while(pars_end != 1)
	{
		if(!isspace(str[i]))	
		{
			pars_begin = 1;
			cmd[j] = str[i];
			++j;
		}
		else if(isspace(str[i]) && pars_begin == 1)
			pars_end = 1;
		++i;	
	}
	cmd[j] = '\0';
	printf("cmd is: %s\n", cmd);
	left = 0;
	right = NUM_OF_CMD - 1;
	/*Binary search to find the command*/
	while(left <= right)
    	{
		mid = (left+ right)/2;
		printf("flag2\n");
        	if(strcmp(cmd, sst_cpu_insts[mid].inst_name) > 0)
		{
			left = mid + 1;
			printf("flag3\n");
		}
		else if(strcmp(cmd, sst_cpu_insts[mid].inst_name) < 0)
		{
			right = mid - 1;
			printf("flag3\n");
		}
		else 
		{	
           		return sst_cpu_insts[mid].i_tag;
		}
   	}
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

int is_instruction(const char str[])
{
	int i;
	const char instructions[][WORD_SIZE] = {".data",".string",".entry",".extern"};

	for(i = 0; i < NUM_OF_INSTRUCTIONS; i++)
	{
		if(strcmp(str,instructions[i]))
			return i;
	}
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

	token = strchr(line, tokens[1].tok);
	if(token != NULL)
		index_of_tok = (int)(token - line);
	else
		final_portion = token + 1;
	if (final_portion != NULL)
       	 	token = strchr(final_portion, tokens[1].tok);
	else
        	token = NULL;
	printf("final portion is: %s\n", final_portion);
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


