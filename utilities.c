#include "utilities.h" /* defintion of macro variabels that contain the machine properties and utilites functions*/

/* Contains functions for checking the existence of input files and the creation of output files*/
/* Contains a function for checking if a given token is a keyword*/
/*-------------------------------------------------------------------------------------------*/

/* information about the machine's operations and key words - all are defined in assembler.c*/
extern Operation OperationsTable[];    
extern char *DataTable[] ;  			/* {".data", ".string"} */
extern char *ReferenceTable[] ; 		/* { ".entry", ".extern"} */          
extern char *RegistersTable[] ;  	/* {"r0","r1","r2","r3","r4","r5","r6","r7"}*/
extern int NUM_OF_OPERATIONS;  		/* 16 - *total number of commamds of the machine*/
extern int NUM_OF_DATA_TYPES ; 		/* 2 - .data .string*/ 
extern int NUM_OF_REFERENCE_TYPES ; /* 2- .entry, .extern */
extern int NUM_OF_REGISTERS ; 		/* 8 - total number of registers*/

/*------------------------------------------------------------------------------------------*/

/* get a token (a string) as input and check if it is a key-word. returns 1 if token is a keyword and 0 otherwise */

int check_if_key_word(char word[]) {

	int i;

	for (i=0 ; i< NUM_OF_DATA_TYPES; i++) 
			if(strcmp(word,DataTable[i])==0) 		
				return 1;	/* keyword: .data .string*/
				
	for (i=0 ; i< NUM_OF_REFERENCE_TYPES; i++) 
		if(strcmp(word,ReferenceTable[i])==0) 		
			return 1;	/* keyword: .entry, .extern*/

	for (i=0 ; i< NUM_OF_REGISTERS; i++) 
		if(strcmp(word,RegistersTable[i])==0) 		
			return 1;	/* keyword: Registers*/
	
	for (i=0 ; i< NUM_OF_OPERATIONS; i++) 
		if(strcmp(word,OperationsTable[i].opName)==0) 
			 return 1; /* keyword: operation command*/
			 

   return 0; /*not keyword*/
}

/*------------------------------------------------------------------------------------------*/
/*get a file name and it's extention (separately) as input and returns if 1 the file is not empty , or not exits*/

int FileExist(char * file_name , char * extention) {
 	
 	char fname[MAX_FILE_NAME];  /* name of source file ".as" */
 	FILE *fd;
 	
	strcpy(fname,file_name);
	
	/*printf("%s\n",fname );*/
		
	fd = fopen(strcat(fname, extention), "rt"); /*source file*/

	if (!validFile(fd))/* returns 0 if not file isn't valid*/
		return 0;
		
	fclose(fd);
	return 1;
}
	
	
/*------------------------------------------------------------------------------------------*/
/*check if the input file is valid: returns 1 if file is exist or not empty and 0 otherwise */
 int  validFile(FILE *fd) {
 	long size; 
 	   	
     if (fd == NULL) {
        fprintf(stderr,"Error: can not open file! The file  will not be compiled  \n");
        /*exit(0);*/
        return 0;
    }
    
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    
    if (size == 0) {
        fprintf(stderr,"Error: The file is empty! The file  will not be compiled \n");
        /*exit(0);*/
        return 0;
    }
    
    
    rewind(fd); /*Return the pointer to the beginning of the file*/
    return 1;
}

/*------------------------------------------------------------------------------------------*/

/*The function checks if file can be created*/
int	validCreateF(char *fname , FILE *fd) { 
	if (!(fd)){
   	fprintf(stderr,"Error: can not create output file: %s! Exiting program.\n" , fname);
   	exit(0);
      return 0;
    }	
    
    return 1;
}




