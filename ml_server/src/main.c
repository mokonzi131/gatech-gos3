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

static int setArguments(int, char**);

int main(int argc, char** argv)
{
	int error;

	assert(sizeof(unsigned short int) == 2);

	// parse command line
	error = setArguments(argc, argv);
	if (error)
	{
		printf("usage: %s [-p port] [-r root_directory]\n", argv[0]);
		printf("  [-p port] : valid (1-65535) port on which to listen (default is 51115)\n");
		printf("  [-r root_directory] : directory to serve (default is ./)\n\n");
		return -1;
	}

	// run the server
	error = ml_server(port, root);
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

static int setArguments(int argc, char** argv)
{
	int check;
	int index;
	int portnum;

	while ((check = getopt(argc, argv, "p:r:")) != -1)
	{
		switch(check)
		{
			case 'p':
				portnum = atoi(optarg);
				if (portnum <= 0 || portnum > 65535)
					return -1;
				port = portnum;
				break;
			case 'r':
				root = optarg;
				break;
			case '?':
			default:
				return -1;
		}
	}

	for (index = optind; index < argc; ++index)
		return -1;

	return 0;
}
