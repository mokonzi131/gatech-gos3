// Michael Landes
// GaTech : GOS : Project 2
///////////////////////////
#include "globals.h"

#include "proxy.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//#include <sys/stat.h>
//#include <pthread.h>

//#include "worker.h"
//#include "safeq.h"

int ml_proxy(unsigned short int port, unsigned int workers, int shared)
{
	return (ERROR);
}
void ml_proxy_shutdown() {}

/* DATA
const unsigned short int ml_DEFAULT_PORT_NUMBER = 51115;
const unsigned short int ml_PORT_MAX = 65535;
const unsigned int ml_DEFAULT_WORKERS_NUMBER = 5;
const unsigned int ml_WORKERS_MAX = 50;

static const unsigned int BACKLOG = 20;

static int terminate = 0;

static char* rootDirectory;
static int hServerSocket;
static struct sockaddr_in ServerAddress;
static unsigned int workers;
static pthread_t* workerPool;
static int* workerArgs;

/* PRIVATE INTERFACE
static int initialize(unsigned short int, const char*, unsigned int);
static int run(void);

/* PUBLIC INTERFACE
int ml_server(unsigned short int port, const char* root, unsigned int workers)
{
	int result;

	result = initialize(port, root, workers);
	switch(result)
	{
		case (SUCCESS):
			break;
		default:
			printf("ERROR: Could not initialize the server\n");
			return result;
	}

	result = run();
	switch(result)
	{
		case (SUCCESS):
			break;
		default:
			printf("ERROR: Server is down\n");
			return result;
	}

	return result;
}

char* ml_server_getRootDir()
{
	return rootDirectory;
}

void ml_server_shutDown()
{
	shutdown(hServerSocket, 0);
	terminate = 1;
}

/* IMPLEMENTATION
static int initialize(unsigned short int port, const char* root, unsigned int _workers)
{
	int nAddressSize = sizeof(struct sockaddr_in);
	int i;
	struct stat rootstat;

	printf("INITIALIZING Server...\n");

	// setup the port
	port = (port == 0 ? ml_DEFAULT_PORT_NUMBER : port);
	assert(port > 0 && port <= ml_PORT_MAX);

	// setup the root directory
	rootDirectory = (char*) malloc (sizeof(char) * strlen((root == NULL) ? "." : root) + 1);
	strcpy(rootDirectory, (root == NULL) ? "." : root);
	if (rootDirectory[strlen(rootDirectory)-1] == '/') rootDirectory[strlen(rootDirectory)-1] = '\0';
	if (stat(rootDirectory, &rootstat) < 0)
	{
		printf("ERROR: (%s) does not exist\n", rootDirectory);
		return (CHECK_DIR_ERROR);
	}
	if (!S_ISDIR(rootstat.st_mode))
	{
		printf("ERROR: (%s) exists, but isn't a directory\n", rootDirectory);
		return (CHECK_DIR_ERROR);
	}

	// initialize socket
	hServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (hServerSocket == SOCKET_ERROR)
	{
		printf("ERROR: Failed to create socket\n");
		return (INIT_SERVER_ERROR);
	}

	// setup socket structure
	ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	ServerAddress.sin_port = htons(port);
	ServerAddress.sin_family = AF_INET;

	// bind to a port
	if (bind(hServerSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
	{
		printf("ERROR: Failed to bind socket to port\n");
		return (INIT_SERVER_ERROR);
	}

	// establish a listen queue
	if (listen(hServerSocket, BACKLOG) == SOCKET_ERROR)
	{
		printf("ERROR: Server failed to listen\n");
		return (INIT_SERVER_ERROR);
	}

	// establish safeq (for passing connections to workers)
	if (ml_safeq_initialize())
	{
		printf("ERROR: Safeq failed to initialize\n");
		return (INIT_SERVER_ERROR);
	}

	// create and launch worker threads
	workers = (_workers == 0 ? ml_DEFAULT_WORKERS_NUMBER : _workers);
	assert(workers > 0 && workers <= ml_WORKERS_MAX);
	workerPool = (pthread_t*) malloc (workers * sizeof(pthread_t));
	workerArgs = (int*) malloc (workers * sizeof(int));
	for (i = 0; i < workers; ++i)
	{
		workerArgs[i] = i;
		if ((pthread_create(&workerPool[i], NULL, ml_worker, (void*)&workerArgs[i])) != SUCCESS)
		{
			printf("ERROR: Failed to create thread (%d)", i);
			return (INIT_SERVER_ERROR);
		}
	}

	// report setup results
	getsockname(hServerSocket, (struct sockaddr*)&ServerAddress, (socklen_t*)&nAddressSize);
	printf("  server_ip = %s\n", inet_ntoa(ServerAddress.sin_addr));
	printf("  port      = %d\n", ntohs(ServerAddress.sin_port));
	printf("  directory = %s\n", rootDirectory);
	printf("  workers   = %d\n", workers);
	printf("\n");

	return (SUCCESS);
}

static int run(void)
{
	int hSocket;
	static struct sockaddr_in ClientAddress;
	int nAddressSize = sizeof(struct sockaddr_in);

	printf("RUNNING Server...\n");
	while(!terminate)
	{
		hSocket = accept(hServerSocket, (struct sockaddr*)&ClientAddress, (socklen_t*)&nAddressSize);
		//printf("(socket %d) <- New connection from (machine %s) on (port %d)\n", hSocket, inet_ntoa(ClientAddress.sin_addr), ntohs(ClientAddress.sin_port));
		ml_safeq_put(hSocket);
	}

	return 0;
}
*/
