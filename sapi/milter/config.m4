dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for milter support)
AC_ARG_WITH(milter,
[  --with-milter=DIR       Build PHP as a Milter for use with Sendmail.],[
  PHP_MILTER=$withval
],[
  PHP_MILTER=no
])
AC_MSG_RESULT($PHP_MILTER)

if test "$PHP_MILTER" != "no"; then
  PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/milter/Makefile.frag)
  PHP_BUILD_THREAD_SAFE
  SAPI_MILTER_PATH=sapi/milter/php-milter
  PHP_SUBST(SAPI_MILTER_PATH)

  PHP_SELECT_SAPI(php-milter, program, php_milter.c,,'$(SAPI_MILTER_PATH)')
  
  case $host_alias in
    *darwin*)
      BUILD_MILTER="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_MILTER_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_MILTER_PATH)"
    ;;
    *)
      BUILD_MILTER="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_MILTER_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_MILTER_PATH)"
    ;;
  esac
  INSTALL_MILTER="\$(INSTALL) -m 0755 \$(SAPI_MILTER_PATH) \$(INSTALL_ROOT)\$(bindir)/php-milter"
  PHP_SUBST(BUILD_MILTER)
  PHP_SUBST(INSTALL_MILTER)
else
  PHP_DISABLE_MILTER
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
