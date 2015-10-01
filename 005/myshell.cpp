#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

//#include <sys/types.h> 
//#include <sys/wait.h>

#include <iostream>

using namespace std;

#define BUF_SIZE	256

//-------------------------------------------------------------------------
void sigint_handler(int sig)
{ 
	printf("\nGoodbye...\n"); 
	exit (0);
}
 
int main(int argc, char** argv, char** env)
{
    static struct sigaction act;

    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);		// ^C

	int rc=0;
	char buf[BUF_SIZE];
	char path[BUF_SIZE];
	path[0]=0;

	sprintf(path, "%s:%s", getenv("PATH"), getcwd(buf, BUF_SIZE));
	setenv("PATH", path, 1);

	do
	{
		printf("sh > ");
		fflush (stdout);

		if( !fgets(buf, BUF_SIZE, stdin) )	break;	// ^D
		rc = system (buf);
	}
	while (1);

	return rc;	
}
