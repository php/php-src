/* Ignore ECONNRESET of FreeBSD */
#include "ruby/missing.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#undef getpeername
int
ruby_getpeername(int s, struct sockaddr * name,
         socklen_t * namelen)
{
    int err = errno;
    errno = 0;
    s = getpeername(s, name, namelen);
    if (errno == ECONNRESET) {
	errno = 0;
	s = 0;
    }
    else if (errno == 0)
	errno = err;
    return s;
}

#undef getsockname
int
ruby_getsockname(int s, struct sockaddr * name,
         socklen_t * namelen)
{
    int err = errno;
    errno = 0;
    s = getsockname(s, name, namelen);
    if (errno == ECONNRESET) {
	errno = 0;
	s = 0;
    }
    else if (errno == 0)
	errno = err;
    return s;
}

#undef shutdown
int
ruby_shutdown(int s, int how)
{
    int err = errno;
    errno = 0;
    s = shutdown(s, how);
    if (errno == ECONNRESET) {
	errno = 0;
	s = 0;
    }
    else if (errno == 0)
	errno = err;
    return s;
}

#undef close
int
ruby_close(int s)
{
    int err = errno;
    errno = 0;
    s = close(s);
    if (errno == ECONNRESET) {
	errno = 0;
	s = 0;
    }
    else if (errno == 0)
	errno = err;
    return s;
}
