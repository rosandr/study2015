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
#include <arpa/inet.h>

#include <pthread.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 256

//=========================================================================
void usage (char* name)
{
    printf("\nUsage:\t%s [ip] port\n", name);
    printf("\tip  - server IPv4 address,\n");
    printf("\tlocalport  - server port number  [10000 - 64000],\n");
    printf("Example: ./%s 192.168.1.99 10000.\n", name);
}

void print_menu()
{
    printf("\nenter your selection: \n");
    printf("\t1 - disconnect and exit\n");
    printf("\t2 - get 10 last messages\n");
    printf("\t3 - chat your message to all\n");
}

//-------------------------------------------------------------------------
int s;
struct sockaddr_in addr;
pthread_t thread;

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

int main(int argc, char** argv, char** env)
{
    struct in_addr ip;
    unsigned short port;

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

    char buf[BUF_SIZE];

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

    //---------------------------------------------------------
    while(1)
    {
        print_menu();

        char sel = getchar();
        if (fgets (buf, BUF_SIZE, stdin) != NULL) ;     // ???

        switch (sel)
        {
        case '1':
            //_abort=1;
            printf("client exited\n");
            return 0;
        case '2':
            buf[0]=sel;
            buf[1]='&';
            nb=send( s, buf, 2, 0);
            break;
        case '3':
            printf("Please, type your message and press <ENTER>\n");

            buf[0]=sel;
            buf[1]='&';
            if (fgets (buf+2, BUF_SIZE, stdin) != NULL)
                nb = send( s, buf, strlen(buf), 0);
            break;

        default:
            break;
        }
        usleep(100000);
    }

    //---------------------------------------------------------
    void* res;
    pthread_cancel(thread);
    pthread_join( thread, &res);

    if (res == PTHREAD_CANCELED)
        printf(" thread was canceled\n");

    close(s);
    return 0;
}
