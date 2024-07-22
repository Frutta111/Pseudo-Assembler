
#include "utilities.h"  /* defintion of macro variabels that contain the machine properties and utilites functions*/
#include "IC_DC_images.h" /* functions that translate assembly according to the machine word syntax and create the machie-word-list*/
#include "first_pass.h" /* functions of the first pass phase that create the symbol table*/
#include "print_outputs.h" /* functions that create the output files if there are no compilation errors. print machie-word-list and information from the symbol table   */

/*-------------------------------------------------------------------------------------------*/
/* This program creates output files and free memory  of machie-word-lists and  symbol table.  
  for detailed information about the functions go to print_outputs.h                            */
/*------------------------------------------------------------------------------------------*/

/* output files extensions = {".ob",".ext", ".ent"}; */


extern Machine_word_list IC_machine_word_list; /*create by IC_DC_images*/ /*machine-word list table*/
extern Machine_word_list DC_machine_word_list; /*create by IC_DC_images*/ /*machine-word list table*/
extern Label_list label_list; /*create by first_pass and updeted by second_pass*/ /*symbol table*/


/*----------------------------------------------------------------------*/

/*the main function of this section*/

void create_output_files(char *fname , int print_flag) {

	char fname1[MAX_FILE_NAME];  /* name of sorce file */
	char fname2[MAX_FILE_NAME];  /* name of sorce file */
	char fname3[MAX_FILE_NAME];  /* name of sorce file */
	
	int num_ic_words=0; /*for size of code image */
	int num_dc_words=0; /*for size of data image */
	
	/*----- remove old files with the same name as output  ----*/
	strcpy(fname1,fname);
	strcpy(fname2,fname);
	strcpy(fname3,fname);
	
	/* remove output files if exist then create output files if they are not empty */
	remove(strcat(fname1,".ob"));
	remove(strcat(fname1,".ext"));
	remove(strcat(fname1,".ent"));
	
	
	/*calculate size of image (counts of word) */
	num_ic_words = size_of_machine_words(&IC_machine_word_list) ; /*size of code image */
	num_dc_words = size_of_machine_words(&DC_machine_word_list); /*size of data image */
	
	
	if (!print_flag) {	/* If there are no compilation errors*/
	
		if ((num_ic_words + num_dc_words) > MEMORY_SIZE) { /* if image exceededs memory limit*/
			fprintf(stderr , "Exceeded the limit of the available memory size. The compiler output files will not be generated\n");
		}
		
		else {	
	
				if ( (num_ic_words + num_dc_words)  > 0) 
					write_object_file(fname , num_ic_words , num_dc_words );  /*object file*/
		
				if (size_of_entry_label(&label_list) > 0) 			
					write_entry_file(fname);		/*entry labels definition - for linker*/	
					
				if (size_of_extern_label(&IC_machine_word_list) > 0) 	
					write_extern_file(fname);		/*extern labels call - for linker*/
				fprintf(stderr , "No errors occurred, output files were created as expected.\n");	
				
		}
		
	}
	
	else  /* If there are compilation errors*/
		fprintf(stderr, "There are errors in the extended input file (.am), the assembler output files will not be generated\n");
	
	
	/*-----free memory-----*/	
	free_machine_word_list(&IC_machine_word_list); 
	free_machine_word_list(&DC_machine_word_list); 
	free_label_list(&label_list);
	
	/*printf("print outputs check if empty \n");*/
	/*printLabelList(&label_list); 
	print_machine_words_list(&IC_machine_word_list);
	print_machine_words_list(&DC_machine_word_list);*/
	
	
}

/* ---------------------------------------------------------------------*/
/* create_object_file and write the machine words on it */

void write_object_file (char *fname , int num_ic_words , int num_dc_words) {
	
	FILE *fd1;   	
	char fname1[MAX_FILE_NAME];  /* name of sorce file ".ob" */
		
	strcpy(fname1,fname);
	
	/*----- print on file  -----*/
	
	fd1 = fopen(strcat(fname1,".ob"), "wt");  	/* open file */	
	validCreateF(fname1, fd1); /* check if file can be created */
	
	fprintf(fd1,"      %d     %d \n"  ,num_ic_words , num_dc_words);  
	print_machine_words_on_file(&IC_machine_word_list, fd1);
	print_machine_words_on_file(&DC_machine_word_list, fd1); 
	
	fclose(fd1); /* close file */
	
}

/*------------------------------------------------------------------------*/
/* calculate the size of structue Machine_word_list and returns it's size*/

int size_of_machine_words(Machine_word_list * machine_word_list){
	
	int size=0;
 
    Machine_word_node* current = machine_word_list->head;
    
    while (current != NULL) {
    	  current = current->next;
        size++;
    }

	return size;
}

/*------------------------------------------------------------------------*/
/*print the relevant data of structure Machine_word_list (linked list) on a file*/	

void print_machine_words_on_file(Machine_word_list * machine_word_list , FILE * fd) {
   
    Machine_word_node* current = machine_word_list->head;
    
    while (current != NULL) {
        fprintf(fd, "%04d      %s\n", current->data.address, current->data.machine_word);
        current = current->next;
    }
}

/*--------------------------------------------------------------------------------------------*/                            
/*create_entry_file  and call print_entry_labels_on_file to writes on it name of the entry label and the memory address where the content of the entry label is.*/

void write_entry_file (char *fname) {
	
	FILE *fd1;   	
	char fname1[MAX_FILE_NAME];  /* name of sorce file ".ob" */
		
	strcpy(fname1,fname);
	
	/*----- print on file  -----*/
	
	fd1 = fopen(strcat(fname1,".ent"), "wt");  	/* open file */	
	validCreateF(fname1 , fd1); /* check if file can be created */
	
	print_entry_labels_on_file(&label_list, fd1); 
	
	fclose(fd1); /* close file */
	
}
/*------------------------------------------------------------------------*/
/*calculate the size of structue Label_list where data.is_entry==1 and returns it's size*/

int size_of_entry_label(Label_list * list){
	
	Label_node* current = list->head;
   int size=0;
    
    while (current != NULL) {
    	
    	if (current->data.is_entry) /*entry*/
        size++;
              
        current = current->next;
    }
	return size;
}

/*------------------------------------------------------------------------*/
 /*print relevant data of entry labels from structure label_list on a file*/

void print_entry_labels_on_file(Label_list * list, FILE * fd) 
{
    Label_node* current = list->head;
    
    while (current != NULL) {
    	
    	if (current->data.is_entry) /*entry*/
        fprintf(fd, "%s   %d\n", current->data.name, current->data.address);
              
        current = current->next;
    }

}


/* ---------------------------------------------------------------------*/

/*create_extern_file  and writes the memory address where the extern label is being called*/

void write_extern_file (char *fname) {
	
	FILE *fd1;   	
	char fname1[MAX_FILE_NAME];  /* name of sorce file ".ob" */
		
	strcpy(fname1,fname);
	
	/*----- print on file  -----*/
	
	fd1 = fopen(strcat(fname1,".ext"), "wt");  	/* open file */	
	validCreateF(fname1, fd1); /* check if file can be created */
	
	print_extern_labels_on_file(&IC_machine_word_list, fd1); 
	
	fclose(fd1); /* close file */
	
}

/*------------------------------------------------------------------------*/
/*calculate the size of structue machine_word_list  where data.is_extern==1 and returns it's size*/

int size_of_extern_label(Machine_word_list * machine_word_list) {
	int size=0;
	
	 Machine_word_node* current = machine_word_list->head;
	
	while (current != NULL) {
    	
    	if (current->data.is_extern == 1) /*extern*/
       size++;              
        current = current->next;
    }
	
	return size;
}

/*------------------------------------------------------------------------*/
/*print relevant data of extern labels from IC_Machine_word_list*/
void print_extern_labels_on_file(Machine_word_list * machine_word_list , FILE * fd) {
   
    Machine_word_node* current = machine_word_list->head;
    
    while (current != NULL) {
    	
    	if  (current->data.is_extern == 1) /*extern*/
        fprintf(fd, "%s   %d\n", current->data.label, current->data.address);
              
        current = current->next;
    }
    
}


