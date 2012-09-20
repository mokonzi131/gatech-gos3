// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#include "server.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SOCKET_ERROR    -1
#define BACKLOG			5

/* DATA */
const unsigned short int ml_DEFAULT_PORT_NUMBER = 51115;

static char rootDirectory[512];
static int hServerSocket;
static struct sockaddr_in ServerAddress;

static int initialize(const unsigned short int, const char*);
static int run(void);

/* PUBLIC INTERFACE */
int ml_server(const unsigned short int port, const char* root)
{
	int error;

	error = initialize(port, root);
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
static int initialize(unsigned short int port, const char* root)
{
	int nAddressSize = sizeof(struct sockaddr_in);

	printf("INITIALIZING Server...\n");

	// setup the port
	port = (port == 0 ? ml_DEFAULT_PORT_NUMBER : port);
	assert(port > 0 && port <= 65535);

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

	// report setup
	getsockname(hServerSocket, (struct sockaddr*)&ServerAddress, (socklen_t*)&nAddressSize);
	printf("  server_ip = %s\n", inet_ntoa(ServerAddress.sin_addr));
	printf("  port      = %d\n", ntohs(ServerAddress.sin_port));
	printf("  directory = %s\n", rootDirectory);
	printf("\n");

	return 0;
}

#define BUFFER_SIZE         100
#define MESSAGE             "HTTP/1.0 404 Not Found\r\n\r\n"
static int run(void)
{
	int hSocket;
	static struct sockaddr_in ClientAddress;
	int nAddressSize = sizeof(struct sockaddr_in);
	char pBuffer[BUFFER_SIZE];

	//struct hostent* pHostInfo; // Internet socket address struct

	printf("RUNNING Server...\n");

	while(1)
	{
		hSocket = accept(hServerSocket, (struct sockaddr*)&ClientAddress, (socklen_t*)&nAddressSize);

		// temp output checking...
		printf("Received new connection (socket=%d) from machine (%s) on port (%d):\n"
				, hSocket
				, inet_ntoa(ClientAddress.sin_addr)
				, ntohs(ClientAddress.sin_port));
		// print stream
		read(hSocket, pBuffer, BUFFER_SIZE);
		printf("[%s]\n\n", pBuffer);
		strcpy(pBuffer, MESSAGE);
		write(hSocket, pBuffer, strlen(pBuffer)+1);
		// close socket
        if (close(hSocket) == SOCKET_ERROR)
        {
			printf("ERROR: Failed to close the socket\n");
			return -1;
        }

		// place hSocket on queue
	}

	return 0;
}
