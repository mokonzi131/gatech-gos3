/// Michael Landes
/// GaTech : GOS : Project 1
/// ////////////////////////

#ifndef ML_CLIENT
#define ML_CLIENT

typedef enum {
	SUCCESS,
	FAILURE,
	CMD_INPUTS_ERROR
} ml_error_t;

ml_error_t ml_client(char*, unsigned short int, unsigned int, unsigned int);

#endif
