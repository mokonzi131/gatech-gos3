/// Michael Landes
/// GaTech : GOS : Project 3
/// \\\///\\\///\\\///\\\///
#include "globals.h"

#include "proxy.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <string.h>

#include "worker.h"
#include "safeq.h"

#define INIT_ERROR		-2
#define RUN_ERROR		-3

/// DATA ///
int TERMINATE = 0;
static const unsigned int BACKLOG = 30;

static unsigned short int p_port;
static unsigned int p_workers;
static int p_socket;
static struct sockaddr_in p_address;
static pthread_t* p_workerPool;
static int* p_workerArgs;

/// PRIVATE INTERFACE ///
static int initialize(unsigned short int port, unsigned int workers);
static int run(void);
static void teardown(void);

/// PUBLIC INTERFACE ///
int ml_proxy(unsigned short int port, unsigned int workers)
{
	int result;

	result = initialize(port, workers);
	switch(result)
	{
		case (SUCCESS):
			break;
		default:
			printf("PROXY: Failed to Initialize the proxy server...\n");
			goto cleanup;
	}

	result = run();
	switch(result)
	{
		case (SUCCESS):
			break;
		default:
			printf("PROXY: Proxy server failed while running...\n");
	}

cleanup:
	teardown();
	return result;
}

void ml_proxy_shutdown()
{
	shutdown(p_socket, SHUT_RDWR);
	TERMINATE = 1;
}

/// IMPLEMENTATAION ///
static int initialize(unsigned short int port, unsigned int workers)
{
	int i;

	printf("PROXY SERVER INITIALIZING [ml_proxy]\n");

	// setup the port
	p_port = ((port == 0 || port > MAX_PORT) ? DEFAULT_PROXY_PORT : port);

	// setup the socket
	p_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (p_socket == ERROR)
	{
		printf("INIT: Failed to create socket...\n");
		return (INIT_ERROR);
	}

	// bind port to socket
	p_address.sin_addr.s_addr = htonl(INADDR_ANY);
	p_address.sin_port = htons(p_port);
	p_address.sin_family = AF_INET;
	if (bind(p_socket, (struct sockaddr*)&p_address, sizeof(p_address)) == ERROR)
	{
		printf("INIT: Failed to bind socket to port (%d)...\n", p_port);
		return (INIT_ERROR);
	}

	// turn on listening
	if (listen(p_socket, BACKLOG) == ERROR)
	{
		printf("INIT: Proxy failed to listen on port...\n");
		return (INIT_ERROR);
	}

	// turn on the safeq
	if (ml_safeq_initialize())
	{
		printf("INIT: Safeq failed to initialize...\n");
		return (INIT_ERROR);
	}

	// setup the workers
	p_workers = ((workers == 0 || workers > MAX_WORKERS)? DEFAULT_PROXY_WORKERS : workers);
	p_workerPool = (pthread_t*) malloc (p_workers * sizeof(pthread_t));
	p_workerArgs = (int*) malloc (p_workers * sizeof(int));

	for (i = 0; i < p_workers; ++i)
	{
		p_workerArgs[i] = i;
		if ((pthread_create(&p_workerPool[i], NULL, ml_worker, (void*)&p_workerArgs[i])) != SUCCESS)
		{
			printf("INIT: Failed to create thread (%d)...\n", i);
			return (INIT_ERROR);
		}
	}

	return (SUCCESS);
}

static int run(void)
{
	socklen_t size = sizeof(struct sockaddr_in);
	int hClient;
	static struct sockaddr_in clientaddr;

	getsockname(p_socket, (struct sockaddr*)&p_address, &size);
	printf("PROXY SERVER RUNNING: [%d] workers\n", p_workers);
	printf("\tHost.......................%s\n", inet_ntoa(p_address.sin_addr));
	printf("\tPort.......................%d\n", ntohs(p_address.sin_port));

	while(!TERMINATE)
	{
		hClient = accept(p_socket, (struct sockaddr*)&clientaddr, &size);
		if (hClient <= 0) continue;
		//printf("(socket %d) <- New connection from (machine %s) on port %d\n", hClient, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		ml_safeq_put(hClient);
	}

	return (SUCCESS);
}

static void teardown(void)
{
	int i;
	void* status;

	printf("PROXY SERVER TEARDOWN...\n");

	// call back workers
	for (i = 0; i < p_workers; ++i) ml_safeq_put(0);
	for (i = 0; i < p_workers; ++i)
	{
		pthread_join(p_workerPool[i], &status);
	}

	ml_safeq_teardown();

	free(p_workerPool);
	free(p_workerArgs);
	close(p_socket);
}
