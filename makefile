assembler:	assembler.o utilities.o PreProcessor.o first_pass.o syntax_validation.o second_pass.o  IC_DC_images.o print_outputs.o
	gcc -ansi -Wall -g -pedantic utilities.o PreProcessor.o first_pass.o syntax_validation.o second_pass.o  IC_DC_images.o print_outputs.o assembler.o -o assembler

assembler.o: assembler.c print_outputs.h IC_DC_images.h second_pass.h first_pass.h PreProcessor.h utilities.h 
	gcc -c -ansi -Wall -g -pedantic assembler.c
			
utilities.o: utilities.c utilities.h
	gcc -c -ansi -Wall -g -pedantic utilities.c
	
PreProcessor.o: PreProcessor.c PreProcessor.h utilities.h
	gcc -c -ansi -Wall -g -pedantic PreProcessor.c
	
IC_DC_images.o: IC_DC_images.c IC_DC_images.h utilities.h first_pass.h
	gcc -c -ansi -Wall -g -pedantic IC_DC_images.c
	
first_pass.o: first_pass.c first_pass.h utilities.h IC_DC_images.h syntax_validation.h
	gcc -c -ansi -Wall -g -pedantic first_pass.c

second_pass.o: second_pass.c second_pass.h  utilities.h IC_DC_images.h first_pass.h syntax_validation.h
	gcc -c -ansi -Wall -g -pedantic second_pass.c

syntax_validation.o: syntax_validation.c syntax_validation.h utilities.h 
	gcc -c -ansi -Wall -g -pedantic syntax_validation.c	
	
print_outputs.o: print_outputs.c print_outputs.h  utilities.h IC_DC_images.h first_pass.h
	gcc -c -ansi -Wall -g -pedantic print_outputs.c
	
	 
