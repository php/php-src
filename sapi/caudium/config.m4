dnl
dnl $Id$
dnl 

RESULT=no
PHP_ARG_WITH(caudium,,
[  --with-caudium[=DIR]    Build PHP as a Pike module for use with Caudium.
                          DIR is the Caudium server dir [/usr/local/caudium/server]], no, no)

AC_MSG_CHECKING([for Caudium support])

if test "$PHP_CAUDIUM" != "no"; then
  if test "$prefix" = "NONE"; then CPREF=/usr/local/; fi
  if test ! -d $PHP_CAUDIUM ; then
    if test "$prefix" = "NONE"; then
      PHP_CAUDIUM=/usr/local/caudium/server/
    else
      PHP_CAUDIUM=$prefix/caudium/server/
    fi
  fi
  if test -f $PHP_CAUDIUM/bin/caudium; then
    PIKE=$PHP_CAUDIUM/bin/caudium
  elif test -f $PHP_CAUDIUM/bin/pike; then
    PIKE=$PHP_CAUDIUM/bin/pike
  else
    AC_MSG_ERROR([Could not find a pike in $PHP_CAUDIUM/bin/])
  fi
  if $PIKE -e 'float v; int rel;sscanf(version(), "Pike v%f release %d", v, rel);v += rel/10000.0; if(v < 7.0268) exit(1); exit(0);'; then
    PIKE_MODULE_DIR=`$PIKE --show-paths 2>&1| grep '^Module' | sed -e 's/.*: //'`
    PIKE_INCLUDE_DIR=`echo $PIKE_MODULE_DIR | sed -e 's,lib/pike/modules,include/pike,' -e 's,lib/modules,include/pike,' `
    if test -z "$PIKE_INCLUDE_DIR" || test -z "$PIKE_MODULE_DIR"; then
      AC_MSG_ERROR(Failed to figure out Pike module and include directories)
    fi
    AC_MSG_RESULT(yes)
    PIKE=`echo $PIKE | pike -e 'int tries=100;
       string orig,pike=Stdio.File("stdin")->read()-"\n";
       orig=pike;
       if(search(orig, "/"))
         orig = combine_path(getcwd(), orig);
       while(!catch(pike=readlink(pike)) && tries--)
         ;
       write(combine_path(dirname(orig), pike)); '`
    PHP_ADD_INCLUDE($PIKE_INCLUDE_DIR)
    if test "$prefix" != "NONE"; then
      PIKE_C_INCLUDE=$prefix/include/`basename $PIKE`
    else
      PIKE_C_INCLUDE=/usr/local/include/`basename $PIKE`
    fi
    AC_MSG_CHECKING([for C includes in $PIKE_C_INCLUDE])
    if test -f $PIKE_C_INCLUDE/version.h; then
      PIKE_TEST_VER=`$PIKE -e 'string v; int rel;sscanf(version(), "Pike v%s release %d", v, rel); write(v+"."+rel);'`
      ###### VERSION MATCH CHECK #######
      PMAJOR="^#define PIKE_MAJOR_VERSION"
      PMINOR="^#define PIKE_MINOR_VERSION"
      PBUILD="^#define PIKE_BUILD_VERSION"

      PIKE_CMAJOR_VERSION=0
      PIKE_CMINOR_VERSION=0
      PIKE_CBUILD_VERSION=0

      PIKE_CMAJOR_VERSION=`grep "$PMAJOR" $PIKE_C_INCLUDE/version.h | sed -e 's/\(#define.*N \)\(.*\)/\2/'`
      if test -z "$PIKE_CMAJOR_VERSION"; then
        if test -n "`grep f_version $PIKE_C_INCLUDE/version.h`"; then
          PIKE_CMAJOR_VERSION=6
        fi
      else
        PIKE_CMINOR_VERSION=`grep "$PMINOR" $PIKE_C_INCLUDE/version.h | sed -e 's/\(#define.*N \)\(.*\)/\2/'`
        PIKE_CBUILD_VERSION=`grep "$PBUILD" $PIKE_C_INCLUDE/version.h | sed -e 's/\(#define.*N \)\(.*\)/\2/'`
      fi

      if test "$PIKE_TEST_VER" = "${PIKE_CMAJOR_VERSION}.${PIKE_CMINOR_VERSION}.${PIKE_CBUILD_VERSION}"; then
        PHP_ADD_INCLUDE($PIKE_C_INCLUDE)
        PIKE_INCLUDE_DIR="$PIKE_INCLUDE_DIR, $PIKE_C_INCLUDE"
        AC_MSG_RESULT(found)
      else
        AC_MSG_RESULT(version mismatch)
      fi
    else
      AC_MSG_RESULT(not found)
    fi
  else
    AC_MSG_ERROR([Caudium PHP5 requires Pike 7.0 or newer])
  fi
  PIKE_VERSION=`$PIKE -e 'string v; int rel;sscanf(version(), "Pike v%s release %d", v, rel); write(v+"."+rel);'`   
  AC_DEFINE(HAVE_CAUDIUM,1,[Whether to compile with Caudium support])
  PHP_SELECT_SAPI(caudium, shared, caudium.c)
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED $PHP_CAUDIUM/lib/$PIKE_VERSION/PHP5.so"
  RESULT="  *** Pike binary used:         $PIKE
  *** Pike include dir(s) used: $PIKE_INCLUDE_DIR
  *** Pike version:             $PIKE_VERSION"
  dnl Always use threads since thread-free support really blows.
  PHP_BUILD_THREAD_SAFE
fi
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
