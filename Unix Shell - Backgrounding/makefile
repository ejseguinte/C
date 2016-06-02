# Simple Makefile for csc60_stat program
CC=gcc
CFLAGS=-std=gnu99  -g -Wall
OBJ=csc60mshell.o
EXE=a.out

all: $(EXE)
$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE)
csc60mshell.o: csc60mshell.c
	$(CC) $(CFLAGS) -c csc60mshell.c
clean:
	rm $(OBJ)
