#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
//#include <asm-generic/errno-base.h>
#include <pthread.h>
#include <math.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 80

typedef struct param
{
    double left;
    double right;
    double result;
    pthread_t* thread;
}Param;



void* run(void* arg)
{
    //sleep(1);
	Param* par = (Param*)arg;
	double delta = (par->right-par->left);
	double abs = (par->right+par->left)/2;

//	par->result= delta*abs;	// y=x
	par->result= delta*sin(abs);	// y=sine(x)
	//printf("%e  %e  %e\n",par->left, par->right, par->result);
    return 0;
}


void usage (char* name)
{
    printf("\nUsage:\t%s N n\n", name);
    printf("\tN - interval, [double, positive],\n\tn - sample number [1-64].\n");
    printf("Example: %s 3.14 20\n", name);
}


int main(int argc, char** argv, char** env)
{
    char *endprt;
    if (argc <3)
    {
	usage(basename(argv[0]));
	return 0;
    }

    double interval = strtod (argv[1], &endprt);
    int sample = atoi (argv[2]);


printf("%e\n", interval);


    if ((interval <= 0) || (sample <= 0))
    {
		usage(basename(argv[0]));
		return 0;
    }

    if ((endprt == argv[1]) || (sample > 64))
    {
	usage(basename(argv[0]));
	return 0;
    }

	Param* paramList[64]; 
    int ret;

	for (int i=0; i<sample; i++)
	{
    	pthread_t* thread = new pthread_t;
	paramList[i]= new Param;
	paramList[i]->left=((double)interval/sample)*i;
	paramList[i]->right=((double)interval/sample)*(i+1);

    	ret = pthread_create(thread, NULL, run, paramList[i]);
    	if(ret)
    	{
		errno=ret;
		perror("pthread_create");
		return -1;
    	}
    		//pthread_join(thread, NULL);
		paramList[i]->thread=thread;
	}

	for (int i=0; i<sample; i++)
		pthread_join( *(paramList[i]->thread), NULL);

	double totalSum=0;
	for (int i=0; i<sample; i++)
	{
		totalSum+=paramList[i]->result;
		delete paramList[i]->thread;
		delete paramList[i];
	}

	cout << "\tfunction y=SINE(x), interval [0 - " << interval << "], sample count=" <<sample <<endl;
	cout << "\tintegral="<<totalSum <<endl;

    return 0;
}
