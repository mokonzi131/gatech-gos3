// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////
#include "globals.h"

#include "worker.h"

#include <stdio.h>
#include "safeq.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/* DATA */
/* PRIVATE INTERFACE */
/* PUBLIC INTERFACE */
#define BUFFER_SIZE         100
#define MESSAGE             "HTTP/1.0 404 Not Found\r\n Very Sorry Sir/Madame... \r\n\0"

void* ml_worker(void* argument)
{
	int tid;
	unsigned int socket = 0;

	tid = *((int*)argument);
	printf("Hello World from thread (%d)\n", tid);

	while(1)
	{
		static struct sockaddr_in ClientAddress;
	int nAddressSize = sizeof(struct sockaddr_in);
	char pBuffer[BUFFER_SIZE];

		ml_safeq_get(&socket);
		if (socket == 0)
			continue;


	// temp output checking...
	getsockname(socket, (struct sockaddr*)&ClientAddress, (socklen_t*)&nAddressSize);
		printf("Thread (%d) received new connection (socket=%d) from machine (%s) on port (%d):\n"
				, tid
				, socket
				, inet_ntoa(ClientAddress.sin_addr)
				, ntohs(ClientAddress.sin_port));
		// print stream
		read(socket, pBuffer, BUFFER_SIZE);
		printf("[%s]\n\n", pBuffer);
		strcpy(pBuffer, MESSAGE);
		write(socket, pBuffer, strlen(pBuffer)+1);
		// close socket
        if (close(socket) == SOCKET_ERROR)
        {
			printf("ERROR: Failed to close the socket\n");
			return -1;
        }

		socket = 0;
	}

	return NULL;
}

/* IMPLEMENTATION */

