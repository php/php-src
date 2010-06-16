dnl
dnl $Id$
dnl

fpm_version="0.6.5"
minimum_libevent_version="1.4.11"

PHP_ARG_ENABLE(fpm,,
[  --enable-fpm              EXPERIMENTAL: Enable building of the fpm SAPI executable], no, no)

dnl libevent check function {{{
dnl @synopsis AC_LIB_EVENT([MINIMUM-VERSION])
dnl
dnl Test for the libevent library of a particular version (or newer).
dnl Source: http://svn.apache.org/repos/asf/incubator/thrift/trunk/aclocal/ax_lib_event.m4
dnl Modified: This file was modified for autoconf-2.13 and the PHP_ARG_WITH macro.
dnl
dnl If no path to the installed libevent is given, the macro will first try
dnl using no -I or -L flags, then searches under /usr, /usr/local, /opt,
dnl and /opt/libevent.
dnl If these all fail, it will try the $LIBEVENT_ROOT environment variable.
dnl
dnl This macro requires that #include <sys/types.h> works and defines u_char.
dnl
dnl This macro calls:
dnl   AC_SUBST(LIBEVENT_CFLAGS)
dnl   AC_SUBST(LIBEVENT_LIBS)
dnl
dnl And (if libevent is found):
dnl   AC_DEFINE(HAVE_LIBEVENT)
dnl
dnl It also leaves the shell variables "success" and "ac_have_libevent"
dnl set to "yes" or "no".
dnl
dnl NOTE: This macro does not currently work for cross-compiling,
dnl       but it can be easily modified to allow it.  (grep "cross").
dnl
dnl @category InstalledPackages
dnl @category C
dnl @version 2007-09-12
dnl @license AllPermissive
dnl
dnl Copyright (C) 2009 David Reiss
dnl Copying and distribution of this file, with or without modification,
dnl are permitted in any medium without royalty provided the copyright
dnl notice and this notice are preserved.

AC_DEFUN([AC_LIB_EVENT_DO_CHECK],
[
# Save our flags.
CPPFLAGS_SAVED="$CPPFLAGS"
LDFLAGS_SAVED="$LDFLAGS"
LIBS_SAVED="$LIBS"
LD_LIBRARY_PATH_SAVED="$LD_LIBRARY_PATH"

# Set our flags if we are checking a specific directory.
if test -n "$ac_libevent_path" ; then
  LIBEVENT_CPPFLAGS="-I$ac_libevent_path/include"
 
  if test -z "$PHP_LIBDIR"; then
    LIBEVENT_LDFLAGS="-L$ac_libevent_path/lib"
  else 
    LIBEVENT_LDFLAGS="-L$ac_libevent_path/$PHP_LIBDIR"
  fi

  LD_LIBRARY_PATH="$ac_libevent_path/lib:$LD_LIBRARY_PATH"
else
  LIBEVENT_CPPFLAGS=""
  LIBEVENT_LDFLAGS=""
fi

# Required flag for libevent.
LIBEVENT_LIBS="-levent"

# Prepare the environment for compilation.
CPPFLAGS="$CPPFLAGS $LIBEVENT_CPPFLAGS"
LDFLAGS="$LDFLAGS $LIBEVENT_LDFLAGS"
LIBS="$LIBS $LIBEVENT_LIBS"
export CPPFLAGS
export LDFLAGS
export LIBS
export LD_LIBRARY_PATH

success=no

# Compile, link, and run the program.  This checks:
# - event.h is available for including.
# - event_get_version() is available for linking.
# - The event version string is lexicographically greater
#   than the required version.
AC_TRY_RUN([
#include <sys/types.h>
#include <event.h>

int main(int argc, char *argv[])
{
	const char* lib_version = event_get_version();
	const char* wnt_version = "$WANT_LIBEVENT_VERSION";
	for (;;) {
		/* If we reached the end of the want version.  We have it. */
		if (*wnt_version == '\0' || *wnt_version == '-') {
			return 0;
		}
		/* If the want version continues but the lib version does not, */
		/* we are missing a letter.  We don't have it. */
		if (*lib_version == '\0' || *lib_version == '-') {
			return 1;
		}

		/* In the 1.4 version numbering style, if there are more digits */
		/* in one version than the other, that one is higher. */
		int lib_digits;
		for (lib_digits = 0;
		lib_version[lib_digits] >= '0' &&
		lib_version[lib_digits] <= '9';
		lib_digits++)
		;
		int wnt_digits;
		for (wnt_digits = 0;
		wnt_version[wnt_digits] >= '0' &&
		wnt_version[wnt_digits] <= '9';
		wnt_digits++)
		;
		if (lib_digits > wnt_digits) {
			return 0;
		}
		if (lib_digits < wnt_digits) {
			return 1;
		}
		/* If we have greater than what we want.  We have it. */
		if (*lib_version > *wnt_version) {
			return 0;
		}
		/* If we have less, we don't. */
		if (*lib_version < *wnt_version) {
			return 1;
		}
		lib_version++;
		wnt_version++;
	}
	return 0;
}
],[
success=yes
])

# Restore flags.
CPPFLAGS="$CPPFLAGS_SAVED"
LDFLAGS="$LDFLAGS_SAVED"
LIBS="$LIBS_SAVED"
LD_LIBRARY_PATH="$LD_LIBRARY_PATH_SAVED"
])

AC_DEFUN([AC_LIB_EVENT],
[

PHP_ARG_WITH(libevent-dir,,
[  --with-libevent-dir[=PATH]  libevent install prefix, for fpm SAPI. (default: /usr/local)], /usr/local, yes)

if test "$PHP_LIBEVENT_DIR" != "no"; then
  WANT_LIBEVENT_VERSION=ifelse([$1], ,1.2,$1)

  AC_MSG_CHECKING(for libevent >= $WANT_LIBEVENT_VERSION install prefix)

  libevent_prefix=$ac_default_prefix
  if test $prefix != "NONE" -a $prefix != "" -a $prefix != "no" ; then 
    libevent_prefix=$prefix
  fi

  if test "$PHP_LIBEVENT_DIR" = "yes"; then
    PHP_LIBEVENT_DIR=$libevent_prefix
  fi

  if test "$PHP_LIBEVENT_DIR" != "yes" && test "$PHP_LIBEVENT_DIR" != "/usr/local"; then
    dnl don't try to be too smart, check only $PHP_LIBEVENT_DIR if specified
    ac_libevent_path=$PHP_LIBEVENT_DIR
    AC_LIB_EVENT_DO_CHECK
    if test "$success" = "no"; then
      AC_MSG_ERROR([Could not find libevent >= $WANT_LIBEVENT_VERSION in $PHP_LIBEVENT_DIR])
    fi
   else 
    dnl check default prefixes then
    for ac_libevent_path in "" $PHP_LIBEVENT_DIR /usr /usr/local /opt /opt/local /opt/libevent ; do
      AC_LIB_EVENT_DO_CHECK
      if test "$success" = "yes"; then
        break;
      fi
    done
  fi

  if test "$success" != "yes" ; then
    AC_MSG_RESULT(no)
    ac_have_libevent=no
    AC_MSG_ERROR([libevent >= $WANT_LIBEVENT_VERSION could not be found])
  else
    AC_MSG_RESULT($ac_libevent_path)
    ac_have_libevent=yes
    AC_DEFINE(HAVE_LIBEVENT, 1, [define if libevent is available])
  fi

  LIBEVENT_LIBS="-levent"

  if test -n "$ac_libevent_path"; then
    LIBEVENT_CFLAGS="-I$ac_libevent_path/include"
    LIBEVENT_LIBS="-L$ac_libevent_path/$PHP_LIBDIR $LIBEVENT_LIBS"
    LIBEVENT_PATH="$ac_libevent_path/$PHP_LIBDIR"
  fi

  AC_SUBST(LIBEVENT_CFLAGS)
  AC_SUBST(LIBEVENT_LIBS)
  AC_SUBST(LIBEVENT_PATH)

else
  AC_MSG_ERROR([FPM requires libevent >= $WANT_LIBEVENT_VERSION. Please specify libevent install prefix with --with-libevent-dir=yes])
fi

])
dnl }}}

dnl configure checks {{{
AC_DEFUN([AC_FPM_STDLIBS],
[
  AC_CHECK_FUNCS(setenv clearenv)

  AC_SEARCH_LIBS(socket, socket)
  AC_SEARCH_LIBS(inet_addr, nsl)

  AC_CHECK_HEADERS([errno.h fcntl.h stdio.h stdlib.h unistd.h sys/uio.h])
  AC_CHECK_HEADERS([sys/select.h sys/socket.h sys/time.h])
  AC_CHECK_HEADERS([arpa/inet.h netinet/in.h])
])

AC_DEFUN([AC_FPM_PRCTL],
[
  AC_MSG_CHECKING([for prctl])

  AC_TRY_COMPILE([ #include <sys/prctl.h> ], [prctl(0, 0, 0, 0, 0);], [
    AC_DEFINE([HAVE_PRCTL], 1, [do we have prctl?])
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
  ])
])

AC_DEFUN([AC_FPM_CLOCK],
[
  have_clock_gettime=no

  AC_MSG_CHECKING([for clock_gettime])

  AC_TRY_LINK([ #include <time.h> ], [struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);], [
    have_clock_gettime=yes
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
  ])

  if test "$have_clock_gettime" = "no"; then
    AC_MSG_CHECKING([for clock_gettime in -lrt])

    SAVED_LIBS="$LIBS"
    LIBS="$LIBS -lrt"

    AC_TRY_LINK([ #include <time.h> ], [struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);], [
      have_clock_gettime=yes
      AC_MSG_RESULT([yes])
    ], [
      LIBS="$SAVED_LIBS"
      AC_MSG_RESULT([no])
    ])
  fi

  if test "$have_clock_gettime" = "yes"; then
    AC_DEFINE([HAVE_CLOCK_GETTIME], 1, [do we have clock_gettime?])
  fi

  have_clock_get_time=no

  if test "$have_clock_gettime" = "no"; then
    AC_MSG_CHECKING([for clock_get_time])

    AC_TRY_RUN([ #include <mach/mach.h>
      #include <mach/clock.h>
      #include <mach/mach_error.h>

      int main()
      {
        kern_return_t ret; clock_serv_t aClock; mach_timespec_t aTime;
        ret = host_get_clock_service(mach_host_self(), REALTIME_CLOCK, &aClock);

        if (ret != KERN_SUCCESS) {
          return 1;
        }

        ret = clock_get_time(aClock, &aTime);
        if (ret != KERN_SUCCESS) {
          return 2;
        }

        return 0;
      }
    ], [
      have_clock_get_time=yes
      AC_MSG_RESULT([yes])
    ], [
      AC_MSG_RESULT([no])
    ])
  fi

  if test "$have_clock_get_time" = "yes"; then
    AC_DEFINE([HAVE_CLOCK_GET_TIME], 1, [do we have clock_get_time?])
  fi
])

AC_DEFUN([AC_FPM_TRACE],
[
  have_ptrace=no
  have_broken_ptrace=no

  AC_MSG_CHECKING([for ptrace])

  AC_TRY_COMPILE([
    #include <sys/types.h>
    #include <sys/ptrace.h> ], [ptrace(0, 0, (void *) 0, 0);], [
    have_ptrace=yes
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
  ])

  if test "$have_ptrace" = "yes"; then
    AC_MSG_CHECKING([whether ptrace works])

    AC_TRY_RUN([
      #include <unistd.h>
      #include <signal.h>
      #include <sys/wait.h>
      #include <sys/types.h>
      #include <sys/ptrace.h>
      #include <errno.h>

      #if !defined(PTRACE_ATTACH) && defined(PT_ATTACH)
      #define PTRACE_ATTACH PT_ATTACH
      #endif

      #if !defined(PTRACE_DETACH) && defined(PT_DETACH)
      #define PTRACE_DETACH PT_DETACH
      #endif

      #if !defined(PTRACE_PEEKDATA) && defined(PT_READ_D)
      #define PTRACE_PEEKDATA PT_READ_D
      #endif

      int main()
      {
        long v1 = (unsigned int) -1; /* copy will fail if sizeof(long) == 8 and we've got "int ptrace()" */
        long v2;
        pid_t child;
        int status;

        if ( (child = fork()) ) { /* parent */
          int ret = 0;

          if (0 > ptrace(PTRACE_ATTACH, child, 0, 0)) {
            return 2;
          }

          waitpid(child, &status, 0);

      #ifdef PT_IO
          struct ptrace_io_desc ptio = {
            .piod_op = PIOD_READ_D,
            .piod_offs = &v1,
            .piod_addr = &v2,
            .piod_len = sizeof(v1)
          };

          if (0 > ptrace(PT_IO, child, (void *) &ptio, 0)) {
            ret = 3;
          }
      #else
          errno = 0;

          v2 = ptrace(PTRACE_PEEKDATA, child, (void *) &v1, 0);

          if (errno) {
            ret = 4;
          }
      #endif
          ptrace(PTRACE_DETACH, child, (void *) 1, 0);

          kill(child, SIGKILL);

          return ret ? ret : (v1 != v2);
        }
        else { /* child */
          sleep(10);
          return 0;
        }
      }
    ], [
      AC_MSG_RESULT([yes])
    ], [
      have_ptrace=no
      have_broken_ptrace=yes
      AC_MSG_RESULT([no])
    ])
  fi

  if test "$have_ptrace" = "yes"; then
    AC_DEFINE([HAVE_PTRACE], 1, [do we have ptrace?])
  fi

  have_mach_vm_read=no

  if test "$have_broken_ptrace" = "yes"; then
    AC_MSG_CHECKING([for mach_vm_read])

    AC_TRY_COMPILE([ #include <mach/mach.h>
      #include <mach/mach_vm.h>
    ], [
      mach_vm_read((vm_map_t)0, (mach_vm_address_t)0, (mach_vm_size_t)0, (vm_offset_t *)0, (mach_msg_type_number_t*)0);
    ], [
      have_mach_vm_read=yes
      AC_MSG_RESULT([yes])
    ], [
      AC_MSG_RESULT([no])
    ])
  fi

  if test "$have_mach_vm_read" = "yes"; then
    AC_DEFINE([HAVE_MACH_VM_READ], 1, [do we have mach_vm_read?])
  fi

  proc_mem_file=""

  if test -r /proc/$$/mem ; then
    proc_mem_file="mem"
  else
    if test -r /proc/$$/as ; then
      proc_mem_file="as"
    fi
  fi

  if test -n "$proc_mem_file" ; then
    AC_MSG_CHECKING([for proc mem file])
  
    AC_TRY_RUN([
      #define _GNU_SOURCE
      #define _FILE_OFFSET_BITS 64
      #include <stdint.h>
      #include <unistd.h>
      #include <sys/types.h>
      #include <sys/stat.h>
      #include <fcntl.h>
      #include <stdio.h>
      int main()
      {
        long v1 = (unsigned int) -1, v2 = 0;
        char buf[128];
        int fd;
        sprintf(buf, "/proc/%d/$proc_mem_file", getpid());
        fd = open(buf, O_RDONLY);
        if (0 > fd) {
          return 1;
        }
        if (sizeof(long) != pread(fd, &v2, sizeof(long), (uintptr_t) &v1)) {
          close(fd);
          return 1;
        }
        close(fd);
        return v1 != v2;
      }
    ], [
      AC_MSG_RESULT([$proc_mem_file])
    ], [
      proc_mem_file=""
      AC_MSG_RESULT([no])
    ])
  fi
  
  if test -n "$proc_mem_file"; then
    AC_DEFINE_UNQUOTED([PROC_MEM_FILE], "$proc_mem_file", [/proc/pid/mem interface])
  fi
  
  fpm_trace_type=""

  if test "$have_ptrace" = "yes"; then
    fpm_trace_type=ptrace
    
  elif test -n "$proc_mem_file"; then
    fpm_trace_type=pread
    
  elif test "$have_mach_vm_read" = "yes" ; then
    fpm_trace_type=mach
    
  else
    AC_MSG_WARN([FPM Trace - ptrace, pread, or mach: could not be found])    
  fi
  
])
dnl }}}

AC_MSG_CHECKING(for FPM build)
if test "$PHP_FPM" != "no"; then
  AC_MSG_RESULT($PHP_FPM)

  AC_LIB_EVENT([$minimum_libevent_version])

  PHP_ADD_LIBRARY_WITH_PATH(event, $LIBEVENT_PATH)

  PHP_TEST_BUILD(event_init, [ ], [
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([build test failed. Please check the config.log for details.])
  ], $LIBEVENT_LIBS)

  AC_FPM_STDLIBS
  AC_FPM_PRCTL
  AC_FPM_CLOCK
  AC_FPM_TRACE

  PHP_ARG_WITH(fpm-user,,
  [  --with-fpm-user[=USER]  Set the user for php-fpm to run as. (default: nobody)], nobody, no)

  PHP_ARG_WITH(fpm-group,,
  [  --with-fpm-group[=GRP]  Set the group for php-fpm to run as. For a system user, this 
                  should usually be set to match the fpm username (default: nobody)], nobody, no)

  if test -z "$PHP_FPM_USER" -o "$PHP_FPM_USER" = "yes" -o "$PHP_FPM_USER" = "no"; then
    php_fpm_user="nobody"
  else
    php_fpm_user="$PHP_FPM_USER"
  fi

  if test -z "$PHP_FPM_GROUP" -o "$PHP_FPM_GROUP" = "yes" -o "$PHP_FPM_GROUP" = "no"; then
    php_fpm_group="nobody"
  else
    php_fpm_group="$PHP_FPM_GROUP"
  fi

  PHP_SUBST_OLD(fpm_version)
  PHP_SUBST_OLD(php_fpm_user)
  PHP_SUBST_OLD(php_fpm_group)
  php_fpm_sysconfdir=`eval echo $sysconfdir`
  PHP_SUBST_OLD(php_fpm_sysconfdir)
  php_fpm_localstatedir=`eval echo $localstatedir`
  PHP_SUBST_OLD(php_fpm_localstatedir)

  AC_DEFINE_UNQUOTED(PHP_FPM_VERSION, "$fpm_version", [fpm version])
  AC_DEFINE_UNQUOTED(PHP_FPM_USER, "$php_fpm_user", [fpm user name])
  AC_DEFINE_UNQUOTED(PHP_FPM_GROUP, "$php_fpm_group", [fpm group name])

  PHP_OUTPUT(sapi/fpm/php-fpm.conf sapi/fpm/init.d.php-fpm sapi/fpm/php-fpm.1)
  PHP_ADD_MAKEFILE_FRAGMENT([$abs_srcdir/sapi/fpm/Makefile.frag], [$abs_srcdir/sapi/fpm], [sapi/fpm])

  SAPI_FPM_PATH=sapi/fpm/php-fpm
  PHP_SUBST(SAPI_FPM_PATH)
  
  if test "$fpm_trace_type" && test -f "$abs_srcdir/sapi/fpm/fpm/fpm_trace_$fpm_trace_type.c"; then
    PHP_FPM_TRACE_FILES="fpm/fpm_trace.c fpm/fpm_trace_$fpm_trace_type.c"
  fi
  
  PHP_FPM_CFLAGS="$LIBEVENT_CFLAGS -I$abs_srcdir/sapi/fpm"

  SAPI_EXTRA_LIBS="$LIBEVENT_LIBS"
  PHP_SUBST(SAPI_EXTRA_LIBS)
 
  INSTALL_IT=":"
  PHP_FPM_FILES="fpm/fastcgi.c \
    fpm/fpm.c \
    fpm/fpm_children.c \
    fpm/fpm_cleanup.c \
    fpm/fpm_clock.c \
    fpm/fpm_conf.c \
    fpm/fpm_env.c \
    fpm/fpm_events.c \
    fpm/fpm_main.c \
    fpm/fpm_php.c \
    fpm/fpm_php_trace.c \
    fpm/fpm_process_ctl.c \
    fpm/fpm_request.c \
    fpm/fpm_shm.c \
    fpm/fpm_shm_slots.c \
    fpm/fpm_signals.c \
    fpm/fpm_sockets.c \
    fpm/fpm_status.c \
    fpm/fpm_stdio.c \
    fpm/fpm_unix.c \
    fpm/fpm_worker_pool.c \
    fpm/zlog.c \
  "

  PHP_SELECT_SAPI(fpm, program, $PHP_FPM_FILES $PHP_FPM_TRACE_FILES, $PHP_FPM_CFLAGS, '$(SAPI_FPM_PATH)')

  case $host_alias in
      *aix*)
        BUILD_FPM="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) \$(EXTRA_LIBS) \$(SAPI_EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_FPM_PATH)"
        ;;
      *darwin*)
        BUILD_FPM="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_SAPI_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(SAPI_EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_FPM_PATH)"
      ;;
      *)
        BUILD_FPM="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) \$(EXTRA_LIBS) \$(SAPI_EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_FPM_PATH)"
      ;;
  esac

  PHP_SUBST(BUILD_FPM)
else
  AC_MSG_RESULT(no)
fi
