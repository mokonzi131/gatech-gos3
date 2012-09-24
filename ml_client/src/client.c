/// Michael Landes
/// GaTech : GOS : Project 1
/// ////////////////////////

#include "client.h"

#include <stdio.h>

/// DATA ///

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

    int hSocket;                 /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;

    if(argc < 3)
      {
	printf("\nUsage: client host-name host-port\n");
	return 0;
      }
    else
      {
	strcpy(strHostName,argv[1]);
	nHostPort=atoi(argv[2]);
      }

    printf("\nMaking a socket");
    /* make a socket */
    hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(hSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* get IP address from name */
    pHostInfo=gethostbyname(strHostName);
    /* copy address into long */
    memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    /* fill address struct */
    Address.sin_addr.s_addr=nHostAddress;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    printf("\nConnecting to %s on port %d",strHostName,nHostPort);

    /* connect to host */
    if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address))
       == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }

    /* read from socket into buffer
    ** number returned by read() and write() is the number of bytes
    ** read or written, with -1 being that an error occured */
    nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
    printf("\nReceived \"%s\" from server\n",pBuffer);
    /* write what we received back to the server */
    write(hSocket,pBuffer,nReadAmount);
    printf("\nWriting \"%s\" to server",pBuffer);

    printf("\nClosing socket\n");
    /* close socket */
    if(close(hSocket) == SOCKET_ERROR)
    {
        printf("\nCould not close socket\n");
        return 0;
    }
