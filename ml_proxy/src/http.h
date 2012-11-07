// Michael Landes
// GaTech : GOS : Project 2
///////////////////////////

#ifndef ML_HTTP
#define ML_HTTP

/// DATA STRUCTURES ///
typedef enum {
	UNKNOWN = -1,
	GET = 0
} MethodType;

typedef enum {
	UNKOWN = -1,
	HTTP = 0
} SchemaType;

typedef struct {
	int status;
	MethodType method;
	SchemaType schema;
	char* host;
	int host_len;
	int port;
	char* uri;
	int uri_len;
} RequestStatus;

/// PUBLIC INTERFACE ///
void ml_http_parseStatus(RequestStatus* request, char* input, int count);
void ml_http_sendProxyError(int hSocket, const char* message);

///// ret ERROR (socket handle, buffer to use) ///
//int ml_http_readLine(int, char[]);
//
///// (socket handle, buffer to use) /// status line is in buffer
//void ml_http_processHTTPRequest(int, char[], char*);

#endif
