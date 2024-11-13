PHP_ARG_ENABLE([fpm],
  [for FPM build],
  [AS_HELP_STRING([--enable-fpm],
    [Enable building of the fpm SAPI executable])],
  [no],
  [no])

dnl Configure checks.
AC_DEFUN([PHP_FPM_CLOCK], [
AC_CHECK_FUNCS([clock_gettime],, [
  LIBS_save=$LIBS
  AC_SEARCH_LIBS([clock_gettime], [rt], [
    ac_cv_func_clock_gettime=yes
    AC_DEFINE([HAVE_CLOCK_GETTIME], [1])
    AS_VAR_IF([ac_cv_search_clock_gettime], ["none required"],,
      [AS_VAR_APPEND([FPM_EXTRA_LIBS], [" $ac_cv_search_clock_gettime"])])
  ])
  LIBS=$LIBS_save
])

AS_VAR_IF([ac_cv_func_clock_gettime], [no],
  [AC_CACHE_CHECK([for clock_get_time], [php_cv_func_clock_get_time],
    [AC_RUN_IFELSE([AC_LANG_SOURCE([
      #include <mach/mach.h>
      #include <mach/clock.h>
      #include <mach/mach_error.h>

      int main(void)
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
    ])],
    [php_cv_func_clock_get_time=yes],
    [php_cv_func_clock_get_time=no],
    [php_cv_func_clock_get_time=no])])
  AS_VAR_IF([php_cv_func_clock_get_time], [yes],
    [AC_DEFINE([HAVE_CLOCK_GET_TIME], [1],
      [Define to 1 if you have the 'clock_get_time' function.])])
])])

AC_DEFUN([PHP_FPM_TRACE],
[AC_CACHE_CHECK([for ptrace], [php_cv_func_ptrace],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
    #include <sys/types.h>
    #include <sys/ptrace.h>
  ],
  [ptrace(0, 0, (void *) 0, 0);])],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([
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

      int main(void)
      {
        /* copy will fail if sizeof(long) == 8 and we've got "int ptrace()" */
        long v1 = (unsigned int) -1;
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
    ])],
    [php_cv_func_ptrace=yes],
    [php_cv_func_ptrace=no],
    [php_cv_func_ptrace=yes])],
  [php_cv_func_ptrace=no])])

AS_VAR_IF([php_cv_func_ptrace], [yes],
  [AC_DEFINE([HAVE_PTRACE], [1],
    [Define to 1 if you have the 'ptrace' function.])],
  [AC_CACHE_CHECK([for mach_vm_read], [php_cv_func_mach_vm_read],
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <mach/mach.h>
      #include <mach/mach_vm.h>
    ], [
      mach_vm_read(
        (vm_map_t)0,
        (mach_vm_address_t)0,
        (mach_vm_size_t)0,
        (vm_offset_t *)0,
        (mach_msg_type_number_t*)0);
    ])],
    [php_cv_func_mach_vm_read=yes],
    [php_cv_func_mach_vm_read=no])])
])

AS_VAR_IF([php_cv_func_mach_vm_read], [yes],
  [AC_DEFINE([HAVE_MACH_VM_READ], [1],
    [Define to 1 if you have the 'mach_vm_read' function.])])

AC_CACHE_CHECK([for proc mem file], [php_cv_file_proc_mem],
[AS_IF([test -r /proc/$$/mem], [proc_mem_file=mem],
  [test -r /proc/$$/as], [proc_mem_file=as],
  [proc_mem_file=])

AS_VAR_IF([proc_mem_file],,,
  [AC_RUN_IFELSE([AC_LANG_SOURCE([[
      #ifndef _GNU_SOURCE
      #define _GNU_SOURCE
      #endif
      #define _FILE_OFFSET_BITS 64
      #include <stdint.h>
      #include <unistd.h>
      #include <sys/types.h>
      #include <sys/stat.h>
      #include <fcntl.h>
      #include <stdio.h>
      int main(void)
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
    ]])],
    [php_cv_file_proc_mem=$proc_mem_file],
    [php_cv_file_proc_mem=],
    [php_cv_file_proc_mem=$proc_mem_file])
  ])
])

AS_VAR_IF([php_cv_file_proc_mem],,,
  [AC_DEFINE_UNQUOTED([PROC_MEM_FILE], ["$php_cv_file_proc_mem"],
    [Define to the /proc/pid/mem interface filename value.])])

AS_IF([test "x$php_cv_func_ptrace" = xyes], [fpm_trace_type=ptrace],
  [test -n "$php_cv_file_proc_mem"], [fpm_trace_type=pread],
  [test "x$php_cv_func_mach_vm_read" = xyes], [fpm_trace_type=mach],
  [fpm_trace_type=])

AS_VAR_IF([fpm_trace_type],,
  [AC_MSG_WARN([FPM Trace - ptrace, pread, or mach: could not be found])])
])

AC_DEFUN([PHP_FPM_BUILTIN_ATOMIC],
[AC_CACHE_CHECK([if compiler supports __sync_bool_compare_and_swap],
  [php_cv_have___sync_bool_compare_and_swap],
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([], [
    int variable = 1;
    return (__sync_bool_compare_and_swap(&variable, 1, 2)
           && __sync_add_and_fetch(&variable, 1)) ? 1 : 0;
  ])],
  [php_cv_have___sync_bool_compare_and_swap=yes],
  [php_cv_have___sync_bool_compare_and_swap=no])])
AS_VAR_IF([php_cv_have___sync_bool_compare_and_swap], [yes],
  [AC_DEFINE([HAVE_BUILTIN_ATOMIC], [1],
    [Define to 1 if compiler supports __sync_bool_compare_and_swap() a.o.])])
])

AC_DEFUN([PHP_FPM_LQ],
[AC_CACHE_CHECK([for TCP_INFO], [php_cv_have_TCP_INFO],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <netinet/tcp.h>], [
    struct tcp_info ti;
    int x = TCP_INFO;
    (void)ti;
    (void)x;
  ])],
  [php_cv_have_TCP_INFO=yes],
  [php_cv_have_TCP_INFO=no])])
AS_VAR_IF([php_cv_have_TCP_INFO], [yes],
  [AC_DEFINE([HAVE_LQ_TCP_INFO], [1], [Define to 1 if you have 'TCP_INFO'.])])

AC_CACHE_CHECK([for TCP_CONNECTION_INFO], [php_cv_have_TCP_CONNECTION_INFO],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <netinet/tcp.h>], [
    struct tcp_connection_info ti;
    int x = TCP_CONNECTION_INFO;
    (void)ti;
    (void)x;
  ])],
  [php_cv_have_TCP_CONNECTION_INFO=yes],
  [php_cv_have_TCP_CONNECTION_INFO=no])])
AS_VAR_IF([php_cv_have_TCP_CONNECTION_INFO], [yes],
  [AC_DEFINE([HAVE_LQ_TCP_CONNECTION_INFO], [1],
    [Define to 1 if you have 'TCP_CONNECTION_INFO'.])])

AC_CACHE_CHECK([for SO_LISTENQLEN], [php_cv_have_SO_LISTENQLEN],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <sys/socket.h>], [
    int x = SO_LISTENQLIMIT;
    int y = SO_LISTENQLEN;
    (void)x;
    (void)y;
  ])],
  [php_cv_have_SO_LISTENQLEN=yes],
  [php_cv_have_SO_LISTENQLEN=no])])
AS_VAR_IF([php_cv_have_SO_LISTENQLEN], [yes],
  [AC_DEFINE([HAVE_LQ_SO_LISTENQ], [1],
    [Define to 1 if you have 'SO_LISTENQ*'.])])
])

AC_DEFUN([PHP_FPM_KQUEUE],
[AC_CACHE_CHECK([for kqueue],
  [php_cv_have_kqueue],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([dnl
    #include <sys/types.h>
    #include <sys/event.h>
    #include <sys/time.h>
  ], [dnl
    int kfd;
    struct kevent k;
    kfd = kqueue();
    /* 0 -> STDIN_FILENO */
    EV_SET(&k, 0, EVFILT_READ , EV_ADD | EV_CLEAR, 0, 0, NULL);
    (void)kfd;
  ])],
  [php_cv_have_kqueue=yes],
  [php_cv_have_kqueue=no])])
AS_VAR_IF([php_cv_have_kqueue], [yes],
  [AC_DEFINE([HAVE_KQUEUE], [1],
    [Define to 1 if system has a working 'kqueue' function.])])
])

AC_DEFUN([PHP_FPM_EPOLL],
[AC_CACHE_CHECK([for epoll],
  [php_cv_have_epoll],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <sys/epoll.h>], [dnl
    int epollfd;
    struct epoll_event e;

    epollfd = epoll_create(1);
    if (epollfd < 0) {
      return 1;
    }

    e.events = EPOLLIN | EPOLLET;
    e.data.fd = 0;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, 0, &e) == -1) {
      return 1;
    }

    e.events = 0;
    if (epoll_wait(epollfd, &e, 1, 1) < 0) {
      return 1;
    }
  ])],
  [php_cv_have_epoll=yes],
  [php_cv_have_epoll=no])])
AS_VAR_IF([php_cv_have_epoll], [yes],
  [AC_DEFINE([HAVE_EPOLL], [1], [Define to 1 if system has a working epoll.])])
])

AC_DEFUN([PHP_FPM_SELECT],
[AC_CACHE_CHECK([for select],
  [php_cv_have_select],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([dnl
    /* According to POSIX.1-2001 */
    #include <sys/select.h>

    /* According to earlier standards */
    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>
  ], [dnl
    fd_set fds;
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 42;
    FD_ZERO(&fds);
    /* 0 -> STDIN_FILENO */
    FD_SET(0, &fds);
    select(FD_SETSIZE, &fds, NULL, NULL, &t);
  ])],
  [php_cv_have_select=yes],
  [php_cv_have_select=no])])
AS_VAR_IF([php_cv_have_select], [yes],
  [AC_DEFINE([HAVE_SELECT], [1],
    [Define to 1 if system has a working 'select' function.])])
])

if test "$PHP_FPM" != "no"; then
  PHP_FPM_CLOCK
  PHP_FPM_TRACE
  PHP_FPM_BUILTIN_ATOMIC
  PHP_FPM_LQ
  PHP_FPM_KQUEUE
  PHP_FPM_EPOLL
  PHP_FPM_SELECT

  AC_CHECK_FUNCS([clearenv setproctitle setproctitle_fast])

  AC_CHECK_HEADER([priv.h], [AC_CHECK_FUNCS([setpflags])])
  AC_CHECK_HEADER([sys/times.h], [AC_CHECK_FUNCS([times])])
  AC_CHECK_HEADER([port.h], [AC_CHECK_FUNCS([port_create])])

  PHP_ARG_WITH([fpm-user],,
    [AS_HELP_STRING([[--with-fpm-user[=USER]]],
      [Set the user for php-fpm to run as. (default: nobody)])],
    [nobody],
    [no])

  PHP_ARG_WITH([fpm-group],,
    [AS_HELP_STRING([[--with-fpm-group[=GRP]]],
      [Set the group for php-fpm to run as. For a system user, this should
      usually be set to match the fpm username (default: nobody)])],
    [nobody],
    [no])

  PHP_ARG_WITH([fpm-systemd],
    [whether to enable systemd integration in PHP-FPM],
    [AS_HELP_STRING([--with-fpm-systemd],
      [Activate systemd integration])],
    [no],
    [no])

  PHP_ARG_WITH([fpm-acl],
    [whether to use Access Control Lists (ACL) in PHP-FPM],
    [AS_HELP_STRING([--with-fpm-acl],
      [Use POSIX Access Control Lists])],
    [no],
    [no])

  PHP_ARG_WITH([fpm-apparmor],
    [whether to enable AppArmor confinement in PHP-FPM],
    [AS_HELP_STRING([--with-fpm-apparmor],
      [Support AppArmor confinement through libapparmor])],
    [no],
    [no])

  PHP_ARG_WITH([fpm-selinux],
    [whether to enable SELinux support in PHP-FPM],
    [AS_HELP_STRING([--with-fpm-selinux],
      [Support SELinux policy library])],
    [no],
    [no])

  AS_VAR_IF([PHP_FPM_SYSTEMD], [no], [php_fpm_systemd=simple], [
    PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 209])

    AC_DEFINE([HAVE_SYSTEMD], [1],
      [Define to 1 if FPM has systemd integration.])
    PHP_FPM_SD_FILES="fpm/fpm_systemd.c"
    PHP_EVAL_LIBLINE([$SYSTEMD_LIBS], [FPM_EXTRA_LIBS], [yes])
    PHP_EVAL_INCLINE([$SYSTEMD_CFLAGS])

    php_fpm_systemd=notify

    dnl Sanity check.
    CFLAGS_save=$CFLAGS
    CFLAGS="$INCLUDES $CFLAGS"
    AC_CHECK_HEADER([systemd/sd-daemon.h],,
      [AC_MSG_FAILURE([Required <systemd/sd-daemon.h> header file not found.])])
    CFLAGS=$CFLAGS_save
  ])

  AC_SUBST([php_fpm_systemd])

  AS_VAR_IF([PHP_FPM_ACL], [no],, [
    AC_CHECK_HEADERS([sys/acl.h])

    dnl *BSD has acl_* built into libc, macOS doesn't have user/group support.
    LIBS_save=$LIBS
    AC_SEARCH_LIBS([acl_free], [acl],
    [AC_CACHE_CHECK([for ACL user/group permissions support],
      [php_cv_lib_acl_user_group],
      [AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <sys/acl.h>], [
        acl_t acl;
        acl_entry_t user, group;
        acl = acl_init(1);
        acl_create_entry(&acl, &user);
        acl_set_tag_type(user, ACL_USER);
        acl_create_entry(&acl, &group);
        acl_set_tag_type(user, ACL_GROUP);
        acl_free(acl);
      ])],
      [php_cv_lib_acl_user_group=yes],
      [php_cv_lib_acl_user_group=no])])
      AS_VAR_IF([php_cv_lib_acl_user_group], [yes], [
        AC_DEFINE([HAVE_FPM_ACL], [1],
          [Define to 1 if PHP-FPM has ACL support.])
        AS_VAR_IF([ac_cv_search_acl_free], ["none required"],,
          [AS_VAR_APPEND([FPM_EXTRA_LIBS], [" $ac_cv_search_acl_free"])])
      ])
    ])
    LIBS=$LIBS_save
  ])

  AS_VAR_IF([PHP_FPM_APPARMOR], [no],, [
    PKG_CHECK_MODULES([APPARMOR], [libapparmor],
      [PHP_EVAL_INCLINE([$APPARMOR_CFLAGS])],
      [AC_CHECK_LIB([apparmor], [aa_change_profile],
        [APPARMOR_LIBS=-lapparmor],
        [AC_MSG_FAILURE([Required libapparmor library not found.])])])
    PHP_EVAL_LIBLINE([$APPARMOR_LIBS], [FPM_EXTRA_LIBS], [yes])

    dnl Sanity check.
    CFLAGS_save=$CFLAGS
    CFLAGS="$INCLUDES $CFLAGS"
    AC_CHECK_HEADER([sys/apparmor.h],
      [AC_DEFINE([HAVE_APPARMOR], [1],
        [Define to 1 if AppArmor confinement is available for PHP-FPM.])],
      [AC_MSG_FAILURE([Required <sys/apparmor.h> header file not found.])])
    CFLAGS=$CFLAGS_save
  ])

  AS_VAR_IF([PHP_FPM_SELINUX], [no],, [
    PKG_CHECK_MODULES([SELINUX], [libselinux],
      [PHP_EVAL_INCLINE([$SELINUX_CFLAGS])],
      [AC_CHECK_LIB([selinux], [security_setenforce],
        [SELINUX_LIBS=-lselinux],
        [AC_MSG_FAILURE([Required SELinux library not found.])])])
    PHP_EVAL_LIBLINE([$SELINUX_LIBS], [FPM_EXTRA_LIBS], [yes])

    dnl Sanity check.
    CFLAGS_save=$CFLAGS
    CFLAGS="$INCLUDES $CFLAGS"
    AC_CHECK_HEADER([selinux/selinux.h],
      [AC_DEFINE([HAVE_SELINUX], [1],
        [Define to 1 if SELinux is available in PHP-FPM.])],
      [AC_MSG_FAILURE([Required <selinux/selinux.h> header file not found.])])
    CFLAGS=$CFLAGS_save
  ])

  if test -z "$PHP_FPM_USER" || test "$PHP_FPM_USER" = "yes" || test "$PHP_FPM_USER" = "no"; then
    php_fpm_user=nobody
  else
    php_fpm_user=$PHP_FPM_USER
  fi

  if test -z "$PHP_FPM_GROUP" || test "$PHP_FPM_GROUP" = "yes" || test "$PHP_FPM_GROUP" = "no"; then
    php_fpm_group=nobody
  else
    php_fpm_group=$PHP_FPM_GROUP
  fi

  AC_SUBST([php_fpm_user])
  AC_SUBST([php_fpm_group])
  php_fpm_sysconfdir=$(eval echo $sysconfdir)
  AC_SUBST([php_fpm_sysconfdir])
  php_fpm_localstatedir=$(eval echo $localstatedir)
  AC_SUBST([php_fpm_localstatedir])
  php_fpm_prefix=$(eval echo $prefix)
  AC_SUBST([php_fpm_prefix])

  PHP_ADD_BUILD_DIR([
    sapi/fpm/fpm
    sapi/fpm/fpm/events
  ])
  AC_CONFIG_FILES([
    sapi/fpm/init.d.php-fpm
    sapi/fpm/php-fpm.8
    sapi/fpm/php-fpm.conf
    sapi/fpm/php-fpm.service
    sapi/fpm/status.html
    sapi/fpm/www.conf
  ])
  PHP_ADD_MAKEFILE_FRAGMENT([$abs_srcdir/sapi/fpm/Makefile.frag])

  SAPI_FPM_PATH=sapi/fpm/php-fpm

  AS_VAR_IF([fpm_trace_type],,,
    [AS_IF([test -f "$abs_srcdir/sapi/fpm/fpm/fpm_trace_$fpm_trace_type.c"],
      [PHP_FPM_TRACE_FILES="fpm/fpm_trace.c fpm/fpm_trace_$fpm_trace_type.c"])])

  PHP_FPM_FILES="fpm/fpm.c \
    fpm/fpm_children.c \
    fpm/fpm_cleanup.c \
    fpm/fpm_clock.c \
    fpm/fpm_conf.c \
    fpm/fpm_env.c \
    fpm/fpm_events.c \
    fpm/fpm_log.c \
    fpm/fpm_main.c \
    fpm/fpm_php.c \
    fpm/fpm_php_trace.c \
    fpm/fpm_process_ctl.c \
    fpm/fpm_request.c \
    fpm/fpm_shm.c \
    fpm/fpm_scoreboard.c \
    fpm/fpm_signals.c \
    fpm/fpm_sockets.c \
    fpm/fpm_status.c \
    fpm/fpm_stdio.c \
    fpm/fpm_unix.c \
    fpm/fpm_worker_pool.c \
    fpm/zlog.c \
    fpm/events/select.c \
    fpm/events/poll.c \
    fpm/events/epoll.c \
    fpm/events/kqueue.c \
    fpm/events/port.c \
  "

  PHP_SELECT_SAPI([fpm],
    [program],
    [$PHP_FPM_FILES $PHP_FPM_TRACE_FILES $PHP_FPM_SD_FILES],
    [-I$abs_srcdir/sapi/fpm -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  AS_CASE([$host_alias],
    [*aix*], [
      BUILD_FPM="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_FPM_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --tag=CC --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_FASTCGI_OBJS) \$(PHP_FPM_OBJS) \$(EXTRA_LIBS) \$(FPM_EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_FPM_PATH)"
    ],
    [*darwin*], [
      BUILD_FPM="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_BINARY_OBJS:.lo=.o) \$(PHP_FASTCGI_OBJS:.lo=.o) \$(PHP_FPM_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(FPM_EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_FPM_PATH)"
    ], [
      BUILD_FPM="\$(LIBTOOL) --tag=CC --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_BINARY_OBJS:.lo=.o) \$(PHP_FASTCGI_OBJS:.lo=.o) \$(PHP_FPM_OBJS:.lo=.o) \$(EXTRA_LIBS) \$(FPM_EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_FPM_PATH)"
    ])

  PHP_SUBST([SAPI_FPM_PATH])
  PHP_SUBST([BUILD_FPM])
  PHP_SUBST([FPM_EXTRA_LIBS])
fi
