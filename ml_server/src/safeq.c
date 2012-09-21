// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////
#include "globals.h"

#include "safeq.h"

#include <pthread.h>
#include <stdio.h>

/* DATA */
/// (currentSocket) protected by (qaccess) ///
static unsigned int currentSocket = 0;
static pthread_mutex_t qaccess;
static pthread_cond_t cconnections;

/* PRIVATE INTERFACE */

/* PUBLIC INTERFACE */
int ml_safeq_put(unsigned int socket)
{
	pthread_mutex_lock(&qaccess);
	{
		currentSocket = socket;
		pthread_cond_broadcast(&cconnections);
	}
	pthread_mutex_unlock(&qaccess);

	return 0;
}

int ml_safeq_get(unsigned int* psocket)
{
	pthread_mutex_lock(&qaccess);
	{
		while(currentSocket == 0)
			pthread_cond_wait(&cconnections, &qaccess);
		if (currentSocket != 0)
		{
			*psocket = currentSocket;
			currentSocket = 0;
		}
	}
	pthread_mutex_unlock(&qaccess);

	return 0;
}

/* IMPLEMENTATION */

