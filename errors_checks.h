/* -------------------------------------------- */

/** 
* MAMAN 14, FILE: errors_checks.h
* @authors: Linoy daniel & Omri Beck
* This file is part of the sst module and supports it by checking the text, or some of it, for errors.
**/

/* -------------------------------------------- */

#define LEN_OF_CMD_ARR 16
#define MAX_ASCII 127
#define REG_SIZE 2


/**
 * Checks the label for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int label_errors(char str[], struct sst *res);
/**
 * Checks the directive data for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int directive_data_errors(char str[], struct sst *res);

/**
 * Checks the directive string for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int directive_string_errors(char str[], struct sst *res);

/**
 * Checks the directives entry and extern for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int directive_entext_errors(char str[], struct sst *res);

/**
 * Checks a command with no operands for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int no_operands_errors(char str[], struct sst *res);

/**
 * Checks a command with with a label as operand for errors.
 * 
 * @param str - the string to check.
 *        res - a pointer to the sst.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int operand_label_errors(char str[], struct sst *res);

/**
 * Checks if a string is empty or not.
 * 
 * @param str - the string to check.
 * @return 1 - No errors found.
 *         FOUND_ERROR - error found.
 */
int is_empty_error(char str[]);

/**
 * Checks if a string is a directive or not.
 * 
 * @param str - the string to check.
 * @return 1 - the string is a directive.
 *         0 - the string is not a directive.
 */
int is_directive(char str[]);

/**
 * Checks if a string is a register or not.
 * 
 * @param str - the string to check.
 * @return 1 - The string is a register.
 *         0 - The string is not a register.
 */
int is_reg(char str[]);

/**
 * Checks if a string is a command or not.
 * 
 * @param str - the string to check.
 * @return 1 - The string is a coammnd.
 *         0 - The string is not a command.
 */
int is_command(char str[]);

/**
 * Checks if a char is ascii or not.
 * 
 * @param str - the char to check.
 * @return 1 - The char is ascii.
 *         0 - The char is not ascii.
 */
int is_ascii(char c);


