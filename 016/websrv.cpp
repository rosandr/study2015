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
#define BUF_SIZE 1256

void usage (char* name)
{
	printf("\nUsage:\t%s localport\n", name);
	printf("\tlocalport  - local port number  [32675 - 64000],\n");
	printf("Example: ./%s 42443   // in one host,\n", name);
	printf("         ./%s 42443   // in another host\n", name);
}

typedef struct param
{
    int sock;
    struct sockaddr_in* inaddr;
    struct sockaddr_in* outaddr;
}Param;


pthread_t thread1;
void sigint_handler(int sig) 
{
	pthread_cancel(thread1);
}

void* run1 (void* arg)
{
    int s;
    Param* par = (Param*)arg;

    char buf[BUF_SIZE];
    buf[0]=0;
    sockaddr_in fromaddr;
    socklen_t ln=sizeof(fromaddr);

    FILE* fd = fopen("index.html", "r");
    if (fd<0)
    {
        perror("open");
        return 0;
    }



    while(1)
    {
	s = accept(par->sock, (sockaddr*)(&fromaddr), &ln);
	if( s<= 0)
	{
	    perror("accept");
	    break;
	}

        int nb=recv(s, buf, BUF_SIZE, 0);
        if (nb <=0)	break;
        buf[nb]=0;
        printf("%s \n", buf);

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
	}
	else
	{
	    to = buf;
	    to=stpcpy(to, "HTTP/1.1 200 OK\n");
	    to=stpcpy(to, "Connection: keep-alive\n");
	    to=stpcpy(to, "Content-Type: text/html; charset=UTF-8\n");
	    to=stpcpy(to, "Keep-Alive: timeout=5,max=97\n");
	    to=stpcpy(to, "\n");

	    while( fgets(str, 120, fd)>0 )
	    {
		to=stpcpy(to, str);
	    }
	    to=stpcpy(to, "\n");
	}
//printf("%s \n", to);

	send(s, buf, to-buf, 0);
	close(s);
    }
    fclose(fd);
    shutdown(s, SHUT_RDWR);
    return 0;
}

int main(int argc, char** argv, char** env)
{
	Param par;
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
	    exit(1);
	}

	par.sock = s;
	par.inaddr = &inaddr;
//	par.outaddr = &outaddr;

	//----------------------------------------------------------
	pthread_create( &thread1, NULL, run1, &par);

	void* res;
	pthread_join( thread1, &res);

	close(s);
	return 0;
}
