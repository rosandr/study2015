#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 80

void usage (char* name)
{
    printf("\nUsage:\t%s file size\n", name);
    printf("\tfile - source file name,\n");
    printf("\tsize - size of element to sort.\n");
    printf("Example: %s sort.txt 1\n", name);
}

static int cmpfnct(const void *p1, const void *p2, void *num)
{
    return memcmp(p1, p2, *(size_t*)num);
}


int main(int argc, char** argv, char** env)
{
    if (argc <3)
    {
        usage(basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    if (access(argv[1], F_OK) == -1)
    {
        printf("\n\tfile not exists or you have no access right\n\n");
        exit(EXIT_FAILURE);
    }

    int bsize = atoi(argv[2]);
    if((bsize<0)||(bsize>256))
    {
        printf("Invalid second param\n");
        exit(EXIT_FAILURE);
    }

    int fd =open (argv[1], O_RDWR, 0666);
    if(fd==-1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int fsize = lseek(fd, 0, SEEK_END);
    if(bsize > fsize)
    {
        printf("File too small\n");
        exit(EXIT_FAILURE);
    }

    // mapping...
    char* memp = (char*)mmap(0, fsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == memp)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    close (fd);

    // sorting...
    qsort_r(memp, fsize/bsize, bsize, cmpfnct, &bsize);

    munmap(memp, fsize);
    return 0;
}
