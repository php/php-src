dnl
dnl $Id$
dnl

AC_DEFUN(PHP_LDAP_CHECKS, [
  if test -f $1/include/ldap.h; then
    LDAP_DIR=$1
    LDAP_INCDIR=$1/include
    LDAP_LIBDIR=$1/lib
  elif test -f $1/include/umich-ldap/ldap.h; then
    LDAP_DIR=$1
    LDAP_INCDIR=$1/include/umich-ldap
    LDAP_LIBDIR=$1/lib
  elif test -f $1/ldap/public/ldap.h; then
    LDAP_DIR=$1
    LDAP_INCDIR=$1/ldap/public
    LDAP_LIBDIR=$1/lib
  fi
])

PHP_ARG_WITH(ldap,for LDAP support,
[  --with-ldap[=DIR]       Include LDAP support.])

if test "$PHP_LDAP" != "no"; then

  PHP_NEW_EXTENSION(ldap, ldap.c, $ext_shared)

  if test "$PHP_LDAP" = "yes"; then
    for i in /usr/local /usr; do
      PHP_LDAP_CHECKS($i)
    done
  else
    PHP_LDAP_CHECKS($PHP_LDAP)
  fi

  if test -z "$LDAP_DIR"; then
    AC_MSG_ERROR(Cannot find ldap.h)
  fi

  dnl The Linux version of the SDK need -lpthread
  dnl I have tested Solaris, and it doesn't, but others may.  Add
  dnl these here if necessary.   -RL
  dnl Is this really necessary? -Troels Arvin

  if test `uname` = "Linux"; then
    LDAP_PTHREAD=pthread
  else
    LDAP_PTHREAD=
  fi

  if test -f $LDAP_LIBDIR/liblber.a -o -f $LDAP_LIBDIR/liblber.so -o -f $LDAP_LIBDIR/liblber.sl; then
    PHP_ADD_LIBRARY_WITH_PATH(lber, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)

  elif test -f $LDAP_LIBDIR/libldap.so.3; then
    PHP_ADD_LIBRARY_WITH_PATH(ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)

  elif test -f $LDAP_LIBDIR/libssldap50.so -o -f $LDAP_LIBDIR/libssldap50.sl; then
    if test -n "$LDAP_PTHREAD"; then 
      PHP_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    PHP_ADD_LIBRARY_WITH_PATH(nspr4, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(plc4, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(plds4, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ssldap50, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ldap50, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(prldap50, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ssl3, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libldapssl41.so -o -f $LDAP_LIBDIR/libldapssl41.sl; then
    if test -n "$LDAP_PTHREAD"; then 
      PHP_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    PHP_ADD_LIBRARY_WITH_PATH(nspr3, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(plc3, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(plds3, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ldapssl41, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libldapssl30.so -o -f $LDAP_LIBDIR/libldapssl30.sl; then
    if test -n "$LDAP_PTHREAD"; then 
      PHP_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    PHP_ADD_LIBRARY_WITH_PATH(ldapssl30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libldap30.so -o -f $LDAP_LIBDIR/libldap30.sl; then
    if test -n "$LDAP_PTHREAD"; then 
      PHP_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    PHP_ADD_LIBRARY_WITH_PATH(ldap30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libumich_ldap.so; then
    PHP_ADD_LIBRARY_WITH_PATH(umich_lber, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(umich_ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)

  elif test -f $LDAP_LIBDIR/libclntsh.so; then
    PHP_ADD_LIBRARY_WITH_PATH(clntsh, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_ORALDAP,1,[ ])

  else
    AC_MSG_ERROR(Cannot find ldap libraries in $LDAP_LIBDIR.)
  fi

  PHP_ADD_INCLUDE($LDAP_INCDIR)
  PHP_SUBST(LDAP_SHARED_LIBADD)
  AC_DEFINE(HAVE_LDAP,1,[ ])

  dnl Check for 3 arg ldap_set_rebind_proc
  _SAVE_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS -I$LDAP_INCDIR"
  AC_CACHE_CHECK([for 3 arg ldap_set_rebind_proc], ac_cv_3arg_setrebindproc,
  [AC_TRY_COMPILE([#include <ldap.h>], [ldap_set_rebind_proc(0,0,0)],
  ac_cv_3arg_setrebindproc=yes, ac_cv_3arg_setrebindproc=no)])
  if test "$ac_cv_3arg_setrebindproc" = yes; then
    AC_DEFINE(HAVE_3ARG_SETREBINDPROC,1,[Whether 3 arg set_rebind_proc()])
  fi
  CPPFLAGS=$_SAVE_CPPFLAGS

  dnl Solaris 2.8 claims to be 2004 API, but doesn't have
  dnl ldap_parse_reference() nor ldap_start_tls_s()
  AC_CHECK_FUNCS([ldap_parse_reference ldap_start_tls_s])
fi 
