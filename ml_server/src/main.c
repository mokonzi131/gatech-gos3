// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#include <stdio.h>
#include <unistd.h>
#include "server.h"
#include <string.h>
#include <assert.h>

static unsigned short int port = 0;
static char* root = NULL;

static int setArguments(int, char**);

int main(int args, char** argv)
{
	int error;

	assert(sizeof(unsigned short int) == 2);

	// parse command line
	error = setArguments(args, argv);
	if (error)
	{
		printf("usage: %s [-p port] [-r root_directory]\n", argv[0]);
		printf("  [-p port] : port on which to listen (default is 51119)\n");
		printf("  [-r root_directory] : directory to serve (default is ./)\n\n");
		return -1;
	}

	// run the server
	error = server(port, root);
	switch(error)
	{
		case -1:
			printf("Server failed unexpectedly\n");
			break;
		default:
			printf("Terminating Server\n");
	}

	return 0;
}

static int setArguments(int args, char** argv)
{
	int check;

	while ((check = getopt(argc, argv, "pr")) != -1)
	{
		// TODO
	}

	return 0;
}
