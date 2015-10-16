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
#define BUF_SIZE 3096

void usage (char* name)
{
	printf("\nUsage:\t%s localport\n", name);
	printf("\tlocalport  - local port number  [10000 - 64000],\n");
	printf("Example: ./%s 42443   // in one host,\n", name);
	printf("         ./%s 42443   // in another host\n", name);
}

int sock;
void sigint_handler(int sig) 
{
    close(sock);
    exit(0);
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
    close(s);
    fclose(fd);
    fclose(fd1);

    return 0;
}
