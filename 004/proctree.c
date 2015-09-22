#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv, char** env)
{
    int procid=0;

    if (argc <2)
	printf("use pid=0\n");
    else
	procid = atoi (argv[1]);
\
    char file_name[256];

    sprintf(file_name, "/proc/%d/status", procid);
    printf("%s\n", file_name );


    FILE* fd = fopen(file_name, "r");
    if (fd == NULL)
	perror("open file");

    char buff[2048];
    size_t nn = fread( buff, 2048, 1, fd );

//printf("%s\n", buff );

    return 0;
}