#include "utilities.h" /* information about machine */
#include "IC_DC_images.h" /* functions that translate assembly according to the machine word syntax and creates the machie-word-list (the memory images of instruction and data) */
#include "first_pass.h"  /* functions of the first pass phase that create the symbol table*/
#include "syntax_validation.h" /*provides assembly code syntax validation services for the first pass*/
/* ---------------------------------------------------------------------------------*/ 
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


extern Machine_word_list DC_machine_word_list;
extern Machine_word_list IC_machine_word_list;
/* extern Machine_word curr_machine_word; */ 



/*---------------------------------------------------------------------------*/

int flag_firstpass; /*called by assembler.c*/ /*counts of errors*/
Label_list label_list = {NULL, NULL};  /*symbole tabel*/ /*called by IC_DC_images*/
int NUM_CODE_WORDS; /* the number of code machine words*/ /*called by print_outputs*/
int NUM_DATA_WORDS; /* the number of code data words*/ /*called by print_outputs*/


/*-------------------------------------------------------------------------------------------*/
char * label;
Label curr_label;
int nrow=0; /* global variable, using to present in which line there was a problem */ 

/*-------------------------------------------------------------------------------------------*/

/* IC = IC + create_code_image(IC, line); */
/* DC = DC + create_code_data(DC, line); */

int first_pass(char *fname)
{
	
	char fname1[MAX_FILE_NAME];  /* name of sorce file ".am" */
	FILE *fd1;   /* fd1:source */
	char delim[] = " \t\r\n";
	char line[ROWSIZE]; /* a string containing a line from the source file */
	char temp_line[ROWSIZE];
	char * ptr;
	int type_of_first_word;
	char * maybe_label;
	int num_of_words;
	int IC=100; int DC=0; 
	Label_node * current;
	
	/*int L=0;*/
	nrow = 0;
		  
	/*----- open sorce file ------*/
	strcpy(fname1,fname);
	fd1 = fopen(strcat(fname1,".am"), "rt"); /*sorce after macro replacement*/
	validFile(fd1); /*check if file is exist and not empty*/
	
	flag_firstpass = 0; /* represent - number of errors in the procedure of first_pas */  

	while (fgets(line, ROWSIZE, fd1) != NULL) 
	{
		nrow++;
		strcpy(temp_line, line);
		ptr = line;
	    while (isspace(*ptr)) 
		{
            ptr++; /* pointer to the first word */ 
        }
		type_of_first_word = get_type_of_first_word(ptr);

        while (isspace(*ptr)) 
		{
            ptr++; /* pointer to the first word */ 
        }

		switch(type_of_first_word) 
		{
			case 1: /* .entry */
				ptr = ptr + strlen(".entry");
				entry_handler(ptr, &label_list); 
				break;

			case 2: /* .extern */
				ptr = ptr + strlen(".extern");
				extern_handler(ptr, &label_list);
				break;

			case 3: /* .data */	
				if(is_data_valid(ptr, nrow))
				{
					DC = DC + create_code_data(DC, ptr, &DC_machine_word_list);	
				}		
				break;

			case 4: /* .string */
				if (is_string_valid(ptr, nrow))
				{
					DC = DC + create_code_data(DC, ptr, &DC_machine_word_list);
				}
				break;

			case 19: case 20: /* op type 0 - no operands after command */
				if (count_number_of_words(ptr) == 1)
				{
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);
				}
				else /* there is "something" after command with no opreands */ 
				{
					flag_firstpass++;
					fprintf(stderr,"Error: in line %d , extra char after no operand command\n", nrow);
				}
				break;

			case 9: case 10: case 12: case 13: case 16: case 17: /* op type 1 - 1 operand after command */
				num_of_words = count_number_of_words(ptr);
				if (num_of_words == 2)
				{
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);
				}
				else if(num_of_words == 1) /* only the operation with no operands */ 
				{
					flag_firstpass++;
					fprintf(stderr,"Error: in line %d , no operand after command\n", nrow);
				}
				else if (num_of_words > 2) /* more than 1 operand */ 
				{
					flag_firstpass++;
					fprintf(stderr,"Error: in line %d , more than one operand after command\n", nrow);
				}
				break;

			case 5: case 6: case 7: case 8: case 11: /* op type 2 - 2 operands after command */
				num_of_words = is_two_operands(ptr); /* gets number of operands */ 
				if (num_of_words == 2)
				{
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);
				}			
				else if (num_of_words < 2) 
				{
					flag_firstpass++;
					fprintf(stderr,"Error: in line %d , less than two operands after command\n", nrow);				
				}
				else if (num_of_words > 2)
				{
					flag_firstpass++;
					fprintf(stderr,"Error: in line %d , more than two operands after command\n", nrow);				
				}
				break;

			case 14: case 15: case 18: /* op type 3 - 3 operands after command */
				num_of_words = count_number_of_words(ptr); /* need to be 2, in this case spaces are not allowed */ 
				if (num_of_words == 2)
				{
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);
				}
				else if (num_of_words < 2)
				{
					flag_firstpass++;
					fprintf(stderr,"Error: in line %d , no operand after command\n", nrow);				
				}
				else if (num_of_words > 2)
				{
					flag_firstpass++;
					fprintf(stderr,"Error: in line %d , more than one operand after command\n", nrow);				
				}
				break;

			case 21: /* comment or empty line - no message needed, just move on */
				break;

			case 22: /* checks potentiialy 'maybe label' */
				
				maybe_label = strtok(temp_line, delim);
				ptr = ptr + strlen(maybe_label);
				
				if (!has_colon(maybe_label)) /* the word is not like 'word:' - so it's ileggal sentence */ 
				{
					flag_firstpass++;					
					fprintf(stderr, "Error: in line %d , sentence is not legal\n", nrow); 
		        }

				else
				{
					label = remove_last_char(maybe_label); /* remove ':' , to sace the name od the label */ 
					if (is_valid_label(label, nrow, &label_list)) 
					{	
						label_handler(ptr, label, &label_list, &IC, &DC, nrow);
					}
				}
		} /* end switch */
	
					 
	}/* end while */

	/*printf("--------------first pass-----------------\n");*/

	/*printf("-------------- IC machine_word_list-----------------\n");*/
	/*print_machine_words_list(&IC_machine_word_list);*/
	
	/*printf("-------------- DC machine_word_list-----------------\n"); */
	/*print_machine_words_list(&DC_machine_word_list); */
	
	/*printf("-------------- label_list-----------------\n"); */
	/*printLabelList(&label_list);*/

	fclose(fd1); /* close file */ 

	current = label_list.head; 
    while (current != NULL) 
	{
		if(current->data.is_data_or_string == 1) 
		{
			current->data.address = current->data.address + IC; /* update the relevant address after all the lines */
		}	
		current = current->next;
	}
  /*  printf("--------------label list after change-----------------\n"); */
  /* printLabelList(&label_list); */
	
	return 1;
}	
			
/*-------------------------------------Functions -------------------------------------------*/ 
/*-------------------------------------------------------------------------------------------*/
/*free linked list - label list */

void free_label_list(Label_list* list) {

    Label_node* curr = list->head;
    Label_node* temp;
    
	while (curr != NULL) 
	{
        temp = curr; 
		curr = curr->next;
        free(temp); /* free the name string inside the Label struct*/
    }
    list->head = NULL;
	list->tail = NULL; /* free the Label_list itself*/
}


/* ------------------------------------------------------------------------------------------------*/
/*****************************/
int get_type_of_first_word(char* ptr)
{
	int i;
	char temp [ROWSIZE];
	if(strncmp(ptr, ".entry ", strlen(".entry ")) == 0)
	{
		return 1;
	}
	if(strncmp(ptr, ".extern ", strlen(".extern ")) == 0)
	{
		return 2;
	}
	if(strncmp(ptr, ".data ", strlen(".data ")) == 0)
	{
		return 3;
	}
	if(strncmp(ptr, ".string ", strlen(".string ")) == 0)
	{
		return 4;
	}
	if(strncmp(ptr, OperationsTable[14].opName, strlen(OperationsTable[14].opName)) == 0)
	{
		return 19;
	}
	if(strncmp(ptr, OperationsTable[15].opName, strlen(OperationsTable[15].opName)) == 0)
	{
		return 20;
	}
	for (i = 0; i < NUM_OF_OPERATIONS; i++) 
	{
		strcpy(temp,OperationsTable[i].opName);
		strcat(temp, " "); /* now temp is command + " " */ 	
		if (strncmp(temp, ptr, strlen(OperationsTable[i].opName+1)) == 0)
	 	{
            return i+5; /* return the command number + 5 */
      	}
 	}
	if(is_empty_line(ptr) || is_comment_line(ptr))
	{
		return 21;
	}
	else /* first word is (maybe) label */
	{
		return 22;
	}
}

/*-------------------------------------------------------------------------------------------*/

void entry_handler(char* ptr, Label_list * label_list)
{
	int i;
	int counter;
	char *commands[16];
	
	while (isspace(*ptr))
	{
		ptr++; /* the pointer is on the second word */
	}

	counter = count_number_of_words(ptr);
	switch(counter) 
	{
		case 0:
			flag_firstpass++; 
			fprintf(stderr,"Error: in line %d , there is no label after .entry\n", nrow);
			break;
		case 2:
			flag_firstpass++;
			fprintf(stderr,"Error: in line %d , there is more than 1 label after .entry\n", nrow);		
			break;
		case 1:
			for (i = 0; i < NUM_OF_OPERATIONS; i++)
			{
		    	commands[i] = OperationsTable[i].opName;
			}
	
			if (is_label_key(commands, NUM_OF_OPERATIONS, ptr) ||
				is_label_key(RegistersTable, NUM_OF_REGISTERS, ptr) ||
				is_label_key(ReferenceTable , NUM_OF_REFERENCE_TYPES, ptr) || 
				is_label_key(DataTable, NUM_OF_DATA_TYPES, ptr))
			{
				flag_firstpass++;
				fprintf(stderr,"Error: in line %d , the label is a key word\n", nrow);
			}
			break;
	}
}
/*-------------------------------------------------------------------------------------------*/


void extern_handler(char * ptr, Label_list * label_list) 
{
	int i;
	char *commands[16];
	int counter;
	char * temp_ptr = ptr;

	while (isspace(*ptr)) 
	{
		ptr++; /* the pointer is on the second word */
	}

	
	counter = count_number_of_words(ptr);
	switch(counter) 
	{
		case 0:
			flag_firstpass++;
			fprintf(stderr,"Error: in line %d , there is no label after .extern\n", nrow);
			break;
		case 2:
			flag_firstpass++;
			fprintf(stderr,"Error: in line %d , there is more than 1 label after .extern\n", nrow);		
			break;
		case 1:
			for (i = 0; i < NUM_OF_OPERATIONS; i++)
			{
				commands[i] = OperationsTable[i].opName;
			}
	
	
			if (is_label_key(commands, NUM_OF_OPERATIONS, ptr) ||
				is_label_key(RegistersTable, NUM_OF_REGISTERS, ptr) ||
				is_label_key(ReferenceTable , NUM_OF_REFERENCE_TYPES, ptr) || 
				is_label_key(DataTable, NUM_OF_DATA_TYPES, ptr))
			{
				flag_firstpass++;		
				fprintf(stderr,"Error: in line %d , the label is a key word\n", nrow); 
			}
			else 
				{
					strncpy(temp_ptr, ptr, strlen(ptr));
					temp_ptr[strlen(ptr)-1]='\0';
					curr_label = create_label(temp_ptr, 0, 1, 0, 0, 0);/* relocatable 0, extern 1, entry 0, add 0, data_string 0   */
					add_label_to_list(curr_label, label_list);
				}
			break;
	}
}

/*-------------------------------------------------------------------------------------------*/

void label_handler(char* ptr, char * label, Label_list * label_list, int* IC, int* DC, int nrow)
{
	int flag_is_entry;
	int flag_is_extern;
	int flag_is_data_or_string;
	int flag_is_command;
	int num_of_command;
	int flag_is_data; 
	int	flag_is_string;
	int i;
	char *commands[16];
	char temp [ROWSIZE];

	while (isspace(*ptr)) 
	{
		ptr++; /* the pointer is on the second word */
	}

	for (i = 0; i < NUM_OF_OPERATIONS; i++)
	{
    	commands[i] = OperationsTable[i].opName;
	}

	flag_is_data_or_string = is_data_or_string(ptr); /*is second word is data or string*/
	flag_is_data = is_data(ptr);
	flag_is_string = is_string(ptr);
	flag_is_command	= is_label_key(commands, NUM_OF_OPERATIONS, ptr); /*is second word is command*/
	flag_is_entry = is_entry(ptr); /*is second word is entry*/
	flag_is_extern = is_extern(ptr); /*is second word is extern*/	

	
	if (flag_is_data_or_string)
	{
		if (flag_is_string)		
		{
			if (is_string_valid(ptr, nrow))
			{
				curr_label = create_label(label, 1, 0, 0, *DC, flag_is_data_or_string); /* relocatable 1, extern 0, entry 0, add DC, data_string 1   */
				add_label_to_list(curr_label, label_list);
				*DC = *DC + create_code_data(*DC, ptr, &DC_machine_word_list);	
			}
		}
		else if (flag_is_data)
		{
			if(is_data_valid(ptr, nrow))
			{
				curr_label = create_label(label, 1, 0, 0, *DC, flag_is_data_or_string); /* relocatable 1, extern 0, entry 0, add DC, data_string 1   */
				add_label_to_list(curr_label, label_list);
				*DC = *DC + create_code_data(*DC, ptr, &DC_machine_word_list);	
			}
		}
	}
	
	else if(flag_is_command)
	{	
		if (strncmp(ptr, OperationsTable[14].opName, strlen(OperationsTable[14].opName)) == 0 || /* rts or stop */ 
			strncmp(ptr, OperationsTable[15].opName, strlen(OperationsTable[15].opName)) == 0)
		{
			if (count_number_of_words(ptr) != 1)
			{	
				flag_firstpass++;
				fprintf(stderr,"Error: in line %d , extra char after no operand command\n", nrow);
			}
			else 
			{
				curr_label = create_label(label, 1, 0, 0, *IC, flag_is_data_or_string); /* relocatable 1, extern 0, entry 0, add IC, data_string 0   */
				add_label_to_list(curr_label, label_list);
				*IC = *IC + create_code_image(*IC,ptr, &IC_machine_word_list);
			}
		}
		else
		{
			for (i = 0; i < NUM_OF_OPERATIONS; i++) 
			{
				strcpy(temp,OperationsTable[i].opName);
				strcat(temp, " "); /* now temp is command + " " */ 	
				if (strncmp(temp, ptr, strlen(OperationsTable[i].opName+1)) == 0)
				{
					num_of_command = i; /* update the command number */
				}
			 }
			
			switch(num_of_command)
			{
				case 14: case 15: /* type 0 */
					if (count_number_of_words(ptr) != 1)
					{	
						flag_firstpass++;
						fprintf(stderr,"Error: in line %d , extra char after no operand command\n", nrow);
					}	
					else 
					{
						curr_label = create_label(label, 1, 0, 0, *IC, flag_is_data_or_string); /* relocatable 1, extern 0, entry 0, add IC, data_string 0   */
						add_label_to_list(curr_label, label_list);
						*IC = *IC + create_code_image(*IC,ptr, &IC_machine_word_list);
					}		
					break;
				case 4: case 5: case 7: case 8: case 11: case 12: /* type 1 */
					if(count_number_of_words(ptr) == 1) /* only the operation with no operands */ 
					{
						flag_firstpass++;
						fprintf(stderr,"Error: in line %d , no operand after command\n", nrow);
					}
					else if (count_number_of_words(ptr) > 2) /* more than 1 operand */ 
					{
						flag_firstpass++;
						fprintf(stderr,"Error: in line %d , more than one operand after command\n", nrow);
					}
					else 
					{
						curr_label = create_label(label, 1, 0, 0, *IC, flag_is_data_or_string); /* relocatable 1, extern 0, entry 0, add IC, data_string 0   */
						add_label_to_list(curr_label, label_list);
						*IC = *IC + create_code_image(*IC,ptr, &IC_machine_word_list);
					}		
					break;
				case 0: case 1: case 2: case 3: case 6: /* type 2 */
					if (is_two_operands(ptr) < 2) /* operands */ 
					{
						flag_firstpass++;
						fprintf(stderr,"Error: in line %d , less than two operands after command\n", nrow);				
					}
					else if (is_two_operands(ptr) > 2)
					{
						flag_firstpass++;
						fprintf(stderr,"Error: in line %d , more than two operands after command\n", nrow);				
					}
					else 
					{
						curr_label = create_label(label, 1, 0, 0, *IC, flag_is_data_or_string); /* relocatable 1, extern 0, entry 0, add IC, data_string 0   */
						add_label_to_list(curr_label, label_list);
						*IC = *IC + create_code_image(*IC,ptr, &IC_machine_word_list);
					}		
					break;
				case 9: case 10: case 13: /* type 3 */
					if (count_number_of_words(ptr) < 2)
					{
						flag_firstpass++;
						fprintf(stderr,"Error: in line %d , no operand after command\n", nrow);				
					}
					else if (count_number_of_words(ptr) > 2)
					{
						flag_firstpass++;
						fprintf(stderr,"Error: in line %d , more than one operand after command\n", nrow);				
					}
					else 
					{
						curr_label = create_label(label, 1, 0, 0, *IC, flag_is_data_or_string); /* relocatable 1, extern 0, entry 0, add IC, data_string 0   */
						add_label_to_list(curr_label, label_list);
						*IC = *IC + create_code_image(*IC,ptr, &IC_machine_word_list);
					}		
					break;  
			}
		} 
	}

	else if(flag_is_entry) /* label before entry - we ignore it */
	{
		fprintf(stderr, "Warning: in line %d , label before .entry\n", nrow);
		ptr = ptr + strlen(".entry");
		entry_handler(ptr, label_list); 
	}

	else if(flag_is_extern) /* label before extern - we ignore it */
	{
		fprintf(stderr, "Warning: in line %d , label before .extern\n", nrow);
		ptr = ptr + strlen(".extern");
		extern_handler(ptr, label_list);
	}

	else
	{
		flag_firstpass++;		
		fprintf(stderr, "Error: in line %d , after label there is no legal sentence\n", nrow);
	}
}

/*-------------------------------------------------------------------------------------------*/

void printLabelList(Label_list * list) 
{
    Label_node* current = list->head;
    while (current != NULL) 
	{
        printf("Label Name: %s\n Address: %d, is_relocatable: %d, is_extern: %d, is_entry: %d,  is_data_or_string: %d\n",
                current->data.name, current->data.address, 
                current->data.is_relocatable, current->data.is_extern, current->data.is_entry, current->data.is_data_or_string);
        current = current->next;
    }

}


/*-------------------------------------------------------------------------------------------*/ 

int has_colon(char * ptr) 
{
    char last_char = ptr[strlen(ptr)-1];
	if(last_char == ':')
	{	
		return 1;
	}	
	return 0; 
}


/* --------------------------------------------------------------------------- */

int is_valid_label(char * label, int nrow, Label_list * label_list) 
{
	int length_word; 
	int i;
	char *commands[16];
		
	length_word = strlen (label);
	if (length_word > MAX_LABEL-1)
	{
		flag_firstpass++;		
		fprintf(stderr,"Error: in line %d , label is too long\n", nrow);
		return 0;
	}

	for (i = 0; i < NUM_OF_OPERATIONS; i++)
	{
    	commands[i] = OperationsTable[i].opName;
	}

	if (is_label_key(commands, NUM_OF_OPERATIONS, label) ||
		is_label_key(RegistersTable, NUM_OF_REGISTERS, label) ||
		is_label_key(ReferenceTable , NUM_OF_REFERENCE_TYPES, label) || 
		is_label_key(DataTable, NUM_OF_DATA_TYPES, label))
	
	{ /* keyword */
		flag_firstpass++;
		fprintf(stderr,"Error: in line %d , the label is a key word\n", nrow);
		return 0; 
	}
	
	if (!isalpha(label[0])) /*ilegal opening char */ 
	{
		flag_firstpass++;		
		fprintf(stderr,"Error: in line %d , the label don't start with letter \n", nrow);
		return 0;	
	}

	if (!is_only_numbers_and_letters(label))
	{		
		flag_firstpass++;		
		fprintf(stderr,"Error: in line %d , label is not legal\n", nrow); 
		return 0;	
	}

	if (label_is_in_list (label, label_list))
	{	
		flag_firstpass++;		
		fprintf(stderr, "Error: in line %d , the label is already exist\n", nrow); 
		return 0;
	}
	
	return 1;
} 
/* --------------------------------------------------------------------	*/


int is_only_numbers_and_letters(char * label) 
{
	int i = 0; 
	while (label[i] != '\0')
	{
		if (!isalnum(label[i])) 
		{
			return 0; /* there is anl ilegal chat in it */
		}
		i++;
	}
	return 1;
}


/* -------------------------------------------------------------------------------*/ 


int is_label_key(char *keys[], int sizeofkeys , char* token) 
{

	int i;
	for (i = 0; i < sizeofkeys; i++) 
	{
		if (strncmp(keys[i], token, strlen(keys[i])) == 0)
	 	{
           return 1; /* keyword */
      	}
 	}
   
   return 0; /*not keyword*/
}	


/* ------------------------------------------------------------------------------*/ 

char * remove_last_char(char * first_word)
{
	char * label; 
	
	label = (char *) malloc (strlen(first_word));
	alloc_message(label)
	
	strncpy (label, first_word, strlen(first_word) - 1); 
	label[strlen(first_word) - 1] = '\0'; 
	
	return label; 
}



/* -----------------------------------------------------------------------------------------------------------------*/ 

int is_entry (char * ptr)
{
	if (strncmp(ptr, ".entry", strlen(".entry")) == 0)
		return 1; 
	return 0; 
}

/* -------------------------------------------------------------------------------------*/


int is_extern (char * ptr)
{
	if (strncmp (ptr, ".extern", strlen(".extern")) == 0)
		return 1;
	return 0;
}

/*-----------------------------------------------------------------------------------------------------------*/

int is_data_or_string (char * ptr)
{
	if (is_data(ptr) || is_string(ptr))
	{
		return 1;
	}	
	return 0;
}

/*------------------------------------------------------------------------------------*/ 

int is_data(char *ptr)
{
	if (strncmp (ptr, ".data", strlen(".data")) == 0)
	{
		return 1;
	}
	return 0;
}

/*------------------------------------------------------------------------------------*/ 

int is_string(char *ptr)
{
	if (strncmp (ptr, ".string", strlen(".string")) == 0)
	{
		return 1;
	}
	return 0;
}
/*------------------------------------------------------------------------------------*/ 
void add_label_to_list (Label label, Label_list * label_list)
{
	/* Allocate memory for a new node */
	Label_node * new_node = (Label_node*)malloc(sizeof(Label_node));
	alloc_message(new_node)
	
	new_node->data = label;
	new_node->next = NULL; 	
	

    /* If the list is empty, set the head and tail pointers to the new node */
    if (label_list->head == NULL) 
	{
        label_list->head = new_node;
        label_list->tail = new_node;
    }	
	else
	{
		label_list->tail->next = new_node; /* update the tail */ 
        label_list->tail = new_node;
	}	
}
  

/*--------------------------------------------------------------------------------------*/ 


Label create_label (char * label,int flag_is_relocatable, int flag_is_extern, int flag_is_entry, int address,  int flag_is_data_or_string )
{
	Label temp_label; 
	
	strncpy (temp_label.name, label, MAX_LABEL);
	
	temp_label.is_relocatable = flag_is_relocatable;
	temp_label.is_extern = flag_is_extern;
	temp_label.is_entry = flag_is_entry;
	temp_label.address = address; 
	temp_label.is_data_or_string = flag_is_data_or_string;

	return temp_label;
}


/*------------------------------------------------------------------------------------------*/ 


int label_is_in_list (char * label, Label_list * label_list)
{
	Label_node * current = label_list->head;
	while (current != NULL)
	{
		if (strcmp(current->data.name, label) == 0) /* the label exist */ 
		{
			return 1; 
		}
		current = current->next; 
	}	
	return 0; 
} 


/* --------------------------------------------------------------------------------------------*/ 

int is_empty_line(char* ptr) 
{
    if (*ptr == '\n' || *ptr == '\0')
        return 1;
    else
        return 0;
}


/*-------------------------------------------------------------------------------------------*/

int is_comment_line(char* ptr)   
{
    if (*ptr == ';')
		return 1;
	else
    	return 0;
}














































