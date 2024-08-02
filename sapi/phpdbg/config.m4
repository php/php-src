PHP_ARG_ENABLE([phpdbg],
  [for phpdbg support],
  [AS_HELP_STRING([--disable-phpdbg],
    [Disable building of phpdbg])],
  [yes],
  [yes])

PHP_ARG_ENABLE([phpdbg-debug],
  [for phpdbg debug build],
  [AS_HELP_STRING([--enable-phpdbg-debug],
    [Build phpdbg in debug mode to enable additional diagnostic output for
    developing and troubleshooting phpdbg itself])],
  [no],
  [no])

PHP_ARG_ENABLE([phpdbg-readline],
  [for phpdbg readline support],
  [AS_HELP_STRING([--enable-phpdbg-readline],
    [Enable readline support in phpdbg (depends on static ext/readline)])],
  [no],
  [no])

if test "$PHP_PHPDBG" != "no"; then
  AC_HEADER_TIOCGWINSZ

  PHP_PHPDBG_CFLAGS="-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
  AS_VAR_IF([PHP_PHPDBG_DEBUG], [no],,
    [AS_VAR_APPEND([PHP_PHPDBG_CFLAGS], [" -DPHPDBG_DEBUG=1"])])

  PHP_PHPDBG_FILES="phpdbg.c phpdbg_parser.c phpdbg_lexer.c phpdbg_prompt.c phpdbg_help.c phpdbg_break.c phpdbg_print.c phpdbg_bp.c phpdbg_list.c phpdbg_utils.c phpdbg_info.c phpdbg_cmd.c phpdbg_set.c phpdbg_frame.c phpdbg_watch.c phpdbg_btree.c phpdbg_sigsafe.c phpdbg_io.c phpdbg_out.c"

  AC_MSG_CHECKING([for phpdbg and readline integration])
  if test "$PHP_PHPDBG_READLINE" = "yes"; then
    if test "$PHP_READLINE" != "no" || test "$PHP_LIBEDIT" != "no"; then
      AC_DEFINE(HAVE_PHPDBG_READLINE, 1, [ ])
      PHPDBG_EXTRA_LIBS="$PHP_READLINE_LIBS"
      AC_MSG_RESULT([ok])
    else
      AC_MSG_RESULT([readline is not available])
    fi
  else
    AC_MSG_RESULT([disabled])
  fi

  AH_TEMPLATE([HAVE_USERFAULTFD_WRITEFAULT],
    [Define to 1 if faulting on write-protected memory support can be compiled
    for userfaultfd.])

  AC_CHECK_DECL([UFFDIO_WRITEPROTECT_MODE_WP],
    [AS_VAR_IF([enable_zts], [yes],
      [AC_DEFINE([HAVE_USERFAULTFD_WRITEFAULT], [1])],
      [AC_MSG_CHECKING([working pthreads])
        AS_VAR_IF([pthreads_working], [yes], [
          AC_MSG_RESULT([$ac_cv_pthreads_cflags -l$ac_cv_pthreads_lib])
          AS_VAR_APPEND([PHP_PHPDBG_CFLAGS], [" $ac_cv_pthreads_cflags"])
          AS_VAR_APPEND([PHPDBG_EXTRA_LIBS], [" -l$ac_cv_pthreads_lib"])
          AC_DEFINE([HAVE_USERFAULTFD_WRITEFAULT], [1])
        ], [
          AC_MSG_RESULT([no])
          AC_MSG_WARN([pthreads not available])
        ])
      ])
    ],,
    [#include <linux/userfaultfd.h>])

  PHP_ADD_MAKEFILE_FRAGMENT([$abs_srcdir/sapi/phpdbg/Makefile.frag],
    [$abs_srcdir/sapi/phpdbg],
    [$abs_builddir/sapi/phpdbg])
  PHP_SELECT_SAPI([phpdbg],
    [program],
    [$PHP_PHPDBG_FILES],
    [$PHP_PHPDBG_CFLAGS])

  BUILD_BINARY="sapi/phpdbg/phpdbg"
  BUILD_SHARED="sapi/phpdbg/libphpdbg.la"

  BUILD_PHPDBG="\$(LIBTOOL) --tag=CC --mode=link \
        \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \
                \$(PHP_GLOBAL_OBJS:.lo=.o) \
                \$(PHP_BINARY_OBJS:.lo=.o) \
                \$(PHP_PHPDBG_OBJS:.lo=.o) \
                \$(EXTRA_LIBS) \
                \$(PHPDBG_EXTRA_LIBS) \
                \$(ZEND_EXTRA_LIBS) \
                \$(PHP_FRAMEWORKS) \
         -o \$(BUILD_BINARY)"

  BUILD_PHPDBG_SHARED="\$(LIBTOOL) --tag=CC --mode=link \
        \$(CC) -shared -Wl,-soname,libphpdbg.so -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \
                \$(PHP_GLOBAL_OBJS) \
                \$(PHP_BINARY_OBJS) \
                \$(PHP_PHPDBG_OBJS) \
                \$(EXTRA_LIBS) \
                \$(PHPDBG_EXTRA_LIBS) \
                \$(ZEND_EXTRA_LIBS) \
         -o \$(BUILD_SHARED)"

  PHP_SUBST([PHPDBG_EXTRA_LIBS])
  PHP_SUBST([BUILD_BINARY])
  PHP_SUBST([BUILD_SHARED])
  PHP_SUBST([BUILD_PHPDBG])
  PHP_SUBST([BUILD_PHPDBG_SHARED])

  PHP_OUTPUT([sapi/phpdbg/phpdbg.1])
fi
