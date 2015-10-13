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

int main(int argc, char** argv, char** env)
{
	unsigned short localport, remoteport;
        pid_t pid1, pid2;

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

	//----------------------------------------------------------
	int s=socket( AF_INET, SOCK_DGRAM, 0 );

	struct sockaddr_in inaddr;
	memset(&inaddr, 0, sizeof(inaddr));
	inaddr.sin_family = AF_INET;
	inaddr.sin_addr.s_addr = htonl((in_addr_t) INADDR_BROADCAST);
	inaddr.sin_port = htons(localport);

	struct sockaddr_in outaddr;
	memset(&outaddr, 0, sizeof(outaddr));
	outaddr.sin_family = AF_INET;
	outaddr.sin_addr.s_addr = htonl((in_addr_t) INADDR_LOOPBACK);
	outaddr.sin_port = htons(remoteport);


	//----------------------------------------------------------
    switch(pid1=fork())	
    {
	case -1:
	    perror("fork1");
	    exit(1);
	case 0:
        while( bind(s, (struct sockaddr*)&inaddr, sizeof(inaddr)) < 0)
	{
    		printf(".");
		fflush(NULL);
    		usleep(100000);
	}

	char buf[BUF_SIZE];
	buf[0]=0;
        sockaddr_in fromaddr;
        socklen_t ln=sizeof(fromaddr);

	printf("Please, type your message...\n");

	while(1)
	{
        	int nb=recvfrom(s, buf, BUF_SIZE, 0, (sockaddr*)&fromaddr, &ln);
		if (nb <=0)	break;
		buf[nb]=0;
		if( fromaddr.sin_port == ntohs(remoteport) )
			printf(">> %s\n",buf);
	}
    }

    switch(pid2=fork())		// transmiter
    {
	case -1:
		perror("fork2");
		exit(2);
	case 0:
		char buf[BUF_SIZE];
		buf[0]=0;
		while( fgets(buf, BUF_SIZE, stdin) !=NULL )
		{
			int n =sendto( s, buf, BUF_SIZE, 0, (sockaddr*)&outaddr, sizeof(sockaddr_in));
			if(n<=0)	break;
		}
    }

    if( waitpid(pid1, &s, 0)!=pid1 || waitpid(pid2, &s, 0)!=pid2 )
	perror("waitpid");

    close(s);
    return 0;
}
