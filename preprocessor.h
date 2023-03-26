/*This file serves as the preprocessor - It consists of the functions that find macros in the files, check them for errors, analyze them and save their content.*/

#define FOUND_ERROR -2
#define END_PROGRAM 0
#define MAX_LINE 80
#define MAX_CMD_ARR 16
#define MCR 1
#define MCR_BODY 2
#define ENDMCR 3
#define DONE 4
#define END_FUNCTION -1
#define MCR_SIZE 3
#define ENDMCR_SIZE 6
#define MAX_FILE_NAME 100

int preprocessor(char *file_name);
