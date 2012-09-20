# VARIABLES #
EXE = bin/server

OBJ = obj/main.o \
		obj/server.o \

OPTIONS = -Wall
DEBUG = -g

# OBJ RULES #
obj/main.o : src/main.c
	gcc $(OPTIONS) -c src/main.c -o obj/main.o
obj/server.o : src/server.c
	gcc $(OPTIONS) -c src/server.c -o obj/server.o

# EXECUTABLE RULES #
$(EXE) : $(OBJ)
	gcc $(OBJ) -o $(EXE)

# MAKE TARGETS #
bin : $(EXE)

test : $(EXE)
	cp $(EXE) test/server

clean :
	rm -f obj/*.o
	rm -f bin/*
	rm -f test/server
