// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#ifndef ML_SERVER
#define ML_SERVER

extern const unsigned short int ml_DEFAULT_PORT_NUMBER;
extern const unsigned short int ml_PORT_MAX;
extern const unsigned int ml_DEFAULT_WORKERS_NUMBER;
extern const unsigned int ml_WORKERS_MAX;

// (port number, directory to serve, number of threads)
int ml_server(const unsigned short int, const char*, const unsigned int);

#endif
