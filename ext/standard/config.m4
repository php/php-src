dnl $Id$ -*- sh -*-

divert(2)dnl

dnl
dnl Check for crypt() capabilities
dnl
AC_DEFUN(AC_CRYPT_CAP,[

  AC_CACHE_CHECK(for standard DES crypt, ac_cv_crypt_des,[
  AC_TRY_RUN([
main() {
#if HAVE_CRYPT
    exit (strcmp((char *)crypt("rasmuslerdorf","rl"),"rl.3StKT.4T8M"));
#else
	exit(0);
#endif
}],[
    ac_cv_crypt_des=yes
  ],[
    ac_cv_crypt_des=no
  ],[
    ac_cv_crypt_des=yes
  ])
  ])
  if test "$ac_cv_crypt_des" = "yes"; then
    AC_DEFINE(PHP3_STD_DES_CRYPT, 1, [ ])
  else
    AC_DEFINE(PHP3_STD_DES_CRYPT, 0, [ ])
  fi

  AC_CACHE_CHECK(for extended DES crypt, ac_cv_crypt_ext_des,[
  AC_TRY_RUN([
main() {
#if HAVE_CRYPT
    exit (strcmp((char *)crypt("rasmuslerdorf","_J9..rasm"),"_J9..rasmBYk8r9AiWNc"));
#else
	exit(0);
#endif
}],[
    ac_cv_crypt_ext_des=yes
  ],[
    ac_cv_crypt_ext_des=no
  ],[
    ac_cv_crypt_ext_des=no
  ])
  ])
  if test "$ac_cv_crypt_ext_des" = "yes"; then
    AC_DEFINE(PHP3_EXT_DES_CRYPT,1, [ ])
  else
    AC_DEFINE(PHP3_EXT_DES_CRYPT,0, [ ])
  fi

  AC_CACHE_CHECK(for MD5 crypt, ac_cv_crypt_md5,[
  AC_TRY_RUN([
main() {
#if HAVE_CRYPT
    char salt[15], answer[40];

    salt[0]='$'; salt[1]='1'; salt[2]='$'; 
    salt[3]='r'; salt[4]='a'; salt[5]='s';
    salt[6]='m'; salt[7]='u'; salt[8]='s';
    salt[9]='l'; salt[10]='e'; salt[11]='$';
    salt[12]='\0';
    strcpy(answer,salt);
    strcat(answer,"rISCgZzpwk3UhDidwXvin0");
    exit (strcmp((char *)crypt("rasmuslerdorf",salt),answer));
#else
	exit(0);
#endif
}],[
    ac_cv_crypt_md5=yes
  ],[
    ac_cv_crypt_md5=no
  ],[
    ac_cv_crypt_md5=no
  ])
  ])
  if test "$ac_cv_crypt_md5" = "yes"; then
    AC_DEFINE(PHP3_MD5_CRYPT,1,[ ])
  else
    AC_DEFINE(PHP3_MD5_CRYPT,0,[ ])
  fi

  AC_CACHE_CHECK(for Blowfish crypt, ac_cv_crypt_blowfish,[
  AC_TRY_RUN([
main() {
#if HAVE_CRYPT
    char salt[25], answer[70];
    
    salt[0]='$'; salt[1]='2'; salt[2]='a'; salt[3]='$'; salt[4]='0'; salt[5]='7'; salt[6]='$'; salt[7]='\0';
    strcat(salt,"rasmuslerd");
    strcpy(answer,salt);
    strcpy(&answer[16],"O............gl95GkTKn53Of.H4YchXl5PwvvW.5ri");
    exit (strcmp((char *)crypt("rasmuslerdorf",salt),answer));
#else
	exit(0);
#endif
}],[
    ac_cv_crypt_blowfish=yes
  ],[
    ac_cv_crypt_blowfish=no
  ],[
    ac_cv_crypt_blowfish=no
  ])
  ])
  if test "$ac_cv_crypt_blowfish" = "yes"; then
    AC_DEFINE(PHP3_BLOWFISH_CRYPT,1,[ ])
  else
    AC_DEFINE(PHP3_BLOWFISH_CRYPT,0,[ ])
  fi
])

AC_CHECK_LIB(c, dlopen, [
 # fake it
 AC_DEFINE(HAVE_LIBDL,1,[ ]) ], [
 AC_CHECK_LIB(dl, dlopen, [
  LIBS="-ldl $LIBS"
  AC_DEFINE(HAVE_LIBDL,1,[ ]) ], []) ])

AC_CHECK_LIB(pam, pam_start, [
  EXTRA_LIBS="$EXTRA_LIBS -lpam"
  AC_DEFINE(HAVE_LIBPAM,1,[ ]) ], []) 

AC_CHECK_LIB(crypt, crypt, [
 AC_ADD_LIBRARY(crypt)
 AC_DEFINE(HAVE_LIBCRYPT,1,[ ]) ], [])

AC_CHECK_FUNCS(getcwd)
AC_CHECK_FUNCS(getwd)

divert(4)dnl

AC_ARG_WITH(regex,
[  --with-regex=TYPE       regex library type: system, apache, php],
[
  REGEX_TYPE=$withval
],[
  REGEX_TYPE=php
])
	
AC_ARG_WITH(system-regex,
[  --with-system-regex     (deprecated) Use system regex library],
[
  if test "$withval" = "yes"; then
    REGEX_TYPE=system
  else
    REGEX_TYPE=php
  fi
])

AC_CRYPT_CAP

PHP_EXTENSION(standard)

