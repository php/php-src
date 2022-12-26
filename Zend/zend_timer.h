#if !defined(ZEND_TIMER_H) && defined(ZTS) && defined(__linux__) && defined(HAVE_TIMER_CREATE)
#define ZEND_TIMER_H

#define ZEND_TIMER 1

#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

// Musl Libc defines this macro, glibc does not
// According to "man 2 timer_create" this field should always be available, but it's not: https://sourceware.org/bugzilla/show_bug.cgi?id=27417
# ifndef sigev_notify_thread_id
# define sigev_notify_thread_id _sigev_un._tid
# endif

#endif
