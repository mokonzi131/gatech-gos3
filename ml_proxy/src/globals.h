/// Michael Landes
/// GaTech : GOS : Project 2
/// \\\///\\\///\\\///\\\///

#include <netinet/in.h>

#ifndef ML_GLOBALS
#define ML_GLOBALS

/// SHARED DEFINITIONS ///
#define	SUCCESS					0
#define DEFAULT_PROXY_PORT		50080
#define MAX_PORT				65535
#define DEFAULT_PROXY_WORKERS	5
#define MAX_WORKERS				50
#define ERROR					-1
#define SAFEQ_ERROR				-2
#define IO_BUF_SIZE 			1000
#define TIMEOUT_SEC				10

/// PROXY.C DEFINED GLOBAL ACCESS ///
extern int TERMINATE;
extern int useSharedMode(in_addr_t client_addr);

#endif

/*////////////////////////////////////////////////////////
struct addrinfo* res;
int error;
for (res = result; res != NULL; res = res->ai_next)
    {
   		struct sockaddr_in* remoteaddr = (struct sockaddr_in*)result->ai_addr;
    	printf("getaddrinfo: %s => %s:%ld\n", clientName, inet_ntoa(remoteaddr->sin_addr), ntohs(remoteaddr->sin_port));
        char hostname[NI_MAXHOST] = "";

        error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
        if (error != 0)
        {
            fprintf(stderr, ">>>>>error in getnameinfo: %s\n", gai_strerror(error));
            continue;
        }
        if (*hostname != '\0')
            printf(">>>>>hostname: %s\n", hostname);
    }
////////////////////////////////////////////////////////*/
