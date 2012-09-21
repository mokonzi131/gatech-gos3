// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#include <stdio.h>
#include <unistd.h>
#include "server.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static unsigned short int port = 0;
static char* root = NULL;
static unsigned int workers = 0;

static int setArguments(int, char**);

int main(int argc, char** argv)
{
	int error;

	assert(sizeof(unsigned short int) == 2);

	// parse command line
	error = setArguments(argc, argv);
	if (error)
	{
		printf("usage: %s [-p port] [-r root_directory] [-w workers]\n", argv[0]);
		printf("  [-p port] : valid (1-%d) port on which to listen (default is %d)\n", ml_PORT_MAX, ml_DEFAULT_PORT_NUMBER);
		printf("  [-r root_directory] : directory to serve (default is ./)\n");
		printf("  [-w workers] : a reasonable (1-%d) number of workers to use (default is %d)\n", ml_WORKERS_MAX, ml_DEFAULT_WORKERS_NUMBER);
		printf("\n");
		return -1;
	}

	// launch the server
	error = ml_server(port, root, workers);
	switch(error)
	{
		case 0:
			printf("TERMINATING Server\n");
			break;
		case -1:
		default:
			printf("Server FAILED unexpectedly\n");
	}

	printf("\n");
	return 0;
}

static int setArguments(int argc, char** argv)
{
	int check;
	int index;
	int test;

	// set appropriate arguments from command line, fail on mal-formed input
	while ((check = getopt(argc, argv, "p:r:w:")) != -1)
	{
		switch(check)
		{
			case 'p':
				test = atoi(optarg);
				if (test <= 0 || test > ml_PORT_MAX)
					return -1;
				port = test;
				break;
			case 'r':
				root = optarg;
				break;
			case 'w':
				test = atoi(optarg);
				if (test <= 0 || test > ml_WORKERS_MAX)
					return -1;
				workers = test;
				break;
			case '?':
			default:
				return -1;
		}
	}

	// don't accept extra commands
	for (index = optind; index < argc; ++index)
		return -1;

	return 0;
}
