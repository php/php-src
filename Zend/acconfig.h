/* This is the default configuration file to read */
#define USE_CONFIG_FILE 1

/* these are defined by automake */
#undef PACKAGE
#undef VERSION

/* define uint by configure if it is missed (QNX and BSD derived) */
#undef uint

/* define ulong by configure if it is missed (most probably is) */
#undef ulong

/* type check for in_addr_t */
#undef in_addr_t

/* Define if you have dirent.h but opendir() resides in libc rather than in libdir */
/* This will cause HAVE_DIRENT_H defined twice sometimes, but it should be problem */
#define HAVE_DIRENT_H 0

/* Define if you have struct flock */
#define HAVE_STRUCT_FLOCK 0

/* Define if you have the resolv library (-lresolv). */
#define HAVE_LIBRESOLV 0

/* Undefine if you want stricter XML/SGML compliance by default */
/* (this disables "<?expression?>" by default) */
#define T_DEFAULT_SHORT_OPEN_TAG 1

/* Define both of these if you want the bundled REGEX library */
#define REGEX 0
#define HSREGEX 0

/* Define if you have libdl (used for dynamic linking) */
#define HAVE_LIBDL 0

#undef ZEND_DEBUG

/* Define if you want to enable bc style precision math support */
#define WITH_BCMATH 0

/* Define if you want to enable memory limit support */
#define MEMORY_LIMIT 0

/* Define if you have broken sprintf function like SunOS 4 */
#define BROKEN_SPRINTF 0

/* Define to compile Zend thread safe */
#undef ZTS
