
/* header of program IC_ID_images.c  */

/*The header contains the information about the machine structure that is essential for machine syntax translation */

/*-----------------------------------------------------------------------------------------------------------*/
	
/*------- syntax of all kinds of possible machine words   ---------------------------------------------------*/
typedef enum  {param1 , param2 ,opcode , source , dest ,era } instruction_word;
typedef enum  {value_o ,era_o } oprand_word;
typedef enum  {source_r ,dest_r , era_r} register_word;
typedef enum  {datum} data_word;

/*---------------------------------structs for machine_word--------------------------------------------------*/

typedef struct Machine_word{
    char machine_word [MACHINE_WORD_SIZE]; /* machine word in special binary base */
    int  address; /* the address of the machine word in the memory image */	
    char label[MAX_LABEL]; /* the lable name (if the word represents an address of a label). otherwise it's empty*/
    int  is_extern ; /* type of the label 1 = label is extern , 0 = otherwise*/
    char token[MAX_LABEL];  /*Not necessary to the process - makes it easier to associate the token with its machine word*/
} Machine_word;

typedef struct Machine_word_node{
	Machine_word data; 
	struct Machine_word_node * next;
} Machine_word_node; 

typedef struct Machine_word_list {
    Machine_word_node * head;         /* Pointer to the first node */
    Machine_word_node * tail;         /* Pointer to the last node */
} Machine_word_list;

/* -----------------------------------functions----------------------------------*/
int get_data_type(char *); /*returns data type of a command in line (the input string). uses DataTable structure*/
int get_opcode(char *); /*returns decimal opcode of a command line  (the input string). uses OperationsTable structure */
int get_address(char * ); /*returns decimal addressing method of an operands (the input token). uses RegistersTable structure. 0 - for immediate , 1 - for label, 3 - for register*/

int get_label_address (char *); /*returns the memory address of a label from  the symbol table (label_list)*/
int get_label_ER (char *); /*returns the ER type of a label - 1 if label is extern (data.is_extern == 1) and 2 if label is relocatable (data.is_relocatable == 1). uses the symbol table (label_list)*/

char** tokenizer(char *);/*returns an array of words that contains a sentence tokenization. separators are  " ()\t\n\"*/ 
int create_operands_word(char *, int , int [] , int [], int); /*create machin word in decimal base for an operand - see below*/
int print_machine_word( int , char * , int , int[], int[], int  ,Machine_word_list * ); /* gives the binary representation of a token according to the machine syntax - see below*/

int analayze_and_print_operands (int, int, char ** , int [] , int  ,Machine_word_list *  ); /*operands alnalayzer - see below*/

int create_code_image(int IC, char * , Machine_word_list * ); /* create the code image memory - see below*/
int create_code_data(int DC , char *  , Machine_word_list * );	/* create the data image memory - see below*/

/*--------------------machine_word_list_functions--------------------------------------------*/ 

Machine_word   create_machine_word(char [] , int , char* , int); /* create_machine_word node - fill it with data */
void add_machine_word_to_list (Machine_word  , Machine_word_list * ); /* add the input machine word node to the list */
void print_machine_words_list(Machine_word_list * ); /*print the machine_word_list on the screen*/
void free_machine_word_list(Machine_word_list * ) ; /* free memory of machine_word_list */

/****************************** More details **************************************************************************************/
/* int create_operands_word(char *token, int operand_type , int value_instruction[] , int value_operand[] , int operation_type)*/

/* reate machin word in decimal base for an operand and returns the operand addresing method.
	inputs:
	- the operand (as string)
	- operand_type (the operand rank : the first/second/third operand (1,2,3) )
	- the machine word of the operation (assembly command) in decimal structure
	- the machine word of the operation (initial value = (0,0) ot (0))
	- the operation_type :(opType) potential number of operands : 0 - no operand ,	1 , 2  , 3 operands 
	
	output:
	- operand addresing method : 	0 - for immediate , 1 - for label, 3 - for register
	- int value_operand[]
	
	
	*/
/*--------------------------------------------------------------------------------------------------------------------------------*/

/*int print_machine_word( int memory_address , char *token , int token_is_label , int val[] , int format_word[] , int size_of_format_segments ,Machine_word_list * machine_word_list)*/ 

/* gives the binary representation of a token according to the machine syntax and insert it and more info to the machine-word-list .
	calls curr_machine_word function to insert data to a  machin_word_list node 
	calls add_machine_word_to_list to add the node to the list.
	
	input:
	- memory_address 
	- token
	- token_is_label : 1 if the token is a label , 0 therwise
	- the machine word in decimal structure
	- the suitable format of the machine word in decimal structure
	- the total number of the format segments
	- machine_word_list to be added to
	
	output: updated machine_word_list (One word has been added to the list).	
	*/
/*--------------------------------------------------------------------------------------------------------------------------------*/
/* int analayze_and_print_operands (int number_of_operands, int IC , char **tokens , int value_instruction[] , int type_op ,Machine_word_list * machine_word_list)*/

/*  get all tokens of the assembly command line that is "instruction statement" and  analayzes it's operands.
   returns the total number of operands and insert their machine - words into the 
   memory image list.
	
	calls functions : create_operands_word , print_machine_word 
	
	input:
	- the total number of operands in the command
	- the next availble addres in the code memory image
	- the commands array of words
	- the  machin-word of the instruction in decimal base 
	- the type of the assebmly operation
	- the Machine_word_list structure that holds the machine words to be added to (code image memory  list)
	
	output:
	- total number of operands
	- updated machine_word_list 	   
*/

/*------------------------------------------------------------------------------------------------------------------------------------*/
/* int create_code_image(int IC, char *string , Machine_word_list * machine_word_list) */

/* main function that genarates that  code image memory.
   Get the assembly command line that is "instruction statement"
   Calls utilities functions. 
   Returns the total number of words added to the instruction memory 
  
  input: 
  - the next availble addres in the code memory image
  - the line to be translate to machine words (a line from the ".am" file) . must be "instruction statement"*/

/*------------------------------------------------------------------------------------------------------------------------------------*/
/* int create_code_data(int DC , char *string  ,Machine_word_list * machine_word_list) */

/* main function that genarates that  data image memory.
   Get the assembly command line that is "data statement"
   Calls utilities functions. 
   Returns the total number of words added to the instruction memory 
  
  input: 
  - the next availble addres in the data memory image
  - the line to be translate to machine words (a line from the ".am" file) . must be "data statement"
*/
  





	
