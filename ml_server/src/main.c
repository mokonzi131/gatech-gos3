// Michael Landes
// GaTech : GOS : Project 1
///////////////////////////

#include <stdio.h>
#include <unistd.h>
#include "server.h"

int main(int args, char** argv)
{
	int i;

	for (i = 0; i < args; ++i)
	{
		printf("%s\n", argv[i]);
	}




	getopt(0, NULL, NULL);

	return 0;
}
