dnl
dnl Check if flush should be called explicitly after buffered io
dnl
AC_CACHE_CHECK([whether flush should be called explicitly after a buffered io],
[php_cv_have_flush_io],
[AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>

int main(void)
{
  char *filename = tmpnam(NULL);
  char buffer[64];
  int result = 0;

  FILE *fp = fopen(filename, "wb");
  if (NULL == fp) {
    return 0;
  }

  fputs("line 1\n", fp);
  fputs("line 2\n", fp);
  fclose(fp);

  fp = fopen(filename, "rb+");
  if (NULL == fp) {
    return 0;
  }

  if (fgets(buffer, sizeof(buffer), fp) == NULL) {
    fclose(fp);
    return 0;
  }

  fputs("line 3\n", fp);
  rewind(fp);
  if (fgets(buffer, sizeof(buffer), fp) == NULL) {
    fclose(fp);
    return 0;
  }

  if (0 != strcmp(buffer, "line 1\n")) {
    result = 1;
  }

  if (fgets(buffer, sizeof(buffer), fp) == NULL) {
    fclose(fp);
    return 0;
  }

  if (0 != strcmp(buffer, "line 3\n")) {
    result = 1;
  }

  fclose(fp);
  unlink(filename);

  return result;
}
]])],
[php_cv_have_flush_io=no],
[php_cv_have_flush_io=yes],
[php_cv_have_flush_io=no])])
AS_VAR_IF([php_cv_have_flush_io], [yes],
  [AC_DEFINE([HAVE_FLUSHIO], [1],
    [Define to 1 if flush should be called explicitly after a buffered io.])])

PHP_ARG_WITH([external-libcrypt],
  [for external libcrypt or libxcrypt],
  [AS_HELP_STRING([--with-external-libcrypt],
    [Use external libcrypt or libxcrypt])],
  [no],
  [no])

AH_TEMPLATE([PHP_USE_PHP_CRYPT_R],
  [Define to 1 if PHP uses its own crypt_r, and to 0 if using the external crypt
  library.])

php_ext_standard_sources=
AS_VAR_IF([PHP_EXTERNAL_LIBCRYPT], [no], [
  AC_DEFINE([PHP_USE_PHP_CRYPT_R], [1])
  php_ext_standard_sources=m4_normalize(["
    crypt_blowfish.c
    crypt_freesec.c
    crypt_sha256.c
    crypt_sha512.c
    php_crypt_r.c
  "])
], [
AC_SEARCH_LIBS([crypt], [crypt],
  [AC_DEFINE([HAVE_CRYPT], [1],
    [Define to 1 if you have the 'crypt' function.])],
  [AC_MSG_FAILURE([Cannot use external libcrypt as crypt() is missing.])])

AC_SEARCH_LIBS([crypt_r], [crypt],
  [AC_DEFINE([HAVE_CRYPT_R], [1],
    [Define to 1 if you have the 'crypt_r' function.])],
  [AC_MSG_FAILURE([Cannot use external libcrypt as crypt_r() is missing.])])

PHP_CRYPT_R_STYLE
AC_CHECK_HEADERS([crypt.h])

AC_CACHE_CHECK([for standard DES crypt], [ac_cv_crypt_des],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
  char *encrypted = crypt("rasmuslerdorf","rl");
  return !encrypted || strcmp(encrypted,"rl.3StKT.4T8M");
}])],
  [ac_cv_crypt_des=yes],
  [ac_cv_crypt_des=no],
  [ac_cv_crypt_des=yes])])

AC_CACHE_CHECK([for extended DES crypt], [ac_cv_crypt_ext_des],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
  char *encrypted = crypt("rasmuslerdorf","_J9..rasm");
  return !encrypted || strcmp(encrypted,"_J9..rasmBYk8r9AiWNc");
}])],
  [ac_cv_crypt_ext_des=yes],
  [ac_cv_crypt_ext_des=no],
  [ac_cv_crypt_ext_des=no])])

AC_CACHE_CHECK([for MD5 crypt], [ac_cv_crypt_md5],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([[
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
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
}]])],
  [ac_cv_crypt_md5=yes],
  [ac_cv_crypt_md5=no],
  [ac_cv_crypt_md5=no])])

AC_CACHE_CHECK([for Blowfish crypt], [ac_cv_crypt_blowfish],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([[
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
  char salt[30], answer[70];
  char *encrypted;

  salt[0]='$'; salt[1]='2'; salt[2]='a'; salt[3]='$';
  salt[4]='0'; salt[5]='7'; salt[6]='$'; salt[7]='\0';
  strcat(salt,"rasmuslerd............");
  strcpy(answer,salt);
  strcpy(&answer[29],"nIdrcHdxcUxWomQX9j6kvERCFjTg7Ra");
  encrypted = crypt("rasmuslerdorf",salt);
  return !encrypted || strcmp(encrypted,answer);
}]])],
  [ac_cv_crypt_blowfish=yes],
  [ac_cv_crypt_blowfish=no],
  [ac_cv_crypt_blowfish=no])])

AC_CACHE_CHECK([for SHA512 crypt], [ac_cv_crypt_sha512],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([[
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
  char salt[21], answer[21+86];
  char *encrypted;

  strcpy(salt,"\$6\$rasmuslerdorf\$");
  strcpy(answer, salt);
  strcat(answer, "EeHCRjm0bljalWuALHSTs1NB9ipEiLEXLhYeXdOpx22gmlmVejnVXFhd84cEKbYxCo.XuUTrW.RLraeEnsvWs/");
  encrypted = crypt("rasmuslerdorf",salt);
  return !encrypted || strcmp(encrypted,answer);
}]])],
  [ac_cv_crypt_sha512=yes],
  [ac_cv_crypt_sha512=no],
  [ac_cv_crypt_sha512=no])])

AC_CACHE_CHECK([for SHA256 crypt], [ac_cv_crypt_sha256],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([[
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <stdlib.h>
#include <string.h>

int main(void) {
  char salt[21], answer[21+43];
  char *encrypted;

  strcpy(salt,"\$5\$rasmuslerdorf\$");
  strcpy(answer, salt);
  strcat(answer, "cFAm2puLCujQ9t.0CxiFIIvFi4JyQx5UncCt/xRIX23");
  encrypted = crypt("rasmuslerdorf",salt);
  return !encrypted || strcmp(encrypted,answer);
}]])],
  [ac_cv_crypt_sha256=yes],
  [ac_cv_crypt_sha256=no],
  [ac_cv_crypt_sha256=no])])

  if test "$ac_cv_crypt_blowfish" = "no" || test "$ac_cv_crypt_des" = "no" || test "$ac_cv_crypt_ext_des" = "no" || test "$ac_cv_crypt_md5" = "no" || test "$ac_cv_crypt_sha512" = "no" || test "$ac_cv_crypt_sha256" = "no"; then
    AC_MSG_FAILURE([Cannot use external libcrypt as some algo are missing.])
  fi

  AC_DEFINE([PHP_USE_PHP_CRYPT_R], [0])
])

AS_VAR_IF([cross_compiling], [yes],
  [AS_CASE([$host_alias], [*linux*],
    [AC_DEFINE([HAVE_FNMATCH], [1])])],
  [AC_FUNC_FNMATCH])

dnl
dnl Check if there is a support means of creating a new process and defining
dnl which handles it receives
dnl
AC_CHECK_FUNCS([fork CreateProcess],
  [php_can_support_proc_open=yes; break;],
  [php_can_support_proc_open=no])
AC_MSG_CHECKING([if your OS can spawn processes with inherited handles])
AS_VAR_IF([php_can_support_proc_open], [yes],
  [AC_DEFINE([PHP_CAN_SUPPORT_PROC_OPEN], [1],
    [Define to 1 if your system has fork/vfork/CreateProcess.])])
AC_MSG_RESULT([$php_can_support_proc_open])

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

AS_VAR_IF([PHP_ENABLE_CHROOT_FUNC], [yes],
  [AC_DEFINE([ENABLE_CHROOT_FUNC], [1],
    [Define to 1 to enable the 'chroot' function.])])

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

AC_CHECK_FUNCS([posix_spawn_file_actions_addchdir_np elf_aux_info])

dnl
dnl Obsolete check for strptime() declaration. The strptime, where available,
dnl needs the _GNU_SOURCE defined or compiler flag -std=gnuXX appended to be
dnl declared in time.h. PHP's strptime is also deprecated as of PHP 8.1.
dnl
AC_CHECK_DECL([strptime],
  [AC_DEFINE([HAVE_DECL_STRPTIME], [1],
    [Define to 1 if you have the declaration of 'strptime'.])],,
  [#include <time.h>])

dnl
dnl Check for argon2
dnl
PHP_ARG_WITH([password-argon2],
  [for Argon2 support],
  [AS_HELP_STRING([--with-password-argon2],
    [Include Argon2 support in password_*])])

AS_VAR_IF([PHP_PASSWORD_ARGON2], [no],, [
  PKG_CHECK_MODULES([ARGON2], [libargon2])
  PHP_EVAL_INCLINE([$ARGON2_CFLAGS])
  PHP_EVAL_LIBLINE([$ARGON2_LIBS])

  AC_DEFINE([HAVE_ARGON2LIB], [1],
    [Define to 1 if the system has the 'libargon2' library.])
])

dnl
dnl Check net/if.h for net_get_interfaces. Darwin and BSD-like systems need
dnl sys/socket.h to be included with net/if.h.
dnl
AC_CHECK_HEADERS([net/if.h],,, [dnl
  #ifdef HAVE_SYS_SOCKET_H
  #include <sys/socket.h>
  #endif
])

AC_CACHE_CHECK([for usable getifaddrs], [php_cv_func_getifaddrs],
[AC_LINK_IFELSE([AC_LANG_PROGRAM([
  #include <sys/types.h>
  #include <ifaddrs.h>
], [
  struct ifaddrs *interfaces;
  if (!getifaddrs(&interfaces)) {
    freeifaddrs(interfaces);
  }
])],
[php_cv_func_getifaddrs=yes],
[php_cv_func_getifaddrs=no])])
AS_VAR_IF([php_cv_func_getifaddrs], [yes],
  [AC_DEFINE([HAVE_GETIFADDRS], [1],
    [Define to 1 if you have the 'getifaddrs' function.])])

dnl
dnl Setup extension sources
dnl
PHP_NEW_EXTENSION([standard], m4_normalize([
    array.c
    assert.c
    base64.c
    basic_functions.c
    browscap.c
    crc32_x86.c
    crc32.c
    credits.c
    crypt.c
    css.c
    datetime.c
    dir.c
    dl.c
    dns.c
    exec.c
    file.c
    filestat.c
    filters.c
    flock_compat.c
    formatted_print.c
    fsock.c
    ftok.c
    ftp_fopen_wrapper.c
    head.c
    hrtime.c
    html.c
    http_fopen_wrapper.c
    http.c
    image.c
    incomplete_class.c
    info.c
    iptc.c
    levenshtein.c
    libavifinfo/avifinfo.c
    link.c
    mail.c
    math.c
    md5.c
    metaphone.c
    microtime.c
    net.c
    pack.c
    pageinfo.c
    password.c
    php_fopen_wrapper.c
    proc_open.c
    quot_print.c
    scanf.c
    sha1.c
    soundex.c
    streamsfuncs.c
    string.c
    strnatcmp.c
    syslog.c
    type.c
    uniqid.c
    url_scanner_ex.c
    url.c
    user_filters.c
    uuencode.c
    var_unserializer.c
    var.c
    versioning.c
    $php_ext_standard_sources
  ]),
  [no],,
  [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

PHP_ADD_BUILD_DIR([$ext_builddir/libavifinfo])

PHP_ADD_MAKEFILE_FRAGMENT
PHP_INSTALL_HEADERS([ext/standard/])
