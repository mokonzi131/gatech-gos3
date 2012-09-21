// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#include "safeq.h"

#include <pthread.h>
#include <stdio.h>

/* DATA */
// (currentSocket) protected by (qaccess) //
static unsigned int currentSocket = 0;
static pthread_mutex_t qaccess;

/* PUBLIC INTERFACE */
int ml_safeq_put(unsigned int socket)
{
	pthread_mutex_lock(&qaccess);
	{
		currentSocket = socket;
	}
	pthread_mutex_unlock(&qaccess);

	return 0;
}

int ml_safeq_get(unsigned int* psocket)
{
	pthread_mutex_lock(&qaccess);
	{
		if (currentSocket != 0)
		{
			*psocket = currentSocket;
			currentSocket = 0;
		}
	}
	pthread_mutex_unlock(&qaccess);

	return 0;
}

/* PRIVATE INTERFACE */

