//#define _GNU_SOURCE
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 256

void usage (char* name)
{
    printf("\nUsage: %s <child-hostname>\n", name);
}

char* newname;

int main(int argc, char** argv, char** env)
{
    pid_t pid;
    struct utsname uts;

    if (argc < 2) 
    {
	usage(basename(argv[0]));
        exit(EXIT_FAILURE);
    }
    newname=argv[1];

    switch( pid=fork())		// transmiter
    {
	case -1:
	    perror("fork");
	    exit(EXIT_FAILURE);

	case 0:

	if (unshare(CLONE_NEWUTS) == -1)
    	{
	    perror("unshare");
	    exit(EXIT_FAILURE);
	}

        struct utsname uts;
	if (sethostname(newname, strlen(newname)) == -1)
	{
    	    perror("sethostname");
	    exit(EXIT_FAILURE);
	}

	if (uname(&uts) == -1)
	{
    	    perror("uname");
	    exit(EXIT_FAILURE);
	}

	printf("uts.nodename in child:  %s\n", uts.nodename);
        sleep(5);
	return 0;
    }

    if (uname(&uts) == -1)
    {
        perror("uname");
        exit(EXIT_FAILURE);
    }

    printf("uts.nodename in parent: %s\n", uts.nodename);
    sleep(1);

    if (waitpid(pid, NULL, 0) == -1)
        perror("waitpid");

    exit(EXIT_SUCCESS);
}
