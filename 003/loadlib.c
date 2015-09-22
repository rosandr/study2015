#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>



int main(int argc, char** argv, char** env)
{
    void* dl_handle;
    int (*func)(char*);		// ptr to func
    char* error;
    char  msg[] = "Hello from dynamically loaded library\n";

    // open library
    dl_handle = dl_open( "libhello.so", RTLD_LAZY );
    if( !dl_handle )
    {
	fprintf(stderr, "lib open error: %s\n", dlerror() );
	return 1;
    }
    
    // find funcion
    func = dlsym( dl_handle, "print_hello" );
    error = dlerror();

    if( error != NULL )
    {
	fprintf(stderr, "func locate  error: %s\n", error );
	return 2;
    }

    // apply func
    func(msg);

    // close
    dlclose (dl_handle);
    return 0;
}