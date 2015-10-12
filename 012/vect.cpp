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
#include <vector>
using namespace std;

#define BUF_SIZE 80

typedef struct param
{
    pthread_t* thread;
}Param;

	vector<int> vector;

	int _abort=0;

void* wrun(void* arg)
{
    //sleep(1);
	//Param* par = (Param*)arg;
	//double delta = (par->right-par->left);
	//double abs = (par->right+par->left)/2;

//	par->result= delta*abs;	// y=x
	//par->result= delta*sin(abs);	// y=sine(x)
	//printf("%e  %e  %e\n",par->left, par->right, par->result);
    return 0;
}


void usage (char* name)
{
    printf("\nUsage:\t%s writers readers num", name);
    printf("\twriters - wrirint thread number,\n\treaders - reading thread number,\n\tnum - container max size .\n");
    printf("Example: %s 30 20 50\n", name);
}


int main(int argc, char** argv, char** env)
{
    char *endprt;
    if (argc <3)
    {
	usage(basename(argv[0]));
	return 0;
    }

	int wrnum = atoi(argv[1]);
	int rdnum = atoi(argv[2]);	
	int num = atoi(argv[3]);

    if ((wrnum <= 0) || (rdnum <= 0) || (num<0) )
    {
		usage(basename(argv[0]));
		return 0;
    }


	Param* wrlist = (Param*)calloc(wrnum, sizeof(Param));
	int ret;

	for (int i=0; i<wrnum; i++)
	{
    		pthread_t* thread = new pthread_t;
		Param par;
		par.thread=pthread_t;

    		//ret = pthread_create(thread, NULL, wrun, NULL);
	}








/*
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
*/
    return 0;
}
