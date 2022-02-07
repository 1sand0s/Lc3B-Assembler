;This program counts from 10 to 0
.ORIG x3000
LEA R0, TEN ;This instruction will be loaded into memory location x3000
LDW R1, R0, #31
START RSHFL R1, R1, #15
BRZ DONE
BR START
;blank line
DONE TRAP x25 ;The last executable instruction
TEN .FILL x-00A ;This is 10 in 2's comp, hexadecimal
NOP
RET
JMP R0
JSR DONE
JSRR R6
.END;The pseudo-op, delimiting the source program
BR START
