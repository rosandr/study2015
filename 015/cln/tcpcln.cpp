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
#define BUF_SIZE 3096

void usage (char* name)
{
    printf("\nUsage:\t%s [ip] port\n", name);
    printf("\tip  - server IPv4 address,\n");
    printf("\tlocalport  - server port number  [10000 - 64000],\n");
    printf("Example: ./%s 192.168.1.99 10000.\n", name);
}

//-------------------------------------------------------------------------
typedef struct param
{
    int socket;
    struct sockaddr_in addr;
    //pthread_t thread;
}Param;

int sock;
int _abort=0;       // to stop thread

void sigint_handler(int sig) { _abort=1; }

void print_menu()
{
    printf("\nenter your selection: \n");
    printf("\t1 - disconnect and exit\n");
    printf("\t2 - get 10 last messages\n");
    printf("\t3 - chat your message\n");
}


void* run (void* arg)
{
    char buf[BUF_SIZE];
    Param* par = (Param*)arg;

    while (1)
    {
        int res = connect(par->socket, (sockaddr*)&par->addr, sizeof(sockaddr_in));
        //if(res!=0)  break;

        while(1)
        {
            print_menu();
            char sel = getchar();
            fflush(stdin);

            switch (sel)
            {
            case '1':
                _abort=1;
                printf("client thread exited\n");
                return 0;
            case '2':

                break;
            case '3':
                printf("Please, type your message and press <ENTER>\n");

                if (fgets (buf, BUF_SIZE, stdin) != NULL)
                {
                    printf("wertwert\n");
                    send( par->socket, buf, BUF_SIZE, 0);
                }
                break;

            default:
                break;
            }
        }

    }
    close(sock);
    printf("client thread exited\n");
    return 0;
}







int main(int argc, char** argv, char** env)
{

    struct in_addr ip;
    unsigned short port;
    /*


    struct sockaddr_in {
        sa_family_t    sin_family; // семейство адресов: AF_INET
        u_int16_t      sin_port;   // порт сокета в сетевом порядке байт
        struct in_addr  sin_addr;  // адрес в интернете
    };

    // Адрес в интернете.
    struct in_addr {
        u_int32_t      s_addr;     //
    };





    struct in_addr ip;
    struct hostent *hp;

    if (!inet_aton(ipstr, &ip))
            errx(1, "can't parse IP address %s", ipstr);

    if ((hp = gethostbyaddr((const void *)&ip,
        sizeof ip, AF_INET)) == NULL)
            errx(1, "no name associated with %s", ipstr);

    printf("name associated with %s is %s\n", ipstr, hp->h_name);

    */






    if (argc < 2)
    {
        usage(basename(argv[0]));
        return 0;
    }

    if (argc < 3)
    {
        ip.s_addr = INADDR_ANY;
        port =  atoi(argv[1]);
    }
    else
    {
        char* ipstr = argv[2];
        port =  atoi(argv[3]);

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
    int s=socket( AF_INET, SOCK_STREAM, 0 );

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl((in_addr_t)ip.s_addr);
    addr.sin_port = htons(port);

    Param par;
    par.socket = sock;
    par.addr = addr;

    pthread_t thread;


    if( 0!=pthread_create( &thread, NULL, run, &par) )
    {
        perror("pthread_create");
        return 0;
    }


    while(!_abort)
    {
        //printf(".");
        //fflush(NULL);
        usleep(100000);
    }






/*
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

    //----------------------------------------------------------
    FILE* fd =  fopen("index.html", "r");
    FILE* fd1 = fopen("i.jpeg", "r");

    if ((fd<0) || (fd1<0))
    {
        perror("open");
        return 1;
    }

    fseek(fd, 0, SEEK_END);
    long fdsize = ftell(fd);

    fseek(fd1, 0, SEEK_END);
    long fd1size = ftell(fd1);

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

    send(sock, buf, to-buf, 0);
    //shutdown(sock, SHUT_RDWR);
    close(sock);
    }
*/


    void* res;
    pthread_cancel(thread);
    pthread_join( thread, &res);

    if (res == PTHREAD_CANCELED)
        printf(" thread was canceled\n");

    close(s);

    return 0;
}
