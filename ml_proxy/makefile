# VARIABLES #
EXE = bin/proxy

OBJ = obj/main.o \
		obj/proxy.o \
		obj/safeq.o \
		obj/worker.o \
		obj/http.o \
		obj/workspace.o

OPTIONS = -g -Wall -pthread
#DEBUG = -g

# OBJ RULES #
obj/main.o : src/main.c
	gcc $(OPTIONS) -c src/main.c -o obj/main.o
obj/proxy.o : src/proxy.c
	gcc $(OPTIONS) -c src/proxy.c -o obj/proxy.o
obj/safeq.o : src/safeq.c
	gcc $(OPTIONS) -c src/safeq.c -o obj/safeq.o
obj/worker.o : src/worker.c
	gcc $(OPTIONS) -c src/worker.c -o obj/worker.o
obj/http.o : src/http.c
	gcc $(OPTIONS) -c src/http.c -o obj/http.o
obj/workspace.o : src/workspace.c
	gcc $(OPTIONS) -c src/workspace.c -o obj/workspace.o

# EXECUTABLE RULES #
$(EXE) : $(OBJ)
	gcc -pthread $(OBJ) -o $(EXE)

# MAKE TARGETS #
bin : $(EXE)

clean :
	rm -f obj/*.o
	rm -f bin/*
