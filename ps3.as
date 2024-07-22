;this is a commet
.extern KIL
.entry jrs
.entry LOOP

MAIN: mov r3 ,LENGTH
LOOP: jmp L1(#-1,r6)
mcr m1 
	sub r1, r4
	bne END
endmcr 
prn #-5
;this is a commet
bne LOOP(r4,r3)
m1
L1: inc K
bne LOOP(K,STR) 
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
	m1 
K: .data 2
