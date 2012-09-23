// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////
#include "globals.h"

#include "http.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "server.h"

/* DATA */
static const int BUFFER_SIZE = 1024;

static const char* S_ECHO = "Echo";
static const char* S_OK = "Ok";
static const char* S_BAD_REQUEST = "Bad Request";
static const char* S_FORBIDDEN = "Forbidden";
static const char* S_NOT_FOUND = "Not Found";
static const char* S_NOT_IMPLEMENTED = "Not Implemented";
static const char* R_ALERT = "<html> \n <head><title>ALERT</title></head> \n <body> %s </body> \n </html>\n";

/* PRIVATE INTERFACE */
static int resolveURL(char*, char*);
static int respondAlert(int, int, char*);
static int generateStatusLine(int, char*);
static void processHTTPGetRequest(int, char[], char*);

/* PUBLIC INTERFACE */
int ml_http_readLine(int hSocket, char inBuffer[])
{
	int location = 0;
	int result;

	while (location < BUFFER_SIZE-1 && (result = read(hSocket, inBuffer+location, 1)) == 1)
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

void ml_http_processHTTPRequest(int hSocket, char inBuffer[], char* statusLine)//
{
	char* httpMethod = (char*) malloc (strlen(statusLine));
	char* httpUrl = (char*) malloc (strlen(statusLine));

	sscanf(statusLine, "%s %s %*s", httpMethod, httpUrl);

	printf("METHOD: %s\n", httpMethod);
	printf("URL: %s\n", httpUrl);

	// make the appropriate HTTP method call
	if (strncmp(httpMethod, "GET", 3) == 0)
		processHTTPGetRequest(hSocket, inBuffer, httpUrl);
	else
		respondAlert(hSocket, 501, "Request method not implemented");
	// other methods would be called from here...

	free(httpMethod);
	free(httpUrl);
}

/* IMPLEMENTATION */
static int respondAlert(int hSocket, int type, char* message)
{
	char header[256];
	char content[256];
	int offset = 0;

	// Create the content string
	if (sprintf(content, R_ALERT, message) < 0)
		return (ML_HTTP_ERROR);

	// Create the Status Line
	if (generateStatusLine(type, header) != (SUCCESS))
		return (ML_HTTP_ERROR);

	// Create the Header information
	offset = strlen(header);
	if (sprintf(header+offset, "Server: Mokonzi\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", (int)strlen(content)) < 0)
		return (ML_HTTP_ERROR);

	// Respond
	write(hSocket, header, strlen(header)+1);
	write(hSocket, content, strlen(content)+1);

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

	if (sprintf(output, "HTTP/1.0 %d %s\r\n", type, message) < 0)
		return (ML_HTTP_ERROR);

	return (SUCCESS);
}

static void processHTTPGetRequest(int hSocket, char inBuffer[], char* url)
{
	int result;
	char* resource = (char*) malloc (strlen(ml_server_getRootDir()) + strlen(url) + 10);
	memset(resource, '\0', sizeof(resource));

	// check for absolute url format
	if (url[0] != '/')
	{
		respondAlert(hSocket, 400, "Please supply an absolute url");
		return;
	}

	// we don't handle dynamic url's
	if (strchr(url, '?') != NULL)
	{
		respondAlert(hSocket, 501, "Server only serves static resources");
		return;
	}

	// resolve the url if possible to a file location
	result = resolveURL(url, resource);
	if (result != SUCCESS)
	{
		respondAlert(hSocket, 400, "Bad request");
		return;
	}

	// is path legally located ? else send (Forbidden)
	if (strncmp(resource, ml_server_getRootDir(), strlen(ml_server_getRootDir())))
	{
		respondAlert(hSocket, 403, "Requested file outside root server directory");
		return;
	}

	// is path exist ? else send (Not Found)


	// alright baby let's return this sucker!
	respondAlert(hSocket, 501, "So far So GOOD");

	free(resource);
}

static int resolveURL(char* url, char* resolved)
{
	char* to;
	char* from;

	sprintf(resolved, ml_server_getRootDir(), strlen(ml_server_getRootDir()));
	to = strchr(resolved, '\0');
	from = url;

	// resolve the url, handling directory changes
	while(*from != '\0')
	{
		// handle a ./
		if (*from == '/' && *(from+1) == '.' && *(from+2) == '/')
		{
			from += 2;
			continue;
		}

		// handle a ../
		if (*from == '/' && *(from+1) == '.' && *(from+2) == '.' && *(from+3) == '/')
		{
			from += 3;
			while (to != resolved && *to != '/') --to;
			*to = ' ';
			continue;
		}

		// copy
		*to++ = *from++;
	}
	*to = '\0';

	printf("RESOLVED = %s\n", resolved);
	return (SUCCESS);
}
