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

/// DATA ///
static int hSocket;

/// PRIVATE INTERFACE ///
static ml_error_t initialize(char*, unsigned short int, unsigned int, unsigned int);
static ml_error_t run(void);

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
	struct hostent* pHostInfo;
	struct sockaddr_in Address;
	long nHostAddress;
	char pBuffer[100];
	unsigned nReadAmount;
	char strHostName[255];
	int nHostPort = port;

	hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (hSocket == SOCKET_ERROR)
	{
		printf("ERROR: Could not make a socket\n");
		return (FAILURE);
	}

	strcpy(strHostName, server);
	pHostInfo = gethostbyname(strHostName);
	memcpy(&nHostAddress, pHostInfo->h_addr, pHostInfo->h_length);

	Address.sin_addr.s_addr = nHostAddress;
	Address.sin_port = htons(nHostPort);
	Address.sin_family = AF_INET;

	printf("Connecting to %s on port %d\n", strHostName, nHostPort);
	if (connect(hSocket, (struct sockaddr*)&Address, sizeof(Address)) == SOCKET_ERROR)
	{
		printf("Could not connect to host\n");
		return (FAILURE);
	}

	write (hSocket, "GET / HTTP/1.0\r\n\r\n", 23);
	nReadAmount = read(hSocket, pBuffer, 1024);
	printf("Read %d: %s\n", nReadAmount, pBuffer);

	if (close(hSocket) == SOCKET_ERROR)
	{
		printf("Couldn't close socket\n");
		return (FAILURE);
	}

	// set variables
	// check access to server
	// build a list of file URL's
	// initialize threads
	return (FAILURE);
}

static ml_error_t run(void)
{
	// create threads
	// setup timer
	// release threads
	// join
	// stop timer
	return (FAILURE);
}
