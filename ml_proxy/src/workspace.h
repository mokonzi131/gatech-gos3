/// Michael Landes
/// GaTech : GOS : Project 2
/// \\\///\\\///\\\///\\\///

#ifndef ML_WORKSPACE
#define ML_WORKSPACE

/// DATA TYPES ///
typedef struct ml_shm_workspace {
	key_t shmkey;
	key_t semkey;
	struct ml_shm_workspace* next;
} ml_shm_workspace;

/// METHODS ///
int ml_workspace_initialize(void);
int ml_workspace_teardown(void);

int ml_workspace_return(ml_shm_workspace*);
ml_shm_workspace* ml_workspace_retrieve(void);
//int ml_workspace_retrieve(ml_shm_workspace**);

#endif

///////////////////////////
// initialize should always be called before _get or _put are ever called
// teardown should always be called when the q is done being used
///////////////////////////
