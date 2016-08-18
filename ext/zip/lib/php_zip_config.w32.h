#ifndef HAD_CONFIG_H
#define HAD_CONFIG_H
#define HAVE__CLOSE
#define HAVE__DUP
#define HAVE__FDOPEN
#define HAVE__FILENO
#define HAVE__OPEN
#define HAVE__SNPRINTF
/* #undef HAVE__STRCMPI */
#define HAVE__STRDUP
#define HAVE__STRICMP
/* #undef HAVE_FSEEKO */
/* #undef HAVE_FTELLO */
/* #undef HAVE_MKSTEMP */
#define HAVE_MOVEFILEEXA
/* #undef HAVE_SNPRINTF */
/* #undef HAVE_STRCASECMP */
/* #undef HAVE_STRINGS_H */
/* #undef HAVE_STRUCT_TM_TM_ZONE */
/* #undef HAVE_UNISTD_H */
#define PACKAGE "libzip"
#define VERSION "0.10.b"

/* #undef HAVE_SSIZE_T */

#ifndef HAVE_SSIZE_T

#ifndef SIZE_T_LIBZIP
#define SIZE_T_LIBZIP 4
#endif
#ifndef INT_LIBZIP
#define INT_LIBZIP 4
#endif
#ifndef LONG_LIBZIP
#define LONG_LIBZIP 4
#endif
#ifndef LONG_LONG_LIBZIP
#define LONG_LONG_LIBZIP 8
#endif
#ifndef SIZEOF_OFF_T
#define SIZEOF_OFF_T 4
#endif

#  if SIZE_T_LIBZIP == INT_LIBZIP
# ifndef ssize_t
typedef int ssize_t;
# endif
#  elif SIZE_T_LIBZIP == LONG_LIBZIP
typedef long ssize_t;
#  elif SIZE_T_LIBZIP == LONG_LONG_LIBZIP
typedef long long ssize_t;
#  else
#error no suitable type for ssize_t found
#  endif
#endif

# undef strcasecmp
# define strcasecmp _strcmpi

#if !defined(EOPNOTSUPP) && defined(_WIN32)
# define EOPNOTSUPP 130
#endif

#if !defined(EOVERFLOW) && defined(_WIN32)
# define EOVERFLOW 132
#endif

#endif /* HAD_CONFIG_H */
