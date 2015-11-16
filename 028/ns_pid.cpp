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

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 256

void usage (char* name)
{
    printf("\nUsage: %s <child-level>\n", name);
}

char* newname;

int child_proc(int level)
{
    level--;
    if(level<0)	return 0;


    switch( pid=fork())		// transmiter
    {
	case -1:
	    perror("fork");
	    exit(EXIT_FAILURE);

	case 0:

	if (unshare(CLONE_NEWPID) == -1)
    	{
	    perror("unshare");
	    exit(EXIT_FAILURE);
	}

        if (mount("proc", mount_point, "proc", 0, NULL) == -1)
        {
	    perror("mount");
	    exit(2);
	}

	char mount_point[BUF_SIZE];
        sprintf(mount_point, "multi_proc%c", (char)('0'+level) );

	child_proc();
	execlp("sleep", "sleep", "600", (char *) NULL);
    }
}


int main(int argc, char** argv, char** env)
{
    pid_t pid;
    //struct utsname uts;

    if (argc < 2) 
    {
	usage(basename(argv[0]));
        exit(EXIT_FAILURE);
    }
    

    int level = atoi( argv[1]);
    if(level > 5)
    {
	usage(basename(argv[0]));
        exit(EXIT_FAILURE);
    }




    char mount_point[BUF_SIZE];

    for(int i=0; i<level; i++)
    {
        sprintf(mount_point, "multi_proc%c", (char)('0'+i) );
        mkdir(mount_point, 0555);
    }


    child_proc(level);

    exit(EXIT_SUCCESS);
}
