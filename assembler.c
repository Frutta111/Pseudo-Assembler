#include "utilities.h" 		/* defintion of macro variabels that contain the machine properties and utilites functions*/
#include "PreProcessor.h"	/* functions of the pre-processor phase - macro translation*/
#include "IC_DC_images.h" /* functions that translate assembly according to the machine word syntax and creates the machie-word-list (the memory images 
of instruction and data) */
#include "first_pass.h"	/* functions of the first pass phase that create the symbol table*/
#include "second_pass.h" /* functions of the second pass phase that update the symbol table*/
#include "print_outputs.h" /* functions that create the output files if there are no compilation errors. print machie-word-list and information from the symbol table   */
/*--------------------------------------------------------------------------------------------------*/
/* SUBMITTED BY:    Efrat Friedrich   */
/*--------------------------------------------------------------------------------------------------*/
/* EXAMPLES of RESULTS 
   See file testres.txt : ./assembler ps1 ps2 ps3 ps4 ps5 ps6  2>testers_errors.txt
   and file testers_errors.txt for outputs,
   or directory screenshots*/
/*--------------------------------------------------------------------------------------------------*/
/* NOTICE!
  The program will stop and will not continue to run even if other input files are waiting in the queue 
  in the following extreme cases only:
1) When dynamic memory allocation failed
2) When the operation of creating one of the output files failed.*/
/*--------------------------------------------------------------------------------------------------*/
/* MAIN DATA STRUCTURES! 
  These are the main data structures in this assembler which are used (called) in several stages (programs):
- OperationsTable - (defined in utilities.h). Special structure that stores the information about the 
	assembly operations that the machine allows.
- DataTable , ReferenceTable , RegistersTable - (defined in assembler.c). Arrays of strings that contain
	 the names of the respective operators: data, label characterizations, registers.
- IC_machine_word_list and  DC_machine_word_list (defined in IC_DC_images.h). A special structure of a 
	linked list that contains the machine words that indicate the assembly code being translated and 
	additional auxiliary information. There are two lists for instructions and data. The lists are 
	managed separately.
- label_list (defined in first_pass.h) . A special structure of a linked list containing all the 
   information about all the labels declared in the assembly program. */
/*--------------------------------------------------------------------------------------------------*/
/* MACHINE LIMITATIONS! (are defined in utilities.h)
 - max size of line is 80. 
 - max size of mcr name or label is 30 
 - max length of cammand name is 4
 - length of input file is 100
 - max size machine word is 14
 - memory size is 256
 - memory first address is 100 */
/*--------------------------------------------------------------------------------------------------*/
/*FUNCTIONALITY OF THE ASSEMBLER PROGRAMS:
- "utilities.h" - defintion of macro variabels that contain the machine properties and utilites functions
- "PreProcessor.c, PreProcessor.h" - Initiall scan of the ".as" input file : The pre-processor phase - macro translation
- "first_pass.c , first_pass.h"	 - First scan of the ".am" input file: The first pass phase that create the symbol table
- "second_pass.c, second_pass.h"	- Second scan of the ".am" input file: The second pass phase that updates the symbol table
- "IC_DC_images.c , IC_DC_images.h"  - functions that translate assembly according to the machine word 
 	syntax and create the machie-word-list (the memory images of instruction and data)
- "syntax_validation.c , syntax_validation.h" - This program provides assembly code syntax validation services for the first pass
- "print_outputs.c , print_outputs.h" - Functions that create the output files if there are no compilation errors. prints
    machie-word-list and information from the symbol table */
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/****************************************************************************************************/



/*-----------------  information about machine operations  ------------------------------------------*/
/* The data table contains the information about the machine's operations */
/* for it's structure definition go to utilities.h. Columns are: operation Name ,operation Code ,operation Type */
/* operation Type: potential number of operands : 0 - no operand ,	1 , 2  , 3 operands  */	
Operation OperationsTable [] =
        {
            {"mov", 0, 2},
            {"cmp", 1, 2},
            {"add", 2, 2},
            {"sub", 3, 2},
            {"not", 4, 1},
            {"clr", 5, 1},
            {"lea", 6, 2},
            {"inc", 7, 1},
            {"dec", 8, 1},
            {"jmp", 9, 3},
            {"bne", 10, 3},
            {"red", 11, 1},
				{"prn", 12, 1},
            {"jsr", 13, 3},
            {"rts", 14, 0},
            {"stop",15, 0},
        }; 

/* information about the machine's key words */      
char *DataTable[] =  {".data", ".string"} ; 
char *ReferenceTable[] =  { ".entry", ".extern"} ;           
char *RegistersTable[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};
int NUM_OF_OPERATIONS = sizeof(OperationsTable) / sizeof(OperationsTable[0]); /* 16 -total number of commamds of the machine*/
int NUM_OF_DATA_TYPES = sizeof(DataTable) / sizeof(DataTable[0]); /* 2 .data .string*/ 
int NUM_OF_REFERENCE_TYPES = sizeof(ReferenceTable) / sizeof(ReferenceTable[0]); /* 2 .entry, .extern */
int NUM_OF_REGISTERS = sizeof(RegistersTable) / sizeof(RegistersTable[0]); /* 8 total number of registers*/


/*-----------------------------------------------------------------------------------------------*/

/*compilation flags*/
extern int flag_preprocessor; /* defined in PreProcessor.c*/
extern int flag_firstpass; /* defined in first_pass.c*/
extern int flag_secondpass; /* defined in second_pass.c*/

/* ---------------------------------------------------------------------------------------------*/



int main(int argc, char *argv[])
{

   
	int i=0, n=0;
	char file_name[MAX_FILE_NAME];   /*MAX_FILE_NAME=100*/	
	int flag_print;	 
			
	if (argc==1) {
		fprintf(stderr,"Error: Enter at least one file name\n");
		exit(0);
	}

    
	for (i=1; i < argc ; i++) {
	
		/*-- get  file name and check if exist--*/
		
		fprintf(stderr,"---------------------------------------------------------\n");
		
		fprintf(stderr,"Running indication of process of program  - %s.as : \n", argv[i]);
		
		n = strlen(argv[i]); 
		
		if ( n > MAX_FILE_NAME) {
			fprintf(stderr , "Error: name of input file is too long. maximum length is %d.\n",MAX_FILE_NAME);
			continue;
		} 
		
		strcpy(file_name, argv[i]);
		file_name[n+1]='\0';
		
		if (!FileExist(file_name , ".as")) /* do not continue compilation if ".as" file is empty or not exist */
			continue;
		
		
		/*-- pass 0 - call PreProcessor   --*/				
		pre_processor(file_name);
		
		if (!FileExist(file_name , ".am")) /* do not continue compilation if ".am" file is empty or not exist*/
			continue;
	
		/*-- pass 1 - call first_pass   --*/
		if (flag_preprocessor == 0)
			first_pass(file_name);
				
	
	  	/*-- pass 2 - call secound_pass   --*/
		if ((flag_preprocessor + flag_firstpass) == 0)
			second_pass(file_name);
		
	 	
		/*-- print flags   --*/		
		flag_print = flag_preprocessor + flag_firstpass + flag_secondpass;	
		
		if (flag_print > 0) {
			fprintf(stderr , "\nflag_preprocessor=%d , flag_firstpass=%d , flag_secondpass=%d\n" , flag_preprocessor, flag_firstpass, flag_secondpass);
		}
			
 		
		/*-- export output files   --*/	
		create_output_files(file_name ,flag_print );
		
	} /* end argc*/
		
	return 0;
}


