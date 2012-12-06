# VARIABLES #
EXE = bin/proxy

OBJ = obj/main.o \
		obj/proxy.o \
		obj/safeq.o \
		obj/worker.o \
		obj/http.o \
		obj/rpc.o

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
obj/rpc.o : src/rpc.c
	gcc $(OPTIONS) -c src/rpc.c -o obj/rpc.o

# EXECUTABLE RULES #
$(EXE) : $(OBJ)
	gcc -c src/mlrpc_clnt.c -o obj/mlrpc_clnt.o
	gcc -c src/mlrpc_xdr.c -o obj/mlrpc_xdr.o
	gcc -c src/mlrpc_svc.c -o obj/mlrpc_svc.o
	gcc -o bin/rpcserver obj/mlrpc_xdr.o obj/mlrpc_svc.o obj/rpc.o jpeg-6b/lowres.o jpeg-6b/lowres-write.o jpeg-6b/libjpeg.a
	gcc -pthread $(OBJ) -o $(EXE) jpeg-6b/lowres.o jpeg-6b/lowres-write.o jpeg-6b/libjpeg.a obj/mlrpc_clnt.o obj/mlrpc_xdr.o

# MAKE TARGETS #
bin : $(EXE)

clean :
	rm -f obj/*.o
	rm -f bin/*
