# The makefile for MP5
# make		-- to build program lab5
#
#

lab5 : tree.o lab5.o
	gcc -Wall -g tree.o lab5.o -o lab5 -lm

tree.o : tree.c tree.h
	gcc -Wall -g -c tree.c

lab5.o : lab5.c
	gcc -Wall -g -c lab5.c

clean : 
	rm -f *.o lab5 a.out
