// Michael Landes
// GaTech : GOS : Project 2
///////////////////////////
#include "globals.h"

#include "worker.h"

#include <stdio.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "safeq.h"
#include "http.h"

/// DATA ///

/// PRIVATE INTERFACE ///
static void processConnection(int h_socket, char* buffer);
static void processProxyRequest(int hSocket, char* buffer, RequestStatus* client_status);

/// PUBLIC INTERFACE ///
void* ml_worker(void* argument)
{
	//int tid = *((int*)argument);
	unsigned int h_socket = 0;
	char* buffer = (char*) malloc (IO_BUF_SIZE * sizeof(char));

	//printf("hello worker %d\n", tid);

	while(1)
	{
		h_socket = ml_safeq_get();
		if (h_socket == 0) break;

		//printf("Thread (%d) handling socket (%d)\n", tid, h_socket);
		//usleep(100000);
		processConnection(h_socket, buffer);

		// the great shutdown mystery...???
		//shutdown(h_socket, SHUT_WR);
		close(h_socket);
	}

	free(buffer);
	//printf("goodbye worker %d\n", tid);
	return 0;
}

/// IMPLEMENTATION ///
static void processConnection(int hSocket, char* buffer)
{
	int count = 0;
	RequestStatus client_status;

	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;
	setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof tv);

	// read once from client
	if ((count = read(hSocket, buffer, IO_BUF_SIZE)) < 0)
	{
		ml_http_sendProxyError(hSocket, "ML_PROXY: Timing out after waiting for data...");
		return;
	}
	//printf("%d chars = [%.*s]\n", count, count, buffer);

	// parse client header, validate
	ml_http_parseStatus(&client_status, buffer, count);
	if (client_status.status != (SUCCESS))
	{
		ml_http_sendProxyError(hSocket, "ML_PROXY: Failed to parse request...");
		return;
	}
	client_status.port = client_status.port <= 0 || client_status.port > MAX_PORT ? 80 : client_status.port;

	// make sure it fits our standards for a 'proxy request'
	if (client_status.method != GET) /// HTTP protocol assumed because spec is [GET <path>\r\n]
	{
		ml_http_sendProxyError(hSocket, "ML_PROXY: Invalid method type...");
		return;
	}
	if (client_status.schema != HTTP)
	{
		ml_http_sendProxyError(hSocket, "ML_PROXY: Invalid schema type...");
		return;
	}

	processProxyRequest(hSocket, buffer, &client_status);
}

static void processProxyRequest(int hClient, char* buffer, RequestStatus* client_status)
{
	char request[512];
	char clientName[124];
	struct hostent hostbuf;
	struct hostent* hp;
	size_t hostbuflen = 1024;
	char* tmphostbuf = (char*) malloc (hostbuflen);
	int res;
	int herr;

	strncpy(clientName, client_status->host, client_status->host_len);
	printf("%s\n", clientName);
	while ((res = gethostbyname2_r(clientName, AF_INET,
			&hostbuf, tmphostbuf, hostbuflen,
			&hp, &herr)) == ERANGE)
			{
				hostbuflen *= 2;
				tmphostbuf = (char*) realloc (tmphostbuf, hostbuflen);
			}
	if (res || hp == NULL)
	{
		printf("NULL\n");
		return;
	}

	struct sockaddr_in RemoteAddress;
	int hServer;

	long hostAddress;
	memcpy(&hostAddress, hp->h_addr, hp->h_length);

	RemoteAddress.sin_addr.s_addr = hostAddress;
	RemoteAddress.sin_port = htons(client_status->port);
	RemoteAddress.sin_family = AF_INET;

	hServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// formulate server request
	sprintf(request, "GET %.*s HTTP/1.0\r\n", client_status->uri_len, client_status->uri);
	sprintf(request + strlen(request), "Host: %.*s:%d\r\n",
			client_status->host_len, client_status->host, client_status->port);
	sprintf(request + strlen(request), "User-Agent: ML_PROXY/1.0\r\n\r\n");

	// send server request
	if (connect(hServer, (struct sockaddr*)&RemoteAddress, sizeof(RemoteAddress)) != (SUCCESS))
	{
		ml_http_sendProxyError(hClient, "ML_PROXY: Failed to connect to remote server...");
		return;
	}
	write(hServer, request, strlen(request));

	// pipe response back to client
	int bytes = 0;
	while((bytes = read(hServer, buffer, IO_BUF_SIZE)) > 0)
	{
		write(hClient, buffer, strlen(buffer));
		//printf("%s", buffer);
	}
}
