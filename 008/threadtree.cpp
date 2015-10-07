#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <dirent.h>
#include <errno.h>

#define BUF_SIZE 80

using namespace std;

int getPPid( int pid )
{
    int ret=-1;
    char file_name[BUF_SIZE];

    sprintf(file_name, "/proc/%d/status", pid);

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
	{
		DIR* p_dir;
		p_dir = opendir("/proc");

		struct dirent* dp = readdir(p_dir);
		while(dp)
		{
			pid = strtol(dp->d_name, NULL, 10);
			if (pid>0)	//	(errno != ERANGE)
			{
				printf("%d", pid);

				while( (ppid=getPPid(pid))>0)
				{
					printf(" -> %d", ppid);
					pid=ppid;
				}
			}
			dp = readdir(p_dir);
    		cout << endl;
		}
		closedir(p_dir);
	}
    else
	{
		pid = atoi (argv[1]);

		printf("%d ", pid);

		while( (ppid=getPPid(pid))>0)
		{
			printf(" -> %d", ppid);
			pid=ppid;
		}
	}
    cout << endl;
    //printf("\n");
    return 0;
}
