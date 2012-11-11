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

#include "safeq.h"
#include "http.h"

/// PRIVATE INTERFACE ///
static void processConnection(int hClient, char* buffer);
static void processProxyRequest(int hClient, char* buffer, RequestStatus* client_status);
static void proxySocket(int hClient, int hServer, char* buffer, RequestStatus* client_status);
static void proxyShared();

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
	if (useSharedMode(remoteaddr->sin_addr.s_addr))
	{
		proxyShared();
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

	shutdown(hClient, 2);
}

static void proxyShared()
{
	printf("sharing...\n");
}
