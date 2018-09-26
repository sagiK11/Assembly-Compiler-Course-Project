Assembler: Assembler.o FirstPass.o SecondPass.o DataTypes.o LineAnalysis.o AuxFunc.o
	gcc -g -ansi -pedantic -Wall Assembler.o AuxFunc.o DataTypes.o FirstPass.o SecondPass.o 				LineAnalysis.o -o assembler

Assembler.o: Assembler.c 
	gcc -c -ansi -pedantic -Wall Assembler.c -o Assembler.o
	
FirstPass.o:FirstPass.c data.h
	gcc -c -ansi -pedantic -Wall FirstPass.c -o FirstPass.o
	
SecondPass.o: SecondPass.c data.h
	gcc -c -ansi -pedantic -Wall SecondPass.c -o SecondPass.o
	
DataTypes.o:DataTypes.c data.h
	gcc -c -ansi -pedantic -Wall DataTypes.c -o DataTypes.o
	
LinAnalysis.o:LineAnalysis.c data.h
	gcc -c -ansi -pedantic -Wall LinAnalysis.c	-o LinAnalysis.o

AuxFunc.o:AuxFunc.c data.h
	gcc -c -ansi -pedantic -Wall AuxFunc.c -o AuxFunc.o
	
