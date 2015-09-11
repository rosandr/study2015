/*
 *	Simple Hello, World program
 */

#include <stdio.h>
#include "hello.h"

int main (int argc, char* argv[])
{
	char msg[]="HELLO WORLD\n";

	print_hello(msg);

	printf( "\nHELLO, WORLD from main\n" );
	return 0;
}




