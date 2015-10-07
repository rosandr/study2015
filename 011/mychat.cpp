#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <iostream>
using namespace std;
#define BUF_SIZE 256


void usage (char* name)
{
    printf("\nUsage:\t%s [pipein] [pipeout]\n", name);
    printf("\tpipein  - input named pipe\n");
    printf("\tpipeout - output named pipe\n");
    printf("Example: ./%s [inp] [outp]\n", name);
}


int main(int argc, char** argv, char** env)
{
    if (argc == 2)
    {
	usage(basename(argv[0]));
	return 0;
    }

    char pinDefName[] = "pipin";
    char poutDefName[]= "pipout";

    char* topin = pinDefName;
    char* topout = poutDefName;

    if (argc == 3)
    {
	topin =argv[1];
	topout=argv[2];
    }

    mkfifo(topin,  S_IFIFO|S_IRWXU|S_IRWXG|S_IRWXO);
    mkfifo(topout, S_IFIFO|S_IRWXU|S_IRWXG|S_IRWXO);


    FILE* fdpin = fopen(topin,  "r");		// reading pipe
    FILE* fdpout= fopen(topout, "w");		// writing pipe

    if( (fdpin == NULL)|| (fdpout == NULL))
    {
	perror("fopen");
	exit(EXIT_FAILURE);
    }

    char buf[BUF_SIZE];
    fgets(buf, BUF_SIZE, stdin);

    printf("%s\n",buf);







/*
    while(1)
    {




	
    }



    int count=0;
    char buf[BUF_SIZE];
    while( fgets(buf, BUF_SIZE, stdin) != NULL )
    {
        fputs(buf, stdout);
	char* from = buf;
	from = strstr( from, argv[1] );

	while(from != NULL)
	{
	    count++;
	    from++;
	    from = strstr( from, argv[1] );
	}
    }

    fprintf (fd, "%s was found %d time(s)", argv[1], count);
*/
    fclose(fdpin);
    fclose(fdpout);
    return 0;
}
