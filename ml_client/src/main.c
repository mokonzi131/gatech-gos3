/// Michael Landes
/// GaTech : GOS : Project 1
/// ////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "client.h"

/// DATA ///
static char* ARG_server = NULL;
static unsigned short int ARG_port = 0;
static unsigned int ARG_numWorkers = 0;
static unsigned int ARG_numRequests = 0;
static unsigned int ARG_numFiles = 0;
static char* ARG_proxy = NULL;

/// PRIVATE INTERFACE ///
static ml_error_t setArguments(int, char**);

/// MAIN ///
int main(int argc, char** argv)
{
	ml_error_t result;

	// parse command line
	result = setArguments(argc, argv);
	if (result != SUCCESS)
	{
		printf("Usage: %s -s server -p port -w num_workers -r num_requests -f num_files -x proxy\n", argv[0]);
		printf("  -s server : name of the server on the network\n");
		printf("  -p port : number of the port to use to connect to the server\n");
		printf("  -w num_wokers : a reasonable number of workers to use\n");
		printf("  -r num_request : a reasonable number of requests to use per worker\n");
		printf("  -f num_files : the number of files in the dest dir to examine\n");
		printf("  -x proxy : the network name of the proxy server\n");
		printf("\n");
		return result;
	}

	// run client
	result = ml_client(ARG_server, ARG_port, ARG_numWorkers, ARG_numRequests, ARG_numFiles, ARG_proxy);
	if (result != SUCCESS)
		printf("ERROR: Unable to run Client... Aborting\n");
	printf("\n");
	return 0;
}

/// IMPLEMENTATION ///
static ml_error_t setArguments(int argc, char** argv)
{
	int check;
	int test;

	// set appropriate arguments from the command line, reject on mal-formed input
	while ((check = getopt(argc, argv, "s:p:w:r:f:x:")) != -1)
	{
		switch(check)
		{
			case 's':
				ARG_server = optarg;
				break;
			case 'p':
				test = atoi(optarg);
				if (test <= 0)
					return (CMD_INPUTS_ERROR);
				ARG_port = test;
				break;
			case 'w':
				test = atoi(optarg);
				if (test <= 0)
					return (CMD_INPUTS_ERROR);
				ARG_numWorkers = test;
				break;
			case 'r':
				test = atoi(optarg);
				if (test <= 0)
					return (CMD_INPUTS_ERROR);
				ARG_numRequests = test;
				break;
			case 'f':
				test = atoi(optarg);
				if (test <= 0)
					return (CMD_INPUTS_ERROR);
				ARG_numFiles = test;
				break;
			case 'x':
				ARG_proxy = optarg;
				break;
			case '?':
			default:
				return (CMD_INPUTS_ERROR);
		}
	}

	// reject if extra commands are present
	if (optind < 11)
		return (CMD_INPUTS_ERROR);
//	{/// TODO remove this temp section for production
//		ARG_server = "localhost";
//		ARG_port = 51115;
//		ARG_numWorkers = 10;
//		ARG_numRequests = 100;
//	}

	return (SUCCESS);
}
