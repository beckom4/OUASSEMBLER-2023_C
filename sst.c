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
	{' ',"close brackets"},
	{'\t',"close brackets"},
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
    {"lew",sst_tag_cpu_i_lea},
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
	const char *first_portion, portion;
	int i, index_of_tok, label_flag;
	struct sst res;
	
	if(is_comment(line) == 1)
		res.syntax_option = sst_comment;
	else if(is_empty(line))
		res.syntax_option = sst_white_space;
	else
	{
		portion = strchr(line, tokens[1]->tok)
		index_of_tok = (int)(portion - line);
		for(i = 0; i < index_of_tok; i++)
			first_portion[i] = line[i];
		if(is_label(first_portion) == 1)
			
		
	}
   	return res;
}

int is_command(const char str[])
{
   	int left, right, mid; 
	left = 0;
	right = NUM_OF_CMD - 1;
	/*Binary search to find the command*/
	while(left <= right)
    	{
		mid = (left+ right)/2;
        	if(strcmp(str, sst_cpu_insts[mid].inst_name) > 0)
			left = mid;
		else if(strcmp(str, sst_cpu_insts[mid].inst_name) < 0)
			right = mid;
		else 
           		return sst_cpu_insts[mid].i_tag;
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
	if(str[len] != ':')
		return -1;
	else
		return 1;		
}


