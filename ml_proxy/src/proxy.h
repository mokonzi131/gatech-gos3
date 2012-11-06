// Michael Landes
// GaTech : GOS : Project 2
///////////////////////////

#ifndef ML_PROXY
#define ML_PROXY

/**	Initialize and Start the proxy server **/
int ml_proxy(unsigned short int port, unsigned int workers, int shared);

/** Signal the proxy server to shutdown **/
void ml_proxy_shutdown();

#endif
