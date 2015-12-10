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

DEV_STAT dev_stat;
char buf[BUF_SIZE];
int fd=0;

int testproc (int test)
{
    int nb, len;
    switch (test)
    {
    case 1:
        printf("...exited\n");
        exit(0);
    case 2:
/*
        buf[0]=0;
        //while( fgets (buf, BUF_SIZE, file) > 0)
        fgets (buf, BUF_SIZE, file);
        printf("%d  %s\n", strlen(buf), buf);
*/
        nb=read( fd, buf, BUF_SIZE );
        if(nb<0)
        {
            printf("\tKbuf read error. (May be module not loaded?)\n");
            return -1;
        }
        else if (nb==0)
        {
            fprintf( stdout, "Chardev is empty.\n");
            return -1;
        }
        else fprintf( stdout, "%s\n", buf);
        break;
    case 3:
        //printf("Please, type string and press <ENTER>\n");
        //if ( fgets(buf, BUF_SIZE, stdin) != NULL)
        sprintf(buf, "%s", "qwertyuiop\n");
        {
            len = strlen(buf);
            if(len>0)
            {
                nb=write(fd, buf, len);
                //if (nb>0)
                //    printf("wr %s[%d]  ok\n", buf, strlen(buf));
            }
        }
        break;
    case 4:
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
            printf("IRQ  count:%d\n", dev_stat.irq_cnt);
        }
        break;

    case 5:
        ioctl( fd, IOCTL_RESET_STAT, 0);
        break;

    case 6:
        ioctl( fd, IOCTL_GET_PROCLIST, 0);
        break;

    case 7:
        lseek( fd, 0, SEEK_SET);
        break;

    default:
        break;
    }
    return 0;
}

//-------------------------------------------------------------------------
int main(int argc, char** argv, char** env)
{
    char devfile_name[] = "/dev/kbuf";
    FILE* file = fopen(devfile_name, "w+");
    fd = fileno(file);

    if(file==NULL)
    {
        perror("File open");
        printf("\nPlease, create %s manualy and insert module.\n", devfile_name);
        return 0;
    }

    /*
    //---------------------------------------------------------
    while(1)
    {
        print_menu();

        char sel = getchar();
        if (fgets (buf, BUF_SIZE, stdin) != NULL) ;     // ???
        testproc(sel);
    }
    */

    printf("\n\t4 - get statistic\n");
    testproc(4);

    printf("\n\t2 - read chardev\n");
    testproc(2);

    printf("\n\t2 - read chardev\n");
    testproc(2);

    printf("\n\t3 - write \"qwertyuiop\" to chardev\n");
    testproc(3);

    printf("\n\t3 - write \"qwertyuiop\" to chardev\n");
    testproc(3);

    printf("\n\t2 - read chardev\n");
    testproc(2);

    printf("\n\t7 - seek_set 0 \n");
    testproc(7);

    printf("\n\t2 - read chardev\n");
    testproc(2);

    printf("\n\t2 - read chardev\n");
    testproc(2);

    printf("\n\t2 - read chardev\n");
    testproc(2);

    printf("\n\t4 - get statistic\n");
    testproc(4);
/*
    printf("\n\t5 - reset statistic\n");
    testproc(5);

    printf("\n\t4 - get statistic\n");
    testproc(4);
*/
    printf("\n\t6 - print process \n");
    testproc(6);

    //printf("\n\t7 - seek_set 0 \n");
    testproc(7);

    //printf("\n\t2 - read chardev\n");
    testproc(2);

    printf("\n\t1 - exit\n");
    testproc(1);

    return 0;
}
