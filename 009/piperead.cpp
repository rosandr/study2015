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
#define BUF_SIZE 80



void usage (char* name)
{
    printf("\nUsage:\t%s <bash command>\n", name);
    printf("Example: %s cat /proc/meminfo\n", name);
}


int main(int argc, char** argv, char** env)
{

    if (argc <2)
    {
	usage(basename(argv[0]));
	return 0;
    }

	char str[BUF_SIZE];
	char* to = str;

	for(int i=1; i<argc; i++)
	{
	    to = stpcpy(to, argv[i]);
	    to = stpcpy(to, " ");
	}

	FILE* fd = popen (str, "r");
	while( fgets(str, BUF_SIZE, fd ) != NULL)
	{
		int len = strlen(str);
		for(int i=0; i<len/2; i++)
		{
			char p;
			p=str[i];
			str[i]=str[len-i-1];
			str[len-i-1]=p;
		}
		printf("%s\n", str);
	}
    return 0;
}
