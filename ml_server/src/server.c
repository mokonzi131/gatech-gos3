// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "worker.h"
#include "safeq.h"

#define SOCKET_ERROR    -1

/* DATA */
const unsigned short int ml_DEFAULT_PORT_NUMBER = 51115;
const unsigned short int ml_PORT_MAX = 65535;
const unsigned int ml_DEFAULT_WORKERS_NUMBER = 5;
const unsigned int ml_WORKERS_MAX = 50;

const unsigned int BACKLOG = 20;

static char rootDirectory[512];
static int hServerSocket;
static struct sockaddr_in ServerAddress;
static unsigned int workers;

static int initialize(unsigned short int, const char*, unsigned int);
static int run(void);

/* PUBLIC INTERFACE */
int ml_server(unsigned short int port, const char* root, unsigned int workers)
{
	int error;

	error = initialize(port, root, workers);
	switch(error)
	{
		case 0:
			break;
		default:
			printf("ERROR: Could not initialize the server\n");
			return error;
	}

	error = run();
	switch(error)
	{
		case 0:
			break;
		default:
			printf("ERROR: Server is down\n");
			return error;
	}

	return 0;
}

/* PRIVATE INTERFACE */
static int initialize(unsigned short int port, const char* root, unsigned int _workers)
{
	int nAddressSize = sizeof(struct sockaddr_in);

	printf("INITIALIZING Server...\n");

	// setup the port
	port = (port == 0 ? ml_DEFAULT_PORT_NUMBER : port);
	assert(port > 0 && port <= ml_PORT_MAX);

	// setup the root directory TODO <<<<< ----------------------------------------
	strcpy(rootDirectory, "TODO");

	// initialize socket
	hServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (hServerSocket == SOCKET_ERROR)
	{
		printf("ERROR: Failed to create socket\n");
		return -1;
	}

	// setup socket structure
	ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	ServerAddress.sin_port = htons(port);
	ServerAddress.sin_family = AF_INET;

	// bind to a port
	if (bind(hServerSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
	{
		printf("ERROR: Failed to bind socket to port\n");
		return -1;
	}

	// establish a listen queue
	if (listen(hServerSocket, BACKLOG) == SOCKET_ERROR)
	{
		printf("ERROR: Server failed to listen\n");
		return -1;
	}

	// create and launch worker threads
	// TODO <<<<< --------------------------------------------------------------------
	workers = (_workers == 0 ? ml_DEFAULT_WORKERS_NUMBER : _workers);
	assert(workers > 0 && workers <= ml_WORKERS_MAX);

	// report setup results
	getsockname(hServerSocket, (struct sockaddr*)&ServerAddress, (socklen_t*)&nAddressSize);
	printf("  server_ip = %s\n", inet_ntoa(ServerAddress.sin_addr));
	printf("  port      = %d\n", ntohs(ServerAddress.sin_port));
	printf("  directory = %s\n", rootDirectory);
	printf("  workers   = %d\n", workers);
	printf("\n");

	return 0;
}

#define BUFFER_SIZE 100
static int run(void)
{
	int hSocket;
	static struct sockaddr_in ClientAddress;
	int nAddressSize = sizeof(struct sockaddr_in);
	char pBuffer[BUFFER_SIZE];
	// temp thread testing...
	pthread_t threads[10];
	int thread_args[10];
	int rc;
	int i;

	for (i = 0; i < 10; ++i)
	{
		thread_args[i] = i;
		printf("Creating thread (%d)\n", i);
		rc = pthread_create(&threads[i], NULL, ml_worker, (void*)&thread_args[i]);
		assert(rc == 0);
	}
	// end temp thread testing

	printf("RUNNING Server...\n");
	while(1)
	{
		hSocket = accept(hServerSocket, (struct sockaddr*)&ClientAddress, (socklen_t*)&nAddressSize);

		// place hSocket on queue
		printf("Server says (%d)\n", hSocket);

		ml_safeq_put(hSocket);
	}

	/* Thread testing issue
		for (i = 0; i < 10; ++i)
	{
		rc = pthread_join(threads[i], NULL);
		assert(rc == 0);
	}
	*/

	return 0;
}
