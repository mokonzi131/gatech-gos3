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
typedef union {
	char array[SHM_BUF_SIZE];
	struct {
		int done;
		size_t size;
		char data[SHM_BUF_SIZE - sizeof(int) - sizeof(size_t)];
	};
} ml_shm_block;

#endif
