#include "utilities.h"		/* defintion of macro variabels that contain the machine properties and utilites functions*/
#include "IC_DC_images.h" 	/* functions that translate assembly according to the machine word syntax and creates the machie-word-list*/
#include "first_pass.h"		/* functions of the first pass phase that create the symbol table*/

/*------------------------------------------------------------------------------------------------------------------*/
/*This program provides the assembler "translation services"  for the first and second stages.
 
 It analayse the assembly instruction and save the it in the code-image list or in the in the data-image list.
 The information is stored in a two structures of type "Machine_word_list" calld IC_machine_word_list and DC_machine_word_list.
 
 The translation into machine language is performed only if the input line is syntactically correct!
  
 for detailed information about the functions go to  IC_DC_images.h. 
   */
/*-----------------------------------------------------------------------------------------------------------------*/

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

extern Label_list label_list; /*defined in first_pass*/ /*symbole table*/
/* ---------------------------global variables for machine_list----------------------------------------*/ 

Machine_word_list IC_machine_word_list = {NULL,NULL}; 
Machine_word_list DC_machine_word_list = {NULL,NULL}; 

/*-------------------------------------------------------------------------------------------*/
/*partition of all kinds of the machine word*/
int FORMAT_INSTRUCTION[] = {2,2,4,2,2,2}; /*partition of instruction_word into segments*/
int FORMAT_OPERAND[] = {12,2}; /*partition of oprand_word into segments*/
int FORMAT_REGISTER[] = {6,6,2}; /*partition of register_word into segments*/
int FORMAT_DATUM[] = {MACHINE_WORD_SIZE}; /*partition of datum_word into segments*/

/*length of partition vectors*/
int SIZE_FORMAT_OPR =sizeof(FORMAT_OPERAND)/sizeof(int);      /*length of FORMAT_OPERAND array*/
int SIZE_FORMAT_REG =sizeof(FORMAT_REGISTER)/sizeof(int);     /*length of FORMAT_REGISTER array*/
int SIZE_FORMAT_INS =sizeof(FORMAT_INSTRUCTION)/sizeof(int); /*length of FORMAT_INSTRUCTION array*/
int SIZE_FORMAT_DAT =sizeof(FORMAT_DATUM)/sizeof(int); /*length of FORMAT_DATUM array*/

/*------------------------------------------------------------------------------------------*/

/* functions */

/*-------------------------------------------------------------------------*/
/*returns data type of a command in line (the input string). uses DataTable structure*/
int get_data_type(char *string) {

	char command[ROWSIZE]; /*={"/0"};*/
	char word[ROWSIZE]; /*={"/0"} ;*/	
	int i=NUM_OF_DATA_TYPES;

	strcpy(command,string);
	  
   sscanf(command, "%s", word); /* read the first word*/
		
	for (i=0 ; i< NUM_OF_DATA_TYPES; i++) 
			if(strcmp(word,DataTable[i])==0) 
				break;		
	return i;

}

/*--------------------------------------------------------------------------*/
/*returns decimal opcode of a command line  (the input string). uses OperationsTable structure */

int get_opcode(char *string) {

	char command[ROWSIZE]; /*={"/0"};*/
	char word[ROWSIZE]; /*={"/0"} ;	*/
	int i=NUM_OF_OPERATIONS;

	strcpy(command,string);
	  
   sscanf(command, "%s", word); /* read the first word*/
		
	for (i=0 ; i< NUM_OF_OPERATIONS; i++) 
			if(strcmp(word,OperationsTable[i].opName)==0) 
				break;		
	return i;

}

/*-----------------------------------------------------------------------*/
/*returns decimal addressing method of an operands (the input token). uses RegistersTable structure */
/*0 - for immediate , 1 - for label, 3 - for register*/

int get_address(char *token) {
	
	int i;
	
	if(token[0]=='#')
		return 0;
	
	for (i=0 ; i< NUM_OF_REGISTERS; i++) {
		if(strcmp(token,RegistersTable[i])==0) 
		return 3;
	}
	
	return 1;
}

/*-----------------------------------------------------------------------*/

/* creates operands word and returns the operand addresing method: 0 - immediate , 1 - label, 3 - register*/

int create_operands_word(char *token, int operand_type , int value_instruction[] , int value_operand[] , int operation_type) {
			
	int addressing;
	int number=0;	

		
	if ((operation_type == 1 || operation_type == 2)) 
		addressing = (operand_type == 1) ? value_instruction[source] : value_instruction[dest];
			
	else if (operation_type == 3)
		addressing = (operand_type == 1) ? value_instruction[param1] : value_instruction[param2];
		
			
	if (addressing == 0) {    /* immediate operand */
		number = strtol(token + 1, NULL, 10);
		value_operand[value_o] = number;
		value_operand[era_o] = 0;	
		
	
		return 0;
	}
	
	else if (addressing == 3) {  /* register operand */
		number = strtol(token + 1, NULL, 10);
	
		if (operand_type == 1){
    		value_operand[source_r] = number;
    		value_operand[dest_r] = 0;
    	}
		else {
			value_operand[source_r] = 0;
 		   value_operand[dest_r] = number;
		}
		
		value_operand[era_r]=0;	
		

		return 3;
	}
			
	value_operand[value_o] = get_label_address(token); /*get label address*/
	value_operand[era_o] = get_label_ER(token); /*external , relocated*/
		
	/*printf("value_operand[value_o]=%d,value_operand[era_o]=%d, token=%s , get_label_ER(token)=%d\n" ,
		value_operand[value_o],value_operand[era_o],token, get_label_ER(token));*/
	return 1;
	

} 
/*-----------------------------------------------------------------------*/

/*returns the memory address of a label from  the symbol table (label_list)*/

int get_label_address (char *token) {

	Label_node* current;
	int number=0;
	
	current = label_list.head;
		 while (current != NULL) 
		{
			if (strncmp(token, current->data.name, strlen(current->data.name)) == 0)
				number = current->data.address;
							
			current=current->next;
		}
		
		return number;
}
					
/*-----------------------------------------------------------------------*/

/* returns the ER type of a label - 1 if label is extern and 2 if label is relocatable*/

int get_label_ER (char *token) {

	Label_node* current;
	int type_label=0;
	
	current = label_list.head;
		 while (current != NULL) 
		{
			if (strncmp(token, current->data.name, strlen(current->data.name)) == 0)
			{				
				if (current->data.is_extern == 1) {
					type_label = 1;
					}
					
				if (current->data.is_relocatable == 1){
					type_label = 2;
				}
			}
			current=current->next;
		}
		
		return type_label;
}

					
/*-----------------------------------------------------------------------*/

/* This function prints binary representation according to the machine syntax  */

int print_machine_word( int memory_address , char *token , int token_is_label , int val[] , int format_word[] , int size_of_format_segments ,Machine_word_list * machine_word_list) {
	
	char bits[MACHINE_WORD_SIZE+1];
	unsigned int mask;
	int i,j,k;
	int a;
	
	Machine_word curr_machine_word;
	
	i=0;
	
	for (j=0 ; j < size_of_format_segments ; j++) {
		
		a = val[j];
		k = format_word[j];
		
		mask = 1 << (k-1); 		/*set initial mask to 100... according to the size of the corresponding segment (size of bits) */
	
		while(mask){	
		
			if((a&mask)==0)		/*Then print 1 or 0 bit accordigly */
				bits[i]='.';		
			else 
				bits[i]='/';
			
			mask>>=1;		/*Update mask by moving one step to the right and Check again until mask=0  */
			i++;
		}	
	}
	
			bits[MACHINE_WORD_SIZE]='\0';	
			
			/*printf("%d	%s\n", memory_address, bits , token);*/ 
			
			curr_machine_word= create_machine_word(bits, memory_address , token , token_is_label);
			add_machine_word_to_list(curr_machine_word, machine_word_list);	
					
	return 1;

}


/*------------------------------------machine word list --------------------------------------*/
 
 /*print the machine_word_list on the screen*/

void print_machine_words_list(Machine_word_list * machine_word_list) 
{
    Machine_word_node* current = machine_word_list->head;
    while (current != NULL) 
	{
        printf("word is: %s  Address: %d	token is: %s		label: %s 			is_extern: %d\n",
        current->data.machine_word, current->data.address , current->data.token, current->data.label 
        , current->data.is_extern);
        current = current->next;
    }

}


/*-----------------------------------machine word list --------------------------------------*/

/* create_machine_word node - fill it with data */

Machine_word create_machine_word(char bits [], int memory_address , char* token , int token_is_label )
{
	Machine_word temp_word; 
	
	if (token_is_label){
		strcpy (temp_word.label , token);
		temp_word.is_extern = (get_label_ER(token) == 1);
	}
	
	else {
		temp_word.label[0]='\0';
		temp_word.is_extern=0;
		
	}
				
	strcpy (temp_word.token , token); 
	
	strcpy (temp_word.machine_word , bits);
	temp_word.address = memory_address; 
	

	return temp_word;
}

/*-----------------------------------machine word list --------------------------------------*/

/* add the input machine word node to the list */

void add_machine_word_to_list (Machine_word machine_word , Machine_word_list * machine_word_list)
{
	/* Allocate memory for a new node */
	Machine_word_node * new_node = (Machine_word_node*)malloc(sizeof(Machine_word_node));
	alloc_message(new_node) /*Check if memory allocation was successful - otherwise exit program */
	
	new_node->data = machine_word;
	new_node->next = NULL; 	
	
    /* If the list is empty, set the head and tail pointers to the new node */
    if (machine_word_list->head == NULL) 
	{
        machine_word_list->head = new_node;
        machine_word_list->tail = new_node;
    }	
	else
	{
		machine_word_list->tail->next = new_node;
        machine_word_list->tail = new_node;
	}	
}
  
/*----------------------------------------------------------------------------------------------*/

/* free memory of machine_word_list */

void free_machine_word_list(Machine_word_list * list) 
{
    Machine_word_node* current = list->head;
    Machine_word_node *temp;
 
    while (current != NULL) 
	{
		temp = current;
		current = current->next;
		free(temp);
    }
    list->head = NULL;
    list->tail = NULL; 
}

/*----------------------------------------------------------------------------------------------*/ 

/*returns an array of words that contains a sentence tokenization. separators are  " ()\t\n\"*/ 

char** tokenizer(char* sentence) {
    char** tokens;  
    char* token;
    int i = 0, size = ROWSIZE;
    char** temp;
    
    tokens = (char**) malloc(ROWSIZE * sizeof(char*)); 
    alloc_message(tokens) /*Check if memory allocation was successful - otherwise exit program */
    
  /*  char separators[] = " ()"\t\n\";*/
    
    token = strtok(sentence, " (),\t\n"); 
    while (token != NULL) {
        if (i >= size) {
            size += ROWSIZE;
            temp = (char**) realloc(tokens, size * sizeof(char*)); 
            alloc_message(temp) /*Check if memory allocation was successful - otherwise exit program */
            
            tokens = temp;
        }
        
        tokens[i] = (char*) malloc((strlen(token) + 1) * sizeof(char)); 
        alloc_message(tokens[i]) /*Check if memory allocation was successful - otherwise exit program */
        
        strcpy(tokens[i], token);
        token = strtok(NULL, " (),\t\n");
        i++;
    }
    tokens[i] = NULL; 
    return tokens;
}


/*-----------------------------------------------------------------------*/
/* main function that genarates that  code image memory.
  get the assembly command line that is "instruction statement" 
  calls utilities functions. 
  Returns the total number of words added to the instruction memory  */

int create_code_image(int IC, char *string , Machine_word_list * machine_word_list) {
 	 	
 	char command[ROWSIZE]; /*={"/0"};*/
	char** tokens ;
		
	int opcode_op , type_op;
	int value_instruction[] =	{0,0,0,0,0,0}; /*instruction word in decimal base*/
    
   int L; /*the number of machine words after translation */
   int number_of_operands=0;
		
	int i;
	
	strcpy(command,string);
	tokens = tokenizer(command); /*Tokenization of line*/	
	
	i = 0;
    while (tokens[i] != NULL) 
          i++;
   
   number_of_operands = i - 1;
   
	/*printf("number_of_operands=%d\n",number_of_operands);
   i = 0;
   while (tokens[i] != NULL) {
     printf("%s\n", tokens[i]); 
     i++;
    }
   */
   	
	/*info about instruction*/
	opcode_op = get_opcode(string);
	type_op = OperationsTable[opcode_op].opType;
	
	/*create instruction word in decimal base and print it in special binary base*/
	value_instruction[opcode]=opcode_op;
	switch (type_op) 
	{
		case(1): 
			value_instruction[dest]=get_address(tokens[1]);	/*get address method of operand 1*/				
			break;
		
		case(2): 
			value_instruction[source]=get_address(tokens[1]); /*get address method of operand 1*/
			value_instruction[dest]=get_address(tokens[2]);	/*get address method of operand 1*/
			break;	
				
		case(3):
			value_instruction[dest]=1;	/*if one operand in the command then set address method==1*/		
 			
 			if (number_of_operands > 1) {
 				value_instruction[dest]=2;	/*if more than one operand in the command then addressing method==2 */	
				value_instruction[param1]=get_address(tokens[2]); /*get address method of operand 3*/
				value_instruction[param2]=get_address(tokens[3]); /*get address method of operand 4*/
			}				
			break;	
	}  /* end switch*/
	
	
	print_machine_word(IC, tokens[0] , 0, value_instruction, FORMAT_INSTRUCTION, SIZE_FORMAT_INS, machine_word_list);
	
	
	/*--create operand words in decimal base and print it in special binary base--*/

	L = analayze_and_print_operands (number_of_operands, IC , tokens , value_instruction , type_op ,machine_word_list);


	/*-- free memory --*/
	 i = 0;
    while (tokens[i] != NULL) {
        free(tokens[i]); 
          i++;
    }  
    free(tokens); 

	return L; /*final number of machine words */
}


/*---------------------------------------------------------------------------*/
/* get all tokens of the assembly command line that is "instruction statement" and  analayzes it's operands.
   returns the total number of operands and insert their machine - words into the 
   memory image list.
 */

int analayze_and_print_operands (int number_of_operands, int IC , char **tokens , int value_instruction[] , int type_op ,Machine_word_list * machine_word_list) {

	int **value_operand;	
	int op1_is_register=0;
	int op2_is_register=0;
	int op1_is_label=0;
	int op2_is_label=0;	
	int L; /*number of words after translation*/	
	int i;
	int rslt=0; /* addressing method of an operan (result value of "create_operands_word()")*/

	/*allocate memory*/
	   if (number_of_operands > 0 ) {
   
		value_operand = (int **) malloc (number_of_operands * sizeof(int *));
		alloc_message(value_operand) /*Check if memory allocation was successful - otherwise exit program */
		          
		for (i=0 ; i < number_of_operands ; i++){
			value_operand[i] = (int *) malloc (MACHINE_WORD_SIZE * sizeof(int)); 
			alloc_message(value_operand[i]) /*Check if memory allocation was successful - otherwise exit program */
		}
	}


	switch (type_op)  /* type of the assembly operation command*/
	{
		case(0): /* no operand type */
			L=1; 
			break;
			
		case(1): 	/* 1 operand type */	
				rslt = create_operands_word(tokens[1], 2 , value_instruction , value_operand[0] , type_op) ;
				op1_is_register = (rslt == 3);
				op1_is_label = (rslt == 1);
			
			if (op1_is_register == 1) 
			 	print_machine_word(IC+1, tokens[1], op1_is_label, value_operand[0], FORMAT_REGISTER, SIZE_FORMAT_REG , machine_word_list);
			else if (op1_is_label == 1)
				print_machine_word(IC+1, tokens[1], op1_is_label, value_operand[0], FORMAT_OPERAND, SIZE_FORMAT_OPR , machine_word_list);
			else
				print_machine_word(IC+1, tokens[1], op1_is_label, value_operand[0], FORMAT_OPERAND, SIZE_FORMAT_OPR , machine_word_list);
				
			L=2;				
			break;
		
		case(2): /* 2 operand type*/
			rslt = create_operands_word(tokens[1], 1 , value_instruction , value_operand[0] , type_op) ;
			op1_is_register = (rslt == 3);
			op1_is_label = (rslt == 1);
			
			rslt = create_operands_word(tokens[2], 2 , value_instruction , value_operand[1] , type_op);
			op2_is_register = (rslt == 3);
			op2_is_label = (rslt == 1);
		
			if ( (op1_is_register + op2_is_register) == 2){  

				for (i=0 ; i < SIZE_FORMAT_REG ; i++)
					value_operand[0][i] = value_operand[0][i] + value_operand[1][i];
						
					print_machine_word(IC+1, strcat(tokens[1],tokens[2]) ,0 , value_operand[0], FORMAT_REGISTER, SIZE_FORMAT_REG ,  machine_word_list);
				L=2;
		
			}
					
			else {
				 if (op1_is_register == 1) 
				  	print_machine_word(IC+1, tokens[1], op1_is_label, value_operand[0], FORMAT_REGISTER, SIZE_FORMAT_REG , machine_word_list);
				 else 
				 	print_machine_word(IC+1, tokens[1], op1_is_label, value_operand[0], FORMAT_OPERAND, SIZE_FORMAT_OPR , machine_word_list);
								 	
				 if (op2_is_register == 1) 
				  	print_machine_word(IC+2, tokens[2], op2_is_label, value_operand[1], FORMAT_REGISTER, SIZE_FORMAT_REG , machine_word_list);
				 else 
				 	print_machine_word(IC+2, tokens[2], op2_is_label, value_operand[1], FORMAT_OPERAND, SIZE_FORMAT_OPR , machine_word_list);
				 L=3;
			}
					
			break;	
				
		case(3): /* 3 operand type*/
		
			create_operands_word(tokens[1], 2 , value_instruction , value_operand[0] , 1) ;
			print_machine_word(IC+1, tokens[1], 1, value_operand[0], FORMAT_OPERAND, SIZE_FORMAT_OPR , machine_word_list);	
			L=2;
			
			if (number_of_operands > 1) {

				rslt = create_operands_word(tokens[2], 1 , value_instruction , value_operand[1] , type_op) ;
				op1_is_register = (rslt == 3);
				op1_is_label = (rslt == 1);
				
				rslt = create_operands_word(tokens[3], 2 , value_instruction , value_operand[2] , type_op);
				op2_is_register = (rslt == 3);
				op2_is_label = (rslt == 1);
				
	
				if ( (op1_is_register + op2_is_register) == 2){

					for (i=0 ; i < SIZE_FORMAT_REG ; i++)
						value_operand[1][i] = value_operand[1][i] + value_operand[2][i];
						
					print_machine_word(IC+2, strcat(tokens[2],tokens[3]) ,0 , value_operand[1], FORMAT_REGISTER, SIZE_FORMAT_REG , machine_word_list);
					L=3;
		
				}
					
				else {
					 if (op1_is_register == 1) 
					  	print_machine_word(IC+2, tokens[2], op1_is_label, value_operand[1], FORMAT_REGISTER, SIZE_FORMAT_REG , machine_word_list);
					 else 
					 	print_machine_word(IC+2, tokens[2], op1_is_label, value_operand[1], FORMAT_OPERAND, SIZE_FORMAT_OPR , machine_word_list);
									 	
					 if (op2_is_register == 1) 
					  	print_machine_word(IC+3, tokens[3], op1_is_label, value_operand[2], FORMAT_REGISTER, SIZE_FORMAT_REG , machine_word_list);
					 else 
					 	print_machine_word(IC+3, tokens[3], op1_is_label, value_operand[2], FORMAT_OPERAND, SIZE_FORMAT_OPR , machine_word_list);
					 L=4;
				}
			}		
			break;
	
	}
	
	   if (number_of_operands > 0 ) {
		 for (i=0 ; i < number_of_operands; i++)
		    free(value_operand[i]); 
	 
		 free(value_operand); 
	 }
	 
	return L; /*final number of machine words */
}

/*---------------------------------------------------------------------------*/

/* main function that genarates that  data image memory.
   Get the assembly command line that is "data statement"
   Calls utilities functions. 
   Returns the total number of words added to the instruction memory 
  
  input: 
  - the next availble addres in the data memory image
  - the line to be translate to machine words (a line from the ".am" file) . must be "data statement"
*/
  
int create_code_data(int DC , char *string  ,Machine_word_list * machine_word_list) {
	
	char command[ROWSIZE]; /*={"/0"}*/
	char** tokens ;
	
	int data_type = get_data_type(string);
		   
   int L=0; /*the number of machine words after translation */
	
	int value_data;
	
	int i;
	
	char w[2]; /**/
		
	strcpy(command,string);
	tokens = tokenizer(command); /*Tokenization of line*/	
	
	i = 0;
    while (tokens[i] != NULL) 
          i++;
  
	
	/*create machine word and print it in special binary base*/
	
	switch (data_type) {
	
		case(0): /*.data*/
			i = 1;
			while (tokens[i] != NULL) {
				value_data = atoi(tokens[i]);
				/*printf("tokens[i]=%s , value_data=%d\n" ,tokens[i],value_data);*/
				print_machine_word(DC , tokens[i], 0 , &value_data , FORMAT_DATUM , SIZE_FORMAT_DAT , machine_word_list);
				i++;
				L++;
				DC++;
			 }
			break;
			
		case(1): /*string*/
 			
			for (i = 1 ; i < strlen(tokens[1]) - 1 ; i++ ) {
				value_data = (int)(tokens[1][i]); /*ascii_code */
				/*printf("tokens[1][i]=%c , value_data=%d\n" ,tokens[1][i],value_data);*/
				w[0]=tokens[1][i]; w[1]='\0';
				print_machine_word(DC , w , 0 , &value_data , FORMAT_DATUM , SIZE_FORMAT_DAT , machine_word_list );
				L++;
				DC++;
			 }
			 
			 
			 value_data=0;
			print_machine_word(DC , "\0" , 0, &value_data , FORMAT_DATUM , SIZE_FORMAT_DAT , machine_word_list);
				L++;
				DC++;				
			break;
	  
	}
    
	/*-- free memory --*/
	 i = 0;
    while (tokens[i] != NULL) {
        free(tokens[i]); 
          i++;
    }  
    free(tokens); 

	return L; /*final number of machine words */
}

/*---------------------------------------------------------------------------*/





