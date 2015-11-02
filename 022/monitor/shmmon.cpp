#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <iostream>
#include <vector>
using namespace std;

#define BUF_SIZE 80
#define SHM_ID 74565
#define SHM_SIZE (1024*2)

//-------------------------------------------------------------------------

vector<int> *Vector;

int _abort=0;
void sigint_handler(int sig) { _abort=1; }

int main(int argc, char** argv, char** env)
{
    static struct sigaction act;
	act.sa_handler = sigint_handler;
	sigaction(SIGINT, &act, NULL);		// ^C

    printf("\n(Use CTRL-C to terminate %s)\n\n", argv[0]);

    //------------------------------------------------------
    //int shmget(key_t key, size_t size, int shmflg);
    int shm_memId = shmget(SHM_ID, SHM_SIZE, 0);

    if(shm_memId == -1)
    {
        printf("run <shmvect ...> first!\n");
        return 1;
    }

    if((Vector=(vector<int> *)shmat(shm_memId, 0, SHM_RND)) == (void *)-1)
    {
        perror("shmat");
        return 1;
    }

	while(!_abort)
	{
        cout << "vector contains  " << Vector->size() << " elements"  <<endl;
        usleep(1000000);
	}

    if(shmdt(Vector))  perror("shmdt");
    return 0;
}
