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
//static char FAILURE_TO_INTERPRET[] = "SERVER failed to interpret your request, closing connection\n";
//static char UNHANDLED_PROTOCOL[] = "SERVER does not implement your protocol, closing connection\n";

/// PRIVATE INTERFACE ///
static void processConnection(int h_socket, char* readBuffer);

/// PUBLIC INTERFACE ///
void* ml_worker(void* argument)
{
	//int tid = *((int*)argument);
	unsigned int h_socket = 0;
	char* readBuffer = (char*) malloc (IO_BUF_SIZE * sizeof(char));

	//printf("hello worker %d\n", tid);

	while(1)
	{
		h_socket = ml_safeq_get();
		if (h_socket == 0) break;

		//printf("Thread (%d) handling socket (%d)\n", tid, h_socket);
		//usleep(100000);
		processConnection(h_socket, readBuffer);

		shutdown(h_socket, SHUT_RDWR);
		close(h_socket);
		h_socket = 0;
	}

	free(readBuffer);
	//printf("goodbye worker %d\n", tid);
	return 0;
}

/// IMPLEMENTATION ///
static void processConnection(int hSocket, char inBuffer[])
{
	int count = 0;
	ProxyRequest request;

	// read and parse request
	count = read(hSocket, inBuffer, IO_BUF_SIZE);
	ml_http_parseRequest(&request, inBuffer, count);

	// dump request data to console
	printf("%d chars = [%s]\n", count, inBuffer);
	printf("==========\n");
	if (request.status == (SUCCESS)) printf("status = SUCCESS\n");
	else printf("status = ERROR\n");
	if (request.method == GET) printf("method = GET\n");
	else printf("method = UNHANDLED\n");
	if (request.schema == HTTP) printf("schema = HTTP\n");
	else printf("schema = UNHANDLED\n");
	printf("host = %.*s\n", request.host_len, request.host);
	printf("port = %d\n", request.port);
	printf("uri = %.*s\n", request.uri_len, request.uri);

//	// read the request, if there is no request line, END
//	result = ml_http_readLine(hSocket, inBuffer);
//	if (result != SUCCESS)
//	{
//		write(hSocket, FAILURE_TO_INTERPRET, strlen(FAILURE_TO_INTERPRET)+1);
//		return;
//	}

///	// if protocol is not HTTP, return un-handled protocol message END
///	if (!ml_http_isHTTP(inBuffer))
///	{
///		write(hSocket, UNHANDLED_PROTOCOL, strlen(UNHANDLED_PROTOCOL)+1);
///		return;
///	}
/// removed because the specs ask us to handle request in format: [GET <path>\r\n]

	// call processHTTPRequest()
//	statusLine = (char*) malloc (sizeof(char) * (strlen(inBuffer)+1));
//	strcpy(statusLine, inBuffer);
//	ml_http_processHTTPRequest(hSocket, inBuffer, statusLine);
//	free(statusLine);
}
