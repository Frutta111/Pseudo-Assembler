
/*syntax_validation */
/*--------------------------------------------------------------------------------*/


int count_number_of_words (char * ptr);
/* The function gets 'line' and return how many seperate words there is in the line */ 


/*--------------------------------------------------------------------------------*/


int is_data_valid (char * ptr, int nrow);
/* The function gets 'line', and number of line and check that there is a legal number of words in case of 'data' line 
	
	return: 1 - legal
			0 - not legal 
	the function update flag, and print appropriate message */ 

/*--------------------------------------------------------------------------------*/


int is_string_valid(char * ptr , int nrow);
/* The function gets 'line', and number of line and check that there is a legal number of words in case of 'string' line 
	
	return: 1 - legal
			0 - not legal 
	the function update flag, and print appropriate message */ 

/*--------------------------------------------------------------------------------*/


int is_two_operands(char * ptr);
/* The function gets 'line' and checks number of operand in line 
	
	return: number of operands. */


/*--------------------------------------------------------------------------------*/


int is_register(char * ptr);
/* The function check if the strings is legal register 

	return : 1- legal register 
			 0- not legal register */

/*--------------------------------------------------------------------------------*/


int is_label_exist (char * ptr);
/* The function check if the "potentially label is legal and exist in the list  

	return : 1- legal label 
			 0- not legal label */

/*--------------------------------------------------------------------------------*/


int is_number_operand_legal (char * ptr);
/* The function check if the pointer is potentially legal number for any operation except prn   

	return : 1- legal  
			 0- not legal  */

/*----------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
int is_number_operand_legal_prn (char * ptr);
/* The function check if the pointer is potentially legal number for prn operation  

	return : 1- legal  
			 0- not legal  */

/*----------------------------------------------------------------------------------------------------*/

int type1_is_valid(char * ptr, int nrow); 
/* The function check if the string is legal case of type 1 operation (not prn operation)  

	return : 1- legal  
			 0- not legal  */

/*----------------------------------------------------------------------------------------------------*/

int lea_is_valid(char * ptr, int nrow);
/* The function check if the string is legal case of lea operation 

	return : 1- legal  
			 0- not legal  */

/*----------------------------------------------------------------------------------------------------*/


int type2_is_valid (char * ptr, int nrow);
/* The function check if the string is legal case of type 2 operation except lea operation  

	return : 1- legal  
			 0- not legal  */

/*----------------------------------------------------------------------------------------------------*/


int type3_is_valid (char * ptr, int nrow);
/* The function check if the string is legal case of type 3 operation   

	return : 1- legal  
			 0- not legal  */

/*----------------------------------------------------------------------------------------------------*/

