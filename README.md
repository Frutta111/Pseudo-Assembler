# Pseudo-Assembler
Final Project for the course C Programming Lab #20465 by The Open University of Israel

This is an assembler for an imaginary computer and imaginary assembly language. 

Hardware: The computer has 7 registers: r0..r7 + PSW (program status word). Each register has 14 bits. Memory size: 256*14 bits.

Software: There are 16 commands, each of which has funct and opcode (for more info, check the PDF file)

Input: .as files - Assembly-like source code. (the arguments are passed without the extension)

Output for each file:

- .am file (with no macros)
- .obj file, with the machine code
- .ext file, with details about the .extern labels, if any.
- .ent file, with details about the .entry labels, if any.

Score: 94%
