all: assembler

assembler: main.o Assembler.o Data.o Errors.o Files.o Labels.o LineParsing.o LinkedList.o Macros.o Operations.o Table.o utils.o
	gcc -ansi -Wall -pedantic -o assembler main.o Assembler.o Data.o Errors.o Files.o Labels.o LineParsing.o LinkedList.o Macros.o Operations.o Table.o utils.o

main.o: main.c
	gcc -ansi -Wall -pedantic -c main.c

Assembler.o: Assembler.c Assembler.h
	gcc -ansi -Wall -pedantic -c Assembler.c

Data.o: Data.c Data.h
	gcc -ansi -Wall -pedantic -c Data.c

Errors.o: Errors.c Errors.h
	gcc -ansi -Wall -pedantic -c Errors.c

Files.o: Files.c Files.h
	gcc -ansi -Wall -pedantic -c Files.c

Labels.o: Labels.c Labels.h
	gcc -ansi -Wall -pedantic -c Labels.c

LineParsing.o: LineParsing.c LineParsing.h
	gcc -ansi -Wall -pedantic -c LineParsing.c

LinkedList.o: LinkedList.c LinkedList.h
	gcc -ansi -Wall -pedantic -c LinkedList.c

Macros.o: Macros.c Macros.h
	gcc -ansi -Wall -pedantic -c Macros.c

Operations.o: Operations.c Operations.h
	gcc -ansi -Wall -pedantic -c Operations.c

Table.o: Table.c Table.h
	gcc -ansi -Wall -pedantic -c Table.c

utils.o: utils.c utils.h
	gcc -ansi -Wall -pedantic -c utils.c

clean:
	rm *.o
