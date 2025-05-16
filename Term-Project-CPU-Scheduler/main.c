#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <string.h>
#include "ossim.h"


void println() // Debug purpose
{
    printf("\n");
}

int main(int argc, char* argv[])
{
	char filename[1024] = "";
	if(argc >= 2) {
		strcpy(filename, argv[1]);
	}




	Menu();
	return 0;
}
