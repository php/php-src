dnl
dnl $Id$
dnl

dnl Check for extensions with which Recode can not work
if test "$PHP_RECODE" != "no"; then
  test "$PHP_IMAP"  != "no" && recode_conflict="$recode_conflict imap"
  test "$PHP_MYSQL" != "no" && recode_conflict="$recode_conflict mysql"
  test "$PHP_YAZ"   != "no" && recode_conflict="$recode_conflict yaz"

  if test -n "$recode_conflict"; then
    AC_MSG_ERROR([recode extension can not be configured together with:$recode_conflict])
  fi
fi
