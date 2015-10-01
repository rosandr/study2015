/*
 *	Simple Hello, World program
 */

#include <stdio.h>
#include "hello.h"

int main (int argc, char* argv[])
{
	char msg[]="HELLO WORLD";

	printf( "%s from main", msg );
	printf("\n");

	print_hello (msg);
	printf("\n");

	print_hello2(msg);
	printf("\n");

	return 0;
}


