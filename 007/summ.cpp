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

typedef struct param
{
    double left;
    double righg;
}Param;


void* run(void* arg)
{
    sleep(5);
    return 0;
}


void usage (char* name)
{
    printf("\nUsage:\t%s N n\n", name);
    printf("\tN - period, [1-64],\n\tn - sample number [1-64].\n");
}


int main(int argc, char** argv, char** env)
{
    pthread_t thread;
    int ret;
    
    if (argc <3)
    {
	usage(basename(argv[0]));
	return 0;
    }

    int period = atoi (argv[1]);
    int sample = atoi (argv[2]);

    if ((period <= 0) || (sample <= 0))
    {
	usage(basename(argv[0]));
	return 0;
    }

    if ((period > 64) || (sample > 64))
    {
	usage(basename(argv[0]));
	return 0;
    }


    //Param* param = new Param;
    Param param;
    ret = pthread_create(&thread, NULL, run, &param);
    if(ret)
    {
	errno=ret;
	perror("pthread_create");
	return -1;
    }

    pthread_join(thread, NULL);


    cout << endl;
    return 0;
}
