// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#include "server.h"

#include <stdio.h>

/* DATA */
static char* host = "host";
static int port = 1019;
static int ip = 123;
static char* root = "hello world";

static int initialize();
static int run();

/* PUBLIC INTERFACE */
int ml_server(unsigned short int _port, char* _root)
{
	printf("port: %d\n", _port);
	printf("root: %s\n", _root);
	// TODO
	return -1;
}

/* PRIVATE INTERFACE */
static int initialize(void)
{
	printf("init...\n");
}

static int run(void)
{
	printf("run...\n");
}
