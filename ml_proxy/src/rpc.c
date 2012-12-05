/// Michael Landes
/// GaTech : GOS : Project 3
/// \\\///\\\///\\\///\\\///
#include "globals.h"

#include "rpc.h"

#include <stdio.h>


int ml_rpc_getImage(RequestStatus* status, char** img_buffer, size_t* img_length)
{
	printf("%.*s:%d\t\t%.*s\n", status->host_len, status->host, status->port, status->uri_len, status->uri);

	// send http request for image

	// read response

	// call compression function

	return (ERROR);
}
