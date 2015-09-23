#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 80

int getPPid( int pid )
{
    int ret=-1;
    char file_name[BUF_SIZE];

    sprintf(file_name, "/proc/%d/status", pid);
    //printf("%s\n", file_name );

    FILE* fd = fopen(file_name, "r");
    if (fd == NULL)
    {
	perror("open file");
	return -1;
    }

    char buff[BUF_SIZE];

    while( fgets( buff, BUF_SIZE, fd ) != NULL )
    {
        if ( strstr ( buff, "PPid:") == NULL)
	    continue;
	else
	    ret= atoi (buff+5);
	break;
    }
    fclose(fd);
    return ret;
}


int main(int argc, char** argv, char** env)
{
    int pid=1, ppid=1;

    if (argc <2)
	printf("use pid=1\n");
    else
	pid = atoi (argv[1]);

    printf("%d ", pid);

    while( (ppid=getPPid(pid))>0)
    {
	printf(" -> %d", ppid);
	pid=ppid;
    }

    printf("\n");
    return 0;
}