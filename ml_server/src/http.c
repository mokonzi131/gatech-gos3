// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////
#include "globals.h"

#include "http.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "server.h"

/* DATA */
static const char* S_ECHO = "Echo";
static const char* S_OK = "Ok";
static const char* S_BAD_REQUEST = "Bad Request";
static const char* S_FORBIDDEN = "Forbidden";
static const char* S_NOT_FOUND = "Not Found";
static const char* S_NOT_IMPLEMENTED = "Not Implemented";

/* PRIVATE INTERFACE */
static int resolveURL(char*, char*);
static int respondAlert(int, int, char*);
static int generateStatusLine(int, char*);

/* PUBLIC INTERFACE */
int ml_http_readLine(int hSocket, char inBuffer[])
{
	int location = 0;
	int result;

	while ((result = read(hSocket, inBuffer+location, 1)) == 1)
	{
		if (inBuffer[location++] == '\n')
			break;
	}

	if (result <= 0)
		return (ML_HTTP_ERROR);

	inBuffer[location] = '\0';

	return (SUCCESS);
}

int ml_http_isHTTP(const char* status)
{
	const char* position = status;

	// move to third word (delimited by ' ')
	while (*position != ' ' && *position != '\0') ++position;
	if (*position++ != ' ') return (0);
	while (*position != ' ' && *position != '\0') ++position;
	if (*position++ != ' ') return (0);

	// check for the HTTP protocol tag
	if ((position = strstr(position, "HTTP")) != NULL) return (1);

	return (0);
}

void ml_http_processHTTPRequest(int hSocket, char inBuffer[])//
{
	int result;
	char* url;

//	result = resolveURL(inBuffer, url);
//	if (result != SUCCESS)
	{
//		respondError(hSocket, R_OK, "Fake Error Message");
	}

	// determine method, generate error response if necessary
	// retrieve path, and resolve resource location (with protection)
	// read header lines...
	// build response
	// return resource

	// echo testing
	printf("STATUS: %s\n", inBuffer);
	respondAlert(hSocket, 101, "hello world!");
}

/* IMPLEMENTATION */
static int resolveURL(char* status, char* resolved)
{
	printf("RESOLVED = %s + %s\n", ml_server_getRootDir(), status);
	return (ML_HTTP_ERROR);
}

static int respondAlert(int hSocket, int type, char* message)
{
	char response[256];
	int offset = 0;

	///HTTP/1.0 101 Echo ...... message
	memset(response, '\0', sizeof(response));

	// Create the Status Line
	if (generateStatusLine(type, response) != (SUCCESS))
		return (ML_HTTP_ERROR);
	offset = strlen(response);

	// Create the Header information

	// Create the HTML body

	write(hSocket, response, strlen(response)+1);

	return (SUCCESS);
}

static int generateStatusLine(int type, char* output)
{
	const char* message;

	switch(type)
	{
		case 101:
			message = S_ECHO;
			break;
		case 200:
			message = S_OK;
			break;
		case 400:
			message = S_BAD_REQUEST;
			break;
		case 403:
			message = S_FORBIDDEN;
			break;
		case 404:
			message = S_NOT_FOUND;
			break;
		case 501:
			message = S_NOT_IMPLEMENTED;
			break;
		default:
			return (ML_HTTP_ERROR);
	}

	sprintf(output, "HTTP/1.0 %d %s\r\n", type, message);

	return (SUCCESS);
}
