
/*  header of program print_outputs*/

void create_output_files(char * , int ); /*the main function of this section*/

void write_object_file (char * , int , int); /* create_object_file and write the machine words on it */
void write_entry_file (char * ); /*create_entry_file  and writes the memory address where the content of the internal label is*/
void write_extern_file (char *) ;/*create_extern_file  and writes the memory address where the extern label is being called*/

void print_machine_words_on_file(Machine_word_list * , FILE * ); /*print relevant data of structure Machine_word_list on a file*/	
void print_entry_labels_on_file(Label_list * , FILE * ); /*print relevant data of entry labels from structure label_list on a file*/
void print_extern_labels_on_file(Machine_word_list * , FILE * ) ;/*print relevant data of extern labels from IC_Machine_word_list*/

int size_of_machine_words(Machine_word_list *); /* calculate the size of structue Machine_word_list and returns it's size*/
int size_of_entry_label(Label_list * );	/*calculate the size of structue Label_list where data.is_entry==1 and returns it's size*/
int size_of_extern_label(Machine_word_list * ); /*calculate the size of structue Machine_word_list  where data.is_extern==1 and returns it's size*/


/**************  more details *******************************************************************************/

/* void create_output_files(char *fname , int print_flag) */
/*the main function of this section
 
 1. remove output files if exist then create output files if they are not empty
 2. call write_object_file , write_entry_file , write_extern_file functions only if the content is not empty

*/
 

/*--------------------------------------------------------------------------------------------------------*/
/*void write_object_file (char * , int , int);*/
/* create_object_file and call print_entry_labels_on_file function to write on it the memory address serial number and the machine 		   words that stored in it .
	output: "ob" file 
	input: file name , size of code-image , size of data-image 
	global variable used: IC and DC machine words lists*/
/*--------------------------------------------------------------------------------------------------------*/
/*void print_machine_words_on_file(Machine_word_list * , FILE * ); */
/*print relevant data of structure Machine_word_list on a file.
  input: Machine_word_list , file pointer
	the relevant data are 
		- data.address
		- data.machine_word
 */	
/*--------------------------------------------------------------------------------------------------------*/
/*void write_entry_file (char * ); */
/*create_entry_file  and call print_entry_labels_on_file function to writes on it name of the entry label and the
 memory address where the content of the entry label is.
	output: "ent" file 
	input: file name 
	global variable used: label_list*/
/*--------------------------------------------------------------------------------------------------------*/
/*void print_entry_labels_on_file(Label_list * , FILE * );*/
/*print relevant data of entry labels from structure label_list on a file
  input: Label_list , file pointer
	the relevant data are 
		- data.name
		- data.address
*/
/*--------------------------------------------------------------------------------------------------------*/
/*void write_extern_file (char * ); */
/*write_extern_file  and call print_extern_labels_on_file function to writes on it name of the extern label 
and the memory address where the label is supposed to be linked by the linker.
	output: "ext" file 
	input: file name 
	global variable used: IC_machine_word_list*/
/*--------------------------------------------------------------------------------------------------------*/

/*void print_extern_labels_on_file(Machine_word_list * , FILE * ) ;*/
/*print relevant data of extern labels from IC_Machine_word_list
  input: Label_list , file pointer
  the relevant data are 
		- data.label
		- data.address
*/



