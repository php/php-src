dnl $Id$

AC_MSG_CHECKING(for IMAP support)
AC_ARG_WITH(imap,
[  --with-imap[=DIR]       Include IMAP support.  DIR is the IMAP include
                          and c-client.a directory.],
[
	if test "$withval" = "yes"; then
		if test -f /usr/local/include/mail.h; then
		withval=/usr/local
		elif test -f /usr/include/mail.h; then
			withval=/usr
		elif test -f /usr/include/imap/mail.h; then
			withval=/usr
		elif test -f /usr/include/c-client/mail.h; then
			withval=/usr
		fi
	fi
	if test "$withval" != "no" && test "$withval" != "yes"; then
		IMAP_DIR=$withval
		if test -f $IMAP_DIR/include/imap/mail.h; then
			IMAP_INC_DIR=$IMAP_DIR/include/imap
		elif test -f $IMAP_DIR/include/c-client/mail.h; then
			IMAP_INC_DIR=$IMAP_DIR/include/c-client
		else
			IMAP_INC_DIR=$withval/include
		fi
		if test ! -f $IMAP_INC_DIR/mail.h; then
			AC_MSG_ERROR(could not find mail.h in $IMAP_INC_DIR !)
		fi
		if test ! -f  $IMAP_INC_DIR/rfc822.h; then
			AC_MSG_ERROR(could not find rfc822.h in $IMAP_INC_DIR !)
		fi
		if test ! -f $IMAP_INC_DIR/linkage.h; then
			AC_MSG_ERROR(could not find linkage.h in $IMAP_INC_DIR !)
		fi
		if test -f $IMAP_DIR/lib/libimap.a; then
			IMAP_LIBDIR="$IMAP_DIR/lib"
			IMAP_LIB="imap"
		elif test ! -f $IMAP_DIR/lib/libc-client.a; then
			if test ! -f $IMAP_DIR/lib/libc-client4.so; then
				AC_MSG_ERROR(You need to copy or link $IMAP_DIR/lib/c-client.a to $IMAP_DIR/lib/libc-client.a)
			else
				IMAP_LIBDIR="$IMAP_DIR/lib"
				IMAP_LIB="c-client4"
			fi
		else
			IMAP_LIBDIR="$IMAP_DIR/lib"
			IMAP_LIB="c-client"
		fi
		AC_ADD_LIBRARY_WITH_PATH($IMAP_LIB, $IMAP_LIBDIR)
		AC_ADD_INCLUDE($IMAP_INC_DIR)
		AC_DEFINE(HAVE_IMAP,,[ ])
		AC_MSG_RESULT(yes)
		PHP_EXTENSION(imap)
	else
		AC_MSG_ERROR(could not find imap library!)
	fi
],[
    AC_MSG_RESULT(no)
])
  

