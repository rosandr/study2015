/*
 *	Simple Hello, World shared lib
 */

#include <stdio.h>
#include "hello.h"

int print_hello (char* msg)
{
	printf( "%s from libhello.so", msg );
	return 0;
}




