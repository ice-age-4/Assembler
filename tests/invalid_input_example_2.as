;this file passes the preassembler and demonstrates errors of first pass
	; comment
.data 1,4,-300, 999999999
my label: stop
myLabel 	: stop
anotherLabel:stop
: jsr &L1
L1:
.extern 
abc: clr r7
sub #-999999999, r2
cmp &L2,#4 
inc #4

mcro abc
lea L2, r7
mcroend
List: .data 0
aaaaabbbbbcccccdddddeeeeefffffgg: not List
A_b$: clr r0
mov: mov r1, r2

L2: mov #2, r1
rts L2
add #100, 
red
L2: add r5, r1
prn ,#5 
jmp L2, 
add L2, , r5
add L2 r5
prn #
 
L3: .string
.data ,1,2,3
.data 1,2,3,
.data 1a, 4
.data a1, 4
.data 1 2 ,3
.string "abcd"e
/string "ccc"


