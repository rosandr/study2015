#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
//#include <asm-generic/errno-base.h>
#include <sys/wait.h>
#include <linux/sched.h>
#include <math.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 80
#define MAX_SAMPLE_SIZE 128
#define STACK_SIZE (1024 * 1024)

typedef struct param
{
    double left;
    double right;
    double result;
    pid_t pid;
}Param;

static int run(void* arg)
{
    sleep(1);
    Param* par = (Param*)arg;
    double delta = (par->right-par->left);
    double abs = (par->right+par->left)/2;

    //par->result= delta*abs;	// y=x
    par->result= delta*sin(abs);	// y=sine(x)
    //printf("%e  %e  %e\n",par->left, par->right, par->result);
    return 0;
}

void usage (char* name)
{
    printf("\nUsage:\t%s N n\n", name);
    printf("\tN - interval, [double, positive],\n\tn - sample number [1-%d].\n", MAX_SAMPLE_SIZE);
    printf("Example: %s 3.14 20\n", name);
}


int main(int argc, char** argv, char** env)
{
    int status;
    char *endprt;
    if (argc <3)
    {
	usage(basename(argv[0]));
	return 0;
    }

    double interval = strtod (argv[1], &endprt);
    int sample = atoi (argv[2]);

    if ((interval <= 0) || (sample <= 0))
    {
	usage(basename(argv[0]));
	return 0;
    }

    if ((endprt == argv[1]) || (sample > MAX_SAMPLE_SIZE))
    {
	usage(basename(argv[0]));
	return 0;
    }

    char *stack;                    /* Start of stack buffer */
    char *stackTop;                 /* End of stack buffer */

    stack = (char*)malloc(STACK_SIZE*sample);
    if (stack == NULL)
    {
	perror("malloc");
	return -1;
    }
    stackTop = stack;

    Param* paramList[MAX_SAMPLE_SIZE]; 
    pid_t res;

    for (int i=0; i<sample; i++)
    {
	paramList[i]= new Param;
	paramList[i]->left=((double)interval/sample)*i;
	paramList[i]->right=((double)interval/sample)*(i+1);

	stackTop += STACK_SIZE;
    	res = clone (run, stackTop, CLONE_VM, paramList[i]);

    	if(res == -1)
    	{
	    perror("child create ");
	    return -1;
    	}
	paramList[i]->pid=res;
    }

    for (int i=0; i<sample; i++)
    {
	res = waitpid( paramList[i]->pid, &status, __WCLONE);    // Wait for child
	//printf("%d\n", res);
    }
    
    double totalSum=0;
    for (int i=0; i<sample; i++)
    {
    	totalSum+=paramList[i]->result;
    	delete paramList[i];
    }

    cout << "\tfunction y=SINE(x), interval [0 - " << interval << "], sample count=" <<sample <<endl;
    cout << "\tintegral="<<totalSum <<endl;

    return 0;
}
