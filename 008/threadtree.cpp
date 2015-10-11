#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <dirent.h>
#include <errno.h>

#define BUF_SIZE 80

using namespace std;

int printTList( int pid )
{
    int ret=-1, tpid;
    char dir_name[BUF_SIZE];
    DIR* p_dir;
    
    sprintf(dir_name, "/proc/%d/task", pid);
    p_dir = opendir( dir_name);

    struct dirent* dp = readdir(p_dir);
    while(dp)
    {
	tpid = strtol(dp->d_name, NULL, 10);
	if(tpid>0)
	{
        	if (tpid==pid)	//	(errno != ERANGE)
		    printf("%d - main task\n", pid);
		else
		    printf( "  |-%d\n", tpid);
	}
	dp = readdir(p_dir);
	//cout << endl;
    }
    closedir(p_dir);
    return ret;
}


int main(int argc, char** argv, char** env)
{
	int pid=1;

	if (argc <2)
	{
		DIR* p_dir;
		p_dir = opendir("/proc");

		struct dirent* dp = readdir(p_dir);
		while(dp)
		{
			pid = strtol(dp->d_name, NULL, 10);
			if (pid>0)	//	(errno != ERANGE)
				printTList(pid);
			dp = readdir(p_dir);
    		cout << endl;
		}
		closedir(p_dir);
	}
	else
	{
		pid = atoi (argv[1]);
		printTList(pid);
	}
	cout << endl;
	return 0;
}
