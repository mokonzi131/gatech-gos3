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

/* DATA */
const unsigned short int ml_DEFAULT_PORT_NUMBER = 51115;

static unsigned short int portNumber;
static char hostMachine[512];
static char rootDirectory[512];

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
static int initialize(const unsigned short int _port, const char* _root)
{
	printf("INITIALIZING Server...\n");

	// setup the root directory
	strcpy(rootDirectory, "hello");

	// setup the host machine
	strcpy(hostMachine, "localhost");

	// setup the port
	portNumber = (_port == 0 ? ml_DEFAULT_PORT_NUMBER : _port);
	assert(portNumber > 0 && portNumber <= 65535);

	printf("   port = %d\n", portNumber);
	printf("   root = %s\n", rootDirectory);

	return 0;
}

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define MESSAGE             "This is the message I'm sending back and forth"
#define QUEUE_SIZE          5
static int run(void)
{
	int hServerSocket; // handles to socket
	int hSocket;
	//struct hostent* pHostInfo; // Internet socket address struct
	struct sockaddr_in Address; // Internet socket address struct
	int nAddressSize = sizeof(struct sockaddr_in);
	//char pBuffer[BUFFER_SIZE];

	// initialize socket
	hServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (hServerSocket == SOCKET_ERROR)
	{
		printf("ERROR: Failed to create socket\n");
		return -1;
	}

	// setup socket structure
	Address.sin_addr.s_addr = htonl(INADDR_ANY);
	Address.sin_port = htons(portNumber);
	Address.sin_family = AF_INET;

	// bind to a port
	printf("Binding to port %d\n", portNumber);
	if (bind(hServerSocket, (struct sockaddr*)&Address, sizeof(Address)) == SOCKET_ERROR)
	{
		printf("ERROR: Failed to bind socket to port\n");
		return -1;
	}

	// report setup
	getsockname(hServerSocket, (struct sockaddr*)&Address, (socklen_t*)&nAddressSize);
	printf("Opened socket as fd (%d) on port (%d) for stream i/o\n", hServerSocket, ntohs(Address.sin_port));
	printf("server\n\
			sin_family			= %d\n\
			sin_addr.s_addr		= %s\n\
			sin_port			= %d\n"
			, Address.sin_family
			, inet_ntoa(Address.sin_addr)
			, ntohs(Address.sin_port));

	// establish a listen queue
	if (listen(hServerSocket, QUEUE_SIZE) == SOCKET_ERROR)
	{
		printf("ERROR: Server failed to listen\n");
		return -1;
	}

	printf("RUNNING Server...\n");

	while(1)
	{
		printf("Waiting for a connection\n");
		hSocket = accept(hServerSocket, (struct sockaddr*)&Address, (socklen_t*)&nAddressSize);
	}

	return 0;
}
/*
#include <netdb.h>
#include <unistd.h>

    for(;;)
    {
        printf("\nWaiting for a connection\n");
        // get the connected socket
        hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);

	printf("\nGot a connection");
        strcpy(pBuffer,MESSAGE);
	printf("\nSending \"%s\" to client",pBuffer);
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
}
*/
