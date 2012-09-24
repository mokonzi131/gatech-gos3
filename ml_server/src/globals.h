// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#ifndef ML_GLOBALS
#define ML_GLOBALS

/* status definitions (type int) */
#define	SUCCESS					0
#define	CMD_INPUTS_ERROR		-1
#define SERVER_ERROR			-2
#define SOCKET_ERROR			-1
#define INIT_SERVER_ERROR		-4
#define CHECK_DIR_ERROR			-5
#define SAFEQ_ERROR				-6
#define ML_HTTP_ERROR			-7

/* constants defined by server.c */
extern const unsigned short int ml_DEFAULT_PORT_NUMBER;
extern const unsigned short int ml_PORT_MAX;
extern const unsigned int ml_DEFAULT_WORKERS_NUMBER;
extern const unsigned int ml_WORKERS_MAX;

#endif
