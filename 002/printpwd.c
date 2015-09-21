#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv, char** env)
{
    char* pwd = getenv( "PWD");
    printf( "Curren directory: %s\n", pwd);




    int nn;


    while (*env)
    {
	//printf("%s\n", *env);
	char ppwd[]="PWD=";
	nn = strncmp( *env, ppwd, sizeof(ppwd) );
	
	if ( nn == sizeof(ppwd))
	    printf("Current dir %s: \n", (*env)+4);

	env++;
    }


    return 0;
}