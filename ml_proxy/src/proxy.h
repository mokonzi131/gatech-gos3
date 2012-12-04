/// Michael Landes
/// GaTech : GOS : Project 3
/// \\\///\\\///\\\///\\\///

#ifndef ML_PROXY
#define ML_PROXY

/**	Initialize and Start the proxy server **/
int ml_proxy(unsigned short int port, unsigned int workers);

/** Signal the proxy server to shutdown **/
void ml_proxy_shutdown();

#endif
