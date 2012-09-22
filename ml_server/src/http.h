// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#ifndef ML_HTTP
#define ML_HTTP

/// ret ERROR (socket handle, buffer to use) ///
int ml_http_readLine(int, char[]);

/// ret T/F (pointer to buffer containing input stream) ///
int ml_http_isHTTP(const char*);

///
void ml_http_processHTTPRequest(int, char[]);

/// (...) ///
void ml_http_returnError(int hSocket, const char* message, int type); // TODO

void ml_http_returnDirectory(int hSocket); // TODO

#endif
