#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/wait.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

//#include <pthread.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 1024

#include "../chardev.h"
/*
//=========================================================================
void usage (char* name)
{
    printf("\nUsage:\t%s [ip] port\n", name);
    printf("\tip  - server IPv4 address,\n");
    printf("\tlocalport  - server port number  [10000 - 64000],\n");
    printf("Example: ./%s 192.168.1.99 10000.\n", name);
}
*/
void print_menu()
{
    printf("\nenter your selection: \n");
    printf("\t1 - exit\n");
    printf("\t2 - read chardev\n");
    printf("\t3 - write to chardev\n");
    printf("\t4 - get statistic\n");
    printf("\t5 - reset statistic\n");
    printf("\t6 - print process list\n");
}

//-------------------------------------------------------------------------
//int s;
//struct sockaddr_in addr;
//pthread_t thread;
/*
void sigint_handler(int sig)
{
    shutdown(s, SHUT_RDWR);
    close(s);

    pthread_cancel(thread);
    pthread_join( thread, NULL);

    exit(0);
}

void* run (void* arg)
{
    char buf[BUF_SIZE];
    while(1)
    {
        int nb = recv(s, buf, BUF_SIZE, 0);
        if ( nb>0 )
        {
            buf[nb]=0;
            printf("%s\n", buf);
        }
        else
        {
            printf("connection refused\n");
            break;
        }
    }

    close(s);
    printf("client thread exited\n");
    return 0;
}
*/
int main(int argc, char** argv, char** env)
{
    int nb;
    //struct in_addr ip;
    //unsigned short port;
/*
    if (argc < 2)
    {
        usage(basename(argv[0]));
        return 0;
    }

    if (argc < 3)
    {
        ip.s_addr = htonl(INADDR_ANY);
        port =  atoi(argv[1]);
    }
    else
    {
        char* ipstr = argv[1];
        port = atoi(argv[2]);

        if (!inet_aton(ipstr, &ip))
        {
            printf("can't parse IP address %s\n", ipstr);
            usage(basename(argv[0]));
            return 0;
        }
    }

    if((port<9999)||(port>64000))
    {
        printf("port range error: %d\n", port);
        usage(basename(argv[0]));
        return 0;
    }

    static struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);		// ^C

    //----------------------------------------------------------
    s=socket( AF_INET, SOCK_STREAM, 0 );

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ((in_addr_t)ip.s_addr);
    addr.sin_port = htons(port);
*/
    char buf[BUF_SIZE];
/*
    int nb = connect(s, (sockaddr*)&addr, sizeof(sockaddr_in));
    if(nb!=0)
    {
        perror("connect");
        exit(1);
    }

    if( 0!=pthread_create( &thread, NULL, run, NULL) )
    {
        perror("pthread_create");
        close(s);
        return 0;
    }
*/

    char devfile_name[] = "/dev/kbuf";
    FILE* fd = fopen(devfile_name, "r+");

    if(fd==NULL)
    {
        perror("File open");
        printf("\nPlease, create %s manualy and insert module.\n", devfile_name);
        return 0;
    }

    //---------------------------------------------------------
    while(1)
    {
        print_menu();

        char sel = getchar();
        if (fgets (buf, BUF_SIZE, stdin) != NULL) ;     // ???

        switch (sel)
        {
        case '1':
            printf("...exited\n");
            return 0;
        case '2':

            buf[0]=0;
            fgets (buf, BUF_SIZE, fd);
            printf("%d  %s\n", strlen(buf), buf);

/*
            buf[0]=0;
            nb=read( fileno(fd), buf, BUF_SIZE );
            fprintf( stdout, "%s\n", buf);

            if(nb<0)
            {
                printf("\tKbuf read error. (May be module not loaded?)\n");
                return 0;
            }
            else if (nb==0)
            {
                fprintf( stdout, "Chardev is empty.\n");
                fprintf( stdout, "%s\n", buf);
                continue;
            }
            else fprintf( stdout, "%s\n", buf);
*/
            //fgets (buf, BUF_SIZE, fd);
            //printf("%d\n", strlen(buf));

            /*
            if ((fgets (buf, BUF_SIZE, fd) == NULL) && (strlen(buf)==0) )
            {
                //printf("\tKbuf read error. (May be module not loaded?)\n");
                fprintf( stdout, "Chardev is empty.\n");
                //return 0;
            }*/
            //else fprintf( stdout, "%s\n", buf);
            break;
        case '3':
            printf("Please, type string and press <ENTER>\n");
            if (fgets (buf, BUF_SIZE, stdin) != NULL)
            {
                if(fputs(buf, fd)<0)
                {
                    printf("\tKbuf write error. (May be module not loaded?)\n");
                    return 0;
                }
                printf("wr ok\n");
            }
            break;
        case '4':
        case '5':
        case '6':
            printf("not implemented yet\n");
            break;
        default:
            break;
        }
        usleep(100000);
    }
/*
    //---------------------------------------------------------
    void* res;
    pthread_cancel(thread);
    pthread_join( thread, &res);

    if (res == PTHREAD_CANCELED)
        printf(" thread was canceled\n");

    close(s);
*/
    return 0;
}
