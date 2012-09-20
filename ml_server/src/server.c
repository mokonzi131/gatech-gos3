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

	return 0;
}

#define BUFFER_SIZE         100
#define MESSAGE             "This is the message I'm sending back and forth"
static int run(void)
{
	int hSocket;
	static struct sockaddr_in ClientAddress;
	int nAddressSize = sizeof(struct sockaddr_in);

	//struct hostent* pHostInfo; // Internet socket address struct
	//char pBuffer[BUFFER_SIZE];

	printf("RUNNING Server...\n");

	while(1)
	{
		printf("Waiting for a connection\n");
		hSocket = accept(hServerSocket, (struct sockaddr*)&ClientAddress, (socklen_t*)&nAddressSize);
		printf("connection\n\
			sin_family			= %d\n\
			sin_addr.s_addr		= %s\n\
			sin_port			= %d\n"
			, ClientAddress.sin_family
			, inet_ntoa(ClientAddress.sin_addr)
			, ntohs(ClientAddress.sin_port));
	}

	return 0;
}
/*
#include <netdb.h>
#include <unistd.h>

    for(;;)
    {
	printf("\nGot a connection");
        strcpy(pBuffer,MESSAGE);
	printf("\nSending \"%s\" tolisten client",pBuffer);
        //number returned by read() and write() is the number of bytes
        // read or written, with -1 being that an error occured
        // write what we received back to the server
        write(hSocket,pBuffer,strlen(pBuffer)+1);
        // read from socket into buffer
        read(hSocket,pBuffer,BUFFER_SIZE);

        if(strcmp(pBuffer,MESSAGE) == 0)
            printf("\nThe messages match");
        else
            printf("\nSomething was changed in the message");

	printf("\nClosing the socket");
        // close socket
        if(close(hSocket) == SOCKET_ERROR)
        {
         printf("\nCould not close socket\n");
         return 0;
        }
    }
*/
