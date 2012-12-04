/// Michael Landes
/// GaTech : GOS : Project 3
/// \\\///\\\///\\\///\\\///
#include "globals.h"

#include "http.h"

#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/// DATA ///
static const char* R_ALERT = "<html> \n <head><title>ALERT</title></head> \n <body> %s </body> \n </html>\n";
//
/// PRIVATE INTERFACE ///
void toLower(char*, int);
//
/// PUBLIC INTERFACE ///
void ml_http_parseStatus(RequestStatus* request, char* input, int count)
{
	char* location;
	char* search;
	char ascii[10];

	location = search = input;
	memset(ascii, '\0', sizeof(ascii));

	request->status = (SUCCESS);
	request->method = UNKNOWN;
	request->schema = UNKNOWN;
	request->host = NULL;
	request->host_len = 0;
	request->port = 0;
	request->uri = NULL;
	request->uri_len = 0;

	// determine the request method type
	while(search - input < count && *search != ' ') ++search;
	if (strncmp("GET", location, search-location) == 0) request->method = GET;
	location = ++search;
	if(location - input >= count) { request->status = (ERROR); return; }

	if (*location != '/')
	{
		// determine the request schema type
		while(search - input < count && *search != ':') ++search;
		toLower(location, search - location);
		if (strncmp("http", location, search-location) == 0) request->schema = HTTP;
		search += 3;
		location = search;
		if (location - input >= count) { request->status = (ERROR); return; }

		// determine the host
		while(search - input < count && !(*search == ':' || *search == '/')) ++search;
		request->host = location;
		request->host_len = search - location;

		if (*search == ':')
		{
			// determine the port
			location = ++search;
			if (location - input >= count) { request->status = (ERROR); return; }
			while(search - input < count && (*search != '/')) ++search;
			strncpy(ascii, location, search - location);
			request->port = atoi(ascii);
			if (request->port <= 0 || request->port > MAX_PORT) { request->status = (ERROR); return; }
		}

		location = search;
		if (location - input >= count || *location != '/') { request->status = (ERROR); return; }
	}

	// determine the uri
	while(search - input < count && !isspace(*search)) ++search;
	request->uri = location;
	request->uri_len = search - location;
	if (location - input >= count) { request->status = (ERROR); return; }

	// determine the protocol
	// NOTE ignored for this project
}

/// IMPLEMENTATION ///
void ml_http_sendProxyError(int hSocket, const char* message)
{
	char header[256];
	char content[256];

	sprintf(content, R_ALERT, message);
	sprintf(header, "HTTP/1.0 500 Proxy Server Error\r\n");
	sprintf(header + strlen(header),
			"Server: ML_PROXY\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n",
			(int)strlen(content));

	write(hSocket, header, strlen(header)+1);
	write(hSocket, content, strlen(content)+1);
}

void toLower(char* string, int length)
{
	int i;
	for(i = 0; i < length; ++i)
	{
		if (string[i] >= 'A' && string[i] <= 'Z')
			string[i] = string[i] + 32;
	}
}
