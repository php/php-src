dnl $Id$ -*- sh -*-

divert(1)

dnl
dnl Check for crypt() capabilities
dnl
AC_DEFUN(AC_CRYPT_CAP,[

  AC_MSG_CHECKING([for standard DES crypt])
  AC_TRY_RUN([
main() {
#if HAVE_CRYPT
    exit (strcmp((char *)crypt("rasmuslerdorf","rl"),"rl.3StKT.4T8M"));
#else
	exit(0);
#endif
}],[
    AC_DEFINE(PHP3_STD_DES_CRYPT,1)
    AC_MSG_RESULT(yes)
  ],[
    AC_DEFINE(PHP3_STD_DES_CRYPT,0)
    AC_MSG_RESULT(no)
  ],[
    AC_DEFINE(PHP3_STD_DES_CRYPT,1)
    AC_MSG_RESULT(cannot check, guessing yes)
  ])

  AC_MSG_CHECKING([for extended DES crypt])
  AC_TRY_RUN([
main() {
#if HAVE_CRYPT
    exit (strcmp((char *)crypt("rasmuslerdorf","_J9..rasm"),"_J9..rasmBYk8r9AiWNc"));
#else
	exit(0);
#endif
}],[
    AC_DEFINE(PHP3_EXT_DES_CRYPT,1)
    AC_MSG_RESULT(yes)
  ],[
    AC_DEFINE(PHP3_EXT_DES_CRYPT,0)
    AC_MSG_RESULT(no)
  ],[
    AC_DEFINE(PHP3_EXT_DES_CRYPT,0)
    AC_MSG_RESULT(cannot check, guessing no)
  ])

  AC_MSG_CHECKING([for MD5 crypt])
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
    AC_DEFINE(PHP3_MD5_CRYPT,1)
    AC_MSG_RESULT(yes)
  ],[
    AC_DEFINE(PHP3_MD5_CRYPT,0)
    AC_MSG_RESULT(no)
  ],[
    AC_DEFINE(PHP3_MD5_CRYPT,0)
    AC_MSG_RESULT(cannot check, guessing no)
  ])

  AC_MSG_CHECKING([for Blowfish crypt])
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
    AC_DEFINE(PHP3_BLOWFISH_CRYPT,1)
    AC_MSG_RESULT(yes)
  ],[
    AC_DEFINE(PHP3_BLOWFISH_CRYPT,0)
    AC_MSG_RESULT(no)
  ],[
    AC_DEFINE(PHP3_BLOWFISH_CRYPT,0)
    AC_MSG_RESULT(cannot check, guessing no)
  ])
])

AC_CHECK_LIB(c, dlopen, [
 # fake it
 AC_DEFINE(HAVE_LIBDL) ], [
 AC_CHECK_LIB(dl, dlopen, [
  LIBS="-ldl $LIBS"
  AC_DEFINE(HAVE_LIBDL) ], []) ])

AC_CHECK_LIB(pam, pam_start, [
  EXTRA_LIBS="$EXTRA_LIBS -lpam"
  AC_DEFINE(HAVE_LIBPAM) ], []) 

AC_CHECK_LIB(bind, inet_aton, [
  EXTRA_LIBS="$EXTRA_LIBS -lbind"
  AC_DEFINE(HAVE_LIBBIND) ], []) 

AC_CHECK_LIB(crypt, crypt, [
 AC_ADD_LIBRARY(crypt)
 AC_DEFINE(HAVE_LIBCRYPT) ], [])

AC_CHECK_FUNCS(getcwd)
AC_CHECK_FUNCS(getwd)

divert(3)

AC_CRYPT_CAP

PHP_EXTENSION(standard)

