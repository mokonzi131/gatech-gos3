// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////
#include "globals.h"

#include "http.h"

#include <unistd.h>
#include <string.h>

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
	char* position;

	// move to third word (delimited by ' ')
//	while (*position != ' ' && *position != '\0') ++position;
//	if (*position != ' ') return (0);
//	++position;
//	while (*position != ' ' && *position != '\0') ++position;
//	if (*position != ' ') return (0);
//	++position;
//	if ((position = strstr(position, "HTTP")) != NULL) return (1);

	if ((position = strchr(status, ' ')) == NULL) return (0);
	++position;
	if (*position == '\0') return (0);
	if ((position = strchr(position, ' ')) == NULL) return (0);
	++position;
	if (*position == '\0') return (0);
	if ((position = strstr(position, "HTTP")) != NULL) return (1);

	return (0);
}

void ml_http_processHTTPRequest(int hSocket, char inBuffer[])
{
	// echo testing
	write(hSocket, inBuffer, strlen(inBuffer)+1);
}
