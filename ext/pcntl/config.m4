dnl config.m4 for extension pcntl

PHP_ARG_ENABLE(pcntl, whether to enable pcntl support,
[  --enable-pcntl          Enable pcntl support (CLI/CGI only)])

if test "$PHP_PCNTL" != "no"; then
  AC_CHECK_FUNCS(fork, [ AC_DEFINE(HAVE_FORK,1,[ ]) ], [ AC_MSG_ERROR(pcntl: fork() not supported by this platform) ])
  AC_CHECK_FUNCS(waitpid, [ AC_DEFINE(HAVE_WAITPID,1,[ ]) ], [ AC_MSG_ERROR(pcntl: waitpid() not supported by this platform) ])
  AC_CHECK_FUNCS(sigaction, [ AC_DEFINE(HAVE_SIGACTION,1,[ ]) ], [ AC_MSG_ERROR(pcntl: sigaction() not supported by this platform) ])
  AC_CHECK_FUNCS([getpriority setpriority wait3 wait4 sigprocmask sigwaitinfo sigtimedwait])

  AC_MSG_CHECKING([for siginfo_t])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
    #include <signal.h>
    #ifdef HAVE_SIGINFO_H
      #include <siginfo.h>
    #endif
  ]],[[
    siginfo_t info;
  ]])],[
    AC_MSG_RESULT([yes])
    PCNTL_CFLAGS="-DHAVE_STRUCT_SIGINFO_T"
  ], [
    AC_MSG_RESULT([no])
  ])

  PHP_NEW_EXTENSION(pcntl, pcntl.c php_signal.c, $ext_shared, cli, $PCNTL_CFLAGS)
fi
