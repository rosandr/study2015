#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>


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
    printf("\t7 - seek_set 0 \n");
}

//-------------------------------------------------------------------------
int main(int argc, char** argv, char** env)
{
    int nb;
    DEV_STAT dev_stat;
    char buf[BUF_SIZE];

    char devfile_name[] = "/dev/kbuf";
    FILE* file = fopen(devfile_name, "r+");
    int fd = fileno(file);

    if(file==NULL)
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
            //while( fgets (buf, BUF_SIZE, file) > 0)
            fgets (buf, BUF_SIZE, file);
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
            if ( fgets(buf, BUF_SIZE, stdin) != NULL)
            {
                if(fputs(buf, file)<0)
                {
                    printf("\tKbuf write error. (May be module not loaded?)\n");
                    return 0;
                }
                printf("wr %s[%d]  ok\n", buf, strlen(buf));
            }
            break;
        case '4':
            nb=ioctl( fd, IOCTL_GET_STAT, &dev_stat);
            if(nb==0)
            {
                printf("Statistic:\n");
                printf("OPEN call count:%d\n", dev_stat.open_cnt);
                printf("CLOSE call count:%d\n", dev_stat.close_cnt);
                printf("READ call count:%d\n", dev_stat.read_cnt);
                printf("WRITE call count:%d\n", dev_stat.write_cnt);
                printf("SEEK call count:%d\n", dev_stat.seek_cnt);
                printf("IOCTL call count:%d\n", dev_stat.ioctl_cnt);
            }
            break;

        case '5':
            ioctl( fd, IOCTL_RESET_STAT, 0);
            break;

        case '6':
            ioctl( fd, IOCTL_GET_PROCLIST, 0);
            break;

        case '7':
            fseek( file, 0, SEEK_SET);
            //ioctl( fd, IOCTL_GET_PROCLIST, 0);
            break;

        default:
            break;
        }
    }
    return 0;
}
