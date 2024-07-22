mcr macrob
	END: stop
	STR: .string "abcdef"
endmcr

; file ps.as
.entry LENGTH
.extern W

mcr macro
	sub r1, r4
	bne L3
	L1: inc K
endmcr

MAIN: mov r3 ,LENGTH
LOOP: jmp L1(#-1,r6)
prn #-5
bne W(r4,r5)
	macro
.entry LOOP
bne LOOP(K,W)
	macrob
LENGTH: .data 6,-9,15
K: .data 22
.extern L3




