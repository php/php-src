dnl
dnl Check if flush should be called explicitly after buffered io
dnl
AC_CACHE_CHECK([whether flush should be called explicitly after a buffered io], ac_cv_flush_io,[
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>

int main(int argc, char **argv)
{
	char *filename = tmpnam(NULL);
	char buffer[64];
	int result = 0;

	FILE *fp = fopen(filename, "wb");
	if (NULL == fp)
		return 0;
	fputs("line 1\n", fp);
	fputs("line 2\n", fp);
	fclose(fp);

	fp = fopen(filename, "rb+");
	if (NULL == fp)
		return 0;
	fgets(buffer, sizeof(buffer), fp);
	fputs("line 3\n", fp);
	rewind(fp);
	fgets(buffer, sizeof(buffer), fp);
	if (0 != strcmp(buffer, "line 1\n"))
		result = 1;
	fgets(buffer, sizeof(buffer), fp);
	if (0 != strcmp(buffer, "line 3\n"))
		result = 1;
	fclose(fp);
	unlink(filename);

	exit(result);
}
]])],[
  ac_cv_flush_io=no
],[
  ac_cv_flush_io=yes
],[
  ac_cv_flush_io=no
])])
if test "$ac_cv_flush_io" = "yes"; then
  AC_DEFINE(HAVE_FLUSHIO, 1, [Define if flush should be called explicitly after a buffered io.])
fi

PHP_ARG_WITH([external-libcrypt],
  [for external libcrypt or libxcrypt],
  [AS_HELP_STRING([--with-external-libcrypt],
    [Use external libcrypt or libxcrypt])],
  [no],
  [no])

if test "$PHP_EXTERNAL_LIBCRYPT" != "no"; then
  PHP_CHECK_FUNC(crypt, crypt)
  PHP_CHECK_FUNC(crypt_r, crypt)
  AC_CHECK_HEADERS(crypt.h)
  if test "$ac_cv_func_crypt_r" = "yes"; then
    PHP_CRYPT_R_STYLE
  fi

  AC_CACHE_CHECK(for standard DES crypt, ac_cv_crypt_des,[
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
#if HAVE_CRYPT
	char *encrypted = crypt("rasmuslerdorf","rl");
	return !encrypted || strcmp(encrypted,"rl.3StKT.4T8M");
#else
	return 1;
#endif
}]])],[
  ac_cv_crypt_des=yes
],[
  ac_cv_crypt_des=no
],[
  ac_cv_crypt_des=yes
])])

  AC_CACHE_CHECK(for extended DES crypt, ac_cv_crypt_ext_des,[
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
#if HAVE_CRYPT
	char *encrypted = crypt("rasmuslerdorf","_J9..rasm");
	return !encrypted || strcmp(encrypted,"_J9..rasmBYk8r9AiWNc");
#else
	return 1;
#endif
}]])],[
    ac_cv_crypt_ext_des=yes
  ],[
    ac_cv_crypt_ext_des=no
  ],[
    ac_cv_crypt_ext_des=no
  ])])

  AC_CACHE_CHECK(for MD5 crypt, ac_cv_crypt_md5,[
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
#if HAVE_CRYPT
	char salt[15], answer[40];
	char *encrypted;

	salt[0]='$'; salt[1]='1'; salt[2]='$';
	salt[3]='r'; salt[4]='a'; salt[5]='s';
	salt[6]='m'; salt[7]='u'; salt[8]='s';
	salt[9]='l'; salt[10]='e'; salt[11]='$';
	salt[12]='\0';
	strcpy(answer,salt);
	strcat(answer,"rISCgZzpwk3UhDidwXvin0");
	encrypted = crypt("rasmuslerdorf",salt);
	return !encrypted || strcmp(encrypted,answer);
#else
	return 1;
#endif
}]])],[
    ac_cv_crypt_md5=yes
  ],[
    ac_cv_crypt_md5=no
  ],[
    ac_cv_crypt_md5=no
  ])])

  AC_CACHE_CHECK(for Blowfish crypt, ac_cv_crypt_blowfish,[
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
#if HAVE_CRYPT
	char salt[30], answer[70];
	char *encrypted;

	salt[0]='$'; salt[1]='2'; salt[2]='a'; salt[3]='$'; salt[4]='0'; salt[5]='7'; salt[6]='$'; salt[7]='\0';
	strcat(salt,"rasmuslerd............");
	strcpy(answer,salt);
	strcpy(&answer[29],"nIdrcHdxcUxWomQX9j6kvERCFjTg7Ra");
	encrypted = crypt("rasmuslerdorf",salt);
	return !encrypted || strcmp(encrypted,answer);
#else
	return 1;
#endif
}]])],[
    ac_cv_crypt_blowfish=yes
  ],[
    ac_cv_crypt_blowfish=no
  ],[
    ac_cv_crypt_blowfish=no
  ])])

  AC_CACHE_CHECK(for SHA512 crypt, ac_cv_crypt_sha512,[
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
#if HAVE_CRYPT
	char salt[21], answer[21+86];
	char *encrypted;

	strcpy(salt,"\$6\$rasmuslerdorf\$");
	strcpy(answer, salt);
	strcat(answer, "EeHCRjm0bljalWuALHSTs1NB9ipEiLEXLhYeXdOpx22gmlmVejnVXFhd84cEKbYxCo.XuUTrW.RLraeEnsvWs/");
	encrypted = crypt("rasmuslerdorf",salt);
	return !encrypted || strcmp(encrypted,answer);
#else
	return 1;
#endif
  }]])],[
    ac_cv_crypt_sha512=yes
  ],[
    ac_cv_crypt_sha512=no
  ],[
    ac_cv_crypt_sha512=no
  ])])

  AC_CACHE_CHECK(for SHA256 crypt, ac_cv_crypt_sha256,[
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
#if HAVE_CRYPT
	char salt[21], answer[21+43];
	char *encrypted;

	strcpy(salt,"\$5\$rasmuslerdorf\$");
	strcpy(answer, salt);
	strcat(answer, "cFAm2puLCujQ9t.0CxiFIIvFi4JyQx5UncCt/xRIX23");
	encrypted = crypt("rasmuslerdorf",salt);
	return !encrypted || strcmp(encrypted,answer);
#else
	return 1;
#endif
}]])],[
    ac_cv_crypt_sha256=yes
  ],[
    ac_cv_crypt_sha256=no
  ],[
    ac_cv_crypt_sha256=no
  ])])


  if test "$ac_cv_crypt_blowfish" = "no" || test "$ac_cv_crypt_des" = "no" || test "$ac_cv_crypt_ext_des" = "no" || test "$ac_cv_crypt_md5" = "no" || test "$ac_cv_crypt_sha512" = "no" || test "$ac_cv_crypt_sha256" = "no" || test "$ac_cv_func_crypt_r" != "yes"; then
    AC_MSG_ERROR([Cannot use external libcrypt as some algo are missing])
  fi

  AC_DEFINE_UNQUOTED(PHP_USE_PHP_CRYPT_R, 0, [Whether PHP has to use its own crypt_r])
else
  AC_DEFINE_UNQUOTED(PHP_USE_PHP_CRYPT_R, 1, [Whether PHP has to use its own crypt_r])

  PHP_ADD_SOURCES(PHP_EXT_DIR(standard), crypt_freesec.c crypt_blowfish.c crypt_sha512.c crypt_sha256.c php_crypt_r.c)
fi

dnl
dnl Check for __attribute__ ((__aligned__)) support in the compiler
dnl
AC_CACHE_CHECK(whether the compiler supports aligned attribute, ac_cv_attribute_aligned,[
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
]],[[
  unsigned char test[32] __attribute__ ((__aligned__ (__alignof__ (int))));
]])],[
  ac_cv_attribute_aligned=yes
],[
  ac_cv_attribute_aligned=no
])])
if test "$ac_cv_attribute_aligned" = "yes"; then
  AC_DEFINE([HAVE_ATTRIBUTE_ALIGNED], 1, [whether the compiler supports __attribute__ ((__aligned__))])
fi

if test "$cross_compiling" = yes ; then
  case $host_alias in
    *linux*)
      AC_DEFINE([HAVE_FNMATCH], 1,
		     [Define to 1 if your system has a working POSIX `fnmatch'
		      function.])
      ;;
  esac
else
  AC_FUNC_FNMATCH
fi

dnl
dnl Check if there is a support means of creating a new process and defining
dnl which handles it receives
dnl
AC_CHECK_FUNCS(fork CreateProcess, [
  php_can_support_proc_open=yes
  break
],[
  php_can_support_proc_open=no
])
AC_MSG_CHECKING([if your OS can spawn processes with inherited handles])
if test "$php_can_support_proc_open" = "yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(PHP_CAN_SUPPORT_PROC_OPEN,1, [Define if your system has fork/vfork/CreateProcess])
else
  AC_MSG_RESULT(no)
fi

PHP_ENABLE_CHROOT_FUNC=no
case "$PHP_SAPI" in
  embed)
    PHP_ENABLE_CHROOT_FUNC=yes
  ;;

  none)
    for PROG in $PHP_BINARIES; do
      case "$PROG" in
        cgi|cli|phpdbg)
          PHP_ENABLE_CHROOT_FUNC=yes
        ;;

        *)
          PHP_ENABLE_CHROOT_FUNC=no
          break
        ;;
      esac
   done
  ;;
esac

if test "$PHP_ENABLE_CHROOT_FUNC" = "yes"; then
  AC_DEFINE(ENABLE_CHROOT_FUNC, 1, [Whether to enable chroot() function])
fi

dnl
dnl Detect library functions needed by php dns_xxx functions
dnl ext/standard/php_dns.h will collect these in a single define
dnl HAVE_FULL_DNS_FUNCS
dnl
PHP_CHECK_FUNC(res_nsearch, resolv, socket)
PHP_CHECK_FUNC(res_ndestroy, resolv, socket)
PHP_CHECK_FUNC(dns_search, resolv, socket)
PHP_CHECK_FUNC(dn_expand, resolv, socket)
PHP_CHECK_FUNC(dn_skipname, resolv, socket)

dnl
dnl These are old deprecated functions
dnl

PHP_CHECK_FUNC(res_search, resolv, socket)

AC_CHECK_FUNCS([posix_spawn_file_actions_addchdir_np])

dnl
dnl Check for strptime()
dnl
AC_CACHE_CHECK(whether strptime() declaration fails, ac_cv_strptime_decl_fails,[
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <time.h>
]],[[
#ifndef HAVE_STRPTIME
#error no strptime() on this platform
#else
/* use invalid strptime() declaration to see if it fails to compile */
int strptime(const char *s, const char *format, struct tm *tm);
#endif
]])],[
  ac_cv_strptime_decl_fails=no
],[
  ac_cv_strptime_decl_fails=yes
])])
if test "$ac_cv_strptime_decl_fails" = "yes"; then
  AC_DEFINE([HAVE_STRPTIME_DECL_FAILS], 1, [whether strptime() declaration fails])
fi

dnl
dnl Check for argon2
dnl
PHP_ARG_WITH([password-argon2],
  [for Argon2 support],
  [AS_HELP_STRING([[--with-password-argon2]],
    [Include Argon2 support in password_*])])

if test "$PHP_PASSWORD_ARGON2" != "no"; then
  PKG_CHECK_MODULES([ARGON2], [libargon2])
  PHP_EVAL_INCLINE($ARGON2_CFLAGS)
  PHP_EVAL_LIBLINE($ARGON2_LIBS)

  AC_DEFINE(HAVE_ARGON2LIB, 1, [ ])
fi

dnl
dnl Check net/if.h for net_get_interfaces. Darwin and BSD-like systems need
dnl sys/socket.h to be included with net/if.h.
dnl
AC_CHECK_HEADERS([net/if.h],[], [],
[
  #ifdef HAVE_SYS_SOCKET_H
  #include <sys/socket.h>
  #endif
])

AC_MSG_CHECKING([for usable getifaddrs])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
  #include <sys/types.h>
  #include <ifaddrs.h>
]],[[
  struct ifaddrs *interfaces;
  if (!getifaddrs(&interfaces)) {
      freeifaddrs(interfaces);
  }
]])], [ac_have_getifaddrs=yes], [ac_have_getifaddrs=no])
if test "$ac_have_getifaddrs" = "yes" ; then
  AC_DEFINE(HAVE_GETIFADDRS, 1, [whether getifaddrs is present and usable])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

dnl
dnl Setup extension sources
dnl
PHP_NEW_EXTENSION(standard, array.c base64.c basic_functions.c browscap.c crc32.c crypt.c \
                            datetime.c dir.c dl.c dns.c exec.c file.c filestat.c \
                            flock_compat.c formatted_print.c fsock.c head.c html.c image.c \
                            info.c iptc.c link.c mail.c math.c md5.c metaphone.c \
                            microtime.c pack.c pageinfo.c quot_print.c \
                            soundex.c string.c scanf.c syslog.c type.c uniqid.c url.c \
                            var.c versioning.c assert.c strnatcmp.c levenshtein.c \
                            incomplete_class.c url_scanner_ex.c ftp_fopen_wrapper.c \
                            http_fopen_wrapper.c php_fopen_wrapper.c credits.c css.c \
                            var_unserializer.c ftok.c sha1.c user_filters.c uuencode.c \
                            filters.c proc_open.c streamsfuncs.c http.c password.c \
                            net.c hrtime.c crc32_x86.c libavifinfo/avifinfo.c,,,
			    -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)

PHP_ADD_BUILD_DIR($ext_builddir/libavifinfo)

PHP_ADD_MAKEFILE_FRAGMENT
PHP_INSTALL_HEADERS([ext/standard/])
