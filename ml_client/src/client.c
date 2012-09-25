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
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

/// DATA ///
static struct sockaddr_in RemoteAddress;
static const int ml_DEFAULT_WORKERS = 5;
static pthread_t* workerPool;
static ml_client_worker* workerArgs;
static int workerPoolSize;
static pthread_attr_t attr;
static pthread_mutex_t m_ready;
static pthread_cond_t c_start;
static bool ready = false;
static unsigned int numFiles = 1;
static char* call = "GET /file%d.html HTTP/1.0\r\n\r\n";

/// PRIVATE INTERFACE ///
static ml_error_t initialize(char*, unsigned short int, unsigned int, unsigned int, unsigned int);
static ml_error_t run(void);
static void* ml_client_work(void*);

/// PUBLIC INTERFACE ///
ml_error_t ml_client(char* server, unsigned short int port, unsigned int workers, unsigned int requests, unsigned int files)
{
	ml_error_t result;

	result = initialize(server, port, workers, requests, files);
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
static ml_error_t initialize(char* server, unsigned short int port, unsigned int workers, unsigned int requests, unsigned int files) /// TODO build file tree
{
	struct hostent* host;
	long hostAddress;
	int i; //initSocket, i;

	// files setup
	if (files > 0)
		numFiles = files;
	printf("Files: %d\n", files);

	// Setup connection to the remote server
	printf("Testing Connnetion to %s on port %d\n", server, port);
	host = gethostbyname(server);
	memcpy(&hostAddress, host->h_addr, host->h_length);

	RemoteAddress.sin_addr.s_addr = hostAddress;
	RemoteAddress.sin_port = htons(port);
	RemoteAddress.sin_family = AF_INET;

	//initSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//if (connect(initSocket, (struct sockaddr*)&RemoteAddress, sizeof(RemoteAddress)) == SOCKET_ERROR)
	//{
	//	printf("ERROR: Could not open connection to remote server\n");
	//	return (FAILURE);
	//}

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
		workerArgs[i].bytesRead = 0;
		if ((pthread_create(&workerPool[i], &attr, ml_client_work, (void*)&workerArgs[i])) != SUCCESS)
		{
			printf("ERROR: Failed to create thread (%d)", i);
			return (FAILURE);
		}
	}

	return (SUCCESS);
}

static ml_error_t run(void) /// TODO add timer
{
	void* status;
	int i, rc;
	clock_t t0, t1;
	long total = 0;
	float seconds = 0;

	// TODO setup timer
	pthread_mutex_lock(&m_ready);
	{
		ready = true;
		t0 = clock();
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
	t1 = clock();

	// report results
	for(i = 0; i < workerPoolSize; ++i)
		total += workerArgs[i].bytesRead;
	seconds = (float)(t1 - t0)/CLOCKS_PER_SEC;
	printf("TIME: %f seconds\n", seconds);
	printf("Bytes: %ld B\n", total);
	printf("B/s: %f\n", (float)total/seconds);

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
	int bytes = 0;
	char request[1024];

	// wait for the signal from the main thread before starting (for timing purposes)
	pthread_mutex_lock(&m_ready);
	while(!ready)
		pthread_cond_wait(&c_start, &m_ready);
	pthread_mutex_unlock(&m_ready);

	// send the requests, count bytes returned
	printf("Worker %d handling %d requests\n", data->tid, data->numberRequests);
	while (data->numberRequests > 0)
	{
		--(data->numberRequests);

		if((data->hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
			continue;
		if (connect(data->hSocket, (struct sockaddr*)&RemoteAddress, sizeof(RemoteAddress)) == SOCKET_ERROR)
			continue;

		sprintf(request, call, (rand() / ( RAND_MAX / numFiles + 1 )));
		write(data->hSocket, request, strlen(request));
		fcntl(data->hSocket, F_SETFL, (fcntl(data->hSocket, F_GETFL) | O_NONBLOCK));
		while(1)
		{
			bytes = read(data->hSocket, buffer, 1024);
			if (bytes >= 0) data->bytesRead += bytes;
			else if (bytes == -1 && errno == (EAGAIN | EWOULDBLOCK)) continue;
			else break;
		}
		++(data->successes);

		if(close(data->hSocket) == SOCKET_ERROR)
			printf("FAIL\n");
	}
	printf("Worker %d had %d successes and read %ld bytes\n", data->tid, data->successes, data->bytesRead);

	pthread_exit((void*) status);
}
