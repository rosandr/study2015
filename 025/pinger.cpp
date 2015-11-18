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
#include <netinet/ip_icmp.h>

#include <getopt.h>
#include <iostream>
using namespace std;
#define BUF_SIZE 1500

extern int optind;

//=========================================================================
void usage (char* name)
{
    printf("\nUsage:\t%s [options]  <ipaddr>\n", name);
    printf("(use CTRL-C to terminate %s)\n\n", name);
    printf("ipaddr    - ip address;\n");
    printf("Options:  -n N  repeat ping N times (default 4);\n");
    printf("          -h  - print this help.\n");
    printf("Example: ./%s -n 5 192.168.1.99\n", name);

}

//-------------------------------------------------------------------------
int s;
void sigint_handler(int sig)
{
    shutdown(s, SHUT_RDWR);
    close(s);

    exit(0);
}

u_int16_t checkSum(u_int16_t *buffer, int size)
{
    unsigned long cksum=0;
    while (size > 1){
        cksum += *buffer++;
        size  -= sizeof(u_int16_t);
    }
    if (size)
        cksum += *(u_int8_t*)buffer;

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (u_int16_t)(~cksum);
}


int main(int argc, char** argv, char** env)
{
    int opt, repeat = 4;
    struct sockaddr_in addr;
    struct in_addr ip;

    while ((opt = getopt(argc, argv, "hn:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            usage(basename(argv[0]));
            exit(0);

        case 'n':
            repeat = atoi(optarg);
            if(repeat<0)
            {
                printf("Invalid option\n");
                exit(0);
            }
        default:
            break;
        }
    }

    if (optind == argc)
    {
        usage(basename(argv[0]));
        exit(0);
    }

    char* ipstr = argv[optind];
    if (!inet_aton(ipstr, &ip))
    {
        printf("can't parse IP address %s\n", ipstr);
        usage(basename(argv[0]));
        return 0;
    }

    static struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);		// ^C

    //----------------------------------------------------------
    s=socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(s<=0)
    {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ((in_addr_t)ip.s_addr);
    addr.sin_port = htons(0);

    // prepare ICMP echo request
    struct icmphdr echoReq;

    memset(&echoReq, 0, sizeof(struct icmphdr));
    echoReq.type = ICMP_ECHO;
    echoReq.checksum = checkSum((u_int16_t *)&echoReq,
            sizeof(struct icmphdr));

    printf("starting PING %s %d bytes of data\n", ipstr, sizeof(struct icmphdr));

    int nb;
    fd_set rdfs;
    struct timeval tVal;

    while(repeat--)
    {
        nb =sendto( s, (u_int8_t*)&echoReq, sizeof(struct icmphdr), 0,
                       (sockaddr*)&addr, sizeof(sockaddr_in));
        if(nb<=0)
        {
            perror("sendto");
            close(s);
            return -1;
        }

        tVal.tv_sec = 1;
        tVal.tv_usec = 0;

        FD_ZERO(&rdfs);
        FD_SET(s, &rdfs);

        if ((nb = select(5, &rdfs, NULL, NULL, &tVal)) < 0)
        {
            perror("select");
            continue;
        }

        if (FD_ISSET(s, &rdfs))
        {
            char buf[BUF_SIZE];
            sockaddr_in fromaddr;
            socklen_t ln=sizeof(fromaddr);


            nb=recvfrom(s, buf, BUF_SIZE, 0, (sockaddr*)&fromaddr, &ln);
            if (nb <=0)
            {
                perror("recvfrom");
                close(s);
                return -1;
            }

            if(nb == sizeof(struct icmphdr)+20)
            {
                printf("reply from %s  (num bytes=%d) time=\?\?\?\n",ipstr, nb);
                select(0, NULL, NULL, NULL, &tVal);
                continue;
            }
            select(0, NULL, NULL, NULL, &tVal);
        }
        printf("packet lost...\n");
    }

    close(s);
    return 0;
}
