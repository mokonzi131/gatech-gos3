// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#include <netinet/in.h>

#ifndef ML_HTTP
#define ML_HTTP

/// ret ERROR (socket handle, buffer to use) ///
int ml_http_readLine(int, char[]);

/// ret T/F (pointer to buffer containing input stream) ///
int ml_http_isHTTP(const char*);

/// (socket handle, buffer to use) /// status line is in buffer
void ml_http_processHTTPRequest(int, char[], char*);

void ml_http_processSHMRequest(char*);

/// SHM types ///
#define SHM_BUF_SIZE			4096
typedef struct {
	int done;
	int size;
} ml_shm_header;
typedef struct {
	ml_shm_header header;
	char data[SHM_BUF_SIZE - sizeof(ml_shm_header)];
} ml_shm_block;

#endif
