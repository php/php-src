dnl $Id$

AC_DEFUN(PHP_LDAP_CHECKS, [
  if test -f $1/include/ldap.h; then
    LDAP_DIR=$1
    LDAP_INCDIR=$1/include
    LDAP_LIBDIR=$1/lib
  elif test -f $1/include/umich-ldap/ldap.h; then
    LDAP_DIR=$i
    LDAP_INCDIR=$1/include/umich-ldap
    LDAP_LIBDIR=$1/lib
  elif test -f $1/ldap/public/ldap.h; then
    LDAP_DIR=$i
    LDAP_INCDIR=$1/ldap/public
    LDAP_LIBDIR=$1/lib
  fi
])
                          
PHP_ARG_WITH(ldap,whether to include LDAP support,
[  --with-ldap[=DIR]       Include LDAP support.  DIR is the LDAP base install directory.])

if test "$PHP_LDAP" != "no"; then

  PHP_EXTENSION(ldap,$ext_shared)

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
    LDAP_PTHREAD="pthread"
  else
    LDAP_PTHREAD=
  fi

  if test -f $LDAP_LIBDIR/liblber.a -o -f $LDAP_LIBDIR/liblber.so -o -f $LDAP_LIBDIR/liblber.sl; then
    AC_ADD_LIBRARY_WITH_PATH(lber, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_ADD_LIBRARY_WITH_PATH(ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)

  elif test -f $LDAP_LIBDIR/libldapssl41.so -o -f $LDAP_LIBDIR/libldapssl41.sl; then
    if test -n "$LDAP_PTHREAD"; then 
      AC_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    AC_ADD_LIBRARY_WITH_PATH(ldapssl41, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libldapssl30.so -o -f $LDAP_LIBDIR/libldapssl30.sl; then
    if test -n "$LDAP_PTHREAD"; then 
      AC_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    AC_ADD_LIBRARY_WITH_PATH(ldapssl30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libldap30.so -o -f $LDAP_LIBDIR/libldap30.sl; then
    if test -n "$LDAP_PTHREAD"; then 
      AC_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    AC_ADD_LIBRARY_WITH_PATH(ldap30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libumich_ldap.so; then
    AC_ADD_LIBRARY_WITH_PATH(umich_lber, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_ADD_LIBRARY_WITH_PATH(umich_ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)

  elif test -f $LDAP_LIBDIR/libclntsh.so; then
    AC_ADD_LIBRARY_WITH_PATH(clntsh, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_ORALDAP,1,[ ])
  fi  

  AC_ADD_INCLUDE($LDAP_INCDIR)
  PHP_SUBST(LDAP_SHARED_LIBADD)
  AC_DEFINE(HAVE_LDAP,1,[ ])
fi 
