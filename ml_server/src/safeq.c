// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////
#include "globals.h"

#include "safeq.h"

#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

/* DATA */
static const size_t INITIAL_CAPACITY = 1024;

typedef struct {
	unsigned int* array;
	unsigned int* front;
	unsigned int* back;
	size_t capacity;
	size_t size;
} Sockets;

static Sockets sockets = { NULL, NULL, NULL, 0, 0 }; /// protected by (m_qaccess)

static pthread_mutex_t m_qaccess;
static pthread_cond_t c_isItem; /// _get waits for items to exist in q
static pthread_cond_t c_isSpace; /// _put waits for space in the q

/* PRIVATE INTERFACE */

/* PUBLIC INTERFACE */
int ml_safeq_initialize(void)
{
	sockets.array = (unsigned int*) malloc (sizeof(int) * INITIAL_CAPACITY);
	sockets.front = sockets.back = sockets.array;
	sockets.capacity = INITIAL_CAPACITY;
	sockets.size = 0;
	if (sockets.array == NULL) return (SAFEQ_ERROR);
	return (SUCCESS);
}

int ml_safeq_teardown(void)
{
	free(sockets.array);
	sockets.array = sockets.front = sockets.back = NULL;
	sockets.capacity = 0;
	sockets.size = 0;
	return (SUCCESS);
}

int ml_safeq_put(unsigned int socket)
{
	pthread_mutex_lock(&m_qaccess);
	{
		assert(sockets.array != NULL);
		// wait for space in the q
		while (sockets.size == sockets.capacity)
			pthread_cond_wait(&c_isSpace, &m_qaccess);

		// place item into q
		*(sockets.back) = socket;
		++(sockets.size);
		++(sockets.back);
		if (sockets.back == (sockets.array + sockets.capacity))
			sockets.back = sockets.array;

		pthread_cond_broadcast(&c_isItem);
	}
	printf("sq size = %d\n", sockets.size);
	pthread_mutex_unlock(&m_qaccess);

	return (SUCCESS);
}

int ml_safeq_get(unsigned int* psocket)
{
	pthread_mutex_lock(&m_qaccess);
	{
		assert(sockets.array != NULL);
		// wait for an actual item in the q
		while(sockets.size == 0)
			pthread_cond_wait(&c_isItem, &m_qaccess);

		// get item off the q
		*psocket = *(sockets.front);
		--(sockets.size);
		++(sockets.front);
		if (sockets.front == (sockets.array + sockets.capacity))
			sockets.front = sockets.array;

		pthread_cond_signal(&c_isSpace);
	}
	pthread_mutex_unlock(&m_qaccess);

	return (SUCCESS);
}

/* IMPLEMENTATION */
