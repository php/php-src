#include "ruby/config.h"

#if defined _WIN32
#elif defined HAVE_FCNTL && defined HAVE_FCNTL_H

/* These are the flock() constants.  Since this sytems doesn't have
   flock(), the values of the constants are probably not available.
*/
# ifndef LOCK_SH
#  define LOCK_SH 1
# endif
# ifndef LOCK_EX
#  define LOCK_EX 2
# endif
# ifndef LOCK_NB
#  define LOCK_NB 4
# endif
# ifndef LOCK_UN
#  define LOCK_UN 8
# endif

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int
flock(int fd, int operation)
{
    struct flock lock;

    switch (operation & ~LOCK_NB) {
    case LOCK_SH:
	lock.l_type = F_RDLCK;
	break;
    case LOCK_EX:
	lock.l_type = F_WRLCK;
	break;
    case LOCK_UN:
	lock.l_type = F_UNLCK;
	break;
    default:
	errno = EINVAL;
	return -1;
    }
    lock.l_whence = SEEK_SET;
    lock.l_start = lock.l_len = 0L;

    return fcntl(fd, (operation & LOCK_NB) ? F_SETLK : F_SETLKW, &lock);
}

#elif defined(HAVE_LOCKF)

#include <unistd.h>
#include <errno.h>

/*  Emulate flock() with lockf() or fcntl().  This is just to increase
    portability of scripts.  The calls might not be completely
    interchangeable.  What's really needed is a good file
    locking module.
*/

# ifndef F_ULOCK
#  define F_ULOCK	0	/* Unlock a previously locked region */
# endif
# ifndef F_LOCK
#  define F_LOCK	1	/* Lock a region for exclusive use */
# endif
# ifndef F_TLOCK
#  define F_TLOCK	2	/* Test and lock a region for exclusive use */
# endif
# ifndef F_TEST
#  define F_TEST	3	/* Test a region for other processes locks */
# endif

/* These are the flock() constants.  Since this sytems doesn't have
   flock(), the values of the constants are probably not available.
*/
# ifndef LOCK_SH
#  define LOCK_SH 1
# endif
# ifndef LOCK_EX
#  define LOCK_EX 2
# endif
# ifndef LOCK_NB
#  define LOCK_NB 4
# endif
# ifndef LOCK_UN
#  define LOCK_UN 8
# endif

int
flock(int fd, int operation)
{
    switch (operation) {

	/* LOCK_SH - get a shared lock */
      case LOCK_SH:
        rb_notimplement();
        return -1;
	/* LOCK_EX - get an exclusive lock */
      case LOCK_EX:
	return lockf (fd, F_LOCK, 0);

	/* LOCK_SH|LOCK_NB - get a non-blocking shared lock */
      case LOCK_SH|LOCK_NB:
        rb_notimplement();
        return -1;
	/* LOCK_EX|LOCK_NB - get a non-blocking exclusive lock */
      case LOCK_EX|LOCK_NB:
	return lockf (fd, F_TLOCK, 0);

	/* LOCK_UN - unlock */
      case LOCK_UN:
	return lockf (fd, F_ULOCK, 0);

	/* Default - can't decipher operation */
      default:
	errno = EINVAL;
        return -1;
    }
}
#else
int
flock(int fd, int operation)
{
    rb_notimplement();
    return -1;
}
#endif
