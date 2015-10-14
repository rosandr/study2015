#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 256

void usage (char* name)
{
	printf("\nUsage:\t%s localport remoteport\n", name);
	printf("\tlocalport  - local port number  [32675 - 64000],\n");
	printf("\tremoteport - remote port number [32675 - 64000].\n");
	printf("Example: ./%s 42443 42556  // in one xterm,\n", name);
	printf("         ./%s 42556 42443  // in another xterm\n", name);
}

typedef struct param
{
    int sock;
    struct sockaddr_in* inaddr;
    struct sockaddr_in* outaddr;
}Param;


pthread_t thread1, thread2;
void sigint_handler(int sig) 
{
	pthread_cancel(thread1);
	pthread_cancel(thread2);
 }

void* run1 (void* arg)
{
	Param* par = (Param*)arg;

	char buf[BUF_SIZE];
	buf[0]=0;
        sockaddr_in fromaddr;
        socklen_t ln=sizeof(fromaddr);

	printf("Please, type your message...\n");
	while(1)
	{
        	int nb=recvfrom(par->sock, buf, BUF_SIZE, 0, (sockaddr*)&fromaddr, &ln);
		if (nb <=0)	break;
		buf[nb]=0;
		printf(">> %s\n",buf);
	}
	return 0;
}

void* run2 (void* arg)
{
	Param* par = (Param*)arg;
	
	char buf[BUF_SIZE];
	buf[0]=0;
        while( fgets(buf, BUF_SIZE, stdin) !=NULL )
	{
		int n =sendto( par->sock, buf, BUF_SIZE, 0, (sockaddr*)(par->outaddr), sizeof(sockaddr_in));
		if(n<=0)	break;
	}
	return 0;
}


int main(int argc, char** argv, char** env)
{
	Param par;
	unsigned short localport, remoteport;

	if (argc < 3)
	{
	    usage(basename(argv[0]));
	    return 0;
	}

	localport =  atoi(argv[1]);
	remoteport = atoi(argv[2]);

	if((localport<32675)||(localport>64000))
		usage(basename(argv[0]));

	if((remoteport<32675)||(remoteport>64000))
		usage(basename(argv[0]));

	if( localport==remoteport )
	{
		printf("\nPorts must differ!\n");
		usage(basename(argv[0]));
		exit(1);
	}

        static struct sigaction act;
	act.sa_handler = sigint_handler;
	sigaction(SIGINT, &act, NULL);		// ^C

	//----------------------------------------------------------
	int s=socket( AF_INET, SOCK_DGRAM, 0 );

	struct sockaddr_in inaddr;
	memset(&inaddr, 0, sizeof(inaddr));
	inaddr.sin_family = AF_INET;
	inaddr.sin_addr.s_addr = htonl((in_addr_t) INADDR_ANY);
	inaddr.sin_port = htons(localport);

	struct sockaddr_in outaddr;
	memset(&outaddr, 0, sizeof(outaddr));
	outaddr.sin_family = AF_INET;
	outaddr.sin_addr.s_addr = htonl((in_addr_t) INADDR_ANY);
	outaddr.sin_port = htons(remoteport);

        while( bind(s, (struct sockaddr*)&inaddr, sizeof(inaddr)) < 0)
	{
		printf(".");
		fflush(NULL);
		usleep(100000);
	}

	par.sock = s;
	par.inaddr = &inaddr;
	par.outaddr = &outaddr;

	//----------------------------------------------------------
	pthread_create( &thread1, NULL, run1, &par);
	pthread_create( &thread2, NULL, run2, &par);

	void* res;
	pthread_join( thread1, &res);

	if (res == PTHREAD_CANCELED)
               printf(" thread1 was canceled\n");
        else
               printf("thread1 wasn't canceled\n");

	pthread_join( thread2, &res);
	if (res == PTHREAD_CANCELED)
               printf(" thread2 was canceled\n");
        else
               printf("thread2 wasn't canceled\n");

	close(s);
	return 0;
}
