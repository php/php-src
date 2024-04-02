PHP_ARG_ENABLE([pcntl],
  [whether to enable pcntl support],
  [AS_HELP_STRING([--enable-pcntl],
    [Enable pcntl support (CLI/CGI only)])])

if test "$PHP_PCNTL" != "no"; then
  AC_CHECK_FUNCS([fork], [], [AC_MSG_ERROR([pcntl: fork() not supported by this platform])])
  AC_CHECK_FUNCS([waitpid], [], [AC_MSG_ERROR([pcntl: waitpid() not supported by this platform])])
  AC_CHECK_FUNCS([sigaction], [], [AC_MSG_ERROR([pcntl: sigaction() not supported by this platform])])
  AC_CHECK_FUNCS([getpriority setpriority wait3 wait4 sigwaitinfo sigtimedwait unshare rfork forkx])

  AC_CHECK_TYPE([siginfo_t],[PCNTL_CFLAGS="-DHAVE_STRUCT_SIGINFO_T"],,[#include <signal.h>])

  PHP_NEW_EXTENSION(pcntl, pcntl.c php_signal.c, $ext_shared, cli, $PCNTL_CFLAGS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
