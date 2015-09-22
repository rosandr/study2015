#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv, char** env)
{
    char* pwd = getenv( "PWD");
    printf( "Curren directory using getenv():\t%s\n", pwd);

    while (*env)
    {
	char* ppwd = "PWD=";
	int nn = strncmp( *env, ppwd,4 );
	
	if ( nn == 0)
	    printf("\nCurrent directory using env:\t\t%s: \n", (*env) +4);
	env++;
    }
    return 0;
}