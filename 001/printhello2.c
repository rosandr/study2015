/*
 *	Simple Hello, World shared lib
 */

#include <stdio.h>
#include "hello.h"

int print_hello2 (char* msg)
{
	printf( "%s from libhello.so (another function)", msg );
	return 0;
}




