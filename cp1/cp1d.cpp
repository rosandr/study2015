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

extern int optind, optopt;

void usage (char* name)
{
    printf("\nUsage:\t%s [options]  port\n", name);
    printf("(use CTRL-C to terminate %s)\n\n", name);
    printf("          -h  - print this help,\n");
    printf("Example: ./%s 10000\n", name);
}

int sock;
void sigint_handler(int sig) 
{
    syslog(LOG_NOTICE, "WEBsrv stop working by signal: %d", sig);
    closelog();
    close(sock);
    exit(0);
}

void runsrv(int s)
{
    //----------------------------------------------------------
    char buf[BUF_SIZE];

    sockaddr_in fromaddr;
    socklen_t ln=sizeof(fromaddr);

    while(1)
    {
        sock = accept(s, (sockaddr*)(&fromaddr), &ln);
        if( sock<= 0)
        {
            perror("accept");
            break;
        }
/*
        int nb=recv(sock, buf, BUF_SIZE, 0);
        if (nb <=0)	break;
        buf[nb]=0;
        //printf("%s \n", buf);

        char str[120];
        sscanf(buf, "%s %s", str, str);

        char* to = str+1;
        if(strlen(to)==0)	to="index.html";

        if (access(to, F_OK) == -1)
        {
            to = buf;
            to=stpcpy(to, "HTTP/1.1 404 Not Found\n");
            to=stpcpy(to, "Connection: keep-alive\n");
            to=stpcpy(to, "Content-Type: text/html; charset=UTF-8\n");
            to=stpcpy(to, "Keep-Alive: timeout=5,max=97\n");
            to=stpcpy(to, "\n");
            to=stpcpy(to, "Erorr 404 Page not found\n");
        }
        else
        {
            int n = strcmp(to, "i.jpeg");
            if(n==0)
            {
                to = buf;
                fseek(fd1, 0, SEEK_SET);
                to=stpcpy(to, "HTTP/1.1 200 OK\n");
                to=stpcpy(to, "Connection: keep-alive\n");
                to=stpcpy(to, "Content-Type: image/jpeg\n");
                to=stpcpy(to, "Keep-Alive: timeout=5,max=97\n");
                to=stpcpy(to, "\n");
                nb=fread (to, fd1size, 1, fd1);
                if (nb <=0)	break;
                to+=fd1size;
            }
            else
            {
                to = buf;
                fseek(fd, 0, SEEK_SET);
                to=stpcpy(to, "HTTP/1.1 200 OK\n");
                to=stpcpy(to, "Connection: keep-alive\n");
                to=stpcpy(to, "Content-Type: text/html; charset=UTF-8\n");
                to=stpcpy(to, "Keep-Alive: timeout=5,max=97\n");
                to=stpcpy(to, "\n");
                nb=fread (to, fdsize, 1, fd);
                if (nb <=0)	break;
                to+=fdsize;
            }
        }
        to=stpcpy(to, "\n");
*/
//        send(sock, buf, to-buf, 0);
        close(sock);
    }
    closelog();
}

unsigned short localport;
int main(int argc, char** argv, char** env)
{

    int opt;
    int mode = 0;

    if (argc <2)
    {
	usage(basename(argv[0]));
	return 0;
    }



/*
    while ((opt = getopt(argc, argv, "hdp:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            usage(basename(argv[0]));
            exit(0);

        case 'p':
            localport = atoi(optarg);
            if((localport<9999)||(localport>64000))
            {
                printf("Invalid port number\n");
                exit(0);
            }
            break;
//        case 'd':
//            mode = 1;
//            break;

        default:
            usage(basename(argv[0]));
            exit(0);
        }
    }
*/
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

    // daemon mode

        pid_t pid;
        switch( pid=fork() )
        {
        case -1:
            perror("fork");
            exit(1);

        case 0:		// child
            static struct sigaction act;
            act.sa_handler = sigint_handler;
            sigaction(SIGINT, &act, NULL);		// ^C

            setsid();

            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);

            //openlog("WEBsrv", 0, LOG_USER);
            //syslog(LOG_NOTICE, "WEBsrv start working on port: %d", localport);

            runsrv(s);
        }
        //runsrv(s);
        close(sock);

    return 0;
}
