dnl $Id$

AC_MSG_CHECKING(for LDAP support)
AC_ARG_WITH(ldap,
[  --with-ldap[=DIR]       Include LDAP support.  DIR is the LDAP base
                          install directory, defaults to /usr/local/ldap],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      LDAP_INCDIR=/usr/local/ldap/include
      LDAP_LIBDIR=/usr/local/ldap/lib
    else
      LDAP_INCDIR=$withval/include
      LDAP_LIBDIR=$withval/lib
    fi
    LDAP_INCLUDE=-I$LDAP_INCDIR
    LDAP_LFLAGS="-L$LDAP_LIBDIR ${ld_runpath_switch}$LDAP_LIBDIR"
    LDAP_LIBS="-lldap -llber"
    INCLUDES="$INCLUDES $LDAP_INCLUDE"
    EXTRA_LIBS="$EXTRA_LIBS $LDAP_LFLAGS $LDAP_LIBS"

    AC_DEFINE(HAVE_LDAP)

    AC_MSG_RESULT(yes)
    PHP_EXTENSION(ldap)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
