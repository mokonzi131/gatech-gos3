/// Michael Landes
/// GaTech : GOS : Project 3
/// \\\///\\\///\\\///\\\///
#include "mlrpc.h"
#include "globals.h"

#include "rpc.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "../jpeg-6b/lowres.h"


square_out* squareproc_1_svc(square_in* imp, struct svc_req* rqstp)
{
	static square_out out;

	out.real = imp->arg1 * imp->arg1;
	return (&out);
}


int ml_rpc_getImage(RequestStatus* status, char** img_buffer, int* img_length)
{
	printf("%.*s:%d\t\t%.*s\n", status->host_len, status->host, status->port, status->uri_len, status->uri);

	// connect to remote server
	char clientName[124];
	char clientPort[8];

	int hServer;
	struct addrinfo hints;
	struct addrinfo* result;

	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;

	// get string references to the host and the port
	strncpy(clientName, status->host, status->host_len);
	clientName[status->host_len] = '\0';
	memset(clientPort, 0, sizeof(clientPort));
	sprintf(clientPort, "%d", status->port);

	// find the remote server
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(clientName, clientPort, &hints, &result) != (SUCCESS))
	{
		printf("ML_PROXY: Could not resolve server's IP address, aborting...\n");
		goto cleanup;
	}

	// retrieve the remote address, and hServer
	if ((hServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == (ERROR))
	{
		printf("ML_PROXY: Failed to open a socket, aborting...\n");
		goto cleanup;
	}
	setsockopt(hServer, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof tv);

	// connect to the socket
	if (connect(hServer, result->ai_addr, result->ai_addrlen) != (SUCCESS))
	{
		printf("ML_PROXY: Failed to connect to remote server, aborting...\n");
		goto cleanup;
	}

	// send http request for image
	char request[IO_BUF_SIZE];
	int bytes = 0;

	// construct the request
	if (strlen("GET . HTTP/1.0\r\n") + status->uri_len + strlen("Host: .:.\r\n")
		+ status->host_len + strlen("User-Agent: ML_PROXY/1.0\r\n\r\n") > IO_BUF_SIZE)
		goto cleanup;
	sprintf(request, "GET %.*s HTTP/1.0\r\n", status->uri_len, status->uri);
	sprintf(request + strlen(request), "Host: %.*s:%d\r\n",
			status->host_len, status->host, status->port);
	sprintf(request + strlen(request), "User-Agent: ML_PROXY/1.0\r\n\r\n");

	// send request to server
	if (send(hServer, request, strlen(request), 0) == (ERROR))
	{
		printf("ML_PROXY: Failed to send request to remote server, aborting...\n");
		goto cleanup;
	}

	shutdown(hServer, SHUT_WR);

	// read response and extract length
	long image_length = 0;
	bytes = recv(hServer, request, IO_BUF_SIZE, MSG_PEEK);
	if (bytes == (ERROR))
		goto cleanup;

	char* location = request;
	if (strncmp(location, "HTTP", 4) != 0) goto cleanup;
	while (location - request < bytes && *location != ' ') ++location;
	if (strncmp(location, " 200 OK", 7) != 0) goto cleanup;
	while (!(*location == '\r' && *(location+1) == '\n'))
	{
		while (location - request < bytes && *location != '\n') ++location;
		++location;
		if (location - request >= bytes) goto cleanup;
		if (strncmp(location, "Content-Length: ", 16) == 0)
		{
			char length_data[64];
			memset(length_data, 0, sizeof(length_data));
			location += 16;
			char* search = location;
			while (search - request < bytes && *search != '\r') ++search;
			if (search - request >= bytes || search - location > 63) goto cleanup;
			strncpy(length_data, location, search - location);
			image_length = atol(length_data);
		}
	}
	if (!(*location == '\r' && *(location+1) == '\n')) goto cleanup;
	location += 2;
	if (location - request >= bytes || image_length == 0) goto cleanup;
	bytes = read(hServer, request, location - request);

	// compress the image, return the buffer /// char** img_buffer size_t* img_length
	*img_buffer = (char*) malloc (sizeof(char) * image_length);
	if (img_buffer == NULL) goto cleanup;
	memset(*img_buffer, 0, sizeof(img_buffer));

	FILE* input = fdopen(hServer,"r");
	if (input == NULL) goto cleanup;
	if (change_res_JPEG_F (input, img_buffer, img_length) == 0)
	{
		printf("Failed to acquire image!\n");
	}

cleanup:

	freeaddrinfo(result);
	close(hServer);

	return (SUCCESS);
}
