dnl $Id$
dnl
dnl This file contains local autoconf functions.

dnl PHP_ADD_MAKEFILE_FRAGMENT([srcfile[, ext_srcdir[, ext_builddir]]])
dnl
dnl Processes a file called Makefile.frag in the source directory
dnl of the most recently added extension. $(srcdir) and $(builddir)
dnl are substituted with the proper paths. Can be used to supply
dnl custom rules and/or additional targets.
dnl
AC_DEFUN([PHP_ADD_MAKEFILE_FRAGMENT],[
  ifelse($1,,src=$ext_srcdir/Makefile.frag,src=$1)
  ifelse($2,,ac_srcdir=$ext_srcdir,ac_srcdir=$2)
  ifelse($3,,ac_builddir=$ext_builddir,ac_builddir=$3)
  sed -e "s#\$(srcdir)#$ac_srcdir#g" -e "s#\$(builddir)#$ac_builddir#g" $src  >> Makefile.fragments
])


dnl PHP_DEFINE(WHAT[, value])
dnl
dnl Creates builddir/include/what.h and in there #define WHAT value
dnl
AC_DEFUN([PHP_DEFINE],[
  [echo "#define ]$1[]ifelse([$2],,[ 1],[ $2])[" > include/php_]translit($1,A-Z,a-z)[.h]
])

dnl PHP_INIT_BUILD_SYSTEM
dnl
AC_DEFUN([PHP_INIT_BUILD_SYSTEM],[
test -d include || $php_shtool mkdir include
> Makefile.objects
> Makefile.fragments
dnl We need to play tricks here to avoid matching the egrep line itself
pattern=define
egrep $pattern'.*include/php' $srcdir/configure|sed 's/.*>//'|xargs touch 2>/dev/null
])

dnl PHP_GEN_GLOBAL_MAKEFILE
dnl 
dnl Generates the global makefile.
dnl
AC_DEFUN([PHP_GEN_GLOBAL_MAKEFILE],[
  cat >Makefile <<EOF
srcdir = $abs_srcdir
builddir = $abs_builddir
top_srcdir = $abs_srcdir
top_builddir = $abs_builddir
EOF
  for i in $PHP_VAR_SUBST; do
    eval echo "$i = \$$i" >> Makefile
  done

  cat $abs_srcdir/Makefile.global Makefile.fragments Makefile.objects >> Makefile
])

dnl PHP_ADD_SOURCES(source-path, sources[, special-flags[, type]])
dnl
dnl Adds sources which are located relative to source-path to the 
dnl array of type type.  Sources are processed with optional 
dnl special-flags which are passed to the compiler.  Sources
dnl can be either written in C or C++ (filenames shall end in .c 
dnl or .cpp, respectively).
dnl
dnl Note: If source-path begins with a "/", the "/" is removed and
dnl the path is interpreted relative to the top build-directory.
dnl
dnl which array to append to?
AC_DEFUN([PHP_ADD_SOURCES],[
  PHP_ADD_SOURCES_X($1, $2, $3, ifelse($4,cli,PHP_CLI_OBJS,ifelse($4,sapi,PHP_SAPI_OBJS,PHP_GLOBAL_OBJS)))
])
dnl
dnl _PHP_ASSIGN_BUILD_VARS(type)
dnl internal, don't use
AC_DEFUN([_PHP_ASSIGN_BUILD_VARS],[
ifelse($1,shared,[
  b_c_pre=$shared_c_pre
  b_cxx_pre=$shared_cxx_pre
  b_c_meta=$shared_c_meta
  b_cxx_meta=$shared_cxx_meta
  b_c_post=$shared_c_post
  b_cxx_post=$shared_cxx_post
],[
  b_c_pre=$php_c_pre
  b_cxx_pre=$php_cxx_pre
  b_c_meta=$php_c_meta
  b_cxx_meta=$php_cxx_meta
  b_c_post=$php_c_post
  b_cxx_post=$php_cxx_post
])dnl
  b_lo=[$]$1_lo
])

dnl PHP_ADD_SOURCES_X(source-path, sources[, special-flags[, target-var[, shared[, special-post-flags]]]])
dnl
dnl Additional to PHP_ADD_SOURCES (see above), this lets you set the
dnl name of the array target-var directly, as well as whether
dnl shared objects will be built from the sources.  Should not be 
dnl used directly.
dnl 
AC_DEFUN([PHP_ADD_SOURCES_X],[
dnl relative to source- or build-directory?
dnl ac_srcdir/ac_bdir include trailing slash
  case $1 in
  ""[)] ac_srcdir="$abs_srcdir/"; unset ac_bdir; ac_inc="-I. -I$abs_srcdir" ;;
  /*[)] ac_srcdir=`echo "$1"|cut -c 2-`"/"; ac_bdir=$ac_srcdir; ac_inc="-I$ac_bdir -I$abs_srcdir/$ac_bdir" ;;
  *[)] ac_srcdir="$abs_srcdir/$1/"; ac_bdir="$1/"; ac_inc="-I$ac_bdir -I$ac_srcdir" ;;
  esac
  
dnl how to build .. shared or static?
  ifelse($5,yes,_PHP_ASSIGN_BUILD_VARS(shared),_PHP_ASSIGN_BUILD_VARS(php))

dnl iterate over the sources
  old_IFS=[$]IFS
  for ac_src in $2; do
  
dnl remove the suffix
      IFS=.
      set $ac_src
      ac_obj=[$]1
      IFS=$old_IFS
      
dnl append to the array which has been dynamically chosen at m4 time
      $4="[$]$4 [$]ac_bdir[$]ac_obj.lo"

dnl choose the right compiler/flags/etc. for the source-file
      case $ac_src in
	  *.c[)] ac_comp="$b_c_pre $3 $ac_inc $b_c_meta -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $6$b_c_post" ;;
	  *.cpp[)] ac_comp="$b_cxx_pre $3 $ac_inc $b_cxx_meta -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $6$b_cxx_post" ;;
      esac

dnl create a rule for the object/source combo
	  cat >>Makefile.objects<<EOF
$ac_bdir[$]ac_obj.lo: $ac_srcdir[$]ac_src
	$ac_comp
EOF
  done
])

dnl
dnl Disable building CLI
dnl
AC_DEFUN([PHP_DISABLE_CLI],[
  disable_cli=1
])
	
dnl
dnl Separator into the configure --help display.
dnl 
AC_DEFUN([PHP_HELP_SEPARATOR],[
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
AC_DEFUN([PHP_TARGET_RDYNAMIC],[
  if test -n "$GCC"; then
    dnl we should use a PHP-specific macro here
    TSRM_CHECK_GCC_ARG(-rdynamic, gcc_rdynamic=yes)
    if test "$gcc_rdynamic" = "yes"; then
      PHP_LDFLAGS="$PHP_LDFLAGS -rdynamic"
    fi
  fi
])

AC_DEFUN([PHP_REMOVE_USR_LIB],[
  unset ac_new_flags
  for i in [$]$1; do
    case [$]i in
    -L/usr/lib|-L/usr/lib/) ;;
    *) ac_new_flags="[$]ac_new_flags [$]i" ;;
    esac
  done
  $1=[$]ac_new_flags
])

AC_DEFUN([PHP_SETUP_OPENSSL],[
  if test "$PHP_OPENSSL" = "yes"; then
    PHP_OPENSSL="/usr/local/ssl /usr/local /usr /usr/local/openssl"
  fi

  for i in $PHP_OPENSSL; do
    if test -r $i/include/openssl/evp.h; then
      OPENSSL_INCDIR=$i/include
    fi
    if test -r $i/lib/libssl.a -o -r $i/lib/libssl.$SHLIB_SUFFIX_NAME; then
      OPENSSL_LIBDIR=$i/lib
    fi
  done

  if test -z "$OPENSSL_INCDIR"; then
    AC_MSG_ERROR([Cannot find OpenSSL's <evp.h>])
  fi

  if test -z "$OPENSSL_LIBDIR"; then
    AC_MSG_ERROR([Cannot find OpenSSL's libraries])
  fi

  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$OPENSSL_INCDIR
  AC_MSG_CHECKING([for OpenSSL version])
  AC_EGREP_CPP(yes,[
#include <openssl/opensslv.h>
#if OPENSSL_VERSION_NUMBER >= 0x0090600fL
  yes
#endif
  ],[
    AC_MSG_RESULT([>= 0.9.6])
  ],[
    AC_MSG_ERROR([OpenSSL version 0.9.6 or greater required.])
  ])
  CPPFLAGS=$old_CPPFLAGS

  PHP_ADD_INCLUDE($OPENSSL_INCDIR)
  PHP_ADD_LIBPATH($OPENSSL_LIBDIR)

  PHP_CHECK_LIBRARY(crypto, CRYPTO_free, [
    PHP_ADD_LIBRARY(crypto)
  ],[
    AC_MSG_ERROR([libcrypto not found!])
  ],[
    -L$OPENSSL_LIBDIR
  ])

  PHP_CHECK_LIBRARY(ssl, SSL_CTX_set_ssl_version, [
    PHP_ADD_LIBRARY(ssl)
  ],[
    AC_MSG_ERROR([libssl not found!])
  ],[
    -L$OPENSSL_LIBDIR
  ])

  OPENSSL_INCDIR_OPT=-I$OPENSSL_INCDIR
  AC_SUBST(OPENSSL_INCDIR_OPT)
])

dnl PHP_EVAL_LIBLINE(LINE, SHARED-LIBADD)
dnl
dnl Use this macro, if you need to add libraries and or library search
dnl paths to the PHP build system which are only given in compiler
dnl notation.
dnl
AC_DEFUN([PHP_EVAL_LIBLINE],[
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
AC_DEFUN([PHP_EVAL_INCLINE],[
  for ac_i in $1; do
    case $ac_i in
    -I*)
      ac_ii=`echo $ac_i|cut -c 3-`
      PHP_ADD_INCLUDE($ac_ii)
    ;;
    esac
  done
])

AC_DEFUN([PHP_READDIR_R_TYPE],[
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

AC_DEFUN([PHP_SHLIB_SUFFIX_NAME],[
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

AC_DEFUN([PHP_DEBUG_MACRO],[
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

AC_DEFUN([PHP_DOES_PWRITE_WORK],[
  AC_TRY_RUN([
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
$1
    main() {
    int fd = open("conftest_in", O_WRONLY|O_CREAT, 0600);

    if (fd < 0) exit(1);
    if (pwrite(fd, "text", 4, 0) != 4) exit(1);
    /* Linux glibc breakage until 2.2.5 */
    if (pwrite(fd, "text", 4, -1) != -1 || errno != EINVAL) exit(1);
    exit(0);
    }

  ],[
    ac_cv_pwrite=yes
  ],[
    ac_cv_pwrite=no
  ],[
    ac_cv_pwrite=no
  ])
])

AC_DEFUN([PHP_DOES_PREAD_WORK],[
  echo test > conftest_in
  AC_TRY_RUN([
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
$1
    main() {
    char buf[3]; 
    int fd = open("conftest_in", O_RDONLY);
    if (fd < 0) exit(1);
    if (pread(fd, buf, 2, 0) != 2) exit(1);
    /* Linux glibc breakage until 2.2.5 */
    if (pread(fd, buf, 2, -1) != -1 || errno != EINVAL) exit(1);
    exit(0);
    }
  ],[
    ac_cv_pread=yes
  ],[
    ac_cv_pread=no
  ],[
    ac_cv_pread=no
  ])
  rm -f conftest_in
])

AC_DEFUN([PHP_PWRITE_TEST],[
  AC_CACHE_CHECK(whether pwrite works,ac_cv_pwrite,[
    PHP_DOES_PWRITE_WORK
    if test "$ac_cv_pwrite" = "no"; then
      PHP_DOES_PWRITE_WORK([ssize_t pwrite(int, void *, size_t, off64_t);])
      if test "$ac_cv_pwrite" = "yes"; then
        ac_cv_pwrite=64
      fi
    fi
  ])

  if test "$ac_cv_pwrite" != "no"; then
    AC_DEFINE(HAVE_PWRITE, 1, [ ])
    if test "$ac_cv_pwrite" = "64"; then
      AC_DEFINE(PHP_PWRITE_64, 1, [whether pwrite64 is default])
    fi
  fi  
])

AC_DEFUN([PHP_PREAD_TEST],[
  AC_CACHE_CHECK(whether pread works,ac_cv_pread,[
    PHP_DOES_PREAD_WORK
    if test "$ac_cv_pread" = "no"; then
      PHP_DOES_PREAD_WORK([ssize_t pread(int, void *, size_t, off64_t);])
      if test "$ac_cv_pread" = "yes"; then
        ac_cv_pread=64
      fi
    fi
  ])

  if test "$ac_cv_pread" != "no"; then
    AC_DEFINE(HAVE_PREAD, 1, [ ])
    if test "$ac_cv_pread" = "64"; then
      AC_DEFINE(PHP_PREAD_64, 1, [whether pread64 is default])
    fi
  fi  
])

AC_DEFUN([PHP_MISSING_TIME_R_DECL],[
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
AC_DEFUN([PHP_LIBGCC_LIBPATH],[
  changequote({,})
  libgcc_libpath=`$1 --print-libgcc-file-name|sed 's%/*[^/][^/]*$%%'`
  changequote([,])
])

AC_DEFUN([PHP_ARG_ANALYZE_EX],[
ext_output="yes, shared"
ext_shared=yes
case [$]$1 in
shared,*)
  $1=`echo "[$]$1"|sed 's/^shared,//'`
  ;;
shared)
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

PHP_ALWAYS_SHARED([$1])
])

AC_DEFUN([PHP_ARG_ANALYZE],[
ifelse([$3],yes,[PHP_ARG_ANALYZE_EX([$1])])
ifelse([$2],,,[AC_MSG_RESULT([$ext_output])])
])

dnl
dnl PHP_ARG_WITH(arg-name, check message, help text[, default-val[, extension-or-not]])
dnl Sets PHP_ARG_NAME either to the user value or to the default value.
dnl default-val defaults to no.  This will also set the variable ext_shared,
dnl and will overwrite any previous variable of that name.
dnl If extension-or-not is yes (default), then do the ENABLE_ALL check and run
dnl the PHP_ARG_ANALYZE_EX.
dnl
AC_DEFUN([PHP_ARG_WITH],[
PHP_REAL_ARG_WITH([$1],[$2],[$3],[$4],PHP_[]translit($1,a-z0-9-,A-Z0-9_),[ifelse($5,,yes,$5)])
])

AC_DEFUN([PHP_REAL_ARG_WITH],[
ifelse([$2],,,[AC_MSG_CHECKING([$2])])
AC_ARG_WITH($1,[$3],$5=[$]withval,
[
  $5=ifelse($4,,no,$4)

  if test "$PHP_ENABLE_ALL" && test "$6" = "yes"; then
    $5=$PHP_ENABLE_ALL
  fi
])
PHP_ARG_ANALYZE($5,[$2],$6)
])

dnl
dnl PHP_ARG_ENABLE(arg-name, check message, help text[, default-val[, extension-or-not]])
dnl Sets PHP_ARG_NAME either to the user value or to the default value.
dnl default-val defaults to no.  This will also set the variable ext_shared,
dnl and will overwrite any previous variable of that name.
dnl If extension-or-not is yes (default), then do the ENABLE_ALL check and run
dnl the PHP_ARG_ANALYZE_EX.
dnl
AC_DEFUN([PHP_ARG_ENABLE],[
PHP_REAL_ARG_ENABLE([$1],[$2],[$3],[$4],PHP_[]translit($1,a-z-,A-Z_),[ifelse($5,,yes,$5)])
])

AC_DEFUN([PHP_REAL_ARG_ENABLE],[
ifelse([$2],,,[AC_MSG_CHECKING([$2])])
AC_ARG_ENABLE($1,[$3],$5=[$]enableval,
[
  $5=ifelse($4,,no,$4)

  if test "$PHP_ENABLE_ALL" && test "$6" = "yes"; then
    $5=$PHP_ENABLE_ALL
  fi
])
PHP_ARG_ANALYZE($5,[$2],$6)
])

AC_DEFUN([PHP_MODULE_PTR],[
  EXTRA_MODULE_PTRS="$EXTRA_MODULE_PTRS $1,"
])
 
AC_DEFUN([PHP_CONFIG_NICE],[
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

AC_DEFUN([PHP_TIME_R_TYPE],[
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

AC_DEFUN([PHP_SUBST],[
  PHP_VAR_SUBST="$PHP_VAR_SUBST $1"
])

AC_DEFUN([PHP_SUBST_OLD],[
  PHP_SUBST($1)
  AC_SUBST($1)
])

AC_DEFUN([PHP_TM_GMTOFF],[
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
AC_DEFUN([PHP_CONFIGURE_PART],[
  AC_MSG_RESULT()
  AC_MSG_RESULT([${T_MD}$1${T_ME}])
])

AC_DEFUN([PHP_PROG_SENDMAIL],[
AC_PATH_PROG(PROG_SENDMAIL, sendmail,[], $PATH:/usr/bin:/usr/sbin:/usr/etc:/etc:/usr/ucblib:/usr/lib)
if test -n "$PROG_SENDMAIL"; then
  AC_DEFINE(HAVE_SENDMAIL,1,[whether you have sendmail])
fi
])

AC_DEFUN([PHP_RUNPATH_SWITCH],[
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

AC_DEFUN([PHP_STRUCT_FLOCK],[
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

AC_DEFUN([PHP_SOCKLEN_T],[
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
AC_DEFUN([PHP_SET_SYM_FILE],
[
  PHP_SYM_FILE=$1
])

dnl
dnl PHP_BUILD_THREAD_SAFE
dnl
AC_DEFUN([PHP_BUILD_THREAD_SAFE],[
  enable_experimental_zts=yes
  if test "$pthreads_working" != "yes"; then
    AC_MSG_ERROR([ZTS currently requires working POSIX threads. We were unable to verify that your system supports Pthreads.])
  fi
])

AC_DEFUN([PHP_REQUIRE_CXX],[
  if test -z "$php_cxx_done"; then
    AC_PROG_CXX
    AC_PROG_CXXCPP
    php_cxx_done=yes
  fi
])

dnl
dnl PHP_BUILD_SHARED
dnl
AC_DEFUN([PHP_BUILD_SHARED],[
  PHP_BUILD_PROGRAM
  OVERALL_TARGET=libphp4.la
  php_build_target=shared
  
  php_c_pre=$shared_c_pre
  php_c_meta=$shared_c_meta
  php_c_post=$shared_c_post
  php_cxx_pre=$shared_cxx_pre
  php_cxx_meta=$shared_cxx_meta
  php_cxx_post=$shared_cxx_post
  php_lo=$shared_lo
])

dnl
dnl PHP_BUILD_STATIC
dnl
AC_DEFUN([PHP_BUILD_STATIC],[
  PHP_BUILD_PROGRAM
  OVERALL_TARGET=libphp4.la
  php_build_target=static
])

dnl
dnl PHP_BUILD_BUNDLE
dnl
AC_DEFUN([PHP_BUILD_BUNDLE],[
  PHP_BUILD_PROGRAM
  OVERALL_TARGET=libs/libphp4.bundle
  php_build_target=static
])

dnl
dnl PHP_BUILD_PROGRAM
dnl
AC_DEFUN([PHP_BUILD_PROGRAM],[
  OVERALL_TARGET=[]ifelse($1,,php,$1)
  php_c_pre='$(CC)'
  php_c_meta='$(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)'
  php_c_post=' && echo > $[@]'
  php_cxx_pre='$(CXX)'
  php_cxx_meta='$(COMMON_FLAGS) $(CXXFLAGS_CLEAN) $(EXTRA_CXXFLAGS)'
  php_cxx_post=' && echo > $[@]'
  php_lo=o
  
  shared_c_pre='$(LIBTOOL) --mode=compile $(CC)'
  shared_c_meta='$(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) -prefer-pic'
  shared_c_post=
  shared_cxx_pre='$(LIBTOOL) --mode=compile $(CXX)'
  shared_cxx_meta='$(COMMON_FLAGS) $(CXXFLAGS_CLEAN) $(EXTRA_CXXFLAGS) -prefer-pic'
  shared_cxx_post=
  shared_lo=lo

  php_build_target=program
])

dnl
dnl PHP_RUN_ONCE(namespace, variable, code)
dnl
dnl execute code, if variable is not set in namespace
dnl
AC_DEFUN([PHP_RUN_ONCE],[
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
AC_DEFUN([PHP_EXPAND_PATH],[
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
dnl internal, don't use
AC_DEFUN([_PHP_ADD_LIBPATH_GLOBAL],[
  PHP_RUN_ONCE(LIBPATH, $1, [
    test -n "$ld_runpath_switch" && LDFLAGS="$LDFLAGS $ld_runpath_switch$1"
    LDFLAGS="$LDFLAGS -L$1"
    PHP_RPATHS="$PHP_RPATHS $1"
  ])
])dnl
dnl
dnl
dnl
dnl PHP_ADD_LIBPATH(path[, shared-libadd])
dnl
dnl add a library to linkpath/runpath
dnl
AC_DEFUN([PHP_ADD_LIBPATH],[
  if test "$1" != "/usr/lib"; then
    PHP_EXPAND_PATH($1, ai_p)
    ifelse([$2],,[
      _PHP_ADD_LIBPATH_GLOBAL([$ai_p])
    ],[
      if test "$ext_shared" = "yes"; then
        $2="$ld_runpath_switch$ai_p -L$ai_p [$]$2"
      else
        _PHP_ADD_LIBPATH_GLOBAL([$ai_p])
      fi
    ])
  fi
])

dnl
dnl PHP_UTILIZE_RPATHS()
dnl
dnl builds RPATHS/LDFLAGS from PHP_RPATHS
dnl
AC_DEFUN([PHP_UTILIZE_RPATHS],[
  OLD_RPATHS=$PHP_RPATHS
  unset PHP_RPATHS

  for i in $OLD_RPATHS; do
dnl Can be passed to native cc/libtool
    PHP_LDFLAGS="$PHP_LDFLAGS -L$i"
dnl Libtool-specific
    PHP_RPATHS="$PHP_RPATHS -R $i"
dnl cc-specific
    NATIVE_RPATHS="$NATIVE_RPATHS $ld_runpath_switch$i"
  done

  if test "$PHP_RPATH" = "no"; then
    unset PHP_RPATHS
    unset NATIVE_RPATHS
  fi
])

dnl
dnl PHP_ADD_INCLUDE(path [,before])
dnl
dnl add an include path. 
dnl if before is 1, add in the beginning of INCLUDES.
dnl
AC_DEFUN([PHP_ADD_INCLUDE],[
  if test "$1" != "/usr/include"; then
    PHP_EXPAND_PATH($1, ai_p)
    PHP_RUN_ONCE(INCLUDEPATH, $ai_p, [
      if test "$2"; then
        INCLUDES="-I$ai_p $INCLUDES"
      else
        INCLUDES="$INCLUDES -I$ai_p"
      fi
    ])
  fi
])
dnl
dnl internal, don't use
AC_DEFUN([_PHP_X_ADD_LIBRARY],[dnl
  ifelse([$2],,$3="-l$1 [$]$3", $3="[$]$3 -l$1") dnl
])dnl
dnl
dnl internal, don't use
AC_DEFUN([_PHP_ADD_LIBRARY_SKELETON],[
  case $1 in
  c|c_r|pthread*[)] ;;
  *[)] ifelse($3,,[
    _PHP_X_ADD_LIBRARY($1,$2,$5)
  ],[
    if test "$ext_shared" = "yes"; then
      _PHP_X_ADD_LIBRARY($1,$2,$3)
    else
      $4($1,$2)
    fi
  ]) ;;
  esac
])dnl
dnl
dnl
dnl
dnl PHP_ADD_LIBRARY(library[, append[, shared-libadd]])
dnl
dnl add a library to the link line
dnl
AC_DEFUN([PHP_ADD_LIBRARY],[
  _PHP_ADD_LIBRARY_SKELETON([$1],[$2],[$3],[PHP_ADD_LIBRARY],[LIBS])
])

dnl
dnl PHP_ADD_LIBRARY_DEFER(library[, append[, shared-libadd]])
dnl
dnl add a library to the link line (deferred)
dnl
AC_DEFUN([PHP_ADD_LIBRARY_DEFER],[
  _PHP_ADD_LIBRARY_SKELETON([$1],[$2],[$3],[PHP_ADD_LIBRARY_DEFER],[DLIBS])
])

dnl
dnl PHP_ADD_LIBRARY_WITH_PATH(library, path[, shared-libadd])
dnl
dnl add a library to the link line and path to linkpath/runpath.
dnl if shared-libadd is not empty and $ext_shared is yes,
dnl shared-libadd will be assigned the library information
dnl
AC_DEFUN([PHP_ADD_LIBRARY_WITH_PATH],[
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
dnl PHP_ADD_LIBRARY_DEFER_WITH_PATH(library, path[, shared-libadd])
dnl
dnl add a library to the link line (deferred)
dnl and path to linkpath/runpath (not deferred)
dnl if shared-libadd is not empty and $ext_shared is yes,
dnl shared-libadd will be assigned the library information
dnl
AC_DEFUN([PHP_ADD_LIBRARY_DEFER_WITH_PATH],[
ifelse($3,,[
  if test -n "$2"; then
    PHP_ADD_LIBPATH($2)
  fi
  PHP_ADD_LIBRARY_DEFER($1)
],[
  if test "$ext_shared" = "yes"; then
    $3="-l$1 [$]$3"
    if test -n "$2"; then
      PHP_ADD_LIBPATH($2,$3)
    fi
  else
    PHP_ADD_LIBRARY_DEFER_WITH_PATH($1,$2)
  fi
])
])

dnl
dnl PHP_ADD_FRAMEWORK(framework [,before])
dnl
dnl add a (Darwin / Mac OS X) framework to the link
dnl line. if before is 1, the framework is added
dnl to the beginning of the line.

AC_DEFUN([PHP_ADD_FRAMEWORK], [
  PHP_RUN_ONCE(FRAMEWORKS, $1, [
    if test "$2"; then
      PHP_FRAMEWORKS="-framework $1 $PHP_FRAMEWORKS"
    else
      PHP_FRAMEWORKS="$PHP_FRAMEWORKS -framework $1"
    fi
  ])
])

dnl
dnl PHP_ADD_FRAMEWORKPATH(path [,before])
dnl
dnl add a (Darwin / Mac OS X) framework path to the link
dnl and include lines. default paths include (but are
dnl not limited to) /Local/Library/Frameworks and
dnl /System/Library/Frameworks, so these don't need
dnl to be specifically added. if before is 1, the
dnl framework path is added to the beginning of the
dnl relevant lines.

AC_DEFUN([PHP_ADD_FRAMEWORKPATH], [
  PHP_EXPAND_PATH($1, ai_p)
  PHP_RUN_ONCE(FRAMEWORKPATH, $ai_p, [
    if test "$2"; then
      PHP_FRAMEWORKPATH="-F$ai_p $PHP_FRAMEWORKPATH"
    else
      PHP_FRAMEWORKPATH="$PHP_FRAMEWORKPATH -F$ai_p"
    fi
  ])
])

dnl
dnl PHP_ADD_FRAMEWORK_WITH_PATH(framework, path)
dnl
dnl add a (Darwin / Mac OS X) framework path and the
dnl framework itself to the link and include lines.
AC_DEFUN([PHP_ADD_FRAMEWORK_WITH_PATH], [
  PHP_ADD_FRAMEWORKPATH($2)
  PHP_ADD_FRAMEWORK($1)
])

dnl
dnl Set libtool variable
dnl
AC_DEFUN([PHP_SET_LIBTOOL_VARIABLE],[
  if test -z "$LIBTOOL"; then
    LIBTOOL='$(SHELL) $(top_builddir)/libtool $1'
  else
    LIBTOOL="$LIBTOOL $1"
  fi
])

dnl
dnl Check for cc option
dnl
AC_DEFUN([PHP_CHECK_CC_OPTION],[
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

AC_DEFUN([PHP_REGEX],[

if test "$REGEX_TYPE" = "php"; then
  AC_DEFINE(HSREGEX,1,[ ])
  AC_DEFINE(REGEX,1,[ ])
  PHP_ADD_SOURCES(regex, regcomp.c regexec.c regerror.c regfree.c)
elif test "$REGEX_TYPE" = "system"; then
  AC_DEFINE(REGEX,0,[ ])
fi

AC_MSG_CHECKING([which regex library to use])
AC_MSG_RESULT([$REGEX_TYPE])
])

dnl
dnl See if we have broken header files like SunOS has.
dnl
AC_DEFUN([PHP_MISSING_FCLOSE_DECL],[
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
dnl Check for broken sprintf(), C99 conformance
dnl
AC_DEFUN([PHP_AC_BROKEN_SPRINTF],[
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
    AC_DEFINE(PHP_BROKEN_SPRINTF, 1, [Whether sprintf is C99 conform])
  else
    AC_DEFINE(PHP_BROKEN_SPRINTF, 0, [Whether sprintf is C99 conform])
  fi
])

dnl
dnl Check for broken snprintf(), C99 conformance
dnl
AC_DEFUN([PHP_AC_BROKEN_SNPRINTF],[
  AC_CACHE_CHECK(whether snprintf is broken, ac_cv_broken_snprintf,[
    AC_TRY_RUN([
#define NULL (0L)
main() {
	char buf[20];
	int res = 0;
	res = res || (snprintf(buf, 2, "marcus") != 6); 
	res = res || (buf[1] != '\0');
	res = res || (snprintf(buf, 0, "boerger") != 7);
	res = res || (buf[0] != 'm');
	res = res || (snprintf(NULL, 0, "boerger") != 7);
	res = res || (snprintf(buf, sizeof(buf), "%f", 0.12345678) != 8);
	exit(res); 
}
    ],[
      ac_cv_broken_snprintf=no
    ],[
      ac_cv_broken_snprintf=yes
    ],[
      ac_cv_broken_snprintf=no
    ])
  ])
  if test "$ac_cv_broken_snprintf" = "yes"; then
    AC_DEFINE(PHP_BROKEN_SNPRINTF, 1, [Whether snprintf is C99 conform])
  else
    AC_DEFINE(PHP_BROKEN_SNPRINTF, 0, [Whether snprintf is C99 conform])
  fi
])

dnl PHP_SHARED_MODULE(module-name, object-var, build-dir)
dnl
dnl Basically sets up the link-stage for building module-name
dnl from object_var in build-dir.
dnl
AC_DEFUN([PHP_SHARED_MODULE],[
  PHP_MODULES="$PHP_MODULES \$(phplibdir)/$1.la"
  PHP_SUBST($2)
  cat >>Makefile.objects<<EOF
\$(phplibdir)/$1.la: $3/$1.la
	\$(LIBTOOL) --mode=install cp $3/$1.la \$(phplibdir)

$3/$1.la: \$($2) \$(translit($1,a-z-,A-Z_)_SHARED_DEPENDENCIES)
	\$(LIBTOOL) --mode=link \$(CC) \$(COMMON_FLAGS) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(LDFLAGS) -o \[$]@ -export-dynamic -avoid-version -prefer-pic -module -rpath \$(phplibdir) \$(EXTRA_LDFLAGS) \$($2) \$(translit($1,a-z-,A-Z_)_SHARED_LIBADD)

EOF
])

dnl
dnl PHP_SELECT_SAPI(name, type[, sources [, extra-cflags [, build-target]]])
dnl
dnl Selects the SAPI name and type (static, shared, programm)
dnl and optionally also the source-files for the SAPI-specific
dnl objects.
dnl
AC_DEFUN([PHP_SELECT_SAPI],[
  PHP_SAPI=$1
  
  case "$2" in
  static) PHP_BUILD_STATIC;;
  shared) PHP_BUILD_SHARED;;
  bundle) PHP_BUILD_BUNDLE;;
  program) PHP_BUILD_PROGRAM($5);;
  esac
    
  ifelse($3,,,[PHP_ADD_SOURCES([sapi/$1],[$3],[$4],[sapi])])
])

dnl deprecated
AC_DEFUN([PHP_EXTENSION],[
  sources=`awk -f $abs_srcdir/scan_makefile_in.awk < []PHP_EXT_SRCDIR($1)[]/Makefile.in`

  PHP_NEW_EXTENSION($1, $sources, $2, $3)

  if test -r "$ext_srcdir/Makefile.frag"; then
    PHP_ADD_MAKEFILE_FRAGMENT
  fi
])

AC_DEFUN([PHP_ADD_BUILD_DIR],[
  BUILD_DIR="$BUILD_DIR $1"
])

AC_DEFUN([PHP_GEN_BUILD_DIRS],[
  $php_shtool mkdir -p $BUILD_DIR
])

dnl
dnl PHP_NEW_EXTENSION(extname, sources [, shared [,sapi_class[, extra-cflags]]])
dnl
dnl Includes an extension in the build.
dnl
dnl "extname" is the name of the ext/ subdir where the extension resides.
dnl "sources" is a list of files relative to the subdir which are used
dnl to build the extension.
dnl "shared" can be set to "shared" or "yes" to build the extension as
dnl a dynamically loadable library. Optional parameter "sapi_class" can
dnl be set to "cli" to mark extension build only with CLI or CGI sapi's.
dnl extra-cflags are passed to the compiler, with @ext_srcdir@ being
dnl substituted.
AC_DEFUN([PHP_NEW_EXTENSION],[
  ext_builddir=[]PHP_EXT_BUILDDIR($1)
  ext_srcdir=[]PHP_EXT_SRCDIR($1)

  ifelse($5,,ac_extra=,[ac_extra=`echo "$5"|sed s#@ext_srcdir@#$ext_srcdir#g`])

  if test "$3" != "shared" && test "$3" != "yes" && test "$4" != "cli"; then
dnl ---------------------------------------------- Static module

    PHP_ADD_SOURCES(PHP_EXT_DIR($1),$2,$ac_extra,)
    EXT_STATIC="$EXT_STATIC $1"
    if test "$3" != "nocli"; then
      EXT_CLI_STATIC="$EXT_CLI_STATIC $1"
    fi
  else
    if test "$3" = "shared" || test "$3" = "yes"; then
dnl ---------------------------------------------- Shared module
      PHP_ADD_SOURCES_X(PHP_EXT_DIR($1),$2,$ac_extra,shared_objects_$1,yes)
      PHP_SHARED_MODULE($1,shared_objects_$1, $ext_builddir)
      AC_DEFINE_UNQUOTED([COMPILE_DL_]translit($1,a-z_-,A-Z__), 1, Whether to build $1 as dynamic module)
    fi
  fi

  if test "$3" != "shared" && test "$3" != "yes" && test "$4" = "cli"; then
dnl ---------------------------------------------- CLI static module
    if test "$PHP_SAPI" = "cgi"; then
      PHP_ADD_SOURCES(PHP_EXT_DIR($1),$2,$ac_extra,)
      EXT_STATIC="$EXT_STATIC $1"
    else
      PHP_ADD_SOURCES(PHP_EXT_DIR($1),$2,$ac_extra,cli)
    fi
    EXT_CLI_STATIC="$EXT_CLI_STATIC $1"
  fi
  PHP_ADD_BUILD_DIR($ext_builddir)
])

dnl
dnl Solaris requires main code to be position independent in order
dnl to let shared objects find symbols.  Weird.  Ugly.
dnl
dnl Must be run after all --with-NN options that let the user
dnl choose dynamic extensions, and after the gcc test.
dnl
AC_DEFUN([PHP_SOLARIS_PIC_WEIRDNESS],[
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
AC_DEFUN([PHP_WITH_SHARED],[
  PHP_ARG_ANALYZE_EX(withval)
  shared=$ext_shared
  unset ext_shared ext_output
])

dnl The problem is that the default compilation flags in Solaris 2.6 won't
dnl let programs access large files;  you need to tell the compiler that
dnl you actually want your programs to work on large files.  For more
dnl details about this brain damage please see:
dnl http://www.sas.com/standards/large.file/x_open.20Mar96.html

dnl Written by Paul Eggert <eggert@twinsun.com>.

AC_DEFUN([PHP_SYS_LFS],
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

AC_DEFUN([PHP_SOCKADDR_SA_LEN],[
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
AC_DEFUN([PHP_OUTPUT],[
  PHP_OUTPUT_FILES="$PHP_OUTPUT_FILES $1"
])

AC_DEFUN([PHP_DECLARED_TIMEZONE],[
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

AC_DEFUN([PHP_EBCDIC],[
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

dnl Some systems, notably Solaris, cause getcwd() or realpath to fail if a
dnl component of the path has execute but not read permissions
AC_DEFUN([PHP_BROKEN_GETCWD],[
  AC_MSG_CHECKING([for broken getcwd])
  os=`uname -sr 2>/dev/null`
  case $os in
    SunOS*)
	  AC_DEFINE(HAVE_BROKEN_GETCWD,1, [Define if system has broken getcwd])
	  AC_MSG_RESULT([yes]);;
	*)
	  AC_MSG_RESULT([no]);;
  esac
])

AC_DEFUN([PHP_BROKEN_GLIBC_FOPEN_APPEND],[
  AC_MSG_CHECKING([for broken libc stdio])
  AC_CACHE_VAL(have_broken_glibc_fopen_append,[
  AC_TRY_RUN([
#include <stdio.h>
int main(int argc, char *argv[])
{
  FILE *fp;
  long position;
  char *filename = "/tmp/phpglibccheck";
  
  fp = fopen(filename, "w");
  if (fp == NULL) {
	  perror("fopen");
	  exit(2);
  }
  fputs("foobar", fp);
  fclose(fp);

  fp = fopen(filename, "a+");
  position = ftell(fp);
  fclose(fp);
  unlink(filename);
  if (position == 0)
	return 1;
  return 0;
}
],
[have_broken_glibc_fopen_append=no],
[have_broken_glibc_fopen_append=yes ],
AC_TRY_COMPILE([
#include <features.h>
],[
#if !__GLIBC_PREREQ(2,2)
choke me
#endif
],
[have_broken_glibc_fopen_append=yes],
[have_broken_glibc_fopen_append=no ])
)])

  if test "$have_broken_glibc_fopen_append" = "yes"; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(HAVE_BROKEN_GLIBC_FOPEN_APPEND,1, [Define if your glibc borks on fopen with mode a+])
  else
	AC_MSG_RESULT(no)
  fi
])


AC_DEFUN([PHP_FOPENCOOKIE],[
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

		dnl even newer glibcs have a different seeker definition...

		AC_TRY_RUN([
#define _GNU_SOURCE
#include <stdio.h>

struct cookiedata {
	__off64_t pos;
};

__ssize_t reader(void *cookie, char *buffer, size_t size)
{ return size; }
__ssize_t writer(void *cookie, const char *buffer, size_t size)
{ return size; }
int closer(void *cookie)
{ return 0; }
int seeker(void *cookie, __off64_t *position, int whence)
{ ((struct cookiedata*)cookie)->pos = *position; return 0; }

cookie_io_functions_t funcs = {reader, writer, seeker, closer};

main() {
  struct cookiedata g = { 0 };
  FILE *fp = fopencookie(&g, "r", funcs);

  if (fp && fseek(fp, 8192, SEEK_SET) == 0 && g.pos == 8192)
	  exit(0);
  exit(1);
}

					   ],
					   [ cookie_io_functions_use_off64_t=yes ],
					   [ ] )
		
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
		if test "$cookie_io_functions_use_off64_t" = "yes" ; then
          AC_DEFINE(COOKIE_SEEKER_USES_OFF64_T, 1, [ ])
		fi
      fi      

  	fi
])


dnl
dnl PHP_CHECK_LIBRARY(library, function [, action-found [, action-not-found [, extra-libs]]])
dnl
dnl Wrapper for AC_CHECK_LIB
dnl
AC_DEFUN([PHP_CHECK_LIBRARY], [
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
dnl PHP_CHECK_FRAMEWORK(framework, function [, action-found [, action-not-found ]])
dnl
dnl El cheapo wrapper for AC_CHECK_LIB
dnl
AC_DEFUN([PHP_CHECK_FRAMEWORK], [
  save_old_LDFLAGS=$LDFLAGS
  LDFLAGS="-framework $1 $LDFLAGS"
  dnl supplying "c" to AC_CHECK_LIB is technically cheating, but
  dnl rewriting AC_CHECK_LIB is overkill and this only affects
  dnl the "checking.." output anyway.
  AC_CHECK_LIB(c,[$2],[
    LDFLAGS=$save_old_LDFLAGS
    $3
  ],[
    LDFLAGS=$save_old_LDFLAGS
    $4
  ])
])

dnl 
dnl PHP_SETUP_ICONV(shared-add [, action-found [, action-not-found]])
dnl
dnl Common setup macro for iconv
dnl
AC_DEFUN([PHP_SETUP_ICONV], [
  found_iconv=no
  unset ICONV_DIR

  dnl
  dnl Check libc first if no path is provided in --with-iconv
  dnl
  if test "$PHP_ICONV" = "yes"; then
    AC_CHECK_FUNC(iconv, [
      PHP_DEFINE(HAVE_ICONV)
      found_iconv=yes
    ],[
      AC_CHECK_FUNC(libiconv,[
        PHP_DEFINE(HAVE_LIBICONV)
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
        PHP_DEFINE(HAVE_LIBICONV)
      ], [
        PHP_CHECK_LIBRARY($iconv_lib_name, iconv, [
          found_iconv=yes
          PHP_DEFINE(HAVE_ICONV)
        ], [], [
          -L$ICONV_DIR/lib
        ])
      ], [
        -L$ICONV_DIR/lib
      ])
    fi
  fi

  if test "$found_iconv" = "yes"; then
    if test -n "$ICONV_DIR"; then
      AC_DEFINE(HAVE_ICONV, 1, [ ])
      PHP_ADD_LIBRARY_WITH_PATH($iconv_lib_name, $ICONV_DIR/lib, $1)
      PHP_ADD_INCLUDE($ICONV_DIR/include)
    fi
    $2
ifelse([$3],[],,[else $3])
  fi
])

AC_DEFUN([PHP_DEF_HAVE],[AC_DEFINE([HAVE_]translit($1,a-z_-,A-Z__), 1, [ ])])

dnl
dnl PHP_CHECK_FUNC_LIB(func, libs)
dnl This macro checks whether 'func' or '__func' exists
dnl in the specified library.
dnl Defines HAVE_func and HAVE_library if found and adds the library to LIBS.
dnl This should be called in the ACTION-IF-NOT-FOUND part of PHP_CHECK_FUNC
dnl

dnl autoconf undefines the builtin "shift" :-(
dnl If possible, we use the builtin shift anyway, otherwise we use
dnl the ubercool definition I have tested so far with FreeBSD/GNU m4
ifdef([builtin],[builtin(define, phpshift, [builtin(shift, $@)])],[
define([phpshift],[ifelse(index([$@],[,]),-1,,[substr([$@],incr(index([$@],[,])))])])
])

AC_DEFUN([PHP_CHECK_FUNC_LIB],[
  ifelse($2,,:,[
  unset ac_cv_lib_$2[]_$1
  unset ac_cv_lib_$2[]___$1
  unset found
  AC_CHECK_LIB($2, $1, [found=yes], [
    AC_CHECK_LIB($2, __$1, [found=yes], [found=no])
  ])

  if test "$found" = "yes"; then
    ac_libs=$LIBS
    LIBS="$LIBS -l$2"
    AC_TRY_RUN([main() { return (0); }],[found=yes],[found=no],[found=no])
    LIBS=$ac_libs
  fi

  if test "$found" = "yes"; then
    PHP_ADD_LIBRARY($2)
    PHP_DEF_HAVE($1)
    PHP_DEF_HAVE(lib$2)
    ac_cv_func_$1=yes
  else
    PHP_CHECK_FUNC_LIB($1,phpshift(phpshift($@)))
  fi
  ])
])

dnl
dnl PHP_CHECK_FUNC(func, ...)
dnl This macro checks whether 'func' or '__func' exists
dnl in the default libraries and as a fall back in the specified library.
dnl Defines HAVE_func and HAVE_library if found and adds the library to LIBS.
dnl
AC_DEFUN([PHP_CHECK_FUNC],[
  unset ac_cv_func_$1
  unset ac_cv_func___$1
  unset found
  
  AC_CHECK_FUNC($1, [found=yes],[ AC_CHECK_FUNC(__$1,[found=yes],[found=no]) ])

  case $found in
  yes) 
    PHP_DEF_HAVE($1)
    ac_cv_func_$1=yes
  ;;
  ifelse($#,1,,[
    *) PHP_CHECK_FUNC_LIB($@) ;;
  ])
  esac
])

dnl
dnl PHP_AP_EXTRACT_VERSION(/path/httpd)
dnl This macro is used to get a comparable
dnl version for apache1/2.
dnl
AC_DEFUN([PHP_AP_EXTRACT_VERSION],[
  ac_output=`$1 -v 2>&1`
  ac_IFS=$IFS
IFS="- /.
"
  set $ac_output
  IFS=$ac_IFS

  APACHE_VERSION=`expr [$]4 \* 1000000 + [$]5 \* 1000 + [$]6`
])

