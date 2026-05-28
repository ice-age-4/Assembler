; this file demonstrates errors of second pass
 		 

	MAIN:  add r1, r3
List: .data 1,-4,0,90
bne end
.entry end
.entry E1
bne &E1
jsr &List
prn ,#5         
jmp List,         
add List, , r5  
jmp & 

.extern E1

