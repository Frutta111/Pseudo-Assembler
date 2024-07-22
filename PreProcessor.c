#include "utilities.h"  /* defintion of macro variabels that contain the machine properties and utilites functions*/
#include "PreProcessor.h" /* functions of the pre-processor phase - macro translation*/

/*-----------------------------------------------------------------------------------*/
/* This program is called the Pre Processor phase.
	It generates an ".am" file after macro interpretation.
   For more details about the functions in this section go to pre_processor.h */
 /*-----------------------------------------------------------------------------------*/   


/* information about the machine's operations and key words*/ 
/* all are defined in assembler.c*/
extern Operation OperationsTable[];    
extern char *DataTable[] ;  		/* {".data", ".string"} */
extern char *ReferenceTable[] ;  /* { ".entry", ".extern"} */          
extern char *RegistersTable[] ;  /* {"r0","r1","r2","r3","r4","r5","r6","r7"}*/
extern int NUM_OF_OPERATIONS;  /*16 - *total number of commamds of the machine*/
extern int NUM_OF_DATA_TYPES ; /* 2 - .data .string*/ 
extern int NUM_OF_REFERENCE_TYPES ; /* 2- .entry, .extern */
extern int NUM_OF_REGISTERS ; /* 8 - total number of registers*/

/*------------------------------------------------------------------------------------*/

int flag_preprocessor; /*called by assembler.c*/ /*counts of errors*/

/*------------------------------------------------------------------------------------------*/


void pre_processor(char *fname ) {
	
	FILE *fd1 , *fd2;   /* fd1:source  fd2: dest*/
	
	char fname1[MAX_FILE_NAME];  /* name of sorce file ".as" */
	char fname2[MAX_FILE_NAME];  /* name of destination file ".am" */
	
	char extensions[2][5] = {".as",".am"}; /* extensions of source file (the first) and destination file */
	char line[ROWSIZE]; /* a string containing a line from the source file */
	
	char  **macro_names=NULL;	/*dynamic array of words containing the macro names */
	char  **new_macro_names=NULL;	/* temp object for realloc - array of macro names */
	char new_macro[MAX_LABEL];/*name of a new macro*/


	char  **macro_content=NULL;	/*dynamic array of words containing the macro content */
	char  **new_macro_content=NULL;	/* temp object for realloc - array of macro content */
	char  *new_content; /* temp object for realloc - array of macro content */

	int rnum; /* counter of rows in source file */
	int mnum=0; /*counter of macros */
		
	int  i=0, j=0, temp=0;  
	
	
	/*---open files------------------------------------------------------------------------*/	
	
	strcpy(fname1,fname);
	strcpy(fname2,fname);
	
	fd1 = fopen(strcat(fname1,extensions[0]), "rt"); /*open sorce*/
	validFile(fd1); /*check if file is exist and not empty*/
	
	fd2 = fopen(strcat(fname2,extensions[1]),"wt"); /*open dest*/
	validCreateF(fname2, fd2); /*check if file can be created*/
    
   /*----initialize arrays that will contain the macro names and body----------------------*/ 
	 
	macro_names = malloc(0);
	alloc_message(macro_names)
   
   macro_content = malloc(0); 
   alloc_message(macro_content)
    
	/*---Read from fd1 and write on fd2-----------------------------------------------------*/ 
		
	flag_preprocessor=0; /*counts errors*/
	mnum=0; /* macro counter - counts how many macro definitions are in file*/
	
	fgets(line,ROWSIZE,fd1);
	rnum=1; /*row counter*/
 	
	while (!feof(fd1)) {	  
	 	
		/*check if first the word in the line is "mcr"*/
	   /*if it's a valid "mcr" statement and a valid macro-name - store macro-name and it's content*/
		
		if (is_mcr(line)){		 	
	   		
			/*check if mcr command is correct and save new macro name into : "new_macro" string variable*/
	   		temp = is_valid_mcr(rnum, mnum, line , macro_names, new_macro, fname1);
	   		flag_preprocessor+=(1-temp);
	  			
	   		if (temp) { /*if a line is a valid "mcr" statement*/
	   												
					/*resize array to hold new word*/
					new_macro_names = (char**) realloc(macro_names, (mnum+1)*sizeof(char*));
					alloc_message(new_macro_names)	/*Check if memory allocation was successful - otherwise exit program */
					
					macro_names = new_macro_names;
					
					/*allocate memory for new macro-name*/
					macro_names[mnum] = (char*)malloc((strlen(new_macro) + 1) * sizeof(char));
					alloc_message(macro_names[mnum]) /*Check if memory allocation was successful - otherwise exit program */
					
					/*copy word into allocated memory*/
					strcpy(macro_names[mnum], new_macro);
					
					/*printf("macro_names[mnum]=%s\n",macro_names[mnum]);*/
						
					fgets(line,ROWSIZE,fd1); /*read another line*/
					
					
					rnum++; /*row counter*/
					
					/*resize array to hold new macro content*/
					new_macro_content = (char**) realloc(macro_content, (mnum+1)*sizeof(char*));
					alloc_message(new_macro_content) /*Check if memory allocation was successful - otherwise exit program */
										
					macro_content=new_macro_content;
					
					macro_content[mnum]=malloc(0);
					alloc_message(macro_content[mnum]) /*Check if memory allocation was successful - otherwise exit program */
					
			  		/* insert line content of the macro until "endmcr" statement*/
					while (!(is_endmcr(line))){				
						
						/*allocate memory for new macro content line*/					
						new_content = (char*) realloc(macro_content[mnum], (strlen(macro_content[mnum])+strlen(line) + 1) * sizeof(char));
						alloc_message(new_content) /*Check if memory allocation was successful - otherwise exit program */
					
						macro_content[mnum]=new_content;
						
						/*insert content of new into "macro_content" array*/
						strcat(macro_content[mnum], line);
						
						fgets(line,ROWSIZE,fd1); /*read another line*/
										
						rnum++;	/*row counter*/														
					}  /*endmcr*/		
					
					
					/*check and skip line if it's a correct "endmcr" statement */	
					if (is_endmcr(line)) {
					
						fgets(line,ROWSIZE,fd1); /*read another line*/
												
   					rnum++;	/*row counter*/
   					
					} /*is_endmcr*/
					
					 mnum++; /* macro counter*/
													
				} /* in_not_endmcr*/
		  			
	   }/* is mcr */ 
	   
	  
    	/* if a statement is one word and if it is a macro name */	    		   		
		if ( (j = is_calling_mcr(mnum, line, macro_names)) >= 0){	
    		fputs(macro_content[j], fd2); /*print on ".am" file*/
    		
    		/*printf("rnum=%d , %s" , rnum , macro_content[j]);*/
  		}
  		 
   	else 
   		fputs(line, fd2);	/*print on ".am" file*/
   	
   	fgets(line,ROWSIZE,fd1); /*read another line*/
   	

   	rnum++; /*row counter*/

	} /*end of file*/
	
	
	/*-----free memory allocated for array----*/
   for (i=0 ; i < mnum; i++){
   	free(macro_content[i]); 
   	free(macro_names[i]); 
   }
   
	free(macro_names);
	free(macro_content);
	
	/*----close files-----*/
	fclose(fd1);
	fclose(fd2);
	
}


/*------------------------------------------------------------------------------------------*/


/* The function checks if first the word in a string is "mcr" */

int is_mcr (char *string){

	char *word ;
	char temp[ROWSIZE];

	strcpy(temp,string);
	   
   word = strtok(temp, " \t");
 
   /*check the first word*/
   if (strcmp(word, "mcr") == 0)   
   	return 1;
   
   else
   	return 0; /* not macro statement*/
}

/*------------------------------------------------------------------------------------------*/

/* The function checks if the line is a macro statenemt and if its valid*/

int is_valid_mcr (int rnum, int mnum, char *string , char **macro_names , char *new_macro , char *file_name){

   char *word ;
   char temp[ROWSIZE];
	char token2[ROWSIZE];
	int i, word_count=0;
	int len;
	
	strcpy(temp,string);
		   
   word = strtok(temp, " \t\n");
      	
	while (word) {
		if (word!=NULL)
			strcpy(token2, word); /*contains macro name*/
		word_count++;
		word = strtok(NULL, " \t\n");
  	}
    
/* Check if the line contains exactly two words */
	if (!(word_count == 2)) {
   	fprintf(stderr, "Error: in file: %s line %d: not a valid macro statement - macro statement must contain exactly 2 words\n", file_name, rnum);
	  	return 0;	/*not valid macro */
 	}	
 		
/* Check if the name of macro is valid - keyword*/	
   
   if (check_if_key_word(token2)) {
   fprintf(stderr, "Error: in file: %s line %d: not a valid macro name - not allowed to use any key word\n", file_name, rnum);
           return 0; /*not valid macro */
     }
    
      
/* Check if the name of macro is valid - name starts with a letter*/		 
 	len = strlen(token2);
 	
 	if (!(isalpha(token2[0])))  {
 		fprintf(stderr, "Error:  in file: %s line %d: not a valid macro name - name must start with a letter\n", file_name, rnum);
 		return 0;	/*not valid macro */
 	}
			
/* Check if the name of macro is valid - name does not contain illegal chars*/		 	
 	for (i=1 ; i < len; i++ ) {
		if ( ! ( isdigit(token2[i]) || isalpha(token2[i]) )  ) {
			fprintf(stderr, "Error: in file: %s line %d: not a valid macro name - name contains illegal character\n",file_name , rnum);
   		return 0; /*not valid macro */
   	}	
 	}
 	
 	/* Check if the name of macro is valid - name has not already been used before*/		
	for ( i = 0; i < mnum; i++) {
	if (strcmp(macro_names[i], token2) == 0) {
			fprintf(stderr, "Error: in file: %s line %d: not a valid macro name - name already used before\n",file_name, rnum);
         return 0; /*not valid macro */
      }
   }
 	 	
   strcpy(new_macro, token2);
 	
 	/*printf("token2 = %s\n , new_macro=%s\n",token2,new_macro);*/
 		 
	return 1; /* valid macro */
 	    
}

/*------------------------------------------------------------------------------------------*/

/* The function checks if the line is end-macro statement.*/

int is_endmcr (char *string){

	char *word ;
	char temp[ROWSIZE];
	char token[MAX_LABEL];
	int  word_count=0;

	strcpy(temp,string);
	
   word = strtok(temp, " \t\n");
   
   if (word!=NULL)
   	strcpy(token,word);
   	
   while (word) {
		word_count++;
		word = strtok(NULL, " \t\n");
	}
	
	if (word_count==1 && strcmp(token,"endmcr") == 0)
			return 1;
	
	return 0; /* do not end of a macro */

}

/*------------------------------------------------------------------------------------------*/

/* The function checks if the line contains 1 word and if it is a name of a macro that already has been defined .
 	returns macro index if True  , returns (-1) - otherwise */  
   
int is_calling_mcr(int mnum, char *string , char **macro_names){

	char *word ;
	char token[MAX_LABEL];
	char temp[ROWSIZE];
	int i, word_count=0;
	
	strcpy(temp,string);
	
   word = strtok(temp, " \t\n");

   if (word!=NULL)
   	strcpy(token,word);
   	
   while (word) {
		word_count++;
		word = strtok(NULL, " \t\n");
	}
	
   /*check if line contains exactly one word and if it calls a macro-name that already have been defined*/
	if (word_count==1){
		for (i=0 ; i < mnum ; i++) {
			if (strcmp(token,macro_names[i]) == 0){
				return i; /*call number i macro*/
			}
		}
	}
	
	return -1; /* do not call macro */

}


/*---------------------------------------------------------------------------------------------------------*/


	

