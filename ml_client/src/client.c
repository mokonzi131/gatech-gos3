/// Michael Landes
/// GaTech : GOS : Project 1
/// ////////////////////////

#include "client.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

/// DATA ///
static struct sockaddr_in RemoteAddress;
static const ml_DEFAULT_WORKERS = 5;
static pthread_t* workerPool;
static ml_client_worker* workerArgs;
static int workerPoolSize;
static pthread_attr_t attr;
static pthread_mutex_t m_ready;
static pthread_cond_t c_start;
static bool ready = false;

/// PRIVATE INTERFACE ///
static ml_error_t initialize(char*, unsigned short int, unsigned int, unsigned int);
static ml_error_t run(void);
static void* ml_client_work(void*);

/// PUBLIC INTERFACE ///
ml_error_t ml_client(char* server, unsigned short int port, unsigned int workers, unsigned int requests)
{
	ml_error_t result;

	result = initialize(server, port, workers, requests);
	switch(result)
	{
		case (SUCCESS):
			break;
		default:
			printf("ERROR: Failed to initialize the Client!\n");
			return result;
	}

	result = run();
	switch(result)
	{
		case (SUCCESS):
			break;
		default:
			printf("ERROR: Client is down\n");
			return result;
	}

	return result;
}

/// IMPLEMENTATION ///
static ml_error_t initialize(char* server, unsigned short int port, unsigned int workers, unsigned int requests)
{
	struct hostent* host;
	long hostAddress;
	int initSocket, i;

	// Setup connection to the remote server
	printf("Testing Connnetion to %s on port %d\n", server, port);
	host = gethostbyname(server);
	memcpy(&hostAddress, host->h_addr, host->h_length);

	RemoteAddress.sin_addr.s_addr = hostAddress;
	RemoteAddress.sin_port = htons(port);
	RemoteAddress.sin_family = AF_INET;

	initSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connect(initSocket, (struct sockaddr*)&RemoteAddress, sizeof(RemoteAddress)) == SOCKET_ERROR)
	{
		printf("ERROR: Could not open connection to remote server\n");
		return (FAILURE);
	}

	// build list of indexing url's
	// TODO
	//	char pBuffer[100];
	//	unsigned nReadAmount;
	//	char strHostName[255];
	//close(initSocket);

	// create and launch worker threads
	assert(workers > 0);
	workerPoolSize = workers;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	workerPool = (pthread_t*) malloc (workerPoolSize * sizeof(pthread_t));
	workerArgs = (ml_client_worker*) malloc (workerPoolSize * sizeof(ml_client_worker));

	for (i = 0; i < workerPoolSize; ++i)
	{
		workerArgs[i].tid = i;
		workerArgs[i].numberRequests = requests;
		workerArgs[i].successes = 0;
		if ((pthread_create(&workerPool[i], &attr, ml_client_work, (void*)&workerArgs[i])) != SUCCESS)
		{
			printf("ERROR: Failed to create thread (%d)", i);
			return (FAILURE);
		}
	}
	// SIGNAL CONDITION

	return (SUCCESS);
}

static ml_error_t run(void)
{
	void* status;
	int i, rc;

	// TODO setup timer
	pthread_mutex_lock(&m_ready);
	{
		printf("READY\n");
		ready = true;
		sleep(1);
		printf("GO!\n"); /// -> start timer
	}
	pthread_mutex_unlock(&m_ready);
	pthread_cond_broadcast(&c_start);

	// join the threads
	pthread_attr_destroy(&attr);
	for(i = 0; i < workerPoolSize; ++i)
	{
		rc = pthread_join(workerPool[i], &status);
		if (rc) printf("ERROR RC\n");
	}
	printf("FINISHED\n"); /// -> stop timer

	// cleanup
	free(workerPool);
	free(workerArgs);
	pthread_exit(NULL);

	return (FAILURE);
}

static void* ml_client_work(void* input)
{
	ml_error_t status = SUCCESS;
	ml_client_worker* data = (ml_client_worker*) input;
	char buffer[1024];

	// wait for the signal from the main thread before starting (for timing purposes)
	pthread_mutex_lock(&m_ready);
	while(!ready)
		pthread_cond_wait(&c_start, &m_ready);
	pthread_mutex_unlock(&m_ready);

	// send the requests
	printf("Worker %d handling %d requests\n", data->tid, data->numberRequests);
	while (data->numberRequests > 0)
	{
		data->hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connect(data->hSocket, (struct sockaddr*)&RemoteAddress, sizeof(RemoteAddress)) == SOCKET_ERROR)
		{
			continue;
		}
		write(data->hSocket, "GET / HTTP/1.0\r\n\r\n", 23);
		while(read(data->hSocket, buffer, 1024) > 0);
		++(data->successes);
		--(data->numberRequests);
	}
	printf("Worker %d had %d successes\n", data->tid, data->successes);
//	nReadAmount = read(hSocket, pBuffer, 1024);
//	printf("Read %d: %s\n", nReadAmount, pBuffer);

//	if (close(hSocket) == SOCKET_ERROR)
//	{
//		printf("Couldn't close socket\n");
//		return (FAILURE);
//	}
	//close(data->hSocket);

	pthread_exit((void*) status);
}
