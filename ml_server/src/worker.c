// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#include "worker.h"

#include <stdio.h>

/* DATA */

/* PUBLIC INTERFACE */
void* ml_worker(void* argument)
{
	int tid;

	tid = *((int*)argument);
	printf("Hello World from thread (%d)\n", tid);

	// pull connection off the SQ here
	// handle the connection
	// close connection

	return NULL;
}

/* PRIVATE INTERFACE */

