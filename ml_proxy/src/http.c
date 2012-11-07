// Michael Landes
// GaTech : GOS : Project 2
///////////////////////////
#include "globals.h"

#include "http.h"
//
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/stat.h>
//#include <dirent.h>
//#include <sys/types.h>
//#include <sys/sendfile.h>
//
//#include "server.h"
//
/// DATA ///
//static const int BUFFER_SIZE = 1024;
static const char* R_ALERT = "<html> \n <head><title>ALERT</title></head> \n <body> %s </body> \n </html>\n";
//static const char* R_LENGTH = "Content-Length: %d\r\n";
//
/// PRIVATE INTERFACE ///
void toLower(char*, int);
//static int resolveURL(char*, char*);
//static void processHTTPGetRequest(int, char[], char*);
//static void respondDirectory(int, char*);
//static void respondRegularFile(int, char*, int);
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

//int ml_http_readLine(int hSocket, char inBuffer[])
//{
//	int location = 0;
//	int result;
//
//	while (location < BUFFER_SIZE-1 && (result = read(hSocket, inBuffer+location, 1)) == 1)
//	{
//		if (inBuffer[location++] == '\n')
//			break;
//	}
//
//	if (result <= 0)
//		return (ML_HTTP_ERROR);
//
//	inBuffer[location] = '\0';
//
//	return (SUCCESS);
//}
//
//void ml_http_processHTTPRequest(int hSocket, char inBuffer[], char* statusLine)//
//{
//	char* httpMethod = (char*) malloc (strlen(statusLine));
//	char* httpUrl = (char*) malloc (strlen(statusLine));
//
//	sscanf(statusLine, "%s %s %*s", httpMethod, httpUrl);
//
//	// make the appropriate HTTP method call
//	if (strncmp(httpMethod, "GET", 3) == 0)
//		processHTTPGetRequest(hSocket, inBuffer, httpUrl);
//	else
//		sendProxyError(hSocket, "Request method not implemented");
//	// other methods would be called from here...
//
//	free(httpMethod);
//	free(httpUrl);
//}
//

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

//static void processHTTPGetRequest(int hSocket, char inBuffer[], char* url)
//{
//	int result;
//	struct stat filestat;
//	char* resource = (char*) malloc (strlen(ml_server_getRootDir()) + strlen(url) + 10);
//	memset(resource, '\0', sizeof(resource));
//
//	// check for absolute url format
//	if (url[0] != '/')
//	{
//		respondAlert(hSocket, 400, "Please supply an absolute url");
//		free(resource);
//		return;
//	}
//
//	// we don't handle dynamic url's
//	if (strchr(url, '?') != NULL)
//	{
//		respondAlert(hSocket, 501, "Server only serves static resources");
//		free(resource);
//		return;
//	}
//
//	// resolve the url if possible to a file location
//	result = resolveURL(url, resource);
//	if (result != SUCCESS)
//	{
//		respondAlert(hSocket, 400, "Bad request");
//		free(resource);
//		return;
//	}
//
//	// is path legally located ? else send (Forbidden)
//	if (strncmp(resource, ml_server_getRootDir(), strlen(ml_server_getRootDir())))
//	{
//		respondAlert(hSocket, 403, "Requested file outside root server directory");
//		free(resource);
//		return;
//	}
//
//	// is path exist ? else send (Not Found)
//	if (stat(resource, &filestat) < 0)
//	{
//		respondAlert(hSocket, 404, "Unable to locate this file");
//		free(resource);
//		return;
//	}
//
//	// alright baby let's return this sucker!
//	if (S_ISDIR(filestat.st_mode))
//	{
//		respondDirectory(hSocket, resource);
//	}
//	else if (S_ISREG(filestat.st_mode))
//	{
//		respondRegularFile(hSocket, resource, filestat.st_size);
//	}
//	else
//	{
//		respondAlert(hSocket, 403, "Invalid file mode");
//	}
//
//	free(resource);
//}
//
//static int resolveURL(char* url, char* resolved)
//{
//	char* to;
//	char* from;
//
//	sprintf(resolved, ml_server_getRootDir(), strlen(ml_server_getRootDir()));
//	to = strchr(resolved, '\0');
//	from = url;
//
//	// resolve the url, handling directory changes
//	while(*from != '\0')
//	{
//		// handle a ./
//		if (*from == '/' && *(from+1) == '.' && *(from+2) == '/')
//		{
//			from += 2;
//			continue;
//		}
//
//		// handle a ../
//		if (*from == '/' && *(from+1) == '.' && *(from+2) == '.' && *(from+3) == '/')
//		{
//			from += 3;
//			while (to != resolved && *to != '/') --to;
//			*to = ' ';
//			continue;
//		}
//
//		// copy
//		*to++ = *from++;
//	}
//	*to = '\0';
//
//	//printf("RESOLVED = %s\n", resolved);
//	return (SUCCESS);
//}
//
//static void respondDirectory(int hSocket, char* resource)
//{
//	char header[256];
//	char length[100];
//	char* content = (char*) malloc (1024);
//	DIR* dirp;
//	struct dirent* dp;
//	struct stat st;
//
//	//printf("(%d) <= %s\n", hSocket, resource);
//
//	memset(content, '\0', sizeof(content));
//	strcat(content, "<html>\n <head><title>Directory Listing</title></head>\n\
//			<body><h1>Directory Listing:</h1><hr/>\n");
//
//	dirp = opendir(resource);
//	while((dp = readdir(dirp)) != NULL)
//	{
//		if (dp->d_name[0] == '.') continue;
//		strcat(content, "<a href='");
//		strcat(content, dp->d_name);
//		lstat(dp->d_name, &st);
//		if (S_ISDIR(st.st_mode))
//			strcat(content, "/");
//		strcat(content, "'>");
//		strcat(content, dp->d_name);
//		if (S_ISDIR(st.st_mode))
//			strcat(content, "/");
//		strcat(content, "</a><br/>\n");
//	}
//
//	strcat(content, "</body></html>\r\n");
//
//	generateStatusLine(200, header);
//	strcat(header, "Server: Mokonzi\r\n");
//	strcat(header, "Content-Type: text/html\r\n");
//	sprintf(length, R_LENGTH, strlen(content));
//	strcat(header, length);
//	strcat(header, "\r\n");
//
//	write(hSocket, header, strlen(header));
//	write(hSocket, content, strlen(content));
//
//	free(content);
//}
//
//static void respondRegularFile(int hSocket, char* resource, int size)
//{
//	char header[512];
//	char length[100];
//	FILE* file;
//
//	// Create and send the header
//	generateStatusLine(200, header);
//	strcat(header, "Server: Mokonzi\r\n");
//	if (strstr(resource, ".http") == (char*)strlen(resource) - 5)
//		strcat(header, "Content-Type: text/html\r\n");
//	else if (strstr(resource, ".jpg") == (char*)strlen(resource) - 4)
//		strcat(header, "Content-Type: image/jpeg\r\n");
//	else if (strstr(resource, ".gif") == (char*)strlen(resource) - 4)
//		strcat(header, "Content-Type: image/gif\r\n");
//	else
//		strcat(header, "Content-Type: text/plain\r\n");
//	sprintf(length, R_LENGTH, size);
//	strcat(header, length);
//	strcat(header, "\r\n");
//
//	write(hSocket, header, strlen(header));
//
//	// Connect the file to the socket and Blast it through!!! <3 sendfile()
//	file = fopen(resource, "r");
//	if (file != NULL)
//	{
//		sendfile(hSocket, fileno(file), NULL, size);
//		fclose(file);
//	}
//}

void toLower(char* string, int length)
{
	int i;
	for(i = 0; i < length; ++i)
	{
		if (string[i] >= 'A' && string[i] <= 'Z')
			string[i] = string[i] + 32;
	}
}
