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
#include <stdbool.h>

/// DATA ///
static ml_shm_workspace* front;
static ml_shm_workspace* back;
static size_t size;
static pthread_mutex_t m_access; /// lock access to the list of workspaces
static pthread_cond_t c_resource; /// signals the existance of a resource on the list

/// PUBLIC INTERFACE ///
int ml_workspace_initialize(void)
{
	bool finished = false;
	front = back = NULL;
	size = 0;
	int secret = 0;

	while(!finished && size <= MAX_WORKERS)
	{
		ml_shm_workspace* space = (ml_shm_workspace*) malloc (sizeof(ml_shm_workspace));
		space->shmkey = ftok("./", secret);
		space->semkey = ftok("../", secret);
		space->next = NULL;

		if ((shmget(space->shmkey, SHM_BUF_SIZE, 0666 | IPC_CREAT)) == ERROR)
		{
			finished = true;
			free(space);
			break;
		}
		if ((semget(space->semkey, 1, 0666 | IPC_CREAT)) == ERROR)
		{
			int shmid;
			finished = true;
			if ((shmid = shmget(space->semkey, 0, 0)) != ERROR) shmctl(shmid, IPC_RMID, NULL);
			free(space);
			break;
		}

		ml_workspace_return(space);

		++secret;
	}

	return (SUCCESS);
}

int ml_workspace_teardown(void)
{
	pthread_mutex_lock(&m_access);
	{/////////////////////////////
		int shmid, semid;
		ml_shm_workspace* space;

		while(front != NULL)
		{
			space = front;
			front = front->next;

			if ((shmid = shmget(space->shmkey, 0, 0)) != ERROR)
				shmctl(shmid, IPC_RMID, NULL);
			if ((semid = semget(space->semkey, 0, 0)) != ERROR)
				semctl(semid, 0, IPC_RMID);

			free(space);
		}
		front = back = NULL;
		size = 0;
	}//////////////////////////////
	pthread_mutex_unlock(&m_access);

	return (SUCCESS);
}

int ml_workspace_return(ml_shm_workspace* space)
{
	pthread_mutex_lock(&m_access);
	{/////////////////////////////
		if (size == 0)
		{
			front = back = space;
		}
		else
		{
			back->next = space;
			back = space;
		}
		++size;
	}///////////////////////////////
	pthread_mutex_unlock(&m_access);
	pthread_cond_signal(&c_resource);

	return (SUCCESS);
}

ml_shm_workspace* ml_workspace_retrieve(void)
{
	ml_shm_workspace* space = NULL;

	pthread_mutex_lock(&m_access);
	{/////////////////////////////
		while(size < 1)
			pthread_cond_wait(&c_resource, &m_access);
	//////////////////////////////
		space = front;
		front = front->next;
		if (front == NULL) back = NULL;
		--size;
	}///////////////////////////////
	pthread_mutex_unlock(&m_access);

	space->next = NULL;
	return space;
}
