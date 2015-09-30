#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
//#include <unistd.h>

//#include <sys/types.h>
//#include <sys/wait.h>

#include <iostream>

using namespace std;

#define BUF_SIZE 80

//-------------------------------------------------------------------------
void sigint_handler(int sig)
{ 
	//(void)sig; 
	printf("\n%d bye-bye...\n", sig); 
}
 

int main(int argc, char** argv, char** env)
{
    static struct sigaction act;

    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);		// ^C
	
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
