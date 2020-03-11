#include <stdio.h>

#include "debug_api.h"

int main(int argc, char** argv)
{
    int l1=1, l2=2, l3=3;

	fprintf(stderr, "start\r\n");

    allocate_mutex();
    
    //add_debug_level(1);
    //add_debug_level(2);
    //add_debug_level(3);
    add_debug_level(4);
    
    set_debug_level(3);
    set_debug_level(2);
    set_debug_level(1);

	__debug_print(3, 0, 0, "using level 3\n");
	__debug_print(1, __FILE__, __LINE__, "using level 1\n");
	__debug_print(2, __FILE__, __LINE__, "using level 2\n");
	__debug_print(1, __FILE__, __LINE__, "using level %d, but not level %d neither %d\r\n", l1, l2, l3); 

	fprintf(stderr, "end\r\n");

    return 0;
}