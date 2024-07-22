/*   utilities for first_pass.c     */
/*----------------------------------------------*/

#define NO_ADD -1 

/*---------------------------structs fot label and label list-----------------------------------------*/
typedef struct Label{
    	char name [MAX_LABEL];
    	int is_relocatable; /* 1 = true, 0 = false */
   		int is_extern; /* 1 = true, 0 = false */
		int is_entry; /* 1 = true, 0 = false */
    	int address; /* The address of the label in the memory image */
		int is_data_or_string; /* 1 = true, 0 = false */
} Label;


typedef struct Label_node{
	Label data; 
	struct Label_node * next;
} Label_node; 


typedef struct Label_list {
    Label_node * head;         /* Pointer to the first node */
    Label_node * tail;         /* Pointer to the last node */
} Label_list;

/* -------------------------------------------------------------------------------*/ 


int first_pass(char *); /* main program of first pass */ 


/* ----------------------------list functions---------------------------------------------*/ 


void printLabelList(Label_list* list); /* The function get label list, and print it */ 
	
void free_label_list(Label_list* list); /* The function get label list, and frees it */



/* ---------------------------------handler functions-----------------------------------------*/ 

void label_handler(char* ptr, char* label, Label_list * label_list, int* DC, int* IC, int nrow);
/* the function working and create appropriate labels in all different cases */ 


void extern_handler(char * ptr, Label_list * label_list);
/* the function working on cases of .extern as an opening of the sentence */ 

void entry_handler(char * ptr, Label_list * label_list);
/* the function working on cases of .entry as an opening of the sentence */


/*------------------------------functions-----------------------*/

 

int is_valid_label(char * label, int nrow, Label_list * label_list);

/* 

 the function returns if label is valid 
	input : string and number of row. 
	the function checks: 

	1. label is longer than 30. 
	2. label starts with letter
	3. label include only letters and numbers. 
	4. label is not an key word 
	output: 1 - if label is valid
		0 - if not.  */ 
  

/* -------------------------------------------------------------------------------*/ 

int is_only_numbers_and_letters(char * );

/*  the function returns if there is only letters and numbers in the string 

	input paramater - first word from a string
	output: 
		return 1- true. 
		return 0 -false. */ 

/* -------------------------------------------------------------------------------*/ 


int has_colon(char * first_word); /* the function checks if the word ends with colon  , as apotenitally label 

	input paramater - first word from a string
	output: 
			1- potentiallt label
			0- not potentiallt label */ 

/* -------------------------------------------------------------------------------------*/ 

int is_label_key(char *keys[], int sizeofkeys , char token[]);

/* the function check if lable is a key word 

	input parameters: keys(oprentionTable, registers, ReferenceTable, DataTable) size of array, string)

	output: 1- if it's key word. 
			0 - if not. */
/* -------------------------------------------------------------------------------------*/ 


int get_type_of_first_word(char* ptr);

/* the function check if lable is a key word 

	input parameter: pointer ptr (the first word)

	output: appropriate numner according to the case : 
		1-  entry, 2 - extern, 3 - data, 4 - string, 
		5,6,7,8,11 - opreation type 2 = 2 opernds. 
		9,10,12,13,16,17 - opration type 1 - 1 operand. 
		14,15,18 - operation type 3 - 3 operand
		19,20 - type 0 - no operands 
		21 - comment ore empty line 
		22 - other case , label or ilegal word */ 		

/*--------------------------------------------------------------------------------------------------------*/


int is_comment_line(char* ptr); /* check if the line is comment line : 1-true, 0-false */ 


/*---------------------------------------------------------------------------------*/ 


int is_empty_line(char* ptr); /* check if the line is empty line : 1-true, 0-false */ 


/*-------------------------------------------------------------------------------------------------------*/ 

int is_entry (char * first_word); /* the function check if the first word is entry : 1-true, 0-false */ 

/* --------------------------------------------------------------------------------*/

int is_extern (char * first_word); /* the function check if the first word is extern : 1-true, 0-false */ 

/* -------------------------------------------------------------------------------------*/


char * remove_last_char(char * );

/* the function get a label with ':' 

	the function return - the name of the label. */ 

/*-----------------------------------------------------------------------------*/ 


Label create_label (char * label,int flag_is_relocatable, int flag_is_extern, int flag_is_entry, int address, int flag_is_data_or_string);
 
/* the function create a new label according the parameters it gets */ 


/* ---------------------------------------------------------------------------------------------------------*/ 

int is_data_or_string (char * ptr); /* the function check if the first word is data or string : 1-true, 0-false */

/*-----------------------------------------------------------------------------------------------------*/

int is_data(char *ptr); /* the function check if the first word is data */

/*-----------------------------------------------------------------------------------------------------*/

int is_string(char *ptr); /* the function check if the first word is string */

/*-----------------------------------------------------------------------------------------------------*/


int label_is_in_list (char * label, Label_list * label_list); /* the function check if the label is already exist */


/*-----------------------------------------------------------------------------------------------------*/

void add_label_to_list (Label label, Label_list * label_list);  /*  the function create lebel and add it to the list */ 


/*-----------------------------------------------------------------------------------------------------*/

 



