#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <errno.h>
//#include <unistd.h>

//#include <sys/types.h>
//#include <sys/wait.h>

#include <iostream>

using namespace std;

#define BUF_SIZE 80

int main(int argc, char** argv, char** env)
{
	/*
    if (argc <2)
    {
		printf("\nUsage:\t%s cmd [opt] [args]\n", basename(argv[0]));
		printf("\tcmd - shell command,\n\topt - command option\n\targs - command arguments\n");
		printf("Exit:\tCtrl+C\n");printf("Example:\t%s cat /proc/cpuinfo 2>/dev/null\n", basename(argv[0]));
		return 0;
    }
	*/
	int rc=0;
	char buf[BUF_SIZE];
	char cmdline[BUF_SIZE];

	do
	{
		printf("sh >");
		fflush (stdout);

		if( !fgets(buf, BUF_SIZE, stdin) )	break;
		
		sprintf(cmdline, "/bin/bash -c \"%s\"", buf);
		rc= system (cmdline);
	}
	while (!rc);

	return rc;	
}
