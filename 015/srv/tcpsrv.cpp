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
#define BUF_SIZE 3096

#include <list>

//============================================================================
void usage (char* name)
{
    printf("\nUsage:\t%s localport\n", name);
    printf("\tlocalport  - local port number  [10000 - 64000],\n");
    printf("Example: ./%s 10000   // in one host,\n", name);
}

typedef struct param
{
    int socket;
    struct sockaddr_in addr;
    pthread_t thread;
}Client;

int sock;
list<Client*> clientList;

void sigint_handler(int sig)
{
    printf("Start delete clients. Total: %d\n", clientList.size() );
    while (!clientList.empty())
    {
        Client* cln=clientList.back();

        void* res;
        pthread_cancel(cln->thread);
        pthread_join(  cln->thread, &res);

        if (res == PTHREAD_CANCELED)
            printf("thread was canceled from sig_handler\n");
        else
            printf("thread wasn't canceled\n");

        shutdown(cln->socket, SHUT_RDWR);
        close(cln->socket);
        delete cln;
        clientList.pop_back();
    }

    close(sock);
    exit(0);
}


void* run (void* arg)
{
    char buf[BUF_SIZE];
    Client* client = (Client*)arg;

printf("new client started. Total: %d\n", clientList.size() );
    while(1)
    {
        int nb=recv(client->socket, buf, BUF_SIZE, 0);
        if (nb <=0)	break;
        buf[nb]=0;

printf("%s \n", buf);

//printf("%d \n", nb);





    }
    shutdown(client->socket, SHUT_RDWR);
    close(client->socket);
    clientList.remove(client);

    printf("client deleted. Remine: %d\n", clientList.size() );

    delete client;
    return 0;
}


int main(int argc, char** argv, char** env)
{
    unsigned short localport;

    if (argc < 2)
    {
        usage(basename(argv[0]));
        return 0;
    }

    localport =  atoi(argv[1]);
    if((localport<9999)||(localport>64000))
    usage(basename(argv[0]));

    static struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);		// ^C

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

    //----------------------------------------------------------
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

        //----------------------------------------------------------
        // have new connection:
        Client* client = new Client;
        client->socket = sock;
        client->addr = fromaddr;

        //----------------------------------------------------------
        if( 0!=pthread_create( &client->thread, NULL, run, client) )
        {
            perror("pthread_create");
            break;
        }
        clientList.push_back(client);
        //printf("new client accepted. Total: %d\n", clientList.size() );
    }

    while (!clientList.empty())
    {
        Client* client=clientList.back();

        void* res;
        pthread_cancel(client->thread);
        pthread_join( client->thread, &res);

        if (res == PTHREAD_CANCELED)
            printf(" thread was canceled\n");
        else
            printf("thread wasn't canceled\n");

        close(client->socket);
        delete client;
        clientList.pop_back();
    }

    close(s);
    return 0;
}
