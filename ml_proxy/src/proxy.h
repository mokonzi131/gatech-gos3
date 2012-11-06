// Michael Landes
// GaTech : GOS : Project 2
///////////////////////////

#ifndef ML_PROXY
#define ML_PROXY

int ml_proxy(unsigned short int, unsigned int, int);
void ml_proxy_shutdown();

/// (port number, directory to serve, number of threads) ///
int ml_server(const unsigned short int, const char*, const unsigned int);

/// signal the server to shutdown ///
void ml_server_shutDown();

/// ret reference to root directory ///
char* ml_server_getRootDir();

#endif
