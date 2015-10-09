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

#include <iostream>
using namespace std;
#define BUF_SIZE 256

void usage (char* name)
{
    printf("\nUsage:\t%s pipein pipeout\n", name);
    printf("\tpipein  - input named pipe\n");
    printf("\tpipeout - output named pipe\n");
    printf("Example: ./%s ff1 ff2\n", name);
}

int main(int argc, char** argv, char** env)
{
    int fifo1, fifo2;
    pid_t pid1, pid2;

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

    switch(pid1=fork())
    {
	case -1:
	    perror("fork1");
	    exit(1);
	case 0:
//printf("%s\n", topin);

	    int fd = open(topin, O_RDONLY );		// reading pipe

    	    if(fd <0)
	    {
		perror("fopen1");
		exit(EXIT_FAILURE);
	    }
//printf("fopen1\n");

	    char buf[BUF_SIZE];
	    buf[0]=0;

	    while(1)
	    {
		//fputs(buf, stdout);
		//write(1, buf, sizeof(buf));
		int n = read(fd, buf, BUF_SIZE);
		if (n<=0)	break;
		buf[n]=0;
		fprintf(stdout, ">> %s\n",buf);
	    }
	    close(fd);

/*
	    FILE* fd = fopen(topin,  "r");		// reading pipe

	    if(fd == NULL)
	    {
		perror("fopen1");
		exit(EXIT_FAILURE);
	    }

printf("fopen1");

	    char buf[BUF_SIZE];
	    while( fgets(buf, BUF_SIZE, fd) !=NULL )
		puts(buf);

	    fclose(fd);
*/
    }

    switch(pid2=fork())
    {
	case -1:
	    perror("fork2");
	    exit(2);
	case 0:
//printf("%s\n", topout);

	    int fd1 = open(topout, O_WRONLY );		// writing pipe

	    if(fd1 <0)
	    {
		perror("fopen2");
		exit(EXIT_FAILURE);
	    }

//printf("fopen2\n");

	    char buf[BUF_SIZE];
	    buf[0]=0;
	    while( fgets(buf, BUF_SIZE, stdin) !=NULL )
	    {
		int n =write(fd1, buf , strlen(buf));
		if(n<=0)	break;
		//fprintf(stdout, "from f2%s %d\n",buf,  n);
		//buf[0]=0;
	    }
	    close(fd1);

/*
	    FILE* fd1 = fopen(topout,  "w");		// writing pipe

    	    if(fd1 == NULL)
	    {
		perror("fopen2");
		exit(EXIT_FAILURE);
	    }

printf("fopen2\n");

	    char buf[BUF_SIZE];
	    //while( fgets(buf, BUF_SIZE, stdin) !=NULL )
	    while(1)
	    {
		sleep(1);
		//fputs(buf, fd);
		//fputs("1234", fd);
		//fputs("1234124", stdout);
		int n=fprintf(fd1, "1234124123412341\n");
		fprintf(stdout, "1234 %d\n",n);
	    }
	    fclose(fd1);
*/
    }

    if( waitpid(pid1, &fifo1, 0)!=pid1 || waitpid(pid2, &fifo2, 0)!=pid2 )
	perror("waitpid");

    return 0;
}
