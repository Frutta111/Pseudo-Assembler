
#include "utilities.h" /* defintion of macro variabels that contain the machine properties and utilites functions*/
#include "first_pass.h" /* functions of the first pass phase that create the symbol table*/
#include "syntax_validation.h" /*This program provides assembly code syntax validation services for the first pass*/


/*------------------------------------------------------------------------------------*/
/* This program provides assembly code syntax validation services for the first pass */
/*------------------------------------------------------------------------------------*/


extern int flag_firstpass; /* from first_pass.c */
extern int flag_secondpass; /* from second_pass.c */ 
extern char *RegistersTable[] ;  /* {"r0","r1","r2","r3","r4","r5","r6","r7"}*/
extern int NUM_OF_REGISTERS ; /* 8 - total number of registers*/
extern Label_list label_list;

/*------------------------------------------------------------------------------------*/



int count_number_of_words (char * ptr) 
{
	int counter = 0;
	int i;
	int is_word = 0; /* flag to keep track of whether we are currently in a word or not */

    /* iterate through each character in the string */
    for (i = 0; ptr[i] != '\0'; i++)
	{
        if (ptr[i] == ' ' || ptr[i] == '\t' || ptr[i] == '\n') 
		{
            /* we have encountered a space character */
            if (is_word) {
                /* if we were in a word before this space, increment the counter */
                counter++;
                is_word = 0; /* set the flag to indicate we are no longer in a word */
            }
        } else {
            /* we have encountered a non-space character */
            is_word = 1; /* set the flag to indicate we are in a word */ 
        }
    }

    /* if we were in a word at the end of the string, increment the counter one last time */
    if (is_word) {
        counter++;
    }

	return counter;
}


/* ---------------------------------------------------------------------------------------*/ 
int is_data_valid (char * ptr, int nrow)
{
    int is_negative = 0; /* flag to keep track of whether the current integer is negative */
    int is_digit = 0; /* flag to keep track of whether we are currently processing a digit */
    int prev_comma = 1; /* flag to keep track of whether the previous character was a comma (set to true initially) */
    int num = 0; /* variable to accumulate the current integer value */
    int is_num_valid = 0; /* flag to keep track of whether the current integer value is valid */
	int i;
	char * ptr_temp;
	

	ptr = ptr + strlen(".data");
	while (isspace(*ptr))
	{
		ptr++; /* the pointer is on the second word */
	}
	
	ptr_temp = ptr;

	if ((*ptr) == ',')
	{
		flag_firstpass++;
		fprintf(stderr,"Error: in line %d the sentence data starts with ilegal comma\n", nrow);		
		return 0;
	}

	while ((*ptr_temp) != '\0')
	{
		ptr_temp++;	/* move until the end */ 
	}
	
	ptr_temp--; /* before \0 */ 
	
	while (isspace(*ptr_temp))
	{
		ptr_temp--; /* last char */ 
	}

	if (*ptr_temp == ',') /* ends with comma */ 
	{
		flag_firstpass++;
		fprintf(stderr,"Error: in line %d the sentence data ends with ilegal comma\n", nrow);		
		return 0;	
	}
	
    /* iterate through each character in the string */
    for (i = 0; ptr[i] != '\0'; i++) /* pointer to the second word */ 
	{
        if (isdigit(ptr[i])) 
		{
            /* we have encountered a digit */
            is_digit = 1;
            num = num * 10 + (ptr[i] - '0');
            if (num > 8191) 
			{
                /* the current integer value is too large because we can't represt it with 14 bits */
                is_num_valid = 0;
            } 
			else 
			{
                is_num_valid = 1;
            }
        } 
		else if (ptr[i] == '-' && !is_digit && (prev_comma || isspace(ptr[i-1]))) 
		{
            /* we have encountered a minus sign, and it is the first character in a new integer */
            is_negative = 1;
            is_num_valid = 1;
        } 
		else if (ptr[i] == '+' && !is_digit && (prev_comma || isspace(ptr[i-1]))) 
		{
            /* we have encountered a plus sign, and it is the first character in a new integer */
            is_negative = 0;
            is_num_valid = 1;
        } 
		else if (ptr[i] == ',' && is_digit && !prev_comma) 
		{
            /* we have encountered a comma, and it is not the first character in a new integer */
            if (is_num_valid) 
			{
                /* the current integer value is valid, check if it is in range */
                if (is_negative) 
				{
                    /* negative integer */
                    if (num < -8192)   /* the current integer value is too small because we can't represt it with 14 bits */
					{
						flag_firstpass++;						
						fprintf(stderr,"Error: in line %d the number is out of range\n", nrow);
                        return 0;
                    }
                } 
				else 
				{
                    /* positive integer */
                    if (num > 8191) 
					{
						flag_firstpass++;						
						fprintf(stderr,"Error: in line %d the number is out of range\n", nrow);                        
						return 0;
                    }
                }
            } 
			else 
			{
				flag_firstpass++;                
				fprintf(stderr,"Error: in line %d the number is out of range\n", nrow);/* the current integer value is invalid, the input string is invalid */
                return 0;
            }
            is_negative = 0; /* reset the flag for negative integers */
            is_digit = 0; /* reset the flag for digits */
            num = 0; /* reset the variable for the current integer value */
            is_num_valid = 0; /* reset the flag for the current integer value */
            prev_comma = 1; /* set the flag for previous comma */
        } 
		else if (isspace(ptr[i])) 
		{
         	/* we have encountered a space character */
           /* do nothing */
        } 
		else
		{
			flag_firstpass++;                
			fprintf(stderr,"Error: in line %d two commas in a row\n", nrow);                 
			return 0;
        }

        prev_comma = (ptr[i] == ','); /* update the flag for previous comma */
    }

    /* check if the last integer value is valid and in range */
    if (is_num_valid) 
	{
        if (is_negative)
		{
            /* negative integer */
            if (num < -8192) 
			{
				flag_firstpass++;			    
				fprintf(stderr,"Error: in line %d the number is out of range\n", nrow);            
				return 0;
            }
        } 
		else 
		{
            /* positive integer */
            if (num > 8191) 
			{
				flag_firstpass++;
				fprintf(stderr,"Error: in line %d the number is out of range\n", nrow);                
				return 0;
            }
        }
    } 
	else 
	{
        /* the last integer value is invalid, the input string is invalid */
		flag_firstpass++;		
		fprintf(stderr,"Error: in line %d the number is out of range\n", nrow);        
		return 0;
    }

    /* check if the last character is a digit (not a comma) */
    return is_digit;
}

/* ---------------------------------------------------------------------------------------*/ 

int is_string_valid(char * ptr , int nrow)
{
	int count = 0;	
	ptr = ptr + strlen(".string");
	while (isspace(*ptr))
	{
		ptr++; /* the pointer is on the second word */
	}
	
	if (!(*ptr == '"')) /* not legal string */
	{
		flag_firstpass++;
		fprintf(stderr,"Error: in line %d the string doesn't start with \"\n", nrow);        
		return 0;
	}
	
	while (*ptr != '\0') /* start with " */ 
	{
		if (*ptr == '"')
		{
			count++; /* we need for legal string that count = 2 */
		}
		
		/*if (*ptr == ' ' || *ptr == '\t') {
			flag_firstpass++;					
			fprintf(stderr,"Error: in line %d the string mustn't contain spaces\n", nrow);        
			return 0;
		}*/
		
		if (count == 2)
		{	
			ptr++;
			while (*ptr != '\0')
			{
				if (*ptr != ' ' && *ptr != '\t' && *ptr != '\r' && *ptr != '\n') /* checks that there is no any chars after the end of string */ 
				{	
					flag_firstpass++;					
					fprintf(stderr,"Error: in line %d the string doesn't end legally\n", nrow);        
					return 0;
				}	
				ptr++; 
			}
			break;
		}
		ptr++;
	}

	if (count < 2)
	{
		flag_firstpass++;		
		fprintf(stderr,"Error: in line %d the string doesn't ends with \"\n", nrow);        
		return 0;
	}
	
	return 1;
}

/* ---------------------------------------------------------------------------------------*/ 

int is_two_operands(char * ptr)
{
	char temp_line[ROWSIZE];
	char delim[] = " \t\r,";
	char * operand;
	int counter = 0;

	ptr = ptr + 3; /* in this case all operations has the same length of 3 */ 

	while (isspace(*ptr))
	{
		ptr++; /* the pointer is on the first operand */
	}

	strcpy(temp_line, ptr);
	operand = strtok(temp_line, delim);

	while (operand != NULL) 
	{
		counter++;
		operand = strtok(NULL, delim);
	}

	return counter;
}


/* ---------------------------------------------------------------------------------------*/ 

int is_register(char * ptr)
{
	int i;
	for (i = 0; i < NUM_OF_REGISTERS; i++) 
	{
		if (strncmp(RegistersTable[i], ptr, strlen(RegistersTable[i])) == 0)
	 	{
           return 2; /* is register - returns the length of the strings r+number */
      	}
 	}
   
   return 0; /*not register*/
}	


/*--------------------------------------------------------------------------------------------------*/

int is_label_exist (char * ptr)
{
	Label_node * current;
	current = label_list.head;
  	while (current != NULL) 
	{
		if (strncmp(ptr, current->data.name, strlen(current->data.name)) == 0)
		{
			return strlen(current->data.name); /* the length of the label */ 
		}
		current = current->next;			
	}
	return 0;
}


/*--------------------------------------------------------------------------------------------------*/

int is_number_operand_legal(char * ptr)
{
	int num;
	int length_ptr_temp=0;
	char * ptr_temp;
	while (isspace(*ptr))
	{
		ptr++; /* the pointer is on the first operand */
	}
	if (*ptr != '#')
	{
		return 0; 
	}
	ptr++;
	length_ptr_temp++;
	
	if (*ptr == '+' || *ptr == '-')
	{
		ptr_temp=ptr;            
		ptr++;
		length_ptr_temp++;
    }
	else 
	{
		ptr_temp = ptr;
	}
	
	while (isdigit(*ptr))
	{
		ptr++;
		length_ptr_temp++;
	}
	
	num = atoi(ptr_temp);

	if (num < -2048 || num > 2047)
	{
		return 0;
	}
	return (length_ptr_temp); /* return the legnth of the string #xx - to know how many chars to jump */ 
}


/*------------------------------------------------------------------------------------------------------*/
int is_number_operand_legal_prn (char * ptr)
{
	int num;
	int length_ptr_temp=0;
	char * ptr_temp;
	ptr = ptr + 3; /* in oprations  the strlen is 3 */ 
	while (isspace(*ptr))
	{
		ptr++; /* the pointer is on the first operand */
	}
	if (*ptr != '#')
	{
		return 0; 
	}

	ptr++;
	length_ptr_temp++;
	if (*ptr == '+' || *ptr == '-')
	{
		ptr_temp=ptr;            
		ptr++; 
		length_ptr_temp++;
    }
	else 
	{
		ptr_temp = ptr;
	}
	
	while (isdigit(*ptr))
	{
		ptr++;
		length_ptr_temp++;
	}
	
	num = atoi(ptr_temp);

	if (num < -2048 || num > 2047)
	{
		return 0;
	}
	return (length_ptr_temp); /* return the legnth of the string #xx - to know how many chars to jump */ 
}


/*--------------------------------------------------------------------------------------------------*/
 
int type1_is_valid(char * ptr, int nrow)
{
	ptr = ptr + 3; /* in oprations  the strlen is 3 */	
	while (isspace(*ptr))
	{
		ptr++; /* the pointer is on the second word */
	}
	
	if (is_label_exist(ptr))
	{
		return 1;
	}
	
	if (is_register(ptr))
	{
		return 1;
	}
	
	return 0;
}

/*--------------------------------------------------------------------------------------------------*/

int lea_is_valid(char * ptr, int nrow)
{
	int length_temp; /* length of the label that we check if exist */	
	ptr = ptr + 3; /* in oprations  the strlen is 3 */ 	
	while (isspace(*ptr)) 
	{
	    ptr++; /* pointer to the second word */ 
    }

	length_temp = is_label_exist(ptr);
	if (!length_temp)
	{
		flag_secondpass++;				
		fprintf(stderr,"Error: in line %d after operation the operand isn't exist label \n", nrow);	
		return 0;
	}

	ptr = ptr + length_temp;
	while (isspace(*ptr)) 
	{
	    ptr++; /* pointer to the comma */ 
    }

	
	if (! (*ptr == ','))
	{			
		flag_secondpass++;				
		fprintf(stderr,"Error: in line %d after operation there is a missing comma (or label isn't exist) \n", nrow);
		return 0;
	}

	ptr++;
	while (isspace(*ptr)) 
	{
	    ptr++; /* pointer to the next word */ 
    }

	if (!is_label_exist (ptr) && !is_register(ptr))
	{					
		flag_secondpass++;				
		fprintf(stderr,"Error: in line %d after operation the second operand isn't legal \n", nrow);	
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------------------------------*/

int type2_is_valid (char * ptr, int nrow)
{
	ptr = ptr + 3; /* in oprations  the strlen is 3 */ 	
	while (isspace(*ptr)) 
	{
	    ptr++; /* pointer to the next word */ 
    }

	if (!is_register(ptr) && !is_label_exist(ptr) && !is_number_operand_legal(ptr))
	{
		flag_secondpass++;				
		fprintf(stderr,"Error: in line %d after operation the first operand isn't legal \n", nrow);	
		return 0;	
	}

	if (is_register (ptr))
	{
		ptr = ptr + 2; /* length of all registers are 2 */
		while (isspace(*ptr)) 
		{
	 	   ptr++; /* pointer to the next word */ 
   		}

		if (! (*ptr == ','))
		{			
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d after operation there is an ilegal sentnce \n", nrow);
			return 0;
		}

		ptr++;
		while (isspace(*ptr)) 
		{
	  	  ptr++; /* pointer to the next word */ 
   		}

		if (!is_label_exist (ptr) && !is_register(ptr))
		{					
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d after operation the second operand isn't legal \n", nrow);	
			return 0;
		}
	}
	else if (is_label_exist(ptr))
	{
		ptr = ptr + is_label_exist(ptr);
		while (isspace(*ptr)) 
		{
	 	   ptr++; /* pointer to the next word */ 
   		}

		if (! (*ptr == ','))
		{			
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d after operation there is a missing comma (or label isn't exist) \n", nrow);
			return 0;
		}

		ptr++;
		while (isspace(*ptr)) 
		{
	  	  ptr++; /* pointer to the next word */ 
   		}

		if (!is_label_exist (ptr) && !is_register(ptr))
		{					
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d after operation the second operand isn't legal \n", nrow);	
			return 0;
		}
	}
	else if (is_number_operand_legal(ptr))
	{
		ptr = ptr + is_number_operand_legal(ptr);
		while (isspace(*ptr)) 
		{
	 	   ptr++; /* pointer to the next word */ 
   		}

		if (! (*ptr == ','))
		{			
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d after operation there is a missing comma (or ilegal operand number) \n", nrow);
			return 0;
		}

		ptr++;
		while (isspace(*ptr)) 
		{
	  	  ptr++; /* pointer to the next word */ 
   		}

		if (!is_label_exist (ptr) && !is_register(ptr))
		{					
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d after operation the second operand isn't legal \n", nrow);	
			return 0;
		}
	}
	
	return 1;
}

/*--------------------------------------------------------------------------------------------------*/

int type3_is_valid (char * ptr, int nrow)
{
	int num;	
	ptr = ptr + 3; /* in oprations  the strlen is 3 */ 		
	while (isspace(*ptr)) 
	{
	    ptr++; /* pointer to the next word  */ 
    }
	
	if (!is_label_exist(ptr))
	{
		flag_secondpass++;				
		fprintf(stderr,"Error: in line %d after operation the first operand isn't a label \n", nrow);	
		return 0;	
	}
	
	ptr = ptr + is_label_exist(ptr);
	if (*ptr != '(' && *ptr != ' ' && *ptr != '\0' && *ptr != '\n')
	{
		flag_secondpass++;				
		fprintf(stderr,"Error: in line %d after the label, the sentence isn't legal \n", nrow);	
		return 0;	
	}
	
	if (*ptr == '(')
	{
		ptr++; /* char after ( */ 
		/*	printf ("ptr type 3 is %s \n", ptr);*/
	/*printf ("type 3 %d \n", is_number_operand_legal(ptr));*/
		num = (is_label_exist(ptr) + is_register(ptr) + is_number_operand_legal (ptr)); /* can be only one of the options */ 

		if (num == 0) /* the string after ( is just one of the options and legal */ 
		{
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d the oprands in the sentence are not legal \n", nrow);	
			return 0;
		}
		
		ptr = ptr + num; 
		if (*ptr != ',')
		{
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d after the label, the sentence isn't legal/there is missing comma \n", nrow);	
			return 0;
		}
		ptr++; 
		num = (is_label_exist(ptr) + is_register(ptr) + is_number_operand_legal (ptr)); /* can be only one of the options */ 
		if (num == 0)
		{
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d the oprands in the sentence are not legal \n", nrow);	
			return 0;
		}
		
		ptr = ptr + num; 
		if (*ptr != ')')
		{
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d the sentence isn't legal /there is no closser ) \n", nrow);	
			return 0;
		}
		ptr++;
		if (*ptr != '\0' && *ptr != '\n' && *ptr != ' ' && *ptr != '\r')
		{
			flag_secondpass++;				
			fprintf(stderr,"Error: in line %d there are ilegal chars extra \n", nrow);	
			return 0;
		}
	}
	return 1;
}




















