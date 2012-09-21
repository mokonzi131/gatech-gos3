// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#ifndef ML_SAFEQ
#define ML_SAFEQ

/// ret error (int value to enq) ///
int ml_safeq_put(unsigned int);

/// ret error (pointer to int to deq) ///
int ml_safeq_get(unsigned int*);

/// ret error (void) ///
int ml_safeq_initialize(void);

/// ret error (void) ///
int ml_safeq_teardown(void);

#endif

///////////////////////////
// initialize should always be called before _get or _put are ever called
// teardown should always be called when the q is done being used
///////////////////////////
