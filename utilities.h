/* defintion of macro variabels that contain the machine properties and utilites functions and libraries */
/* Contains also unctions for checking the existence of input files and the creation of output files*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*--------------------------------------------------------------------------------*/
/* check  */
#define alloc_message(a)\
	  if (!a){\
		  	fprintf(stderr, "Failed to allocate memory. Exiting program.\n");\
  			exit(0);\
  		}
/*---------------------------------------------------------------------------------*/
 

/* information about machine */

#define ROWSIZE  82     /* max size of line is 80. (in linux "\n" = 1 byte) */
#define MAX_LABEL 31    /* max size of mcr name or label is 30 */
#define MAX_COMMAND_LENGTH 5 /* max length of cammand name is 4*/
#define MAX_FILE_NAME 101 /*max length of input file is 100*/
#define MACHINE_WORD_SIZE 14 /*max size machine word is 14*/
#define MEMORY_SIZE 256 /* memory size is 256*/
#define MEMORY_FIRST_ADDRESS 100 /* memory FIRST ADDRESS is 100*/


/*--------------------------------------------------------------------------------*/
		
/* The data structure that contains the information about the machine's operations */
/*  for content go to assembler.c */
typedef struct {
    char opName [MAX_COMMAND_LENGTH];
    int  opCode; /*decimal*/
    int  opType; /*see below*/
} Operation;

/* opType: 
	0 - no operand
	1 - one operand (without jmp, bne, jsr) 
	2 - two operand
	3 - jump  operand(jmp, bne, jsr) */
/*--------------------------------------------------------------------------------*/




int check_if_key_word(char []); /*Check if a token is  keyword: returns 1 if token is a keyword and 0 otherwise.*/


/* -----------  functions that check validility of sorce and destination files --------------*/

int FileExist(char * , char *); /*get a file name and it's extention (separately) as input and returns 1 if the file is not empty , or not exits*/
int validFile(FILE *); /*check if the input file is valid: returns 1 if file is exist or not empty and 0 otherwise */
int validCreateF(char *, FILE *); /*checks if file can be created. if the file can not be created then exiting program*/
/*--------------------------------------------------------------------------------------------------------------------*/



