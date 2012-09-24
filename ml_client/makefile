# VARIABLES #
EXE = bin/client

OBJ = obj/main.o \
		obj/client.o \
		obj/data.o \
		obj/worker.o

OPTIONS = -g -Wall -pthread
#DEBUG = -g

# OBJ RULES #
obj/main.o : src/main.c
	gcc $(OPTINOS) -c src/main.c -o obj/main.o
obj/client.o : src/client.c
	gcc $(OPTIONS) -c src/client.c -o obj/client.o
obj/data.o : src/data.c
	gcc $(OPTIONS) -c src/data.c -o obj/data.o
obj/worker.o : src/worker.c
	gcc $(OPTIONS) -c src/worker.c -o obj/worker.o

# EXECUTABLE RULES #
$(EXE) : $(OBJ)
	gcc -pthread $(OBJ) -o $(EXE)

# MAKE TARGETS #
bin : $(EXE)

clean :
	rm -f obj/*.o
	rm -f bin/*
