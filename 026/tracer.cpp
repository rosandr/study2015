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
#include <netdb.h>
#include <arpa/inet.h>

#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include <getopt.h>
#include <iostream>
using namespace std;
#define BUF_SIZE 1500
#define REMOTE_PORT 3533

extern int optind;

//=========================================================================
void usage (char* name)
{
    printf("\nUsage:\t%s <destination>\n", name);
    printf("(use CTRL-C to terminate %s)\n\n", name);
    printf("Example: ./%s yandex.ru\n", name);
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
    int repeat = 4;
    struct sockaddr_in addr;
    struct in_addr ip;
/*
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
*/
    if (argc < 2)
    {
        usage(basename(argv[0]));
        return 0;
    }

    char* ipstr = argv[1];
    struct hostent* hp;

    if (!inet_aton(ipstr, &ip))
    {
        hp = gethostbyname(ipstr);
        if(hp == NULL)
        {
            perror("gethostbyname");
            return 1;
        }

        if(hp->h_length != 4)
        {
            printf("can't parse IP address %s\n", ipstr);
            usage(basename(argv[0]));
            return 0;
        }

        ip.s_addr = (*(hp->h_addr+3))<<24;
        ip.s_addr += (*(hp->h_addr+2))<<16;
        ip.s_addr += (*(hp->h_addr+1))<<8;
        ip.s_addr += *(hp->h_addr);
    }

    printf("%s\n", ipstr);
    printf("%x\n", ip.s_addr);
    printf("%x\n", inet_addr(ipstr));


    static struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);		// ^C


    //----------------------------------------------------------
    s=socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(s<=0)
    {
        perror("socket");
        return -1;
    }
/**/
    bool opt=true;
    if(setsockopt (s, IPPROTO_IP, IP_HDRINCL, (char*)&opt, sizeof(opt))<0)
    {
        perror("setsockopt");
        return -1;
    }


    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ((in_addr_t)ip.s_addr);
    addr.sin_port = htons( REMOTE_PORT );

    char rd_buff[BUF_SIZE];
    char wr_buff[BUF_SIZE];
    char* ptr;

    char msgHello[]="Hello to U, my friend";
    struct iphdr ipHdr;
    struct udphdr udpHdr;

    //----------------------------------------------------------
    // prepare ip header
    u_int16_t totalSize = sizeof(udpHdr)+sizeof(ipHdr)+strlen(msgHello);
    u_int16_t ipSize    = sizeof(ipHdr)/sizeof(unsigned long);

    ipHdr.version = 4;
    ipHdr.ihl = ipSize;
    ipHdr.tos = 0;
    ipHdr.tot_len = htons(totalSize);
    ipHdr.id = 0;
    ipHdr.frag_off = 0;
    ipHdr.ttl = 1;
    ipHdr.protocol = IPPROTO_UDP;
    ipHdr.check = 0;
    ipHdr.daddr = ip.s_addr;    //inet_addr(ipstr);
    //ipHdr.saddr = inet_addr("");

    //----------------------------------------------------------
    // prepare udp header
    u_int16_t udpSize = sizeof(udpHdr)+strlen(msgHello);
    udpHdr.source = htons(3333);
    udpHdr.dest = htons(REMOTE_PORT);
    udpHdr.len = htons(udpSize);
    udpHdr.check = 0;

    //----------------------------------------------------------
    // check summ
    ptr = wr_buff;
    memset(ptr, 0, sizeof(wr_buff));
    u_int16_t udpChecksumSize = 0;

    memcpy(ptr, &ipHdr.saddr, sizeof(ipHdr.saddr));
    ptr += sizeof(ipHdr.saddr);
    udpChecksumSize += sizeof(ipHdr.saddr);

    memcpy(ptr, &ipHdr.daddr, sizeof(ipHdr.daddr));
    ptr+= sizeof(ipHdr.daddr);
    udpChecksumSize += sizeof(ipHdr.daddr);

    ptr++;
    udpChecksumSize += 1;

    memcpy(ptr, &ipHdr.protocol, sizeof(ipHdr.protocol));
    ptr += sizeof(ipHdr.protocol);
    udpChecksumSize += sizeof(ipHdr.protocol);

    memcpy(ptr, &udpHdr.len, sizeof(udpHdr.len));
    ptr += sizeof(udpHdr.len);
    udpChecksumSize += sizeof(udpHdr.len);

    memcpy(ptr, &udpHdr, sizeof(udpHdr));
    ptr += sizeof(udpHdr);
    udpChecksumSize += sizeof(udpHdr);

    for(int i = 0; i < strlen(msgHello); i++, ptr++)
            *ptr = msgHello[i];
    udpChecksumSize += strlen(msgHello);

    udpHdr.check = checkSum((u_int16_t *)wr_buff, udpChecksumSize);

    //----------------------------------------------------------
    // prepare buffers
    ptr = wr_buff;
    memset(ptr, 0, sizeof(wr_buff));

    memcpy(ptr, &ipHdr, sizeof(ipHdr));
    ptr += sizeof(ipHdr);
    memcpy(ptr,&udpHdr, sizeof(udpHdr));
    ptr += sizeof(udpHdr);
    memcpy(ptr, msgHello, strlen(msgHello));


    int nb;
    nb =sendto( s, wr_buff, totalSize, 0,
                   (sockaddr*)&addr, sizeof(sockaddr_in));


    printf("total size=%d, nb=%d\n", totalSize, nb);

    fd_set rdfs;
    struct timeval tVal;

    tVal.tv_sec = 1;
    tVal.tv_usec = 0;

    FD_ZERO(&rdfs);
    FD_SET(s, &rdfs);

    if ((nb = select(5, &rdfs, NULL, NULL, &tVal)) < 0)
    {
        perror("select");
        return -1;
    }

    if (FD_ISSET(s, &rdfs))
    {
        //char buf[BUF_SIZE];
        sockaddr_in fromaddr;
        socklen_t ln=sizeof(fromaddr);


        nb=recvfrom(s, rd_buff, BUF_SIZE, 0, (sockaddr*)&fromaddr, &ln);
        if (nb <=0)
        {
            perror("recvfrom");
            close(s);
            return -1;
        }

        //if(nb > 20)
        {
            struct iphdr* p_ipHdr = (struct iphdr*)rd_buff;
            printf("ttl=%d  (num bytes=%d)\n",p_ipHdr->ttl, nb);
        }
    }

    close(s);
    return 0;
}
