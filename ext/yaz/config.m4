dnl $Id$

PHP_ARG_WITH(yaz,for YAZ support,
[  --with-yaz[=DIR]        Include YAZ support (ANSI/NISO Z39.50). DIR is
                          the YAZ bin install directory])


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
    for i in $YAZLIB; do
      case $i in
      -l*)
        ii=`echo $i|cut -c 3-`
        AC_ADD_LIBRARY($ii, YAZ_SHARED_LIBADD)
        ;;
      -L*)
        ii=`echo $i|cut -c 3-`
        AC_ADD_LIBPATH($ii,, YAZ_SHARED_LIBADD)
        ;;
      esac
    done
    for i in $YAZINC; do
      case $i in
      -I*)
        ii=`echo $i|cut -c 3-`
        AC_ADD_INCLUDE($ii)
        ;;
      esac
    done
    PHP_SUBST(YAZ_SHARED_LIBADD)
    PHP_EXTENSION(yaz, $ext_shared)
  fi
fi
