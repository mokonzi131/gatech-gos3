/// Michael Landes
/// GaTech : GOS : Project 1
/// ////////////////////////

#ifndef ML_CLIENT
#define ML_CLIENT

typedef enum {
	SOCKET_ERROR = -1,
	SUCCESS = 0,
	FAILURE = -1,
	CMD_INPUTS_ERROR = -2,
} ml_error_t;

ml_error_t ml_client(char*, unsigned short int, unsigned int, unsigned int);

#endif
