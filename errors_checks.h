#define LEN_OF_CMD_ARR 16
#define MAX_ASCII 127
#define REG_SIZE 2


int label_errors(char str[], struct sst *res);

int directive_data_errors(char str[], struct sst *res);

int directive_string_errors(char str[], struct sst *res);

int directive_entext_errors(char str[], struct sst *res);

int no_operands_errors(char str[], struct sst *res);

int operand_label_errors(char str[], struct sst *res);

int is_empty_error(char str[]);

int is_directive(char str[]);

int is_reg(char str[]);

int is_command(char str[]);

int is_ascii(char c);
