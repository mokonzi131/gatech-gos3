// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#ifndef ML_SAFEQ
#define ML_SAFEQ

// ret error (int value we will enqueue)
int ml_safeq_put(unsigned int);
// ret error (pointer to int value we will set from our dequeue operation)
int ml_safeq_get(unsigned int*);

#endif
