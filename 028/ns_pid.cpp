/* multi_pidns.c

   Copyright 2013, Michael Kerrisk
   Licensed under GNU General Public License v2 or later

   Create a series of child processes in nested PID namespaces.
*/
//#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <limits.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

/* A simple error-handling function: print an error message based
   on the value in 'errno' and terminate the calling process */

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

//#define STACK_SIZE (1024 * 1024)

//static char child_stack[STACK_SIZE];    /* Space for child's stack */
                /* Since each child gets a copy of virtual memory, this
                   buffer can be reused as each child creates its child */

/* Recursively create a series of child process in nested PID namespaces.
   'arg' is an integer that counts down to 0 during the recursion.
   When the counter reaches 0, recursion stops and the tail child
   executes the sleep(1) program. */

static int
childFunc(int level)
{
    static int first_call = 1;
    //long level = (long) arg;

    if (!first_call) {

        /* Unless this is the first recursive call to childFunc()
           (i.e., we were invoked from main()), mount a procfs
           for the current PID namespace */

        char mount_point[PATH_MAX];

        snprintf(mount_point, PATH_MAX, "./multi_proc%c", (char) ('0' + level));

        mkdir(mount_point, 0555);       /* Create directory for mount point */
        if (mount("proc", mount_point, "proc", 0, NULL) == -1)
            errExit("mount");
        printf("Mounting procfs at %s\n", mount_point);
    }

    first_call = 0;

    if (level > 0)
    {

        /* Recursively invoke childFunc() to create another child in a
           nested PID namespace */

        level--;
        pid_t child_pid;




        switch( child_pid=fork())		// transmiter
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
/*
            char mount_point[BUF_SIZE];
            sprintf(mount_point, "multi_proc%c", (char)('0'+level) );

            if (mount("proc", mount_point, "proc", 0, NULL) == -1)
            {
                perror("mount");
                exit(EXIT_FAILURE);
            }
*/
            childFunc(level);
            //umount( mount_point);
        }












        /*
        child_pid = clone(childFunc,
                    child_stack + STACK_SIZE,
                    CLONE_NEWPID | SIGCHLD, (void *) level);
*/


        if (waitpid(child_pid, NULL, 0) == -1)  /* Wait for child */
            errExit("waitpid");

    } else {

        /* Tail end of recursion: execute sleep(1) */

        printf("Final child sleeping\n");
        execlp("sleep", "sleep", "1000", (char *) NULL);
        errExit("execlp");
    }

    return 0;
}

int
main(int argc, char *argv[])
{

    int levels;

    levels = (argc > 1) ? atoi(argv[1]) : 5;
        printf("Caller PID = %ld, levels = %d\n",(long int)getpid(), levels);
    childFunc(levels);

    exit(EXIT_SUCCESS);
}
