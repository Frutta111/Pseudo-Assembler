#include "utilities.h"    /* defintion of macro variabels that contain the machine properties and utilites functions*/
#include "first_pass.h"   /* functions of the first pass phase that create the symbol table*/
#include "IC_DC_images.h" /* functions that translate assembly according to the machine word syntax and creates the machie-word-list (the memory images of instruction and data) */
#include "second_pass.h"  /* functions of the second pass phase that update the symbol table*/
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
extern Label_list label_list;

/* need to get label_list? */ 
/*need to get nrow ?*/ 


/* ---------------------------------------------------------------------------------*/
extern int nrow;
int flag_secondpass; /*called by assembler.c*/ /*counts of errors*/

/* ---------------------------------------------------------------------------------*/

int second_pass(char *fname)
{
	Label_list entry_list = {NULL,NULL};	
	Label_list extern_list = {NULL,NULL};	
	char fname1[MAX_FILE_NAME];  /* name of sorce file ".am" */
	FILE *fd1;   /* fd1:source */
	char delim[] = " \t\r\n";
	char line[ROWSIZE]; /* a string containing a line from the source file */
	char temp_line[ROWSIZE];
	char * ptr;
	int type_of_first_word;
	char * maybe_label;
	 
	/*int	counter_ic = 0;*/
	/*int counter_dc = 0;*/
	int IC;
	int DC;

	IC = 100;
	DC = count_words_in_machine_list(&IC_machine_word_list) + IC; 
	
	
	free_machine_word_list(&IC_machine_word_list);
	free_machine_word_list(&DC_machine_word_list); 


	/*----- open sorce file ------*/
	strcpy(fname1,fname);
	fd1 = fopen(strcat(fname1,".am"), "rt"); /*sorce after macro replacement*/
	validFile(fd1); /*check if file is exist and not empty*/
	
	flag_secondpass = 0;
	nrow = 0;

	while (fgets(line, ROWSIZE, fd1) != NULL) 
	{
		nrow++;
		strcpy(temp_line, line);
		ptr = line;

	    while (isspace(*ptr)) 
		{
            ptr++;
        }
		
		type_of_first_word = get_type_of_first_word(ptr);
	
		while (isspace(*ptr)) 
		{
            ptr++;
        }
		
		switch(type_of_first_word) 
		{
			case 1: /* entry */ 
				entry_handler_second_pass(ptr);
				break;

			case 2: /* extern - there is nothing to do */
				break;

			case 3: case 4: /* data or string */ 
				DC = DC + create_code_data(DC, ptr, &DC_machine_word_list);
				break;

			case 19: case 20: /* type_0*/  
				IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);
				break;
	
			case 9: case 10: case 12: case 13: case 16:  /* type 1 - without prn operation*/ 				
				if (type1_is_valid(ptr, nrow))
				{
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);
				}
				else
				{
					flag_secondpass++;
					fprintf(stderr,"Error: in line %d after operation there is no legal operand (register/label)\n", nrow);
				}
				break;
				
			case 17: /* prn operation */			
				if(is_number_operand_legal_prn(ptr))
				{	
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);
				}
				else if (type1_is_valid(ptr,nrow))
				{
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);					
				}
				else /* the options after prn operations are not legal */ 
				{ 
					flag_secondpass++;				
					fprintf(stderr,"Error: in line %d after operation there is no legal operand \n", nrow);
				}
				break; 

			case 5: case 6: case 7: case 8: /* type 2 - withot lea operation */ 
				if (type2_is_valid (ptr, nrow))
				{
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);	
				}
				break;

			case 11: /* lea operation */
				if (lea_is_valid(ptr, nrow))
				{
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);
				}
				break;

			case 14: case 15:  case 18: /* type 3 */
				if (type3_is_valid(ptr, nrow))
				{				
					IC = IC + create_code_image(IC, ptr, &IC_machine_word_list);
				}
				break; 

			case 21: /* comment or empty line */
				break;

			case 22: /* maybe label */
				maybe_label = strtok(temp_line, delim);
				ptr = ptr + strlen(maybe_label);	

				label_handler_second_pass(ptr, &IC, &DC);
				break;
		}
	}/* end while*/ 

	fclose(fd1);

	/*counter_ic = count_words_in_machine_list(&IC_machine_word_list);*/
	/*counter_dc = count_words_in_machine_list(&DC_machine_word_list);*/

	/*printf("count_ic = %d \n", counter_ic);*/
	/*printf("count_dc = %d \n", counter_dc);*/

	/*printf("--------------second pass-----------------\n");*/

	/*printf("-------------- IC machine_word_list-----------------\n");*/
	/*print_machine_words_list(&IC_machine_word_list);*/
	
	/*printf("-------------- DC machine_word_list-----------------\n");*/
	/*print_machine_words_list(&DC_machine_word_list); */
	
	/*printf("-------------- label_list-----------------\n");*/
 	/*printLabelList(&label_list); */

	create_entry_extern_label_lists(&label_list, &entry_list, &extern_list);
	

	/*if (count_words_in_label_list(&entry_list) != 0)	
	{
		printf("--------------------entry list------------------------------------\n");
		printLabelList(&entry_list);
	}*/

	/*if (count_words_in_label_list(&extern_list) != 0)	
	{
		printf("--------------------extern list------------------------------------\n");
		printLabelList(&extern_list);
	}*/

	
	/*free_machine_word_list(&IC_machine_word_list);
	free_machine_word_list(&DC_machine_word_list);
	free_label_list(&label_list);*/
	/*printLabelList(&label_list); */
	return 1;
} /* end second__pass*/


/*---------------------------------------------------------------------------------------*/ 

void label_handler_second_pass(char* ptr, int *IC, int *DC) 
{
	int flag_is_entry;
	int flag_is_data_or_string;
	int flag_is_command;
	int type_of_first_word;
	int i;
	char *commands[16];

	while (isspace(*ptr)) 
	{
		ptr++; /* the pointer is on the second word */
	}

	for (i = 0; i < NUM_OF_OPERATIONS; i++)
	{
    	commands[i] = OperationsTable[i].opName;
	}

	flag_is_command	= is_label_key(commands, NUM_OF_OPERATIONS, ptr); /*is second word is command*/
	flag_is_data_or_string = is_data_or_string(ptr); /*is second word is data or string*/
	flag_is_entry = is_entry(ptr); /*is second word is entry*/
	type_of_first_word = get_type_of_first_word(ptr);

	if (flag_is_command)
	{
		switch (type_of_first_word)
		{
			case 19: case 20: /* type_0*/  
				*IC = *IC + create_code_image(*IC, ptr, &IC_machine_word_list);
				break;
	
			case 9: case 10: case 12: case 13: case 16:  /* type 1 - without prn operation*/ 				
				if (type1_is_valid(ptr, nrow))
				{
					*IC = *IC + create_code_image(*IC, ptr, &IC_machine_word_list);
				}
				else
				{
					flag_secondpass++;
					fprintf(stderr,"Error: in line %d after operation there is no legal operand (register/label)\n", nrow);
				}
				break;
				
			case 17: /* prn operation */			
				if(is_number_operand_legal_prn(ptr))
				{	
					*IC = *IC + create_code_image(*IC, ptr, &IC_machine_word_list);
				}
				else if (type1_is_valid(ptr,nrow))
				{
					*IC = *IC + create_code_image(*IC, ptr, &IC_machine_word_list);					
				}
				else /* the options after prn operations are not legal */ 
				{ 
					flag_secondpass++;				
					fprintf(stderr,"Error: in line %d after operation there is no legal operand \n", nrow);
				}
				break; 

			case 5: case 6: case 7: case 8: /* type 2 - withot lea operation */ 
				if (type2_is_valid (ptr, nrow))
				{
					*IC = *IC + create_code_image(*IC, ptr, &IC_machine_word_list);	
				}
				break;

			case 11: /* lea operation */
				if (lea_is_valid(ptr, nrow))
				{
					*IC = *IC + create_code_image(*IC, ptr, &IC_machine_word_list);
				}
				break;

			case 14: case 15:  case 18: /* type 3 */
				if (type3_is_valid(ptr, nrow))
				{				
					*IC = *IC + create_code_image(*IC, ptr, &IC_machine_word_list);
				}
				break;
		}
	}
	
	else if (flag_is_data_or_string)
	{
		*DC = *DC + create_code_data(*DC, ptr, &DC_machine_word_list);	
	}

	else if(flag_is_entry) 
	{
		entry_handler_second_pass(ptr);
	}
}

/*---------------------------------------------------------------------------------------*/ 

void entry_handler_second_pass(char *ptr)
{
	Label_node* current;
	int flag_is_label_exist = 0; /* 0 - if the entry label not exist in the label_list, 1 - if it exist */
	ptr = ptr + strlen(".entry");
	
	while (isspace(*ptr)) 
	{
		ptr++;
	}

	current = label_list.head;
    while (current != NULL) 
	{
		if (strncmp(ptr, current->data.name, strlen(current->data.name)) == 0)
		{
			current->data.is_entry = 1;
			flag_is_label_exist = 1;
		}
		
		current = current->next;			
	}
	if (flag_is_label_exist == 0)
	{
		fprintf(stderr, "Error: The label %s is an etnry label, but isn't exist in the label list\n", ptr);
		flag_secondpass++;
	}
}

/*---------------------------------------------------------------------------------------*/

 

/*---------------------------------------------------------------------------------------*/ 

int count_words_in_machine_list(Machine_word_list * machine_word_list)
{
	int count = 0; 	
	Machine_word_node* current = machine_word_list->head;
	
	while (current != NULL)
	{
		count++;
		current = current->next; 		
	}
	return count;
}


/*---------------------------------------------------------------------------------------*/ 
void create_entry_extern_label_lists(Label_list * label_list, Label_list * entry_list, Label_list * extern_list)
{
	
	Label_node * current = label_list->head; 
	
	while (current != NULL)
	{
		if (current->data.is_entry == 1)
		{
			add_label_to_list(current->data, entry_list);
		}
		if (current->data.is_extern == 1)
		{
			add_label_to_list(current->data, extern_list);
		}

		current = current->next;
	} 
}


/*---------------------------------------------------------------------------------------*/ 
int count_words_in_label_list(Label_list * label_list)
{
	int count = 0; 	
	Label_node * current = label_list->head;
	
	while (current != NULL)
	{
		count++;
		current = current->next; 		
	}
	return count;
}
























