/// Michael Landes
/// GaTech : GOS : Project 3
/// \\\///\\\///\\\///\\\///
#include "globals.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "proxy.h"

#define INPUT_PARSE_ERROR		-2

/// DATA ///
static unsigned short int port = 0;
static unsigned int workers = 0;
int OPTIMIZE = 0;

/// PRIVATE INTERFACE ///
static int setArgs(int, char**);
static void h_interrupt(int);
static void h_pipe(int);

/// MAIN ///
int main(int argc, char** argv)
{
	int result = 0;

	// Register signal and signal handler
	signal(SIGINT, h_interrupt);
	signal(SIGPIPE, h_pipe);

	// parse command line
	result = setArgs(argc, argv);
	if (result != SUCCESS)
	{
		printf("usage: %s [-p port] [-w workers] [-o]\n", argv[0]);
		printf("  [-p port] : valid (1-%d) port on which to listen. (default = %d)\n", MAX_PORT, DEFAULT_PROXY_PORT);
		printf("  [-w workers] : a reasonable (1-50) number of worker threads to use. (default = %d)\n", DEFAULT_PROXY_WORKERS);
		printf("  [-o] : optimize - turn on usage of rpc to shrink jpeg images\n");
		printf("\n");
		return result;
	}

	// launch the proxy server
	result = ml_proxy(port, workers);
	switch(result)
	{
		case (SUCCESS):
			break;
		default:
			printf("MAIN: [ml_proxy] terminated unexpectedly...\n");
	}

	printf("\n");
	return result;
}

/// IMPLEMENTATION ///
static void h_interrupt(int signum)
{
	ml_proxy_shutdown();
}

static void h_pipe(int signum)
{
	return;
}

static int setArgs(int argc, char** argv)
{
	int check;
	int index;
	int test;

	// set command line inputs, reject a mal-formed input
	while((check = getopt(argc, argv, "p:w:o")) != ERROR)
	{
		switch(check)
		{
			case 'p':
				test = atoi(optarg);
				if (test <= 0 || test > MAX_PORT)
					return (INPUT_PARSE_ERROR);
				port = test;
				break;
			case 'w':
				test = atoi(optarg);
				if (test <= 0 || test > MAX_WORKERS)
					return (INPUT_PARSE_ERROR);
				workers = test;
				break;
			case 'o':
				OPTIMIZE = 1;
				break;
			case '?':
			default:
				return (INPUT_PARSE_ERROR);
		}
	}

	for (index = optind; index < argc; ++index)
		return (INPUT_PARSE_ERROR);

	return (SUCCESS);
}
