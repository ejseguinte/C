CC=gcc
CFLAGS= -std=gnu99  -g -Wall
OBJ=get_num.o curr_time.o error_functions.o sem_sync.c 
EXE=sem_sync

all: $(EXE)
$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE)
sem_sync.o:get_num.c curr_time.c error_functions.c sem_sync.c
	$(CC) $(CFLAGS) -c sem_sync.c
get_num.o:get_num.h get_num.c
	$(CC) $(CFLAGS) -c get_num.c
curr_time.o:curr_time.h curr_time.c
	$(CC) $(CFLAGS) -c curr_time.c
error_functions.o: error_functions.h tlpi_hdr.h ename.c.inc error_functions.c
	$(CC) $(CFLAGS) -c error_functions.c
clean:
	rm $(OBJ)
