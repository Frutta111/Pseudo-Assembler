int second_pass(char *);
/* The function is the main program of the second pass */  

/*----------------------------------------------------------------------------------------------------*/

void label_handler_second_pass(char* ptr, int *IC, int* DC);
/* the function working and create appropriate labels in different cases in second pass.

	parameters: char * , ic number, dc number */ 

/*----------------------------------------------------------------------------------------------------*/
void entry_handler_second_pass(char *ptr);
/* the function working on cases of .entry as an opening of the sentence */ 

/*----------------------------------------------------------------------------------------------------*/
int count_words_in_machine_list(Machine_word_list * machine_word_list);
/* The function counts the number of words in the list. 
	  
	parameters : machine word list. 
	return : number of words in the list    */

/*----------------------------------------------------------------------------------------------------*/
int count_words_in_label_list(Label_list * label_list);
/* The function counts the number of words in the list. 
	  
	parameters : label list. 
	return : number of words in the list    */

/*----------------------------------------------------------------------------------------------------*/
void create_entry_extern_label_lists(Label_list * label_list, Label_list * entry_list, Label_list * extern_list);
/* The function gets 3 lists - the global label list, entry list and extern label list.

the function create appropriatly extern and entry labels lists. */ 

/*----------------------------------------------------------------------------------------------------*/
