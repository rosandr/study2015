#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
//#include <asm-generic/errno-base.h>
#include <pthread.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 256


void usage (char* name)
{
    printf("\nUsage:\t%s string filename\n", name);
    printf("\tstring   - template to find\n");
    printf("\tfilename - file to store result\n");
    printf("Example: ls -l|%s rwx file.rwx\n", name);
}


int main(int argc, char** argv, char** env)
{
    if (argc <3)
    {
	usage(basename(argv[0]));
	return 0;
    }

    FILE* fd = fopen(argv[2], "w");
    if(fd == NULL)
    {
	perror("fopen");
	exit(EXIT_FAILURE);
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
    fclose(fd);
    return 0;
}
