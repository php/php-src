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
    dir=""
    for c in $YAZLIB; do
      case $c in
       -L*)
         dir=`echo $c|cut -c 3-|sed 's%/\.libs%%g'`
        ;;
       -lyaz*)
        ;;
       *)
       PHP_EVAL_LIBLINE($c, YAZ_SHARED_LIBADD)
        ;;
      esac
    done
    if test -n "$dir"; then
      PHP_ADD_LIBPATH($dir,YAZ_SHARED_LIBADD)
    fi
    PHP_ADD_LIBRARY_DEFER(yaz,1,YAZ_SHARED_LIBADD)
    PHP_EVAL_INCLINE($YAZINC)
    PHP_SUBST(YAZ_SHARED_LIBADD)
    PHP_NEW_EXTENSION(yaz, php_yaz.c, $ext_shared)
  else
    AC_MSG_ERROR([YAZ not found (missing $yazconfig)])
  fi
fi
