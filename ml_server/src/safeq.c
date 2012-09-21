// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////
#include "globals.h"

#include "safeq.h"

#include <pthread.h>
#include <stdio.h>
#include <assert.h>

/* DATA */
typedef struct {
	unsigned int* array;
	unsigned int* front;
	unsigned int* back;
	size_t capacity;
	size_t size;
} Sockets;

static Sockets sockets = { NULL, NULL, NULL, 0, 0 };	/// protected by (qaccess)

/// (currentSocket) protected by (qaccess) with condition (cconnections) ///
static unsigned int currentSocket = 0;
static pthread_mutex_t qaccess;
static pthread_cond_t cconnections;

/* PRIVATE INTERFACE */
static void initialize(void); 	/// protected by (qaccess)
static void expand(void);		/// protected by (qaccess)
static void teardown(void);		/// protected by (qaccess)

/* PUBLIC INTERFACE */
int ml_safeq_put(unsigned int socket)
{
	//assert(sockets.array != NULL);
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
	//assert(sockets.array != NULL);
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
static void initialize(void)
{
	// TODO
}

static void expand(void)
{
	// TODO
}

static void teardown(void)
{
	// TODO
}
