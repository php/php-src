dnl
dnl $Id$
dnl

dnl Process Control (pcntl) extentsion --EXPERIMENTAL--
dnl TODO - Add platform checks 

PHP_ARG_ENABLE(pcntl, whether to enable pcntl support,
[  --enable-pcntl          Enable experimental pcntl support (CGI ONLY!)])

if test "$PHP_PCNTL" != "no"; then
  if test "$PHP_SAPI" != "cgi"; then
      AC_MSG_ERROR(pcntl currently only accepts the CGI SAPI, this will save you from harming your webserver.)
  fi
 
  AC_CHECK_FUNCS(fork, [ AC_DEFINE(HAVE_FORK,1,[ ]) ], [ AC_MSG_ERROR(pcntl: fork() not supported by this platform) ])
  AC_CHECK_FUNCS(waitpid, [ AC_DEFINE(HAVE_WAITPID,1,[ ]) ], [ AC_MSG_ERROR(pcntl: fork() not supported by this platform) ])
  AC_CHECK_FUNCS(sigaction, [ AC_DEFINE(HAVE_SIGACTION,1,[ ]) ], [ AC_MSG_ERROR(pcntl: sigaction() not supported by this platform) ])
  
  PHP_EXTENSION(pcntl, $ext_shared)
fi
