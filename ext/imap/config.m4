dnl $Id$

AC_MSG_CHECKING(for IMAP support)
AC_ARG_WITH(imap,
[  --with-imap[=DIR]       Include IMAP support (DIR is the IMAP include dir and c-client.a dir).],
[
  if test "$withval" != "no"; then
	if test "$withval" = "yes"; then
        IMAP_DIR=/usr/local
	else
		IMAP_DIR=$withval
    fi
    if test ! -f $IMAP_DIR/include/mail.h; then
      AC_MSG_ERROR(could not find mail.h in $IMAP_DIR/include !)
	fi
    if test ! -f  $IMAP_DIR/include/rfc822.h; then
      AC_MSG_ERROR(could not find rfc822.h in $IMAP_DIR/include !)
	fi
    if test ! -f $IMAP_DIR/include/linkage.h; then
      AC_MSG_ERROR(could not find linkage.h in $IMAP_DIR/include !)
	fi
    if test ! -f $IMAP_DIR/lib/c-client.a; then
      AC_MSG_ERROR(could not find c-client.a in $IMAP_DIR/lib !)
	fi
    IMAP_INCLUDE="-I$IMAP_DIR/include/"
    IMAP_LIBS="$IMAP_DIR/lib/c-client.a"

dnl## AC_SUBST(IMAP_INCLUDE)
    INCLUDES="$INCLUDES $IMAP_INCLUDE"
    AC_SUBST(IMAP_LIBS)
    LIBS="$LIBS $IMAP_LIBS"
    AC_DEFINE(HAVE_IMAP)
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(imap)
  else
    AC_MSG_ERROR(could not find imap library!)
  fi
],[
    AC_MSG_RESULT(no)
])
  


