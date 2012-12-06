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

img_out* img_proc_1_svc(img_in* input, struct svc_req* rqstp)
{
	static img_out output;

	char clientPort[8];
	int hServer;
	struct addrinfo hints;
	struct addrinfo* result;

	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;

	char request[IO_BUF_SIZE];
	int bytes = 0;

	printf("%s:%d\t\t%s\n", input->host, input->port, input->uri);

	// get string references to the port
	memset(clientPort, 0, sizeof(clientPort));
	sprintf(clientPort, "%d", input->port);

	// find the remote server
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(input->host, clientPort, &hints, &result) != (SUCCESS))
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

	// construct the request
	if (strlen("GET . HTTP/1.0\r\n") + strlen(input->uri) + strlen("Host: .:.\r\n")
		+ strlen(input->host) + strlen("User-Agent: ML_PROXY/1.0\r\n\r\n") > IO_BUF_SIZE)
		goto cleanup;
	sprintf(request, "GET %s HTTP/1.0\r\n", input->uri);
	sprintf(request + strlen(request), "Host: %s:%d\r\n",
			input->host, input->port);
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

	// set output
	output.buffer.buffer_len = image_length;
	output.buffer.buffer_val = (char*) malloc (sizeof(char) * image_length);
	if (output.buffer.buffer_val == NULL) goto cleanup;
	memset(output.buffer.buffer_val, 0, sizeof(output.buffer.buffer_val));

	FILE* temp = fdopen(hServer, "r");
	if (temp == NULL) goto cleanup;
	if (change_res_JPEG_F(temp, &(output.buffer.buffer_val), &(output.final)) == 0)

cleanup:
	freeaddrinfo(result);
	close(hServer);
	return (&output);
}
