/* config.nw.h.  Configure file for NetWare platform */


/* Define if PHP to setup it's own SIGCHLD handler (not needed on NetWare) */
#define PHP_SIGCHILD 0

/* dns functions found in resolv.lib */
#define HAVE_LIBBIND 1

#define HAVE_GETSERVBYNAME 1
#define HAVE_GETSERVBYPORT 1
#define HAVE_GETPROTOBYNAME 1
#define HAVE_GETPROTOBYNUMBER 1

/* set to enable bcmath */
#define WITH_BCMATH 1

/* set to enable mysql */
#define HAVE_MYSQL 1

/* set to enable FTP support */
#define HAVE_FTP 1

/* set to enable SNMP */
/*#define HAVE_SNMP 1*/

/* defines for PostgreSQL extension */
#define HAVE_PGSQL 1
#define PHP_PGSQL_PRIVATE 1
#define HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT 1
#define HAVE_PQCLIENTENCODING 1
#define HAVE_PQCMDTUPLES 1
#define HAVE_PQOIDVALUE 1

/* set to enable bundled PCRE library */
#define HAVE_BUNDLED_PCRE	1

/* set to enable bundled expat library */
/* #define HAVE_LIBEXPAT 1 */ /* For now */
#define HAVE_WDDX 0

/* set to enable the crypt command */
/* #define HAVE_CRYPT 1 */
/* #define HAVE_CRYPT_H 1 */

/* set to enable force cgi redirect */
#define FORCE_CGI_REDIRECT 0

/* should be added to runtime config*/
#define PHP_URL_FOPEN 1

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* ----------------------------------------------------------------
   The following are defaults for run-time configuration
   ---------------------------------------------------------------*/

#define PHP_SAFE_MODE 0
#define MAGIC_QUOTES 0


/* Undefine if you want stricter XML/SGML compliance by default */
/* this disables "<?expression?>" and "<?=expression?>" */
#define DEFAULT_SHORT_OPEN_TAG "1"


/* ----------------------------------------------------------------
   The following defines are for those modules which require
   external libraries to compile.  These will be removed from 
   here in a future beta, as these modules will be moved out to dll's 
   ---------------------------------------------------------------*/
#define HAVE_ERRMSG_H 0 /*needed for mysql 3.21.17 and up*/
#undef HAVE_ADABAS
#undef HAVE_SOLID


/* ----------------------------------------------------------------
   The following may or may not be (or need to be) ported to the
   windows environment.
   ---------------------------------------------------------------*/

/* Define if you have the link function.  */
#undef HAVE_LINK

/* Define if you have the symlink function.  */
#undef HAVE_SYMLINK

/* Define if you have the usleep function.  */
#undef HAVE_USLEEP

#define HAVE_GETCWD 1
/* #define HAVE_POSIX_READDIR_R 1 */  /* We will use readdir() from LibC */

#define NEED_ISBLANK 1

/* ----------------------------------------------------------------
   The following should never need to be played with
   Functions defined to 0 or remarked out are either already
   handled by the standard VC libraries, or are no longer needed, or
   simply will/can not be ported.

   DONT TOUCH!!!!!  Unless you realy know what your messing with!
   ---------------------------------------------------------------*/

#define DISCARD_PATH 0
#undef HAVE_SETITIMER
#undef HAVE_IODBC
#define HAVE_UODBC 1
#define HAVE_LIBDL 1
#define HAVE_SENDMAIL 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

#define HAVE_LIMITS_H 1

#define HAVE_TZSET 1
#define HAVE_TZNAME 1

/* Define if you have the flock function.  */
#undef HAVE_FLOCK

/* Define if you have alloca, as a function or macro.  */
/* Though we have alloca(), this seems to be causing some problem with the stack pointer -- hence not using it */
/* #define HAVE_ALLOCA 1 */

/* Define if you have <sys/time.h> */
#undef HAVE_SYS_TIME_H

/* Define if you have <signal.h> */
#define HAVE_SIGNAL_H 1

/* Define if your struct stat has st_blksize.  */
#define HAVE_ST_BLKSIZE

/* Define if your struct stat has st_blocks.  */
#define HAVE_ST_BLOCKS

/* Define if your struct stat has st_rdev.  */
#define HAVE_ST_RDEV 1

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
#define HAVE_UTIME_NULL 1

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define both of these if you want the bundled REGEX library */
#define REGEX 1
#define HSREGEX 1

#define HAVE_PCRE 1

#define HAVE_LDAP 1

/* Define if you have the gcvt function.  */
/* #define HAVE_GCVT 1 */

/* Define if you have the getlogin function.  */
/* #define HAVE_GETLOGIN 1 */

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the regcomp function.  */
#define HAVE_REGCOMP 1

/* Define if you have the setlocale function.  */
/* #define HAVE_SETLOCALE 1 */    /* LibC doesn't seem to be supporting fully -- hence commenting for now */

#define HAVE_LOCALECONV 1

#define HAVE_LOCALE_H 1

/* Define if you have the setvbuf function.  */
#ifndef HAVE_LIBBIND
#define HAVE_SETVBUF 1
#endif

/* Define if you have the snprintf function.  */
#define HAVE_SNPRINTF 1
#define HAVE_VSNPRINTF 1
/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the tempnam function.  */
#define HAVE_TEMPNAM 1

/* Define if you have the utime function.  */
#define HAVE_UTIME 1

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <grp.h> header file.  */
#define HAVE_GRP_H 0

/* Define if you have the <pwd.h> header file.  */
#define HAVE_PWD_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <sys/file.h> header file.  */
#undef HAVE_SYS_FILE_H

/* Define if you have the <sys/socket.h> header file.  */
#ifdef USE_WINSOCK
#undef HAVE_SYS_SOCKET_H
#else
#define HAVE_SYS_SOCKET_H 1 /* Added '1' for '#if' to work */
#endif

/* Define if you have the <sys/wait.h> header file.  */
#undef HAVE_SYS_WAIT_H

/* Define if you have the <syslog.h> header file.  */
/* #define HAVE_SYSLOG_H 1 */

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1 /* Added '1' for '#if' to work */

/* Define if you have the dl library (-ldl).  */
#define HAVE_LIBDL 1

/* Define if you have the m library (-lm).  */
#define HAVE_LIBM 1

/* Define if you have the cuserid function.  */
#define HAVE_CUSERID 0

/* Define if you have the rint function.  */
#undef HAVE_RINT

#define HAVE_STRFTIME 1

/* Defined since unsetenv function is defined in LibC.
 * This is used to destroy env values in the function php_putenv_destructor.
 * If we do not use unsetenv, then the environment variables are directlt manipulated.
 * This will then result in LibC not being able to do the maintenance
 * that is required for NetWare.
 */
#define HAVE_UNSETENV 1

/* Default directory for loading extensions.  */
#define PHP_EXTENSION_DIR "sys:/php/ext"

#define SIZEOF_INT 4

/* Define directory constants for PHP and PEAR */

/* This is the default configuration file to read */
#define CONFIGURATION_FILE_PATH "php.ini"

#define APACHE_MODULE_DIR "sys:/apache/modules"
#define PHP_BINDIR "sys:/php"
#define PHP_LIBDIR PHP_BINDIR
#define PHP_DATADIR PHP_BINDIR
#define PHP_SYSCONFDIR PHP_BINDIR
#define PHP_LOCALSTATEDIR PHP_BINDIR
#define PHP_CONFIG_FILE_PATH "sys:/php"
#define PEAR_INSTALLDIR "sys:/php/pear"

#define PHP_CONFIG_FILE_SCAN_DIR NULL
#define PHP_EXTENSION_DIR "sys:/php/ext"

#define PHP_INCLUDE_PATH NULL

#define PHP_PREFIX "sys:/php"
#define PHP_SHLIB_SUFFIX "nlm"

#define USE_CONFIG_FILE 1

