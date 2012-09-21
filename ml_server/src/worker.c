// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////
#include "globals.h"

#include "worker.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "safeq.h"

/* DATA */
/* PRIVATE INTERFACE */
/* PUBLIC INTERFACE */
#define BUFFER_SIZE         100
#define MESSAGE             "HTTP/1.0 404 Not Found\r\n Very Sorry Sir/Madame... \r\n\0"

void* ml_worker(void* argument)
{
	int tid = *((int*)argument);;
	unsigned int socket = 0;

	// test section
	char pBuffer[BUFFER_SIZE];
	// end test section

	printf("Hello World from thread (%d)\n", tid);

	while(1)
	{
		ml_safeq_get(&socket);
		if (socket == 0)
			continue;

		// temp checking...
		printf("Thread (%d) handling socket (%d)\n", tid, socket);
		// end temp checking...

		// print stream
		read(socket, pBuffer, BUFFER_SIZE);
		printf("[%s]\n\n", pBuffer);
		strcpy(pBuffer, MESSAGE);
		write(socket, pBuffer, strlen(pBuffer)+1);

		usleep(80000);
		// close socket
        if (close(socket) == SOCKET_ERROR)
        {
			printf("ERROR: Failed to close the socket\n");
			//return -1;
        }
		socket = 0;
	}

	return NULL;
}

/* IMPLEMENTATION */

