dnl $Id$
dnl
dnl This file contains local autoconf functions.
 
AC_DEFUN(PHP_CONFIG_NICE,[
  rm -f $1
  cat >$1<<EOF
#! /bin/sh
#
# Created by configure

EOF

  for arg in [$]0 "[$]@"; do
    echo "\"[$]arg\" \\" >> $1
  done
  echo '"[$]@"' >> $1
  chmod +x $1
])

AC_DEFUN(PHP_TIME_R_TYPE,[
AC_CACHE_CHECK(for time_r type, ac_cv_time_r_type,[
AC_TRY_RUN([
#include <time.h>

main() {
char buf[27];
struct tm t;
time_t old = 0;
int r;

gmtime_r(&old, &t);
r = (int) asctime_r(&t, buf, 26);
if (r == -1 || (r > 0 && r <= 26)) exit(0);
exit(1);
}
],[
  ac_cv_time_r_type=hpux
],[
  ac_cv_time_r_type=normal
],[
  ac_cv_time_r_type=normal
])
])
if test "$ac_cv_time_r_type" = "hpux"; then
  AC_DEFINE(PHP_HPUX_TIME_R,1,[Whether you have HP-SUX 10.x])
fi
])

AC_DEFUN(PHP_SUBST,[
  PHP_VAR_SUBST="$PHP_VAR_SUBST $1"
  AC_SUBST($1)
])

AC_DEFUN(PHP_FAST_OUTPUT,[
  PHP_FAST_OUTPUT_FILES="$PHP_FAST_OUTPUT_FILES $1"
])

AC_DEFUN(PHP_MKDIR_P_CHECK,[
  AC_CACHE_CHECK(for working mkdir -p, ac_cv_mkdir_p,[
    test -d conftestdir && rm -rf conftestdir
    mkdir -p conftestdir/somedir >/dev/null 2>&1
    if test -d conftestdir/somedir; then
      ac_cv_mkdir_p=yes
    else
      ac_cv_mkdir_p=no
    fi
    rm -rf conftestdir
  ])
])

AC_DEFUN(PHP_FAST_GENERATE,[
  PHP_MKDIR_P_CHECK
  echo creating config_vars.mk
  > config_vars.mk
  for i in $PHP_VAR_SUBST; do
    eval echo "$i = \$$i" >> config_vars.mk
  done
  $SHELL $srcdir/build/fastgen.sh $srcdir $ac_cv_mkdir_p $PHP_FAST_OUTPUT_FILES
])

AC_DEFUN(PHP_TM_GMTOFF,[
AC_CACHE_CHECK([for tm_gmtoff in struct tm], ac_cv_struct_tm_gmtoff,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <$ac_cv_struct_tm>], [struct tm tm; tm.tm_gmtoff;],
  ac_cv_struct_tm_gmtoff=yes, ac_cv_struct_tm_gmtoff=no)])

if test "$ac_cv_struct_tm_gmtoff" = yes; then
  AC_DEFINE(HAVE_TM_GMTOFF)
fi
])

dnl PHP_CONFIGURE_PART(MESSAGE)
dnl Idea borrowed from mm
AC_DEFUN(PHP_CONFIGURE_PART,[
  AC_MSG_RESULT()
  AC_MSG_RESULT(${T_MD}$1${T_ME})
])

AC_DEFUN(PHP_PROG_SENDMAIL,[
AC_PATH_PROG(PROG_SENDMAIL, sendmail, /usr/lib/sendmail, $PATH /usr/bin /usr/sbin /usr/etc /etc /usr/ucblib)
if test -n "$PROG_SENDMAIL"; then
  AC_DEFINE(HAVE_SENDMAIL)
fi
])

AC_DEFUN(PHP_RUNPATH_SWITCH,[
dnl check for -R, etc. switch
AC_MSG_CHECKING(if compiler supports -R)
AC_CACHE_VAL(php_cv_cc_dashr,[
	SAVE_LIBS="${LIBS}"
	LIBS="-R /usr/lib ${LIBS}"
	AC_TRY_LINK([], [], php_cv_cc_dashr=yes, php_cv_cc_dashr=no)
	LIBS="${SAVE_LIBS}"])
AC_MSG_RESULT($php_cv_cc_dashr)
if test $php_cv_cc_dashr = "yes"; then
	ld_runpath_switch="-R"
else
	AC_MSG_CHECKING([if compiler supports -Wl,-rpath,])
	AC_CACHE_VAL(php_cv_cc_rpath,[
		SAVE_LIBS="${LIBS}"
		LIBS="-Wl,-rpath,/usr/lib ${LIBS}"
		AC_TRY_LINK([], [], php_cv_cc_rpath=yes, php_cv_cc_rpath=no)
		LIBS="${SAVE_LIBS}"])
	AC_MSG_RESULT($php_cv_cc_rpath)
	if test $php_cv_cc_rpath = "yes"; then
		ld_runpath_switch="-Wl,-rpath,"
	else
		dnl something innocuous
		ld_runpath_switch="-L"
	fi
fi
])

AC_DEFUN(PHP_STRUCT_FLOCK,[
AC_CACHE_CHECK(for struct flock,ac_cv_struct_flock,
    AC_TRY_COMPILE([
#include <unistd.h>
#include <fcntl.h>
        ],
        [struct flock x;],
        [
          ac_cv_struct_flock=yes
        ],[
          ac_cv_struct_flock=no
        ])
)
if test "$ac_cv_struct_flock" = "yes" ; then
    AC_DEFINE(HAVE_STRUCT_FLOCK, 1)
fi
])

AC_DEFUN(PHP_SOCKLEN_T,[
AC_CACHE_CHECK(for socklen_t,ac_cv_socklen_t,
  AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/socket.h>
],[
socklen_t x;
],[
  ac_cv_socklen_t=yes
],[
  ac_cv_socklen_t=no
]))
if test "$ac_cv_socklen_t" = "yes"; then
  AC_DEFINE(HAVE_SOCKLEN_T, 1, [Whether you have socklen_t])
fi
])

dnl
dnl PHP_SET_SYM_FILE(path)
dnl
dnl set the path of the file which contains the symbol export list
dnl
AC_DEFUN(PHP_SET_SYM_FILE,
[
  PHP_SYM_FILE="$1"
])

dnl
dnl PHP_BUILD_THREAD_SAFE
dnl
AC_DEFUN(PHP_BUILD_THREAD_SAFE,[
  enable_experimental_zts=yes
])

dnl
dnl PHP_BUILD_SHARED
dnl
AC_DEFUN(PHP_BUILD_SHARED,[
  php_build_target=shared
])

dnl
dnl PHP_BUILD_STATIC
dnl
AC_DEFUN(PHP_BUILD_STATIC,[
  php_build_target=static
])

dnl
dnl PHP_BUILD_PROGRAM
dnl
AC_DEFUN(PHP_BUILD_PROGRAM,[
  php_build_target=program
])

dnl
dnl AC_PHP_ONCE(namespace, variable, code)
dnl
dnl execute code, if variable is not set in namespace
dnl
AC_DEFUN(AC_PHP_ONCE,[
  unique=`echo $ac_n "$2$ac_c" | tr -cd a-zA-Z0-9`
  cmd="echo $ac_n \"\$$1$unique$ac_c\""
  if test -n "$unique" && test "`eval $cmd`" = "" ; then
    eval "$1$unique=set"
    $3
  fi
])

dnl
dnl AC_EXPAND_PATH(path, variable)
dnl
dnl expands path to an absolute path and assigns it to variable
dnl
AC_DEFUN(AC_EXPAND_PATH,[
  if test -z "$1" || echo "$1" | grep '^/' >/dev/null ; then
    $2="$1"
  else
    ep_dir="`dirname \"$1\"`"
    ep_realdir="`(cd \"$ep_dir\" && pwd)`"
    $2="$ep_realdir/`basename \"$1\"`"
  fi
])

dnl
dnl AC_ADD_LIBPATH(path)
dnl
dnl add a library to linkpath/runpath
dnl
AC_DEFUN(AC_ADD_LIBPATH,[
  if test "$1" != "/usr/lib"; then
    AC_EXPAND_PATH($1, ai_p)
    AC_PHP_ONCE(LIBPATH, $ai_p, [
      test -n "$ld_runpath_switch" && LDFLAGS="$LDFLAGS $ld_runpath_switch$ai_p"
      LDFLAGS="$LDFLAGS -L$ai_p"
      PHP_RPATHS="$PHP_RPATHS $ai_p"
    ])
  fi
])

dnl
dnl AC_BUILD_RPATH()
dnl
dnl builds RPATH from PHP_RPATHS
dnl
AC_DEFUN(AC_BUILD_RPATH,[
  if test "$enable_rpath" = "yes" && test -n "$PHP_RPATHS"; then
    OLD_RPATHS="$PHP_RPATHS"
	PHP_RPATHS=""
	for i in $OLD_RPATHS; do
        PHP_LDFLAGS="$PHP_LDFLAGS -L$i"
        PHP_RPATHS="$PHP_RPATHS -R $i"
        NATIVE_RPATHS="$NATIVE_RPATHS ${ld_runpath_switch}$i"
	  done
  fi
])

dnl
dnl AC_ADD_INCLUDE(path)
dnl
dnl add a include path
dnl
AC_DEFUN(AC_ADD_INCLUDE,[
  if test "$1" != "/usr/include"; then
    AC_EXPAND_PATH($1, ai_p)
    AC_PHP_ONCE(INCLUDEPATH, $ai_p, [
      INCLUDES="$INCLUDES -I$ai_p"
    ])
  fi
])

dnl
dnl AC_ADD_LIBRARY(library[, append])
dnl
dnl add a library to the link line
dnl
AC_DEFUN(AC_ADD_LIBRARY,[
  AC_PHP_ONCE(LIBRARY, $1, [
    ifelse($#, 1, LIBS="-l$1 $LIBS", LIBS="$LIBS -l$1")
  ])
])

dnl
dnl AC_ADD_LIBRARY_DEFER(library[, append])
dnl
dnl add a library to the link line (defferred)
AC_DEFUN(AC_ADD_LIBRARY_DEFER,[
  AC_PHP_ONCE(LIBRARY, $1, [
    ifelse($#, 1, DLIBS="-l$1 $DLIBS", DLIBS="$DLIBS -l$1")
  ])
])

dnl
dnl AC_ADD_LIBRARY_WITH_PATH(library, path)
dnl
dnl add a library to the link line and path to linkpath/runpath
dnl
AC_DEFUN(AC_ADD_LIBRARY_WITH_PATH,[
  AC_ADD_LIBPATH($2)
  AC_ADD_LIBRARY($1)
])


AC_DEFUN(AM_SET_LIBTOOL_VARIABLE,[
  LIBTOOL='$(SHELL) $(top_builddir)/libtool $1'
])

dnl
dnl Check for cc option
dnl
AC_DEFUN(AC_CHECK_CC_OPTION,[
  echo "main(){return 0;}" > conftest.$ac_ext
  opt="$1"
  var=`echo $ac_n "$opt$ac_c"|tr -c a-zA-Z0-9 _`
  AC_MSG_CHECKING([if compiler supports -$1 really])
  ac_php_compile="${CC-cc} -$opt -o conftest $CFLAGS $CPPFLAGS conftest.$ac_ext 2>&1"
  if eval $ac_php_compile 2>&1 | egrep "$opt" > /dev/null 2>&1 ; then
    eval php_cc_$var=no
	AC_MSG_RESULT(no)
  else
    if eval ./conftest 2>/dev/null ; then
      eval php_cc_$var=yes
	  AC_MSG_RESULT(yes)
    else
      eval php_cc_$var=no
	  AC_MSG_RESULT(no)
    fi
  fi
])

AC_DEFUN(PHP_REGEX,[

if test "$REGEX_TYPE" = "php"; then
  REGEX_LIB=regex/libregex.la
  REGEX_DIR=regex
  AC_DEFINE(HSREGEX)
  AC_DEFINE(REGEX,1)
  PHP_FAST_OUTPUT(regex/Makefile)
elif test "$REGEX_TYPE" = "system"; then
  AC_DEFINE(REGEX,0)
fi

AC_MSG_CHECKING(which regex library to use)
AC_MSG_RESULT($REGEX_TYPE)

PHP_SUBST(REGEX_DIR)
PHP_SUBST(REGEX_LIB)
PHP_SUBST(HSREGEX)
])

dnl
dnl See if we have broken header files like SunOS has.
dnl
AC_DEFUN(AC_MISSING_FCLOSE_DECL,[
  AC_MSG_CHECKING([for fclose declaration])
  AC_TRY_COMPILE([#include <stdio.h>],[int (*func)() = fclose],[
    AC_DEFINE(MISSING_FCLOSE_DECL,0)
    AC_MSG_RESULT(ok)
  ],[
    AC_DEFINE(MISSING_FCLOSE_DECL,1)
    AC_MSG_RESULT(missing)
  ])
])

dnl
dnl Check for broken sprintf()
dnl
AC_DEFUN(AC_BROKEN_SPRINTF,[
  AC_CACHE_CHECK(whether sprintf is broken, ac_cv_broken_sprintf,[
    AC_TRY_RUN([main() {char buf[20];exit(sprintf(buf,"testing 123")!=11); }],[
      ac_cv_broken_sprintf=no
    ],[
      ac_cv_broken_sprintf=yes
    ],[
      ac_cv_broken_sprintf=no
    ])
  ])
  if test "$ac_cv_broken_sprintf" = "yes"; then
    AC_DEFINE(BROKEN_SPRINTF, 1)
  else
    AC_DEFINE(BROKEN_SPRINTF, 0)
  fi
])

dnl
dnl PHP_EXTENSION(extname [, shared])
dnl
dnl Includes an extension in the build.
dnl
dnl "extname" is the name of the ext/ subdir where the extension resides
dnl "shared" can be set to "shared" or "yes" to build the extension as
dnl a dynamically loadable library.
dnl
AC_DEFUN(PHP_EXTENSION,[
  if test -d "$cwd/$srcdir/ext/$1" ; then
    ext_src_base="${abs_srcdir}/ext/$1/"
    ext_base="ext/$1/"
    EXT_SUBDIRS="$EXT_SUBDIRS $1"
    if test "$2" != "shared" && test "$2" != "yes" && test -z "$php_always_shared"; then
      _extlib="lib$1.a"
      EXT_LTLIBS="$EXT_LTLIBS ext/$1/lib$1.la"
      EXT_LIBS="$EXT_LIBS $1/$_extlib"
      EXT_STATIC="$EXT_STATIC $1"
    else
      AC_DEFINE_UNQUOTED([COMPILE_DL_]translit($1,a-z-,A-Z_), 1, Whether to build $1 as dynamic module)
      EXT_SHARED="$EXT_SHARED $1"
    fi
    PHP_FAST_OUTPUT(ext/$1/Makefile)
  fi
])

PHP_SUBST(EXT_SUBDIRS)
PHP_SUBST(EXT_STATIC)
PHP_SUBST(EXT_SHARED)
PHP_SUBST(EXT_LIBS)
PHP_SUBST(EXT_LTLIBS)

dnl
dnl Solaris requires main code to be position independent in order
dnl to let shared objects find symbols.  Weird.  Ugly.
dnl
dnl Must be run after all --with-NN options that let the user
dnl choose dynamic extensions, and after the gcc test.
dnl
AC_DEFUN(PHP_SOLARIS_PIC_WEIRDNESS,[
  AC_MSG_CHECKING(whether -fPIC is required)
  if test "$EXT_SHARED" != ""; then
    os=`uname -sr 2>/dev/null`
    case "$os" in
        "SunOS 5.6"|"SunOS 5.7")
          case "$CC" in
	    gcc*|egcs*) CFLAGS="$CFLAGS -fPIC";;
	    *) CFLAGS="$CFLAGS -fpic";;
	  esac
	  AC_MSG_RESULT(yes);;
	*)
	  AC_MSG_RESULT(no);;
    esac
  else
    AC_MSG_RESULT(no)
  fi
])

dnl
dnl Checks whether $withval is "shared" or starts with "shared,XXX"
dnl and sets $shared to "yes" or "no", and removes "shared,?" stuff
dnl from $withval.
dnl
AC_DEFUN(PHP_WITH_SHARED,[
    case $withval in
	shared)
	    shared=yes
	    withval=yes
	    ;;
	shared,*)
	    shared=yes
	    withval=`echo $withval | sed -e 's/^shared,//'`      
	    ;;
	*)
	    shared=no
	    ;;
    esac
    if test -n "$php_always_shared"; then
		shared=yes
	fi
])

dnl The problem is that the default compilation flags in Solaris 2.6 won't
dnl let programs access large files;  you need to tell the compiler that
dnl you actually want your programs to work on large files.  For more
dnl details about this brain damage please see:
dnl http://www.sas.com/standards/large.file/x_open.20Mar96.html

dnl Written by Paul Eggert <eggert@twinsun.com>.

AC_DEFUN(AC_SYS_LFS,
[dnl
  # If available, prefer support for large files unless the user specified
  # one of the CPPFLAGS, LDFLAGS, or LIBS variables.
  AC_MSG_CHECKING(whether large file support needs explicit enabling)
  ac_getconfs=''
  ac_result=yes
  ac_set=''
  ac_shellvars='CPPFLAGS LDFLAGS LIBS'
  for ac_shellvar in $ac_shellvars; do
    case $ac_shellvar in
      CPPFLAGS) ac_lfsvar=LFS_CFLAGS ;;
      *) ac_lfsvar=LFS_$ac_shellvar ;;
    esac
    eval test '"${'$ac_shellvar'+set}"' = set && ac_set=$ac_shellvar
    (getconf $ac_lfsvar) >/dev/null 2>&1 || { ac_result=no; break; }
    ac_getconf=`getconf $ac_lfsvar`
    ac_getconfs=$ac_getconfs$ac_getconf
    eval ac_test_$ac_shellvar=\$ac_getconf
  done
  case "$ac_result$ac_getconfs" in
    yes) ac_result=no ;;
  esac
  case "$ac_result$ac_set" in
    yes?*) ac_result="yes, but $ac_set is already set, so use its settings"
  esac
  AC_MSG_RESULT($ac_result)
  case $ac_result in
    yes)
      for ac_shellvar in $ac_shellvars; do
        eval $ac_shellvar=\$ac_test_$ac_shellvar
      done ;;
  esac
])

AC_DEFUN(AC_SOCKADDR_SA_LEN,[
  AC_CACHE_CHECK([for field sa_len in struct sockaddr],ac_cv_sockaddr_sa_len,[
    AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/socket.h>],
    [struct sockaddr s; s.sa_len;],
    [ac_cv_sockaddr_sa_len=yes
     AC_DEFINE(HAVE_SOCKADDR_SA_LEN)],
    [ac_cv_sockaddr_sa_len=no])
  ])
])


dnl ## PHP_AC_OUTPUT(file)
dnl ## adds "file" to the list of files generated by AC_OUTPUT
dnl ## This macro can be used several times.
AC_DEFUN(PHP_OUTPUT,[
  PHP_OUTPUT_FILES="$PHP_OUTPUT_FILES $1"
])

AC_DEFUN(PHP_DECLARED_TIMEZONE,[
  AC_CACHE_CHECK(for declared timezone, ac_cv_declared_timezone,[
    AC_TRY_COMPILE([
#include <sys/types.h>
#include <time.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
],[
    time_t foo = (time_t) timezone;
],[
  ac_cv_declared_timezone=yes
],[
  ac_cv_declared_timezone=no
])])
  if test "$ac_cv_declared_timezone" = "yes"; then
    AC_DEFINE(HAVE_DECLARED_TIMEZONE, 1, [Whether system headers declare timezone])
  fi
])
