// Michael Landes
// GaTech : GOS : Project 2
///////////////////////////
#include "globals.h"

#include "worker.h"

#include <stdio.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#include "safeq.h"
#include "http.h"

/// DATA ///

/// PRIVATE INTERFACE ///
static void processConnection(int h_socket, char* readBuffer, char* writeBuffer);
static void processProxyRequest(int hSocket, char* inBuffer, char* outBuffer, RequestStatus* client_status);

/// PUBLIC INTERFACE ///
void* ml_worker(void* argument)
{
	//int tid = *((int*)argument);
	unsigned int h_socket = 0;
	char* readBuffer = (char*) malloc (IO_BUF_SIZE * sizeof(char));
	char* writeBuffer = (char*) malloc (IO_BUF_SIZE * sizeof(char));

	//printf("hello worker %d\n", tid);

	while(1)
	{
		h_socket = ml_safeq_get();
		if (h_socket == 0) break;

		//printf("Thread (%d) handling socket (%d)\n", tid, h_socket);
		//usleep(100000);
		processConnection(h_socket, readBuffer, writeBuffer);

		// the great shutdown mystery...???
		//shutdown(h_socket, SHUT_WR);
		close(h_socket);
	}

	free(readBuffer);
	free(writeBuffer);
	//printf("goodbye worker %d\n", tid);
	return 0;
}

/// IMPLEMENTATION ///
static void processConnection(int hSocket, char* inBuffer, char* outBuffer)
{
	int count = 0;
	RequestStatus client_status;

	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;
	setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof tv);

	// read once from client
	if ((count = read(hSocket, inBuffer, IO_BUF_SIZE)) < 0)
	{
		ml_http_sendProxyError(hSocket, "ML_PROXY: Timing out after waiting for data...");
		return;
	}
	//printf("%d chars = [%.*s]\n", count, count, inBuffer);

	// parse client header, validate
	ml_http_parseStatus(&client_status, inBuffer, count);
	if (client_status.status != (SUCCESS))
	{
		ml_http_sendProxyError(hSocket, "ML_PROXY: Failed to parse request...");
		return;
	}

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

	processProxyRequest(hSocket, inBuffer, outBuffer, &client_status);
}

static void processProxyRequest(int hSocket, char* inBuffer, char* outBuffer, RequestStatus* client_status)
{

	// formulate server request

	// send server request

	// pass response back to client

	// dump client_status data to console
	if (client_status->status == (SUCCESS)) printf("status = SUCCESS\n");
	else printf("status = ERROR\n");
	if (client_status->method == GET) printf("method = GET\n");
	else printf("method = UNHANDLED\n");
	if (client_status->schema == HTTP) printf("schema = HTTP\n");
	else printf("schema = UNHANDLED\n");
	printf("host = %.*s\n", client_status->host_len, client_status->host);
	printf("port = %d\n", client_status->port);
	printf("uri = %.*s\n", client_status->uri_len, client_status->uri);

}
