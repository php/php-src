dnl $Id$
dnl
dnl This file contains local autoconf functions.

sinclude(dynlib.m4)

dnl
dnl Disable building CLI
dnl
AC_DEFUN(PHP_DISABLE_CLI,[
  disable_cli=1
])
	
dnl
dnl Separator into the configure --help display.
dnl 
AC_DEFUN(PHP_HELP_SEPARATOR,[
AC_ARG_ENABLE([],[
$1
],[])
])

dnl
dnl PHP_TARGET_RDYNAMIC
dnl
dnl Checks whether -rdynamic is supported by the compiler.  This
dnl is necessary for some targets to populate the global symbol
dnl table.  Otherwise, dynamic modules would not be able to resolve
dnl PHP-related symbols.
dnl
dnl If successful, adds -rdynamic to PHP_LDFLAGS.
dnl
AC_DEFUN(PHP_TARGET_RDYNAMIC,[
  if test -n "$GCC"; then
    dnl we should use a PHP-specific macro here
    TSRM_CHECK_GCC_ARG(-rdynamic, gcc_rdynamic=yes)
    if test "$gcc_rdynamic" = "yes"; then
      PHP_LDFLAGS="$PHP_LDFLAGS -rdynamic"
    fi
  fi
])

AC_DEFUN(PHP_REMOVE_USR_LIB,[
  unset ac_new_flags
  for i in [$]$1; do
    case [$]i in
    -L/usr/lib|-L/usr/lib/) ;;
    *) ac_new_flags="[$]ac_new_flags [$]i" ;;
    esac
  done
  $1=[$]ac_new_flags
])

AC_DEFUN(PHP_SETUP_OPENSSL,[
  if test "$PHP_OPENSSL" = "no"; then
    PHP_OPENSSL="/usr/local/ssl /usr/local /usr /usr/local/openssl"
  fi

  for i in $PHP_OPENSSL; do
    if test -r $i/include/openssl/evp.h; then
      OPENSSL_DIR=$i
      OPENSSL_INC=$i/include
    fi
  done

  if test -z "$OPENSSL_DIR"; then
    AC_MSG_ERROR([Cannot find OpenSSL's <evp.h>])
  fi

  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$OPENSSL_INC
  AC_MSG_CHECKING([for OpenSSL version])
  AC_EGREP_CPP(yes,[
  #include <openssl/opensslv.h>
  #if OPENSSL_VERSION_NUMBER >= 0x0090500fL
  yes
  #endif
  ],[
    AC_MSG_RESULT([>= 0.9.5])
  ],[
    AC_MSG_ERROR([OpenSSL version 0.9.5 or greater required.])
  ])
  CPPFLAGS=$old_CPPFLAGS

  PHP_ADD_LIBPATH($OPENSSL_DIR/lib)

  AC_CHECK_LIB(crypto, CRYPTO_free, [
    PHP_ADD_LIBRARY(crypto)
  ],[
    AC_MSG_ERROR([libcrypto not found!])
  ])

  AC_CHECK_LIB(ssl, SSL_CTX_set_ssl_version, [
    PHP_ADD_LIBRARY(ssl)
  ],[
    AC_MSG_ERROR([libssl not found!])
  ])
  PHP_ADD_INCLUDE($OPENSSL_INC)
])

dnl PHP_EVAL_LIBLINE(LINE, SHARED-LIBADD)
dnl
dnl Use this macro, if you need to add libraries and or library search
dnl paths to the PHP build system which are only given in compiler
dnl notation.
dnl
AC_DEFUN(PHP_EVAL_LIBLINE,[
  for ac_i in $1; do
    case $ac_i in
    -l*)
      ac_ii=`echo $ac_i|cut -c 3-`
      PHP_ADD_LIBRARY($ac_ii,1,$2)
    ;;
    -L*)
      ac_ii=`echo $ac_i|cut -c 3-`
      PHP_ADD_LIBPATH($ac_ii,$2)
    ;;
    esac
  done
])

dnl PHP_EVAL_INCLINE(LINE)
dnl
dnl Use this macro, if you need to add header search paths to the PHP
dnl build system which are only given in compiler notation.
dnl
AC_DEFUN(PHP_EVAL_INCLINE,[
  for ac_i in $1; do
    case $ac_i in
    -I*)
      ac_ii=`echo $ac_i|cut -c 3-`
      PHP_ADD_INCLUDE($ac_ii)
    ;;
    esac
  done
])
	
AC_DEFUN(PHP_READDIR_R_TYPE,[
  dnl HAVE_READDIR_R is also defined by libmysql
  AC_CHECK_FUNC(readdir_r,ac_cv_func_readdir_r=yes,ac_cv_func_readdir=no)
  if test "$ac_cv_func_readdir_r" = "yes"; then
  AC_CACHE_CHECK(for type of readdir_r, ac_cv_what_readdir_r,[
    AC_TRY_RUN([
#define _REENTRANT
#include <sys/types.h>
#include <dirent.h>

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

main() {
	DIR *dir;
	char entry[sizeof(struct dirent)+PATH_MAX];
	struct dirent *pentry = (struct dirent *) &entry;

	dir = opendir("/");
	if (!dir) 
		exit(1);
	if (readdir_r(dir, (struct dirent *) entry, &pentry) == 0)
		exit(0);
	exit(1);
}
    ],[
      ac_cv_what_readdir_r=POSIX
    ],[
      AC_TRY_CPP([
#define _REENTRANT
#include <sys/types.h>
#include <dirent.h>
int readdir_r(DIR *, struct dirent *);
        ],[
          ac_cv_what_readdir_r=old-style
        ],[
          ac_cv_what_readdir_r=none
      ])
    ],[
      ac_cv_what_readdir_r=none
   ])
  ])
    case $ac_cv_what_readdir_r in
    POSIX)
      AC_DEFINE(HAVE_POSIX_READDIR_R,1,[whether you have POSIX readdir_r]);;
    old-style)
      AC_DEFINE(HAVE_OLD_READDIR_R,1,[whether you have old-style readdir_r]);;
    esac
  fi
])

AC_DEFUN(PHP_SHLIB_SUFFIX_NAME,[
  PHP_SUBST(SHLIB_SUFFIX_NAME)
  SHLIB_SUFFIX_NAME=so
  case $host_alias in
  *hpux*)
	SHLIB_SUFFIX_NAME=sl
	;;
  *darwin*)
	SHLIB_SUFFIX_NAME=dylib
	;;
  esac
])

AC_DEFUN(PHP_DEBUG_MACRO,[
  DEBUG_LOG=$1
  cat >$1 <<X
CONFIGURE:  $CONFIGURE_COMMAND
CC:         $CC
CFLAGS:     $CFLAGS
CPPFLAGS:   $CPPFLAGS
CXX:        $CXX
CXXFLAGS:   $CXXFLAGS
INCLUDES:   $INCLUDES
LDFLAGS:    $LDFLAGS
LIBS:       $LIBS
DLIBS:      $DLIBS
SAPI:       $PHP_SAPI
PHP_RPATHS: $PHP_RPATHS
uname -a:   `uname -a`

X
    cat >conftest.$ac_ext <<X
main()
{
  exit(0);
}
X
    (eval echo \"$ac_link\"; eval $ac_link && ./conftest) >>$1 2>&1
    rm -fr conftest*
])

AC_DEFUN(PHP_MISSING_PREAD_DECL,[
  AC_CACHE_CHECK(whether pread works without custom declaration,ac_cv_pread,[
  AC_TRY_COMPILE([#include <unistd.h>],[size_t (*func)() = pread],[
    ac_cv_pread=yes
  ],[
    echo test > conftest_in
    AC_TRY_RUN([
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
      main() { char buf[3]; return !(pread(open("conftest_in", O_RDONLY), buf, 2, 0) == 2); }
    ],[
      ac_cv_pread=yes
    ],[
      echo test > conftest_in
      AC_TRY_RUN([
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
        ssize_t pread(int, void *, size_t, off64_t);
        main() { char buf[3]; return !(pread(open("conftest_in", O_RDONLY), buf, 2, 0) == 2); }
      ],[
        ac_cv_pread=64
      ],[
        ac_cv_pread=no
      ])
    ],[
      ac_cv_pread=no
    ])
  ])
  ])
  case $ac_cv_pread in
  no) ac_cv_func_pread=no;;
  64) AC_DEFINE(PHP_PREAD_64, 1, [whether pread64 is default]);;
  esac
])

AC_DEFUN(PHP_MISSING_PWRITE_DECL,[
  AC_CACHE_CHECK(whether pwrite works without custom declaration,ac_cv_pwrite,[
  AC_TRY_COMPILE([#include <unistd.h>],[size_t (*func)() = pwrite],[
    ac_cv_pwrite=yes
  ],[
    AC_TRY_RUN([
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
      main() { return !(pwrite(open("conftest_out", O_WRONLY|O_CREAT, 0600), "Ok", 2, 0) == 2); }
    ],[
      ac_cv_pwrite=yes
    ],[
      AC_TRY_RUN([
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
        ssize_t pwrite(int, void *, size_t, off64_t);
        main() { return !(pwrite(open("conftest_out", O_WRONLY|O_CREAT, 0600), "Ok", 2, 0) == 2); }
      ],[
        ac_cv_pwrite=64
      ],[
        ac_cv_pwrite=no
      ])
    ],[
      ac_cv_pwrite=no
    ])
  ])
  ])
  case $ac_cv_pwrite in
  no) ac_cv_func_pwrite=no;;
  64) AC_DEFINE(PHP_PWRITE_64, 1, [whether pwrite64 is default]);;
  esac
])

AC_DEFUN(PHP_MISSING_TIME_R_DECL,[
  AC_MSG_CHECKING([for missing declarations of reentrant functions])
  AC_TRY_COMPILE([#include <time.h>],[struct tm *(*func)() = localtime_r],[
    :
  ],[
    AC_DEFINE(MISSING_LOCALTIME_R_DECL,1,[Whether localtime_r is declared])
  ])
  AC_TRY_COMPILE([#include <time.h>],[struct tm *(*func)() = gmtime_r],[
    :
  ],[
    AC_DEFINE(MISSING_GMTIME_R_DECL,1,[Whether gmtime_r is declared])
  ])
  AC_TRY_COMPILE([#include <time.h>],[char *(*func)() = asctime_r],[
    :
  ],[
    AC_DEFINE(MISSING_ASCTIME_R_DECL,1,[Whether asctime_r is declared])
  ])
  AC_TRY_COMPILE([#include <time.h>],[char *(*func)() = ctime_r],[
    :
  ],[
    AC_DEFINE(MISSING_CTIME_R_DECL,1,[Whether ctime_r is declared])
  ])
  AC_TRY_COMPILE([#include <string.h>],[char *(*func)() = strtok_r],[
    :
  ],[
    AC_DEFINE(MISSING_STRTOK_R_DECL,1,[Whether strtok_r is declared])
  ])
  AC_MSG_RESULT([done])
])

dnl
dnl PHP_LIBGCC_LIBPATH(gcc)
dnl Stores the location of libgcc in libgcc_libpath
dnl
AC_DEFUN(PHP_LIBGCC_LIBPATH,[
  changequote({,})
  libgcc_libpath=`$1 --print-libgcc-file-name|sed 's%/*[^/][^/]*$%%'`
  changequote([,])
])

AC_DEFUN(PHP_ARG_ANALYZE,[
case [$]$1 in
shared,*)
  ext_output="yes, shared"
  ext_shared=yes
  $1=`echo "[$]$1"|sed 's/^shared,//'`
  ;;
shared)
  ext_output="yes, shared"
  ext_shared=yes
  $1=yes
  ;;
no)
  ext_output=no
  ext_shared=no
  ;;
*)
  ext_output=yes
  ext_shared=no
  ;;
esac

if test "$php_always_shared" = "yes"; then
  ext_output="yes, shared"
  ext_shared=yes
  test "[$]$1" = "no" && $1=yes
fi

if test -n "$2"; then
  AC_MSG_RESULT([$ext_output])
fi
])

dnl
dnl PHP_ARG_WITH(arg-name, check message, help text[, default-val])
dnl Sets PHP_ARG_NAME either to the user value or to the default value.
dnl default-val defaults to no.  This will also set the variable ext_shared,
dnl and will overwrite any previous variable of that name.
dnl
AC_DEFUN(PHP_ARG_WITH,[
PHP_REAL_ARG_WITH([$1],[$2],[$3],[$4],PHP_[]translit($1,a-z0-9-,A-Z0-9_))
])

AC_DEFUN(PHP_REAL_ARG_WITH,[
if test -n "$2"; then
  AC_MSG_CHECKING([$2])
fi
AC_ARG_WITH($1,[$3],$5=[$]withval,$5=ifelse($4,,no,$4))
PHP_ARG_ANALYZE($5,[$2])
])

dnl
dnl PHP_ARG_ENABLE(arg-name, check message, help text[, default-val])
dnl Sets PHP_ARG_NAME either to the user value or to the default value.
dnl default-val defaults to no.  This will also set the variable ext_shared,
dnl and will overwrite any previous variable of that name.
dnl
AC_DEFUN(PHP_ARG_ENABLE,[
PHP_REAL_ARG_ENABLE([$1],[$2],[$3],[$4],PHP_[]translit($1,a-z-,A-Z_))
])

AC_DEFUN(PHP_REAL_ARG_ENABLE,[
if test -n "$2"; then
  AC_MSG_CHECKING([$2])
fi
AC_ARG_ENABLE($1,[$3],$5=[$]enableval,$5=ifelse($4,,no,$4))
PHP_ARG_ANALYZE($5,[$2])
])

AC_DEFUN(PHP_MODULE_PTR,[
  EXTRA_MODULE_PTRS="$EXTRA_MODULE_PTRS $1,"
])
 
AC_DEFUN(PHP_CONFIG_NICE,[
  rm -f $1
  cat >$1<<EOF
#! /bin/sh
#
# Created by configure

EOF

  for var in CFLAGS CXXFLAGS CPPFLAGS LDFLAGS LIBS CC CXX; do
    eval val=\$$var
    if test -n "$val"; then
      echo "$var='$val' \\" >> $1
    fi
  done

  for arg in [$]0 "[$]@"; do
    echo "'[$]arg' \\" >> $1
  done
  echo '"[$]@"' >> $1
  chmod +x $1
])

AC_DEFUN(PHP_TIME_R_TYPE,[
AC_CACHE_CHECK(for type of reentrant time-related functions, ac_cv_time_r_type,[
AC_TRY_RUN([
#include <time.h>

main() {
char buf[27];
struct tm t;
time_t old = 0;
int r, s;

s = gmtime_r(&old, &t);
r = (int) asctime_r(&t, buf, 26);
if (r == s && s == 0) return (0);
return (1);
}
],[
  ac_cv_time_r_type=hpux
],[
  AC_TRY_RUN([
#include <time.h>
main() {
  struct tm t, *s;
  time_t old = 0;
  char buf[27], *p;
  
  s = gmtime_r(&old, &t);
  p = asctime_r(&t, buf, 26);
  if (p == buf && s == &t) return (0);
  return (1);
}
  ],[
    ac_cv_time_r_type=irix
  ],[
    ac_cv_time_r_type=POSIX
  ])
],[
  ac_cv_time_r_type=POSIX
])
])
  case $ac_cv_time_r_type in
  hpux) AC_DEFINE(PHP_HPUX_TIME_R,1,[Whether you have HP-UX 10.x]) ;;
  irix) AC_DEFINE(PHP_IRIX_TIME_R,1,[Whether you have IRIX-style functions]) ;;
  esac
])

AC_DEFUN(PHP_SUBST,[
  PHP_VAR_SUBST="$PHP_VAR_SUBST $1"
])

AC_DEFUN(PHP_SUBST_OLD,[
  PHP_SUBST($1)
  AC_SUBST($1)
])

AC_DEFUN(PHP_FAST_OUTPUT,[
  PHP_FAST_OUTPUT_FILES="$PHP_FAST_OUTPUT_FILES $1"
])

AC_DEFUN(PHP_MKDIR_P_CHECK,[
  AC_CACHE_CHECK(for working mkdir -p, ac_cv_mkdir_p,[
    test -d conftestdir && rm -rf conftestdir
    mkdir -p conftestdir/somedir >/dev/null 2>&1
dnl `mkdir -p' must be quiet about creating existing directories
    mkdir -p conftestdir/somedir >/dev/null 2>&1
    if test "$?" = "0" && test -d conftestdir/somedir; then
      ac_cv_mkdir_p=yes
    else
      ac_cv_mkdir_p=no
    fi
    rm -rf conftestdir
  ])
])

AC_DEFUN(PHP_GEN_CONFIG_VARS,[
  PHP_MKDIR_P_CHECK
  echo creating config_vars.mk
  > config_vars.mk
  for i in $PHP_VAR_SUBST; do
    eval echo "$i = \$$i" | sed 's%#%\\#%g' >> config_vars.mk
  done
])

AC_DEFUN(PHP_GEN_MAKEFILES,[
  $SHELL $srcdir/build/fastgen.sh $srcdir $ac_cv_mkdir_p $BSD_MAKEFILE $1
])

AC_DEFUN(PHP_TM_GMTOFF,[
AC_CACHE_CHECK([for tm_gmtoff in struct tm], ac_cv_struct_tm_gmtoff,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <$ac_cv_struct_tm>], [struct tm tm; tm.tm_gmtoff;],
  ac_cv_struct_tm_gmtoff=yes, ac_cv_struct_tm_gmtoff=no)])

if test "$ac_cv_struct_tm_gmtoff" = yes; then
  AC_DEFINE(HAVE_TM_GMTOFF,1,[whether you have tm_gmtoff in struct tm])
fi
])

dnl PHP_CONFIGURE_PART(MESSAGE)
dnl Idea borrowed from mm
AC_DEFUN(PHP_CONFIGURE_PART,[
  AC_MSG_RESULT()
  AC_MSG_RESULT([${T_MD}$1${T_ME}])
])

AC_DEFUN(PHP_PROG_SENDMAIL,[
AC_PATH_PROG(PROG_SENDMAIL, sendmail,[], $PATH:/usr/bin:/usr/sbin:/usr/etc:/etc:/usr/ucblib:/usr/lib)
if test -n "$PROG_SENDMAIL"; then
  AC_DEFINE(HAVE_SENDMAIL,1,[whether you have sendmail])
fi
])

AC_DEFUN(PHP_RUNPATH_SWITCH,[
dnl check for -R, etc. switch
AC_MSG_CHECKING([if compiler supports -R])
AC_CACHE_VAL(php_cv_cc_dashr,[
	SAVE_LIBS=$LIBS
	LIBS="-R /usr/lib $LIBS"
	AC_TRY_LINK([], [], php_cv_cc_dashr=yes, php_cv_cc_dashr=no)
	LIBS=$SAVE_LIBS])
AC_MSG_RESULT([$php_cv_cc_dashr])
if test $php_cv_cc_dashr = "yes"; then
	ld_runpath_switch=-R
else
	AC_MSG_CHECKING([if compiler supports -Wl,-rpath,])
	AC_CACHE_VAL(php_cv_cc_rpath,[
		SAVE_LIBS=$LIBS
		LIBS="-Wl,-rpath,/usr/lib $LIBS"
		AC_TRY_LINK([], [], php_cv_cc_rpath=yes, php_cv_cc_rpath=no)
		LIBS=$SAVE_LIBS])
	AC_MSG_RESULT([$php_cv_cc_rpath])
	if test $php_cv_cc_rpath = "yes"; then
		ld_runpath_switch=-Wl,-rpath,
	else
		dnl something innocuous
		ld_runpath_switch=-L
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
    AC_DEFINE(HAVE_STRUCT_FLOCK, 1,[whether you have struct flock])
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
  PHP_SYM_FILE=$1
])

dnl
dnl PHP_BUILD_THREAD_SAFE
dnl
AC_DEFUN(PHP_BUILD_THREAD_SAFE,[
  enable_experimental_zts=yes
  if test "$pthreads_working" != "yes"; then
    AC_MSG_ERROR([ZTS currently requires working POSIX threads. We were unable to verify that your system supports Pthreads.])
  fi
])

AC_DEFUN(PHP_REQUIRE_CXX,[
  if test -z "$php_cxx_done"; then
    AC_PROG_CXX
    AC_PROG_CXXCPP
    php_cxx_done=yes
  fi
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
  changequote({,})
  unique=`echo $2|sed 's/[^a-zA-Z0-9]/_/g'`
  changequote([,])
  cmd="echo $ac_n \"\$$1$unique$ac_c\""
  if test -n "$unique" && test "`eval $cmd`" = "" ; then
    eval "$1$unique=set"
    $3
  fi
])

dnl
dnl PHP_EXPAND_PATH(path, variable)
dnl
dnl expands path to an absolute path and assigns it to variable
dnl
AC_DEFUN(PHP_EXPAND_PATH,[
  if test -z "$1" || echo "$1" | grep '^/' >/dev/null ; then
    $2=$1
  else
    changequote({,})
    ep_dir="`echo $1|sed 's%/*[^/][^/]*/*$%%'`"
    changequote([,])
    ep_realdir="`(cd \"$ep_dir\" && pwd)`"
    $2="$ep_realdir/`basename \"$1\"`"
  fi
])

dnl
dnl PHP_ADD_LIBPATH(path[, shared-libadd])
dnl
dnl add a library to linkpath/runpath
dnl
AC_DEFUN(PHP_ADD_LIBPATH,[
  if test "$1" != "/usr/lib"; then
    PHP_EXPAND_PATH($1, ai_p)
    if test "$ext_shared" = "yes" && test -n "$2"; then
      $2="-R$1 -L$1 [$]$2"
    else
      AC_PHP_ONCE(LIBPATH, $ai_p, [
        test -n "$ld_runpath_switch" && LDFLAGS="$LDFLAGS $ld_runpath_switch$ai_p"
        LDFLAGS="$LDFLAGS -L$ai_p"
        PHP_RPATHS="$PHP_RPATHS $ai_p"
      ])
    fi
  fi
])

dnl
dnl PHP_BUILD_RPATH()
dnl
dnl builds RPATH from PHP_RPATHS
dnl
AC_DEFUN(PHP_BUILD_RPATH,[
  if test "$PHP_RPATH" = "yes" && test -n "$PHP_RPATHS"; then
    OLD_RPATHS="$PHP_RPATHS"
    unset PHP_RPATHS
    for i in $OLD_RPATHS; do
      PHP_LDFLAGS="$PHP_LDFLAGS -L$i"
      PHP_RPATHS="$PHP_RPATHS -R $i"
      NATIVE_RPATHS="$NATIVE_RPATHS $ld_runpath_switch$i"
    done
  fi
])

dnl
dnl PHP_ADD_INCLUDE(path [,before])
dnl
dnl add an include path. 
dnl if before is 1, add in the beginning of INCLUDES.
dnl
AC_DEFUN(PHP_ADD_INCLUDE,[
  if test "$1" != "/usr/include"; then
    PHP_EXPAND_PATH($1, ai_p)
    AC_PHP_ONCE(INCLUDEPATH, $ai_p, [
      if test "$2"; then
        INCLUDES="-I$ai_p $INCLUDES"
      else
        INCLUDES="$INCLUDES -I$ai_p"
      fi
    ])
  fi
])

AC_DEFUN(PHP_X_ADD_LIBRARY,[
  ifelse($2,,$3="-l$1 [$]$3", $3="[$]$3 -l$1")
])

dnl
dnl PHP_ADD_LIBRARY(library[, append[, shared-libadd]])
dnl
dnl add a library to the link line
dnl
AC_DEFUN(PHP_ADD_LIBRARY,[
 case $1 in
 c|c_r|pthread*) ;;
 *)
ifelse($3,,[
   PHP_X_ADD_LIBRARY($1,$2,LIBS)
],[
   if test "$ext_shared" = "yes"; then
     PHP_X_ADD_LIBRARY($1,$2,$3)
   else
     PHP_ADD_LIBRARY($1,$2)
   fi
])
  ;;
  esac
])

dnl
dnl PHP_ADD_LIBRARY_DEFER(library[, append[, shared-libadd]])
dnl
dnl add a library to the link line (deferred)
dnl
AC_DEFUN(PHP_ADD_LIBRARY_DEFER,[
 case $1 in
 c|c_r|pthread*) ;;
 *)
ifelse($3,,[
   PHP_X_ADD_LIBRARY($1,$2,DLIBS)
],[
   if test "$ext_shared" = "yes"; then
     PHP_X_ADD_LIBRARY($1,$2,$3)
   else
     PHP_ADD_LIBRARY_DEFER($1,$2)
   fi
])
  ;;
  esac
])
  
dnl
dnl PHP_ADD_LIBRARY_WITH_PATH(library, path[, shared-libadd])
dnl
dnl add a library to the link line and path to linkpath/runpath.
dnl if shared-libadd is not empty and $ext_shared is yes,
dnl shared-libadd will be assigned the library information
dnl
AC_DEFUN(PHP_ADD_LIBRARY_WITH_PATH,[
ifelse($3,,[
  if test -n "$2"; then
    PHP_ADD_LIBPATH($2)
  fi
  PHP_ADD_LIBRARY($1)
],[
  if test "$ext_shared" = "yes"; then
    $3="-l$1 [$]$3"
    if test -n "$2"; then
      PHP_ADD_LIBPATH($2,$3)
    fi
  else
    PHP_ADD_LIBRARY_WITH_PATH($1,$2)
  fi
])
])

dnl
dnl PHP_ADD_LIBRARY_DEFER_WITH_PATH(library, path)
dnl
dnl add a library to the link line (deferred)
dnl and path to linkpath/runpath (not deferred)
dnl
AC_DEFUN(PHP_ADD_LIBRARY_DEFER_WITH_PATH,[
  PHP_ADD_LIBPATH($2)
  PHP_ADD_LIBRARY_DEFER($1)
])

dnl
dnl Set libtool variable
dnl
AC_DEFUN(AM_SET_LIBTOOL_VARIABLE,[
  LIBTOOL='$(SHELL) $(top_builddir)/libtool $1'
])

dnl
dnl Check for cc option
dnl
AC_DEFUN(PHP_CHECK_CC_OPTION,[
  echo "main(){return 0;}" > conftest.$ac_ext
  opt=$1
  changequote({,})
  var=`echo $opt|sed 's/[^a-zA-Z0-9]/_/g'`
  changequote([,])
  AC_MSG_CHECKING([if compiler supports -$1 really])
  ac_php_compile="${CC-cc} -$opt -o conftest $CFLAGS $CPPFLAGS conftest.$ac_ext 2>&1"
  if eval $ac_php_compile 2>&1 | egrep "$opt" > /dev/null 2>&1 ; then
    eval php_cc_$var=no
	AC_MSG_RESULT([no])
  else
    if eval ./conftest 2>/dev/null ; then
      eval php_cc_$var=yes
	  AC_MSG_RESULT([yes])
    else
      eval php_cc_$var=no
	  AC_MSG_RESULT([no])
    fi
  fi
])

AC_DEFUN(PHP_REGEX,[

if test "$REGEX_TYPE" = "php"; then
  REGEX_LIB=regex/libregex.la
  REGEX_DIR=regex
  AC_DEFINE(HSREGEX,1,[ ])
  AC_DEFINE(REGEX,1,[ ])
  PHP_FAST_OUTPUT(regex/Makefile)
elif test "$REGEX_TYPE" = "system"; then
  AC_DEFINE(REGEX,0,[ ])
fi

AC_MSG_CHECKING([which regex library to use])
AC_MSG_RESULT([$REGEX_TYPE])

PHP_SUBST(REGEX_DIR)
PHP_SUBST(REGEX_LIB)
PHP_SUBST(HSREGEX)
])

dnl
dnl See if we have broken header files like SunOS has.
dnl
AC_DEFUN(PHP_MISSING_FCLOSE_DECL,[
  AC_MSG_CHECKING([for fclose declaration])
  AC_TRY_COMPILE([#include <stdio.h>],[int (*func)() = fclose],[
    AC_DEFINE(MISSING_FCLOSE_DECL,0,[ ])
    AC_MSG_RESULT([ok])
  ],[
    AC_DEFINE(MISSING_FCLOSE_DECL,1,[ ])
    AC_MSG_RESULT([missing])
  ])
])

dnl
dnl Check for broken sprintf()
dnl
AC_DEFUN(PHP_AC_BROKEN_SPRINTF,[
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
    AC_DEFINE(PHP_BROKEN_SPRINTF, 1, [ ])
  else
    AC_DEFINE(PHP_BROKEN_SPRINTF, 0, [ ])
  fi
])

dnl
dnl PHP_EXTENSION(extname [, shared [,sapi_class]])
dnl
dnl Includes an extension in the build.
dnl
dnl "extname" is the name of the ext/ subdir where the extension resides
dnl "shared" can be set to "shared" or "yes" to build the extension as
dnl a dynamically loadable library. Optional parameter "sapi_class" can
dnl be set to "cli" to mark extension build only with CLI or CGI sapi's.
dnl If "nocli" is passed the extension will be built only with a non-cli
dnl sapi.
dnl
AC_DEFUN(PHP_EXTENSION,[
  EXT_SUBDIRS="$EXT_SUBDIRS $1"
  
  if test -d "$abs_srcdir/ext/$1"; then
dnl ---------------------------------------------- Internal Module
    ext_builddir=ext/$1
    ext_srcdir=$abs_srcdir/ext/$1
  else
dnl ---------------------------------------------- External Module
    ext_builddir=.
    ext_srcdir=$abs_srcdir
  fi

  if test "$2" != "shared" && test "$2" != "yes" && test "$3" != "cli"; then
dnl ---------------------------------------------- Static module
    LIB_BUILD($ext_builddir)
    EXT_LTLIBS="$EXT_LTLIBS $abs_builddir/$ext_builddir/lib$1.la"
    EXT_STATIC="$EXT_STATIC $1"
    if test "$3" != "nocli"; then
      EXT_CLI_LTLIBS="$EXT_CLI_LTLIBS $abs_builddir/$ext_builddir/lib$1.la"
      EXT_CLI_STATIC="$EXT_CLI_STATIC $1"
    fi
  else
    if test "$2" = "shared" || test "$2" = "yes"; then
dnl ---------------------------------------------- Shared module
      LIB_BUILD($ext_builddir,yes)
      AC_DEFINE_UNQUOTED([COMPILE_DL_]translit($1,a-z-,A-Z_), 1, Whether to build $1 as dynamic module)
    fi
  fi

  if test "$2" != "shared" && test "$2" != "yes" && test "$3" = "cli"; then
dnl ---------------------------------------------- CLI only static module
    LIB_BUILD($ext_builddir)
    if test "$PHP_SAPI" = "cgi"; then
      EXT_LTLIBS="$EXT_LTLIBS $abs_builddir/$ext_builddir/lib$1.la"
      EXT_STATIC="$EXT_STATIC $1"
    fi
    EXT_CLI_LTLIBS="$EXT_CLI_LTLIBS $abs_builddir/$ext_builddir/lib$1.la"
    EXT_CLI_STATIC="$EXT_CLI_STATIC $1"
  fi

  PHP_FAST_OUTPUT($ext_builddir/Makefile)
])

dnl
dnl Solaris requires main code to be position independent in order
dnl to let shared objects find symbols.  Weird.  Ugly.
dnl
dnl Must be run after all --with-NN options that let the user
dnl choose dynamic extensions, and after the gcc test.
dnl
AC_DEFUN(PHP_SOLARIS_PIC_WEIRDNESS,[
  AC_MSG_CHECKING([whether -fPIC is required])
  if test -n "$EXT_SHARED"; then
    os=`uname -sr 2>/dev/null`
    case $os in
        "SunOS 5.6"|"SunOS 5.7")
          case $CC in
	    gcc*|egcs*) CFLAGS="$CFLAGS -fPIC";;
	    *) CFLAGS="$CFLAGS -fpic";;
	  esac
	  AC_MSG_RESULT([yes]);;
	*)
	  AC_MSG_RESULT([no]);;
    esac
  else
    AC_MSG_RESULT([no])
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

AC_DEFUN(PHP_SYS_LFS,
[dnl
  # If available, prefer support for large files unless the user specified
  # one of the CPPFLAGS, LDFLAGS, or LIBS variables.
  AC_MSG_CHECKING([whether large file support needs explicit enabling])
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
  AC_MSG_RESULT([$ac_result])
  case $ac_result in
    yes)
      for ac_shellvar in $ac_shellvars; do
        eval $ac_shellvar=\$ac_test_$ac_shellvar
      done ;;
  esac
])

AC_DEFUN(PHP_SOCKADDR_SA_LEN,[
  AC_CACHE_CHECK([for field sa_len in struct sockaddr],ac_cv_sockaddr_sa_len,[
    AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/socket.h>],
    [struct sockaddr s; s.sa_len;],
    [ac_cv_sockaddr_sa_len=yes
     AC_DEFINE(HAVE_SOCKADDR_SA_LEN,1,[ ])],
    [ac_cv_sockaddr_sa_len=no])
  ])
])


dnl ## PHP_OUTPUT(file)
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

AC_DEFUN(PHP_EBCDIC,[
  AC_CACHE_CHECK([whether system uses EBCDIC],ac_cv_ebcdic,[
  AC_TRY_RUN( [
int main(void) { 
  return (unsigned char)'A' != (unsigned char)0xC1; 
} 
],[
  ac_cv_ebcdic=yes
],[
  ac_cv_ebcdic=no
],[
  ac_cv_ebcdic=no
])])
  if test "$ac_cv_ebcdic" = "yes"; then
    AC_DEFINE(CHARSET_EBCDIC,1, [Define if system uses EBCDIC])
  fi
])

AC_DEFUN(AC_ADD_LIBPATH, [indir([PHP_ADD_LIBPATH], $@)])
AC_DEFUN(AC_ADD_LIBRARY, [indir([PHP_ADD_LIBRARY], $@)])
AC_DEFUN(AC_ADD_LIBRARY_WITH_PATH, [indir([PHP_ADD_LIBRARY_WITH_PATH], $@)])
AC_DEFUN(AC_ADD_INCLUDE, [indir([PHP_ADD_INCLUDE], $@)])

AC_DEFUN(PHP_FOPENCOOKIE,[
	AC_CHECK_FUNC(fopencookie, [ have_glibc_fopencookie=yes ])

	if test "$have_glibc_fopencookie" = "yes" ; then
	  	dnl this comes in two flavors:
      dnl newer glibcs (since 2.1.2 ? )
      dnl have a type called cookie_io_functions_t
		  AC_TRY_COMPILE([ #define _GNU_SOURCE
                       #include <stdio.h>
									   ],
	                   [ cookie_io_functions_t cookie; ],
                     [ have_cookie_io_functions_t=yes ],
										 [] )

		  if test "$have_cookie_io_functions_t" = "yes" ; then
        cookie_io_functions_t=cookie_io_functions_t
	      have_fopen_cookie=yes
      else
  	    dnl older glibc versions (up to 2.1.2 ?)
        dnl call it _IO_cookie_io_functions_t
		    AC_TRY_COMPILE([ #define _GNU_SOURCE
                       #include <stdio.h>
									   ],
	                   [ _IO_cookie_io_functions_t cookie; ],
                     [ have_IO_cookie_io_functions_t=yes ],
										 [] )
		    if test "$have_cookie_io_functions_t" = "yes" ; then
          cookie_io_functions_t=_IO_cookie_io_functions_t
	        have_fopen_cookie=yes
		    fi
			fi

		  if test "$have_fopen_cookie" = "yes" ; then
		    AC_DEFINE(HAVE_FOPENCOOKIE, 1, [ ])
			  AC_DEFINE_UNQUOTED(COOKIE_IO_FUNCTIONS_T, $cookie_io_functions_t, [ ])
      fi      

  	fi
])


dnl
dnl PHP_CHECK_LIBRARY(library, function [, action-found [, action-not-found [, extra-libs]]])
dnl
dnl Wrapper for AC_CHECK_LIB
dnl
AC_DEFUN(PHP_CHECK_LIBRARY, [
  save_old_LDFLAGS=$LDFLAGS
  LDFLAGS="$5 $LDFLAGS"
  AC_CHECK_LIB([$1],[$2],[
    LDFLAGS=$save_old_LDFLAGS
    $3
  ],[
    LDFLAGS=$save_old_LDFLAGS
    $4
  ])dnl
])


dnl 
dnl PHP_SETUP_ICONV(shared-add [, action-found [, action-not-found]])
dnl
dnl Common setup macro for iconv
dnl
AC_DEFUN(PHP_SETUP_ICONV, [
  found_iconv=no
  unset ICONV_DIR

  dnl
  dnl Check libc first if no path is provided in --with-iconv
  dnl
  if test "$PHP_ICONV" = "yes"; then
    AC_CHECK_FUNC(iconv, [
      found_iconv=yes
    ],[
      AC_CHECK_FUNC(libiconv,[
        AC_DEFINE(HAVE_LIBICONV, 1, [ ])
        found_iconv=yes
      ])
    ])
  fi

  dnl
  dnl Check external libs for iconv funcs
  dnl
  if test "$found_iconv" = "no"; then
   
    for i in $PHP_ICONV /usr/local /usr; do
      if test -r $i/include/giconv.h; then
        AC_DEFINE(HAVE_GICONV_H, 1, [ ])
        ICONV_DIR=$i
        iconv_lib_name=giconv
        break
      elif test -r $i/include/iconv.h; then
        ICONV_DIR=$i
        iconv_lib_name=iconv
        break
      fi
    done

    if test -z "$ICONV_DIR"; then
      AC_MSG_ERROR([Please specify the install prefix of iconv with --with-iconv=<DIR>])
    fi
  
    if test -f $ICONV_DIR/lib/lib$iconv_lib_name.a ||
       test -f $ICONV_DIR/lib/lib$iconv_lib_name.$SHLIB_SUFFIX_NAME
    then
      PHP_CHECK_LIBRARY($iconv_lib_name, libiconv, [
        found_iconv=yes
        AC_DEFINE(HAVE_LIBICONV, 1, [ ])
      ], [
        PHP_CHECK_LIBRARY($iconv_lib_name, iconv, [
          found_iconv=yes
        ], [], [
          -L$ICONV_DIR/lib
        ])
      ], [
        -L$ICONV_DIR/lib
      ])
    fi
  fi
  
  if test "$found_iconv" = "yes"; then
    AC_DEFINE(HAVE_ICONV, 1, [ ])
    if test -n "$ICONV_DIR"; then
      PHP_ADD_LIBRARY_WITH_PATH($iconv_lib_name, $ICONV_DIR/lib, $1)
      PHP_ADD_INCLUDE($ICONV_DIR/include)
    fi
    $2
ifelse([$3],[],,[else $3])
  fi
])


dnl
dnl PHP_AP_EXTRACT_VERSION(/path/httpd)
dnl This macro is used to get a comparable
dnl version for apache1/2.
dnl
AC_DEFUN(PHP_AP_EXTRACT_VERSION,[
  ac_output=`$1 -v 2>&1`
  ac_IFS=$IFS
IFS="- /.
"
  set $ac_output
  IFS=$ac_IFS

  APACHE_VERSION=`expr [$]4 \* 1000000 + [$]5 \* 1000 + [$]6`
])
