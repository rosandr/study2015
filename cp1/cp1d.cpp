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
#define BUF_SIZE 256

void usage (char* name)
{
    printf("\nUsage:\t%s  port\n", name);
    printf("Example: ./%s 10000\n", name);
}

int sock;
unsigned short localport;

bool usr1_en=true;
int usr1_count=0;
bool usr2_en=true;
int usr2_count=0;

void sigint_handler(int sig) 
{
    if(sig==SIGINT)
    {
        syslog(LOG_NOTICE, "stop working by signal: %d", sig);
        closelog();
        close(sock);
        exit(0);
    }
    if( sig==SIGUSR1 )
    {
        syslog(LOG_NOTICE, "income signal USR1: %d", sig);
        if(usr1_en)    usr1_count++;
    }
    else if(sig==SIGUSR2)
    {
        syslog(LOG_NOTICE, "income signal USR2: %d", sig);
        if(usr2_en)    usr2_count++;
    }
}

char helpStr[] = "Use en1, en2 to enable SIGUSR1/2,\ndis1, dis2 to disable SIGUSR1/2,\nstat to print statistics,\nexit to quit program.\n\n";

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

    send(sock, helpStr, strlen(helpStr), 0);

    while( 1 )
    {
        int nb=recv(sock, buf, BUF_SIZE, 0);
        if (nb <=0)	continue;   // break;?????????????????????????

        buf[nb]=0;

        if(!strncmp(buf, "en1", 3))
        {
            nb = snprintf(buf, sizeof(buf), "%s\n", "enable usr1 counter");
            send(sock, buf, nb, 0);
            usr1_en=true;
        }
        else if(!strncmp(buf, "en2", 3))
        {
            nb = snprintf(buf, sizeof(buf), "%s\n", "enable usr2 counter");
            send(sock, buf, nb, 0);
            usr2_en=true;
        }
        else if(!strncmp(buf, "dis1", 4))
        {
            nb = snprintf(buf, sizeof(buf), "%s\n", "disable usr1 counter");
            send(sock, buf, nb, 0);
            usr1_en=false;
        }
        else if(!strncmp(buf, "dis2", 4))
        {
            nb = snprintf(buf, sizeof(buf), "%s\n", "disable usr2 counter");
            send(sock, buf, nb, 0);
            usr2_en=false;
        }
        else if(!strncmp(buf, "stat", 4))
        {
            nb = snprintf(buf, sizeof(buf), "usr1 count=%d, usr2 count=%d\n", usr1_count, usr2_count);
            send( sock, buf, nb, 0 );
        }
        else if(!strncmp(buf, "exit", 4))
        {
            send(sock, "bye...\n", strlen("bye...\n"), 0);
            break;
        }
        else
            send(sock, helpStr, strlen(helpStr), 0);
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
    if((localport<999)||(localport>54000))
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

        fprintf(stderr, "child pid=%d\n", getpid());

        struct sigaction act;
        memset(&act, 0, sizeof(act));
        act.sa_handler = sigint_handler;

        sigset_t   set;
        sigemptyset(&set);
        sigaddset(&set, SIGINT);
        sigaddset(&set, SIGUSR1);
        sigaddset(&set, SIGUSR2);

        act.sa_mask = set;
        sigaction(SIGINT,  &act, 0);
        sigaction(SIGUSR1, &act, 0);
        sigaction(SIGUSR2, &act, 0);

        setsid();

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        openlog("MYDAEMON", 0, LOG_USER);
        syslog(LOG_NOTICE, "daemon start working on port: %d", localport);

        runsrv(s);
    }
    usleep(100000);
    return 0;
}
