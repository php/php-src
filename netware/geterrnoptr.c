/* Moved into a separate source file -- can be eliminated later */

#include <errno.h>

int *__get_errno_ptr(void)
{   
    return ___errno();
}

