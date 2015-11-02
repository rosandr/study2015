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
typedef struct param
{
    pthread_t* thread;
}Param;

unsigned int num=0;
vector<int> *Vector;
int _abort=0;
pthread_mutex_t mutex;

void sigint_handler(int sig) { _abort=1; }

void* wrrun(void* arg)
{
	while(!_abort)
	{
		int r = rand();
		pthread_mutex_lock (&mutex);
        if( Vector->size() < num )
		{
            Vector->push_back(r);
			//cout << "	value <"<< r << ">  added to vector" <<endl;
			//usleep(100000);
		}
		pthread_mutex_unlock (&mutex);
	}
    return 0;
}

void* rdrun(void* arg)
{
	while(!_abort)
	{
		pthread_mutex_lock (&mutex);
        if( !Vector->empty() )
		{
			//cout << "	value <"<< Vector.at(0) << ">  removed from vector" <<endl;
            Vector->erase(Vector->begin());
			//usleep(100000);
		}
		pthread_mutex_unlock (&mutex);
	}
    return 0;
}


void usage (char* name)
{
    printf("\nUsage:\t%s writers readers num\n", name);
    printf("(use CTRL-C to terminate %s)\n\n", name);
    printf("\twriters - wrirint thread number,\n\treaders - reading thread number,\n\tnum - container max size .\n");
    printf("Example: %s 30 20 50\n", name);
}


int main(int argc, char** argv, char** env)
{
	if (argc <4)
	{
	    usage(basename(argv[0]));
	    return 0;
	}

	int wrnum = atoi(argv[1]);
	int rdnum = atoi(argv[2]);
    int nm = atoi(argv[3]);

    if ((wrnum <= 0) || (rdnum <= 0) || (nm<0) )
	{
		usage(basename(argv[0]));
		return 0;
	}

    num=nm;
    srand(1000);
	pthread_mutex_init (&mutex, NULL);
	
    static struct sigaction act;
	act.sa_handler = sigint_handler;
	sigaction(SIGINT, &act, NULL);		// ^C


    //------------------------------------------------------
    //int shmget(key_t key, size_t size, int shmflg);
    int shm_memId = shmget(SHM_ID, SHM_SIZE, 0);

    if(shm_memId == -1)
    {
        shm_memId = shmget(SHM_ID, SHM_SIZE, IPC_CREAT|0666);
        if(shm_memId == -1)
        {
            perror("shmget");
            return 1;
        }
    }

    if((Vector=(vector<int> *)shmat(shm_memId, 0, SHM_RND)) == (void *)-1)
    {
        perror("shmat");
        return 1;
    }

	Param* wrlist = (Param*)calloc(wrnum, sizeof(Param));
	for (int i=0; i<wrnum; i++)
	{
        pthread_t* thread = new pthread_t;
		Param par;
		par.thread=thread;
		wrlist[i]=par;
        pthread_create(thread, NULL, wrrun, NULL);
	}

	Param* rdlist = (Param*)calloc(rdnum, sizeof(Param));
	for (int i=0; i<rdnum; i++)
	{
        pthread_t* thread = new pthread_t;
		Param par;
		par.thread=thread;
		rdlist[i]=par;
        pthread_create(thread, NULL, rdrun, NULL);
	}

	// main loop
	while(!_abort)
	{
        //cout << "vector contains  " << Vector->size() << " elements"  <<endl;
		usleep(100000);
	}

	// finishing...
	for (int i=0; i<wrnum; i++)
		pthread_join( *(wrlist[i].thread), NULL);

	for (int i=0; i<rdnum; i++)
		pthread_join( *(rdlist[i].thread), NULL);

	free(wrlist);
	free(rdlist);

    if(shmdt(Vector))  perror("shmdt");
    if(shmctl(shm_memId, IPC_RMID, 0))  perror("remove shm");
    return 0;
}
