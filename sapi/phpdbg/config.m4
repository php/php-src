PHP_ARG_ENABLE([phpdbg],
  [for phpdbg support],
  [AS_HELP_STRING([--disable-phpdbg],
    [Disable building of phpdbg])],
  [yes],
  [yes])

PHP_ARG_ENABLE([phpdbg-debug],
  [for phpdbg debug build],
  [AS_HELP_STRING([--enable-phpdbg-debug],
    [Build phpdbg in debug mode])],
  [no],
  [no])

if test "$PHP_PHPDBG" != "no"; then
  AC_HEADER_TIOCGWINSZ
  AC_DEFINE(HAVE_PHPDBG, 1, [ ])

  if test "$PHP_PHPDBG_DEBUG" != "no"; then
    AC_DEFINE(PHPDBG_DEBUG, 1, [ ])
  else
    AC_DEFINE(PHPDBG_DEBUG, 0, [ ])
  fi

  PHP_PHPDBG_CFLAGS="-D_GNU_SOURCE -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
  PHP_PHPDBG_FILES="phpdbg.c phpdbg_parser.c phpdbg_lexer.c phpdbg_prompt.c phpdbg_help.c phpdbg_break.c phpdbg_print.c phpdbg_bp.c phpdbg_list.c phpdbg_utils.c phpdbg_info.c phpdbg_cmd.c phpdbg_set.c phpdbg_frame.c phpdbg_watch.c phpdbg_btree.c phpdbg_sigsafe.c phpdbg_io.c phpdbg_out.c"

  if test -n "$with_readline" && test "$with_readline" != "no"; then
    PHP_SETUP_EDIT([],[
      PHPDBG_EXTRA_LIBS="$PHPDBG_EXTRA_LIBS $EDIT_LIBS"

      PHP_PHPDBG_CFLAGS="$PHP_PHPDBG_CFLAGS $EDIT_CFLAGS"

      # Add -Wno-strict-prototypes for Clang because of upstream library issues.
      # TODO: this can be removed probably for current libedit versions.
      PHP_PHPDBG_CFLAGS="$PHP_PHPDBG_CFLAGS -Wno-strict-prototypes"

      AC_DEFINE([HAVE_PHPDBG_READLINE],[1],[Whether phpdbg has readline integration])
      AC_DEFINE([HAVE_LIBEDIT],[1],[Whether libedit library is available])
      phpdbg_has_readline=yes
    ],[phpdbg_has_readline=no])
  fi
  AC_MSG_CHECKING([whether to enable readline in phpdbg])
  if test "$phpdbg_has_readline" = "yes"; then
    AC_MSG_RESULT([enabled (using libedit)])
  elif test "$phpbg_has_readline" = "no"; then
    AC_MSG_RESULT([disabled (libedit not installed)])
  else
    AC_MSG_RESULT([disabled (depends on ext/readline)])
  fi

  AC_CACHE_CHECK([for userfaultfd faulting on write-protected memory support], ac_cv_phpdbg_userfaultfd_writefault, AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
    #include <linux/userfaultfd.h>
    #ifndef UFFDIO_WRITEPROTECT_MODE_WP
    # error userfaults on write-protected memory not supported
    #endif
  ]])], [ac_cv_phpdbg_userfaultfd_writefault=yes], [ac_cv_phpdbg_userfaultfd_writefault=no]))
  if test "$ac_cv_phpdbg_userfaultfd_writefault" = "yes"; then
    if test "$enable_zts" != "yes"; then
      CFLAGS_SAVE="$CFLAGS"
      LIBS_SAVE="$LIBS"

      PTHREADS_CHECK
      AC_MSG_CHECKING([working pthreads]);

      if test "$pthreads_working" = "yes"; then
      	AC_MSG_RESULT([$ac_cv_pthreads_cflags -l$ac_cv_pthreads_lib]);
      	PHP_PHPDBG_CFLAGS="$PHP_PHPDBG_CFLAGS $ac_cv_pthreads_cflags"
      	PHPDBG_EXTRA_LIBS="$PHPDBG_EXTRA_LIBS -l$ac_cv_pthreads_lib"
      	AC_DEFINE(HAVE_USERFAULTFD_WRITEFAULT, 1, [Whether faulting on write-protected memory support can be compiled for userfaultfd])
      else
        AC_MSG_WARN([pthreads not available])
      fi

      CFLAGS="$CFLAGS_SAVE"
      LIBS="$LIBS_SAVE"
    else
      AC_DEFINE(HAVE_USERFAULTFD_WRITEFAULT, 1, [Whether faulting on write-protected memory support can be compiled for userfaultfd])
    fi
  fi

  PHP_SUBST(PHPDBG_EXTRA_LIBS)

  PHP_ADD_MAKEFILE_FRAGMENT([$abs_srcdir/sapi/phpdbg/Makefile.frag], [$abs_srcdir/sapi/phpdbg], [$abs_builddir/sapi/phpdbg])
  PHP_SELECT_SAPI(phpdbg, program, $PHP_PHPDBG_FILES, $PHP_PHPDBG_CFLAGS, [$(SAPI_PHPDBG_PATH)])

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
                \-DPHPDBG_SHARED \
         -o \$(BUILD_SHARED)"

  PHP_SUBST(BUILD_BINARY)
  PHP_SUBST(BUILD_SHARED)
  PHP_SUBST(BUILD_PHPDBG)
  PHP_SUBST(BUILD_PHPDBG_SHARED)

  PHP_OUTPUT(sapi/phpdbg/phpdbg.1)
fi
