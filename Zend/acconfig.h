/* This is the default configuration file to read */
#define USE_CONFIG_FILE 1

/* define uint by configure if it is missed (QNX and BSD derived) */
#undef uint

/* define ulong by configure if it is missed (most probably is) */
#undef ulong

/* type check for in_addr_t */
#undef in_addr_t

/* Solaris YP check */
#undef SOLARIS_YP

/* Netscape LDAP SDK check */
#undef HAVE_NSLDAP

/* ImageMagick check */
#undef HAVE_LIBMAGICK

/* Define if you have dirent.h but opendir() resides in libc rather than in libdir */
/* This will cause HAVE_DIRENT_H defined twice sometimes, but it should be problem */
#define HAVE_DIRENT_H 0

/* Define if you have struct flock */
#define HAVE_STRUCT_FLOCK 0

/* Define if you have the resolv library (-lresolv). */
#define HAVE_LIBRESOLV 0

/* Define if you have the gd library (-lgd). */
#define HAVE_LIBGD 0

/* Define if you have the GNU gettext library (-lintl). */
#define HAVE_LIBINTL 0

/* Define if you have the zlib library */
#define HAVE_ZLIB 0

/* Define if you have the gd version 1.3 library (-lgd). */
#define HAVE_LIBGD13 0

/* Undefine if you want stricter XML/SGML compliance by default */
/* (this disables "<?expression?>" by default) */
#define DEFAULT_SHORT_OPEN_TAG 1

/* Define if you want the logging to go to ndbm/gdbm/flatfile */
#define LOG_DBM 0
#define LOG_DBM_DIR "."

/* Define if you want the logging to go to a mysql database */
#define LOG_MYSQL 0

/* Define if you want the logging to go to a mysql database */
#define LOG_MSQL 0

/* Define these if you are using an SQL database for logging */
#define LOG_SQL_HOST ""
#define LOG_SQL_DB ""

/* Define if you an ndbm compatible library (-ldbm).  */
#define NDBM 0

/* Define if you have the gdbm library (-lgdbm).  */
#define GDBM 0

/* Define both of these if you want the bundled REGEX library */
#define REGEX 0
#define HSREGEX 0

/* Define if you want Solid database support */
#define HAVE_SOLID 0

/* Define if you want to use the supplied dbase library */
#define DBASE 0

/* Define if you want Hyperwave support */
#define HYPERWAVE 0

/* Define if you have the crypt() function */
#define HAVE_CRYPT 1

/* Define if you have the Oracle database client libraries */
#define HAVE_ORACLE 0

/* Define if you have the Oracle version 8 database client libraries */
#define HAVE_OCI8 0

/* Define if you want to use the iODBC database driver */
#define HAVE_IODBC 0

/* Define if you want to use the OpenLink ODBC database driver */
#define HAVE_OPENLINK 0

/* Define if you have the AdabasD client libraries */
#define HAVE_ADABAS 0

/* Define if you want the LDAP directory interface */
#define HAVE_LDAP 0

/* Define if you want the Cybercash MCK support */
#define HAVE_MCK 0

/* Define if you want the SNMP interface */
#define HAVE_SNMP 0

/* Define if you want the IMAP directory interface */
#define HAVE_IMAP 0

/* Define if you want the ASPELL interface */
#define HAVE_ASPELL 0

/* Define if you want to use a custom ODBC database driver */
#define HAVE_CODBC 0

/* Define to use the unified ODBC interface */
#define HAVE_UODBC 0

/* Define if you have libdl (used for dynamic linking) */
#define HAVE_LIBDL 0

/* Define if you have libdnet_stub (used for Sybase support) */
#define HAVE_LIBDNET_STUB 0

/* Define if you have and want to use libcrypt */
#define HAVE_LIBCRYPT 0

/* Define if you have and want to use libnsl */
#define HAVE_LIBNSL 0

/* Define if you have and want to use libsocket */
#define HAVE_LIBSOCKET 0

/* Define if you have and want to use libpam */
#define HAVE_LIBPAM 0

/* Define if you have the sendmail program available */
#define HAVE_SENDMAIL 0

/* Define if your Apache creates an ap_config.h header file (only 1.3b6 and later) */
#define HAVE_AP_CONFIG_H 0

/* Define if your Apache has src/include/compat.h */
#define HAVE_OLD_COMPAT_H 0

/* Define if your Apache has src/include/ap_compat.h */
#define HAVE_AP_COMPAT_H 0

#ifndef HAVE_EMPRESS
#define HAVE_EMPRESS 0
#endif

#define HAVE_SYBASE 0
#define HAVE_SYBASE_CT 0

#ifndef HAVE_MYSQL
#define HAVE_MYSQL 0
#endif

#ifndef HAVE_MSQL
#define HAVE_MSQL 0
#endif

#ifndef HAVE_PGSQL
#define HAVE_PGSQL 0
#endif

#ifndef HAVE_VELOCIS
#define HAVE_VELOCIS 0
#endif

#ifndef HAVE_IFX
#define HAVE_IFX 0
#endif
#ifndef HAVE_IFX_IUS
#define HAVE_IFX_IUS 0
#endif
#ifndef IFX_VERSION
#define IFX_VERSION 0
#endif

#ifndef HAVE_IBASE
#define HAVE_IBASE 0
#endif

#ifndef HAVE_PQCMDTUPLES
#define HAVE_PQCMDTUPLES 0
#endif

#undef ZEND_DEBUG

/* Define if your system has the gettimeofday() call */
#define HAVE_GETTIMEOFDAY 0

/* Define if your system has the getrusage() call */
#define HAVE_GETRUSAGE 0

/* Define if your system has the putenv() library call */
#define HAVE_PUTENV 0

/* Define if you want to enable bc style precision math support */
#define WITH_BCMATH 0

/* Define if you want to prevent the CGI from working unless REDIRECT_STATUS is defined in the environment */
#define FORCE_CGI_REDIRECT 0

/* Define if you want to prevent the CGI from using path_info and path_translated */
#define DISCARD_PATH 0

/* Define if you want to enable memory limit support */
#define MEMORY_LIMIT 0

/* Define if you want System V semaphore support.
 */
#define HAVE_SYSVSEM 0

/* Define if you have union semun.
 */
#define HAVE_SEMUN 0

/* Define if you want System V shared memory support.  */
#define HAVE_SYSVSHM 0

/* Define if you want to enable displaying source support. */
#define HAVE_DISPLAY_SOURCE 0

/* Define if you have broken header files like SunOS 4 */
#define MISSING_FCLOSE_DECL 0

/* Define if you have broken sprintf function like SunOS 4 */
#define BROKEN_SPRINTF 0

/* Define if you have the expat (XML Parser Toolkit) library */
#define HAVE_LIBEXPAT 0

/* Define if you have the pdflib library */
#define HAVE_PDFLIB 0

/* Define if you have the fdftk library */
#define HAVE_FDFLIB 0

/* Define to compile with mod_dav support */
#define HAVE_MOD_DAV 0

/* Define to enable yp/nis support */
#define HAVE_YP 0
