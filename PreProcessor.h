
void  pre_processor(char * ); /*It is the main function of this section*/
int is_mcr (char *); /*checks if first the word in the line is "mcr"*/
int is_valid_mcr (int , int , char * , char ** , char *, char *); /*checks if the line is a correct macro statenemt*/
int is_endmcr (char *); /*checks if the line is end-macro statement*/
int is_calling_mcr(int , char *, char ** ); /*checks if the line calls a macro that has been defined*/

/**************  more details *******************************************************************************/
/*void  pre_processor(char * );*/

/* It is the main function of this section.
   it generates an  ".am" file after macro interpretation.
   
   input parameters: ".as" name file.
   
   the program read the "as" file and analyzes it line after line.

	The macro definition must be correct :
	- Standard declaration line and standard macro name (see is_valid_mcr function)
	- standard ending line  (see is_endmcr function)
	- call to a macro that has been defined before (see is_calling_mcr function)
   
   In any case of an error the macro will not be interpreted.
   
   outputs: 
   -  ".am" file  after macro interpretation
   -  global variable "flag_preprocessor"- equals 0 if no errors were observed. otherwise equals the numbers of errors.*/
     
/*------------------------------------------------------------------------------------------------------*/


/*int is_mcr (char *);*/ 

/* The function checks if first the word in the line is "mcr" 

   If true - returns  1 
   If false - returns 0
   
   input parameters:
	string = a line in the input file. max size is ROWSIZE.

*/

/* ----------------------------------------------------------------------------------------------------------*/
/*int is_valid_mcr (int , int , char * , char ** , char *, char *);*/

/*	The function checks if the line is a macro statenemt.

	Check if the line contains exactly two words.
   Check if the name of macro is valid 
   	- not a keyword
   	- name does not contain illegal chars
   	- name starts with a letter
   	- name does not contain illegal chars (only alphabet or numbers)
   	- name has not been already used before
   	
   If true - returns  1 and a string named by token2 that contains the name of the macro.
   If false - returns 0

   input parameters:
   - rnum = row number in input file
	- mnum = the number of macros defined so far
	- string = a line in the input file. max size is ROWSIZE. 
   - nmacros= an array of strings containing the macro names defined so far
   - token2 = empty string. will contain the macro name that is being checked by this function. 

   globals variable inputs (for details go to assembler.c: 
	- Operation OperationsTable[];    
	- DataTable[] ;  		 {".data", ".string"} 
	- ReferenceTable[] ;   { ".entry", ".extern"}           
	- RegistersTable[] ;   {"r0","r1","r2","r3","r4","r5","r6","r7"}
	- NUM_OF_OPERATIONS;  16 - *total number of commamds of the machine
	- NUM_OF_DATA_TYPES ;  2 - .data .string 
	- NUM_OF_REFERENCE_TYPES ;  2- .entry, .extern 
	- NUM_OF_REGISTERS ; 8 - total number of registers

   
 */
/* ----------------------------------------------------------------------------------------------------------*/

/*int is_endmcr (char *); */

/* The function checks if the line is end-macro statement.
	checks if line contains exactly one word and if it is "endmcr".
	If true - the function returns 1
	If false - the function returns 0
	
	input parameters:
	string = a line in the input file. max size is ROWSIZE.
	*/
/* ----------------------------------------------------------------------------------------------------------*/

/*int is_calling_mcr(int , char *, char ** );*/ 

/* The function checks if the line calls a macro that already has been defined. 
	checks if line contains exactly one word and if it calls a macro-name that already have been defined.
   If true - returns the corresponding index in the array of macro names. 
   If false - the function returns (-1).

   input parameters:
	mnum = the number of macros defined so far
	string = a line in the input file. max size is ROWSIZE.
   nmacros= An array of strings containing the macro names that have been used/defined so far
   
 */




