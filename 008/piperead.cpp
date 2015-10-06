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
    printf("\nUsage:\t%s N n\n", name);
    printf("\tN - interval, [1-64],\n\tn - sample number [1-64].\n");
}


int main(int argc, char** argv, char** env)
{
/*
    if (argc <3)
    {
	usage(basename(argv[0]));
	return 0;
    }
*/
	char cmd[BUF_SIZE];
	cmd[0]=0;
	char str[BUF_SIZE];
/*
for(int i=0; i<argc; i++)
	//strcpy(cmd, argv[i]);
{
sprintf(cmd, "%s ", argv[i]);
cmd+=strlen(argv[i]);

}
printf("%s\n", cmd);
*/
	FILE* fd = popen ("ls -la", "r");
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
