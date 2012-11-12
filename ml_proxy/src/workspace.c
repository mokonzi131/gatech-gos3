/// Michael Landes
/// GaTech : GOS : Project 2
/// \\\///\\\///\\\///\\\///
#include "globals.h"

#include "workspace.h"

#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

/// DATA ///
//static const size_t INITIAL_CAPACITY = 1024;

//typedef struct {
//	unsigned int* array;
//	unsigned int* front;
//	unsigned int* back;
//	size_t capacity;
//	size_t size;
//} Sockets;

//static Sockets sockets = { NULL, NULL, NULL, 0, 0 }; /// protected by (m_qaccess)

//static pthread_mutex_t m_qaccess;
//static pthread_cond_t c_isItem; /// _get waits for items to exist in q
//static pthread_cond_t c_isSpace; /// _put waits for space in the q
ml_shm_workspace* space;

/// PRIVATE INTERFACE ///

/// PUBLIC INTERFACE ///
int ml_workspace_initialize(void)
{
	space = (ml_shm_workspace*) malloc (sizeof(ml_shm_workspace));
	space->shmkey = ftok("./", 'A');
	space->semkey = ftok("./", 'a');
	space->next = NULL;

	shmget(space->shmkey, SHM_BUF_SIZE, 0666 | IPC_CREAT);
	semget(space->semkey, 1, 0666 | IPC_CREAT);

	printf("workspace up\n");
	return (SUCCESS);
}

int ml_workspace_teardown(void)
{
	int shmid, semid;

	if ((shmid = shmget(space->shmkey, 0, 0)) != ERROR)
		shmctl(shmid, IPC_RMID, NULL);
	if ((semid = semget(space->semkey, 0, 0)) != ERROR)
		semctl(semid, 0, IPC_RMID);

	free(space);
	printf("workspace down\n");
	return (SUCCESS);
}

int ml_workspace_return(ml_shm_workspace* workspace)
{
	printf("workspace return space\n");
	return (SUCCESS);
}

ml_shm_workspace* ml_workspace_retrieve(void)
{
	printf("workspace get space\n");
	return space;
}

//int ml_safeq_initialize(void)
//{
//	sockets.array = (unsigned int*) malloc (sizeof(unsigned int) * INITIAL_CAPACITY);
//	sockets.front = sockets.back = sockets.array;
//	sockets.capacity = INITIAL_CAPACITY;
//	sockets.size = 0;
//	if (sockets.array == NULL) return (SAFEQ_ERROR);
//	return (SUCCESS);
//}

//int ml_safeq_teardown(void)
//{
//	free(sockets.array);
//	sockets.array = sockets.front = sockets.back = NULL;
//	sockets.capacity = 0;
//	sockets.size = 0;
//	return (SUCCESS);
//}

//int ml_safeq_put(unsigned int socket)
//{
//	pthread_mutex_lock(&m_qaccess);
//	{
//		assert(sockets.array != NULL);
		// wait for space in the q
//		while (sockets.size == sockets.capacity)
//			pthread_cond_wait(&c_isSpace, &m_qaccess);

		// place item into q
//		*(sockets.back) = socket;
//		++(sockets.size);
//		++(sockets.back);
//		if (sockets.back == (sockets.array + sockets.capacity))
//			sockets.back = sockets.array;

//		pthread_cond_broadcast(&c_isItem);
//	}
	//printf("sq size = %d\n", sockets.size);
//	pthread_mutex_unlock(&m_qaccess);

//	return (SUCCESS);
//}

//int ml_safeq_get(void)
//{
//	int socket;
//	pthread_mutex_lock(&m_qaccess);
//	{
//		assert(sockets.array != NULL);
		// wait for an actual item in the q
//		while(sockets.size == 0)
//			pthread_cond_wait(&c_isItem, &m_qaccess);

		// get item off the q
//		socket = *(sockets.front);
//		--(sockets.size);
//		++(sockets.front);
//		if (sockets.front == (sockets.array + sockets.capacity))
//			sockets.front = sockets.array;

//		pthread_cond_signal(&c_isSpace);
//	}
//	pthread_mutex_unlock(&m_qaccess);

//	return socket;
//}
