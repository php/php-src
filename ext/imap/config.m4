dnl $Id$

AC_DEFUN(IMAP_INC_CHK,[if test -r $i$1/rfc822.h; then IMAP_DIR=$i; IMAP_INC_DIR=$i$1])

AC_DEFUN(IMAP_LIB_CHK,[
		str="$IMAP_DIR/$1/lib$lib.*"
		for i in `echo $str`; do
			if test -r $i; then
				IMAP_LIB_DIR=$IMAP_DIR/$1
				break 2
			fi
		done
		])

RESULT=no
AC_MSG_CHECKING(for IMAP support)
AC_ARG_WITH(imap,
[  --with-imap[=DIR]       Include IMAP support.  DIR is the IMAP include
                          and c-client.a directory.],
[
  if test "$withval" != "no"; then  
    for i in /usr/local /usr $withval; do
      IMAP_INC_CHK()
      el[]IMAP_INC_CHK(/include)
      el[]IMAP_INC_CHK(/include/imap)
      el[]IMAP_INC_CHK(/imap)
      el[]IMAP_INC_CHK(/c-client)
      fi
    done

    AC_EXPAND_PATH($IMAP_DIR, IMAP_DIR)

    if test -z "$IMAP_DIR"; then
      AC_MSG_ERROR(Cannot find rfc822.h. Please check your IMAP installation)
    fi

    if test -r "$IMAP_DIR/c-client/c-client.a"; then
      ln -s "$IMAP_DIR/c-client/c-client.a" "$IMAP_DIR/c-client/libc-client.a" >/dev/null 2>&1
    fi

    for lib in imap c-client4 c-client; do
      IMAP_LIB=$lib
      IMAP_LIB_CHK(lib)
      IMAP_LIB_CHK(c-client)
    done

    if test -z "$IMAP_LIB_DIR"; then
      AC_MSG_ERROR(Cannot find imap library. Please check your IMAP installation)
    fi

    AC_ADD_INCLUDE($IMAP_INC_DIR)
    AC_ADD_LIBPATH($IMAP_LIB_DIR)
    AC_ADD_LIBRARY_DEFER($IMAP_LIB)

    PHP_EXTENSION(imap)

    AC_DEFINE(HAVE_IMAP,1,[ ])
    RESULT=yes
  fi
])
AC_MSG_RESULT($RESULT)
