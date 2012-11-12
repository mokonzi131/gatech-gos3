/// Michael Landes
/// GaTech : GOS : Project 2
/// \\\///\\\///\\\///\\\///

#include <netinet/in.h>

#ifndef ML_GLOBALS
#define ML_GLOBALS

/// SHARED DEFINITIONS ///
#define	SUCCESS					0
#define DEFAULT_PROXY_PORT		50080
#define MAX_PORT				65535
#define DEFAULT_PROXY_WORKERS	5
#define MAX_WORKERS				50
#define ERROR					-1
#define SAFEQ_ERROR				-2
#define IO_BUF_SIZE 			1000
#define SHM_BUF_SIZE			4096
#define TIMEOUT_SEC				10

/// PROXY.C DEFINED GLOBAL ACCESS ///
extern int TERMINATE;
extern int useSharedMode(in_addr_t client_addr, int hServer);

/// SHM types ///
typedef struct {
	int done;
	int size;
} ml_shm_header;
typedef struct {
	ml_shm_header header;
	char data[SHM_BUF_SIZE - sizeof(ml_shm_header)];
} ml_shm_block;

typedef struct {
	key_t shmkey;
	key_t semkey;
} ml_shm_workspace;

#endif
