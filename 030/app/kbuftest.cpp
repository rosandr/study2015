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

//=========================================================================
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
int main(int argc, char** argv, char** env)
{
    int nb;
    char buf[BUF_SIZE];

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
    }
    return 0;
}
