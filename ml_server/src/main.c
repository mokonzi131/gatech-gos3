// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////
#include "globals.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <signal.h>

#include "server.h"

/* DATA */
static unsigned short int port = 0;
static char* root = NULL;
static unsigned int workers = 0;

/* PRIVATE INTERFACE */
static int setArguments(int, char**);
static void signal_callback_handler(int);
static void h_pipe(int);

/* MAIN */
int main(int argc, char** argv)
{
	int result;

	assert(sizeof(unsigned short int) == 2);
	assert(sizeof(char) == 1);

   // Register signal and signal handler
   signal(SIGINT, signal_callback_handler);
   signal(SIGPIPE, h_pipe);

	// parse command line
	result = setArguments(argc, argv);
	if (result != SUCCESS)
	{
		printf("usage: %s [-p port] [-r root_directory] [-w workers]\n", argv[0]);
		printf("  [-p port] : valid (1-%d) port on which to listen (default is %d)\n", ml_PORT_MAX, ml_DEFAULT_PORT_NUMBER);
		printf("  [-r root_directory] : directory to serve (default is ./)\n");
		printf("  [-w workers] : a reasonable (1-%d) number of workers to use (default is %d)\n", ml_WORKERS_MAX, ml_DEFAULT_WORKERS_NUMBER);
		printf("\n");
		return result;
	}

	// launch the server
	result = ml_server(port, root, workers);
	switch(result)
	{
		case (SUCCESS):
			printf("...TERMINATING Server\n");
			break;
		case (SERVER_ERROR):
		default:
			printf("Server FAILED! TERMINATING...\n");
	}

	printf("\n");
	return result;
}

/* IMPLEMENTATION */
static void signal_callback_handler(int signum)
{
	ml_server_shutDown();
}
static void h_pipe(int signum)
{
	return;
}

static int setArguments(int argc, char** argv)
{
	int check;
	int index;
	int test;

	// set appropriate arguments from command line, reject on mal-formed input
	while ((check = getopt(argc, argv, "p:r:w:")) != -1)
	{
		switch(check)
		{
			case 'p':
				test = atoi(optarg);
				if (test <= 0 || test > ml_PORT_MAX)
					return (CMD_INPUTS_ERROR);
				port = test;
				break;
			case 'r':
				root = optarg;
				break;
			case 'w':
				test = atoi(optarg);
				if (test <= 0 || test > ml_WORKERS_MAX)
					return (CMD_INPUTS_ERROR);
				workers = test;
				break;
			case '?':
			default:
				return (CMD_INPUTS_ERROR);
		}
	}

	// reject if extra commands are present
	for (index = optind; index < argc; ++index)
		return (CMD_INPUTS_ERROR);

	return (SUCCESS);
}
