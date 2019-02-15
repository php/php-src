dnl config.m4 for extension session

PHP_ARG_ENABLE(session, whether to enable PHP sessions,
[  --disable-session       Disable session support], yes)

PHP_ARG_WITH(mm,for mm support,
[  --with-mm[=DIR]           SESSION: Include mm support for session storage], no, no)

if test "$PHP_SESSION" != "no"; then
  PHP_PWRITE_TEST
  PHP_PREAD_TEST
  PHP_NEW_EXTENSION(session, mod_user_class.c session.c mod_files.c mod_mm.c mod_user.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_ADD_EXTENSION_DEP(session, hash, true)
  PHP_ADD_EXTENSION_DEP(session, spl)
  PHP_SUBST(SESSION_SHARED_LIBADD)
  PHP_INSTALL_HEADERS(ext/session, [php_session.h mod_files.h mod_user.h])
  AC_DEFINE(HAVE_PHP_SESSION,1,[ ])
fi

if test "$PHP_MM" != "no"; then
  for i in $PHP_MM /usr/local /usr; do
    test -f "$i/include/mm.h" && MM_DIR=$i && break
  done

  if test -z "$MM_DIR" ; then
    AC_MSG_ERROR(cannot find mm library)
  fi

  if test "$enable_maintainer_zts" = "yes"; then
    dnl The mm library is not thread-safe, and mod_mm.c refuses to compile.
    AC_MSG_ERROR(--with-mm cannot be combined with --enable-maintainer-zts)
  fi

  PHP_ADD_LIBRARY_WITH_PATH(mm, $MM_DIR/$PHP_LIBDIR, SESSION_SHARED_LIBADD)
  PHP_ADD_INCLUDE($MM_DIR/include)
  PHP_INSTALL_HEADERS([ext/session/mod_mm.h])
  AC_DEFINE(HAVE_LIBMM, 1, [Whether you have libmm])
fi
