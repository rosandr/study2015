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

int main(int argc, char** argv, char** env)
{
    char buf[BUF_SIZE];
    while( fgets(buf, BUF_SIZE, stdin) != NULL )
    {
        fputs(buf, stdout);

	for(int i=1; i<argc; i++)
	{
	    printf("%s\n", argv[i]);

	}
    }
    return 0;
}
