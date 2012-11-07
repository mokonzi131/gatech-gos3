// Michael Landes
// GaTech : GOS : Project 2
///////////////////////////

#ifndef ML_HTTP
#define ML_HTTP

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
} ProxyRequest;

///
void ml_http_parseRequest(ProxyRequest* request, char* input, int count);

///// ret ERROR (socket handle, buffer to use) ///
//int ml_http_readLine(int, char[]);
//
///// ret T/F (pointer to buffer containing input stream) ///
//int ml_http_isHTTP(const char*);
//
///// (socket handle, buffer to use) /// status line is in buffer
//void ml_http_processHTTPRequest(int, char[], char*);

//[GET http://nintendo.com/ HTTP/1.1
//Host: nintendo.com
//Proxy-Connection: keep-alive
//User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/535.19 (KHTML, like Gecko) Ubuntu/12.04 Chromium/18.0.1025.168 Chrome/18.0.1025.168 Safari/535.19
//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
//Accept-Encoding: gzip,deflate,sdch
//Accept-Language: en-US,en;q=0.8
//Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3
//
//]

#endif
