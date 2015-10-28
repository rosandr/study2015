#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/select.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 256

void usage (char* name)
{
    printf("\nUsage:\t%s pipein pipeout\n", name);
    printf("\tpipein  - input  fifo name,\n");
    printf("\tpipeout - output fifo name.\n");
    printf("Example: ./%s f1 f2  // in one xterm,\n", name);
    printf("         ./%s f2 f1  // in another xterm\n", name);
}

int main(int argc, char** argv, char** env)
{
    int fifo1, fifo2;

    if (argc < 3)
    {
	usage(basename(argv[0]));
	return 0;
    }

    char* topin =argv[1];
    char* topout=argv[2];

    if (access(topin, F_OK) == -1)
	fifo1=mkfifo(topin,  O_RDWR|S_IFIFO|S_IRWXU|S_IRWXG|S_IRWXO);
    if (access(topout, F_OK) == -1)
	fifo2=mkfifo(topout, O_RDWR|S_IFIFO|S_IRWXU|S_IRWXG|S_IRWXO);

    int fd = open(topin, O_RDONLY );		// reading pipe
    if(fd <0)
    {
	perror("fopen1");
	exit(EXIT_FAILURE);
    }

    int fd1 = open(topout, O_WRONLY );		// writing pipe
    if(fd1 <0)
    {
	perror("fopen2");
	exit(EXIT_FAILURE);
    }


    printf("Please, type your message...\n");

    int ret, running =1;
    fd_set rdfs, wrfs;
    char buf[BUF_SIZE];

    while(running)
    {
	FD_ZERO(&rdfs);
	FD_ZERO(&wrfs);

	FD_SET(fd, &rdfs);
	FD_SET(fd1, &wrfs);

        if ((ret = select(5, &rdfs, &wrfs, NULL, NULL)) < 0)
        {
            //perror("select");
            running = 0; continue;
        }

        if (FD_ISSET(fd, &rdfs))
        {
	    int n = read(fd, buf, BUF_SIZE);
	    if (n<=0)	continue;
	    buf[n]=0;
	    printf(">> %s\n",buf);
	}

        if (FD_ISSET(fd1, &wrfs))
        {
	    buf[0]=0;
	    while( fgets(buf, BUF_SIZE, stdin) !=NULL )
	    {
		int n =write(fd1, buf , strlen(buf));
		if(n<=0)	continue;
	    }
	}
    }

/*
    switch(pid1=fork())		// receiver
    {
	case -1:
	    perror("fork1");
	    exit(1);
	case 0:
	    int fd = open(topin, O_RDONLY );		// reading pipe

    	    if(fd <0)
	    {
		perror("fopen1");
		exit(EXIT_FAILURE);
	    }
	    printf("Please, type your message...\n");

	    char buf[BUF_SIZE];
	    buf[0]=0;

	    while(1)
	    {
		int n = read(fd, buf, BUF_SIZE);
		if (n<=0)	break;
		buf[n]=0;
		printf(">> %s\n",buf);
	    }
	    close(fd);
    }

    switch(pid2=fork())		// transmiter
    {
	case -1:
	    perror("fork2");
	    exit(2);
	case 0:
	    int fd1 = open(topout, O_WRONLY );		// writing pipe

	    if(fd1 <0)
	    {
		perror("fopen2");
		exit(EXIT_FAILURE);
	    }

	    char buf[BUF_SIZE];
	    buf[0]=0;
	    while( fgets(buf, BUF_SIZE, stdin) !=NULL )
	    {
		int n =write(fd1, buf , strlen(buf));
		if(n<=0)	break;
	    }
	    close(fd1);
    }

    if( waitpid(pid1, &fifo1, 0)!=pid1 || waitpid(pid2, &fifo2, 0)!=pid2 )
	perror("waitpid");
*/
    return 0;
}
