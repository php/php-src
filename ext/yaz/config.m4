dnl
dnl $Id$
dnl

PHP_ARG_WITH(yaz,for YAZ support,
[  --with-yaz[=DIR]        Include YAZ support (ANSI/NISO Z39.50). 
                          DIR is the YAZ bin install directory.])


if test "$PHP_YAZ" != "no"; then
  yazconfig=NONE
  if test "$PHP_YAZ" = "yes"; then
    AC_PATH_PROG(yazconfig, yaz-config, NONE)
  else
    if test -r ${PHP_YAZ}/yaz-config; then
      yazconfig=${PHP_YAZ}/yaz-config
    else
      yazconfig=${PHP_YAZ}/bin/yaz-config
    fi
  fi

  if test -f $yazconfig; then
    AC_DEFINE(HAVE_YAZ,1,[Whether you have YAZ])
    . $yazconfig

    dnl Check version (2.0 or greater required)
    AC_MSG_CHECKING([for YAZ version])
    yaz_version=`echo $YAZVERSION | awk 'BEGIN { FS = "."; } { printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`
    if test "$yaz_version" -ge 2000000; then
      AC_MSG_RESULT([$YAZVERSION])
    else
      AC_MSG_ERROR([YAZ version 2.0 or later required.])
    fi

    for c in $YAZLIB; do
      case $c in
       -L*)
         dir=`echo $c|cut -c 3-|sed 's%/\.libs%%g'`
         PHP_ADD_LIBPATH($dir,YAZ_SHARED_LIBADD)
        ;;
       -l*)
         lib=`echo $c|cut -c 3-`
         PHP_ADD_LIBRARY($lib,,YAZ_SHARED_LIBADD)
        ;;
      esac
    done
    PHP_EVAL_INCLINE($YAZINC)
    PHP_NEW_EXTENSION(yaz, php_yaz.c, $ext_shared)
    PHP_SUBST(YAZ_SHARED_LIBADD)
  else
    AC_MSG_ERROR([YAZ not found (missing $yazconfig)])
  fi
fi
