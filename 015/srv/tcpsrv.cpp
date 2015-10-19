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

int s;
list<Client*> clientList;
list<char*> messageList;

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

        shutdown(cln->socket, SHUT_RDWR);
        close(cln->socket);
        delete cln;
        clientList.pop_back();
    }

    close(s);

    list<char*>::iterator it=messageList.begin();
    for ( ; it!=messageList.end(); ++it)
        free(*it);

    close(s);
    exit(0);
}


void* run (void* arg)
{
    char buf[BUF_SIZE];
    Client* client = (Client*)arg;
    int s=client->socket;

printf("new client started. Total: %d\n", clientList.size() );
    while(1)
    {
        fflush(0);
        int nb=recv(s, buf, BUF_SIZE, 0);
        if (nb <=0)	break;
        buf[nb]=0;

        switch (buf[0])
        {
        case '1':
            goto exit;
        case '2':
            if(buf[1]=='&')
            {
                char* to = buf;
                to=stpcpy(to, "Last 10 messages:\n");

                list<char*>::iterator it=messageList.begin();
                for ( ; it!=messageList.end(); ++it)
                {
                    to=stpcpy(to, *it);
                }
                nb = send(s, buf, to-buf, 0);
            }
            break;

        case '3':       // chat to all
            if(buf[1]=='&')
            {

                list<Client*>::iterator it=clientList.begin();
                for ( ; it!=clientList.end(); ++it)
                {
                    if( (*it)->socket == s) continue;
                    nb = send( (*it)->socket, buf+2, strlen(buf)-2, 0);
                }

                char* msg=(char*) malloc(128);
                nb = 128<=strlen(buf)-2?128:strlen(buf)-2;

                if(nb>0)
                {
                    memcpy(msg, buf+2, nb );
                    if(messageList.size()>=10)
                        messageList.pop_front();
                    messageList.push_back(msg);
                }
            }
            break;

        case '4':       // clients list
            if(buf[1]=='&')
            {
                char* to = buf;
                to=stpcpy(to, "Clients' list:\n");

                list<Client*>::iterator it=clientList.begin();
                for ( ; it!=clientList.end(); ++it)
                {


                    /*
                    in_addr adr1;
                    in_addr adr2;
                    adr1.s_addr=inet_addr("124.23.45.67");
                    adr2.s_addr=inet_addr("as.34.34.56");
                    if (adr1.s_addr!=INADDR_NONE)
                        cout << " adr1 correct" << endl;
                    else
                        cout << " adr1 incorect " << endl;

                    if (adr2.s_addr!=INADDR_NONE)
                        cout << " adr2 correct" << endl;
                    else
                        cout << " adr2 incorect" << endl;

                    cout << inet_ntoa(adr1) << endl;
                    cout << inet_ntoa(adr2) << endl;
                    */





                    to=stpcpy(to, inet_ntoa((*it)->addr.sin_addr));
                    //to=stpcpy(to, ":";
                    nb=sprintf(to, ":%d\n", (*it)->addr.sin_port);
                    to+=nb;
                }
                nb = send(s, buf, to-buf, 0);
            }
            break;

        default:
            break;
        }

    }
exit:
    shutdown(s, SHUT_RDWR);
    close(s);
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
    {
        usage(basename(argv[0]));
        return 0;
    }

    static struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);		// ^C

    //----------------------------------------------------------
    s=socket( AF_INET, SOCK_STREAM, 0 );

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
        int sock = accept(s, (sockaddr*)(&fromaddr), &ln);
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

    sigint_handler(12);
/*
    while (!clientList.empty())
    {
        Client* client=clientList.back();

        void* res;
        pthread_cancel(client->thread);
        pthread_join( client->thread, &res);

        if (res == PTHREAD_CANCELED)
            printf(" thread was canceled\n");

        close(client->socket);
        delete client;
        clientList.pop_back();
    }

    close(s);
*/
    return 0;
}
