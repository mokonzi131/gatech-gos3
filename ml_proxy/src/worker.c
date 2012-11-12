// Michael Landes
// GaTech : GOS : Project 2
///////////////////////////
#include "globals.h"

#include "worker.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <assert.h>

#include "safeq.h"
#include "http.h"
#include "workspace.h"

/// PRIVATE INTERFACE ///
static void processConnection(int hClient, char* buffer);
static void processProxyRequest(int hClient, char* buffer, RequestStatus* client_status);
static void proxySocket(int hClient, int hServer, char* buffer, RequestStatus* client_status);
static void proxyShared(int hClient, int hServer, RequestStatus* client_status);

/// PUBLIC INTERFACE ///
void* ml_worker(void* argument)
{
	//int tid = *((int*)argument);
	unsigned int hClient = 0;
	char* buffer = (char*) malloc (IO_BUF_SIZE * sizeof(char));
	//printf("hello worker %d\n", tid);

	while(1)
	{
		hClient = ml_safeq_get();
		if (hClient == 0) break;

		//printf("Thread (%d) handling socket (%d)\n", tid, hClient);
		if (!TERMINATE)
			processConnection(hClient, buffer);

		close(hClient);
	}

	free(buffer);
	//printf("goodbye worker %d\n", tid);
	return 0;
}

/// IMPLEMENTATION ///
static void processConnection(int hClient, char* buffer)
{
	int count = 0;
	RequestStatus client_status;

	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;
	setsockopt(hClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof tv);

	// read once from client
	if ((count = read(hClient, buffer, IO_BUF_SIZE)) < 0)
	{
		ml_http_sendProxyError(hClient, "ML_PROXY: Timing out after waiting for data...");
		return;
	}
	//printf("%d chars = [%.*s]\n", count, count, buffer);

	// parse client header, validate
	ml_http_parseStatus(&client_status, buffer, count);
	if (client_status.status != (SUCCESS))
	{
		ml_http_sendProxyError(hClient, "ML_PROXY: Failed to parse request...");
		return;
	}
	client_status.port = client_status.port <= 0 || client_status.port > MAX_PORT ? 80 : client_status.port;

	// make sure it fits our standards for a 'proxy request'
	if (client_status.method != GET) /// HTTP protocol assumed because spec is [GET <path>\r\n]
	{
		ml_http_sendProxyError(hClient, "ML_PROXY: Invalid method type...");
		return;
	}
	if (client_status.schema != HTTP)
	{
		ml_http_sendProxyError(hClient, "ML_PROXY: Invalid schema type...");
		return;
	}

	// handle a proxy GET request
	processProxyRequest(hClient, buffer, &client_status);
}

static void processProxyRequest(int hClient, char* buffer, RequestStatus* client_status)
{
	char clientName[124];
	char clientPort[8];

	int hServer;
	struct addrinfo hints;
	struct addrinfo* result;
	struct sockaddr_in* remoteaddr;

	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;

	// get string references to the host and the port
	strncpy(clientName, client_status->host, client_status->host_len);
	clientName[client_status->host_len] = '\0';
	memset(clientPort, 0, sizeof(clientPort));
	sprintf(clientPort, "%d", client_status->port);

	// find the remote server
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(clientName, clientPort, &hints, &result) != (SUCCESS))
	{
		ml_http_sendProxyError(hClient, "ML_PROXY: Could not resolve server's IP address, aborting...");
		goto cleanup;
	}

	// retrieve the remote address, and hServer
	remoteaddr = (struct sockaddr_in*)result->ai_addr;
	//printf("getaddrinfo: %s => %s:%ld\n", clientName, inet_ntoa(remoteaddr->sin_addr), ntohs(remoteaddr->sin_port));
	if ((hServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == (ERROR))
	{
		ml_http_sendProxyError(hClient, "ML_PROXY: Failed to open a socket, aborting...");
		goto cleanup;
	}
	setsockopt(hServer, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof tv);

	// connect to the socket
	if (connect(hServer, result->ai_addr, result->ai_addrlen) != (SUCCESS))
	{
		ml_http_sendProxyError(hClient, "ML_PROXY: Failed to connect to remote server, aborting...");
		goto cleanup;
	}

	// perform appropriate proxy task
	if (useSharedMode(remoteaddr->sin_addr.s_addr, hServer))
	{
		proxyShared(hClient, hServer, client_status);
	}
	else
	{
		proxySocket(hClient, hServer, buffer, client_status);
	}

cleanup:
	freeaddrinfo(result);
	close(hServer);
}

static void proxySocket(int hClient, int hServer, char* buffer, RequestStatus* client_status)
{
	char request[IO_BUF_SIZE];
	int bytes = 0;

	// construct the request
	if (strlen("GET . HTTP/1.0\r\n") + client_status->uri_len + strlen("Host: .:.\r\n")
		+ client_status->host_len + strlen("User-Agent: ML_PROXY/1.0\r\n\r\n") > IO_BUF_SIZE)
		return;
	sprintf(request, "GET %.*s HTTP/1.0\r\n", client_status->uri_len, client_status->uri);
	sprintf(request + strlen(request), "Host: %.*s:%d\r\n",
			client_status->host_len, client_status->host, client_status->port);
	sprintf(request + strlen(request), "User-Agent: ML_PROXY/1.0\r\n\r\n");

	// send request to server
	if (send(hServer, request, strlen(request), 0) == (ERROR))
	{
		ml_http_sendProxyError(hClient, "ML_PROXY: Failed to send request to remote server, aborting...");
		return;
	}

	shutdown(hServer, SHUT_WR);

	//  shuttle response back to client
	while((bytes = read(hServer, buffer, IO_BUF_SIZE)) != 0)
	{
		if (bytes == (ERROR)) break;
		bytes = write(hClient, buffer, bytes);
	}

	shutdown(hClient, SHUT_WR);
}

static void proxyShared(int hClient, int hServer, RequestStatus* client_status)
{
	int shmid = (ERROR);
	int semid = (ERROR);
	void* shmaddr = ((void*)ERROR);
	struct shmid_ds info = {};
	struct sembuf sb = {};
	char request[IO_BUF_SIZE];
	ml_shm_workspace* workspace = NULL;

	// get shm segment, get semaphore
	if ((workspace = ml_workspace_retrieve()) == NULL)
	{
		printf("ML_PROXY: Failed to obtain a sharing workspace, aborting...\n");
		goto CLEANUP;
	}
	//printf("using workspace shmkey %ld semkey %ld\n", workspace->shmkey, workspace->semkey);

	if ((shmid = shmget(workspace->shmkey, 0, 0)) == ERROR)
	{
		perror("ML_PROXY: Fialed to retrieve memory");
		goto CLEANUP;
	}
	if ((shmaddr = shmat(shmid, NULL, 0)) == ((void*)ERROR))
	{
		printf("ML_PROXY: Failed to attach shm segment %d, aborting...\n", errno);
		goto CLEANUP;
	}
	shmctl(shmid, IPC_STAT, &info);

	if ((semid = semget(workspace->semkey, 0, 0)) == ERROR)
	{
		perror("ML_PROXY: Failed to retrieve semaphore");
		goto CLEANUP;
	}
	if (semctl(semid, 0, SETVAL, 1) == (ERROR))
	{
		perror("semctl");
		goto CLEANUP;
	}

	sb.sem_num = 0;
	sb.sem_flg = 0;
	ml_shm_block* block = (ml_shm_block*)shmaddr;
	block->header.done = 0;
	block->header.size = 0;

	// send request through socket
	assert(sizeof(int) == sizeof(key_t));
	if ((strlen("SHM . . .\r\n") + sizeof(int) + sizeof(int)
			+ client_status->uri_len) > IO_BUF_SIZE)
	{
		printf("ML_PROXY: Failed to create request, aborting... %d\n", errno);
		goto CLEANUP;
	}
	sprintf(request, "SHM %.*s %d %d\r\n", client_status->uri_len, client_status->uri,
			(int)workspace->shmkey, (int)workspace->semkey);
	if (send(hServer, request, strlen(request), 0) == (ERROR))
	{
		printf("ML_PROXy: Failed to send request to remote server, aborting...\n");
		goto CLEANUP;
	}
	shutdown(hServer, SHUT_WR);

	// shuttle response back to client
	while(!block->header.done || block->header.size > 0)
	{
		sb.sem_op = -1;
		if (semop(semid, &sb, 1) == ERROR)
		{
			perror("SEM: Error in acquiring lock");
			goto CLEANUP;
		}
		/// CRITICAL ///

		size_t count = 0;
		while(count < block->header.size)
		{
			int bytes = write(hClient, block->data + count, block->header.size - count);
			if (bytes == ERROR) break;
			count += bytes;
		}
		block->header.size = 0;

		/// END CRITICAL ///
		sb.sem_op = 1;
		if (semop(semid, &sb, 1) == ERROR)
		{
			perror("SEM: Error in releasing lock");
			goto CLEANUP;
		}
	}

CLEANUP:
	shutdown(hClient, SHUT_WR);
	if (shmaddr != ((void*)ERROR))
	{
		if (shmdt(shmaddr) == (ERROR)) printf("SHM ERROR on detatch: %d\n", errno);
	}
	if (workspace != NULL) ml_workspace_return(workspace);
}
