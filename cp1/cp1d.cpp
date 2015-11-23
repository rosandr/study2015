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

#include <syslog.h>
#include <getopt.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 3096

//extern int optind, optopt;

unsigned short localport;

void usage (char* name)
{
    printf("\nUsage:\t%s  port\n", name);
    printf("Example: ./%s 10000\n", name);
}

int sock;
void sigint_handler(int sig) 
{
    syslog(LOG_NOTICE, "stop working by signal: %d", sig);
    closelog();
    close(sock);
    exit(0);
}

int usr1_count=0;
void sigint_handler_usr1(int sig) 
{
    syslog(LOG_NOTICE, "income signal USR1: %d", sig);
    usr1_count++;
    return;
}

int usr2_count=0;
void sigint_handler_usr2(int sig) 
{
    syslog(LOG_NOTICE, "income signal USR2: %d", sig);
    usr2_count++;
    return;
}


void runsrv(int s)
{
    //----------------------------------------------------------
    char buf[BUF_SIZE];

    sockaddr_in fromaddr;
    socklen_t ln=sizeof(fromaddr);

    sock = accept(s, (sockaddr*)(&fromaddr), &ln);
    if( sock<= 0)
    {
        perror("accept");
        return;
    }


    while(1)
    {
        int nb=recv(sock, buf, BUF_SIZE, 0);
        if (nb <=0)	break;
        //buf[nb]=0;

        printf("%d \n", buf[0]);

	switch(buf[0])
	{
	    case 1:

		break;

	    case 2:

		break;

	    case 3:

		break;
	}
//        send(sock, buf, to-buf, 0);
    }
    close(sock);
    closelog();
}


int main(int argc, char** argv, char** env)
{
    if (argc <2)
    {
	usage(basename(argv[0]));
	return 0;
    }


    localport = atoi(argv[1]);
    if((localport<9999)||(localport>64000))
    {
        printf("port range error: %d\n", localport);
        usage(basename(argv[0]));
        return 0;
    }

    //----------------------------------------------------------
    int s=socket( AF_INET, SOCK_STREAM, 0 );

    struct sockaddr_in inaddr;
    memset(&inaddr, 0, sizeof(inaddr));
    inaddr.sin_family = AF_INET;
    inaddr.sin_addr.s_addr = htonl((in_addr_t) INADDR_ANY);
    inaddr.sin_port = htons(localport);

    while( bind(s, (struct sockaddr*)&inaddr, sizeof(inaddr)) < 0)
    {
	printf(".");
	fflush(NULL);
	usleep(100000);
    }

    if(listen(s, 10)<0)
    {
        perror("listen");
        return 1;
    }

        pid_t pid;
        switch( pid=fork() )
        {
        case -1:
            perror("fork");
            exit(1);

        case 0:		// child
//            static struct sigaction act;
//            act.sa_handler = sigint_handler;
//            sigaction(SIGINT, &act, NULL);		// ^C

            static struct sigaction act1;
            act1.sa_handler = sigint_handler_usr1;
            sigaction(SIGUSR1, &act1, NULL);		// USR1

            static struct sigaction act2;
            act2.sa_handler = sigint_handler_usr2;
            sigaction(SIGUSR2, &act2, NULL);		// USR2


            setsid();

            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);

            openlog("MYDAEMON", 0, LOG_USER);
            syslog(LOG_NOTICE, "daemon start working on port: %d", localport);

            runsrv(s);
        }
	sleep(1);
    return 0;
}
