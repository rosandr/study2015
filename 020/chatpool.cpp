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
    if (argc < 3)
    {
        usage(basename(argv[0]));
        return 0;
    }

    char* ppin =argv[1];
    char* ppout=argv[2];

    if (access(ppin, F_OK) == -1)
        mkfifo(ppin,  O_RDWR|S_IFIFO|S_IRWXU|S_IRWXG|S_IRWXO);
    if (access(ppout, F_OK) == -1)
        mkfifo(ppout, O_RDWR|S_IFIFO|S_IRWXU|S_IRWXG|S_IRWXO);

    int fd = open(ppin, O_RDWR );		// reading pipe
    if(fd <0)
    {
        perror("fopen1");
        exit(EXIT_FAILURE);
    }

    int fd1 = open(ppout, O_RDWR );		// writing pipe
    if(fd1 <0)
    {
        perror("fopen2");
        exit(EXIT_FAILURE);
    }

    //-----------------------------------------------------
    printf("Please, type your message...\n");

    int ret, running =1;
    fd_set rdfs;
    char buf[BUF_SIZE];

    while(running)
    {
        FD_ZERO(&rdfs);

        FD_SET(fd, &rdfs);
        FD_SET(STDIN_FILENO, &rdfs);

        if ((ret = select(5, &rdfs, NULL, NULL, NULL)) < 0)
        {
            perror("select");
            running = 0; continue;
        }

        if (FD_ISSET(fd, &rdfs))
        {
            int n = read(fd, buf, BUF_SIZE);
            if (n<=0)	break;
            buf[n]=0;
            printf(">> %s\n",buf);
        }

        if (FD_ISSET(STDIN_FILENO, &rdfs))
        {
            buf[0]=0;
            if( fgets(buf, BUF_SIZE, stdin) !=NULL )
            {
                int n =write(fd1, buf , strlen(buf));
                if(n<=0)	break;
            }
        }
    }

    close(fd);
    close(fd1);
    return 0;
}
