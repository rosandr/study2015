#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <error.h>
#include <asm-generic/errno-base.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 80

int main(int argc, char** argv, char** env)
{
    if (argc <3)
    {
	printf("\nUsage:\t%s pid sig\n", basename(argv[0]));
	printf("\tpid - process ID,\n\tsig - sinnal number [1-32].\n");
	return 0;
    }

    int pid = atoi (argv[1]);
    int sig = atoi (argv[2]);

    printf("U sure signal=%d to be sent to process=%d (y/n)?...", sig, pid);
    int ch = fgetc(stdin);

    if( ch=='y')
    {
	// printf("%d  y=%d  n=%d", ch, 'y', 'n');

	int rez = kill ( pid, sig );
	switch(rez)
	{
	case EINVAL:	// invalid signal
	    cerr << "Invalid signal: " << sig;
	    break;
	case ESRCH:	// pid  does not exist	
	    cerr << "Invalid pid: " << pid;    
	    break;
	case -1:
	    cerr << "Process does not exist: " << pid;    
	    break;                             	
	case EPERM:	// pid  does not have permission
	    cerr << "U have no permissions: ";	    
	    break;                            	
	default:
	    break;
	}
    }
    cout << endl;
    return 0;
}
