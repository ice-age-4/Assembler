;this is a legal program
Main: add #4, r3
	lea L1, r4
LIST:  .data 1,5,64, -3 ,	-5
clr r0
.entry MSG
  bne &END
cmp L1, #0
sub LIST, L1
MSG: .string "hello!"
.entry Main
.extern L1
not L2
.extern E
red r7
L2: prn #123
END: stop

LOOP: mov r5, LIST
mov r6, E

