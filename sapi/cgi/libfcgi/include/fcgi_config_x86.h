/* 
 *  Default fcgi_config.h when building on WinNT (configure is not run). 
 */

/* Define if you have the <arpa/inet.h> header file. */
#undef HAVE_ARPA_INET_H

/* Define if there's a fileno() prototype in stdio.h */
#undef HAVE_FILENO_PROTO

/* Define if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define if we have f{set,get}pos functions */
#define HAVE_FPOS 1

/* Define if you have the `dnet_stub' library (-ldnet_stub). */
#undef HAVE_LIBDNET_STUB

/* Define if you have the `ieee' library (-lieee). */
#undef HAVE_LIBIEEE

/* Define if you have the `nsl' library (-lnsl). */
#undef HAVE_LIBNSL

/* Define if you have the pthread library */
#undef HAVE_LIBPTHREAD

/* Define if you have the `resolv' library (-lresolv). */
#undef HAVE_LIBRESOLV

/* Define if you have the `socket' library (-lsocket). */
#undef HAVE_LIBSOCKET

/* Define if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if we need cross-process locking */
#undef USE_LOCKING

/* Define if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define if you have the <netdb.h> header file. */
#undef HAVE_NETDB_H

/* Define if you have the <netinet/in.h> header file. */
#undef HAVE_NETINET_IN_H

/* Define if sockaddr_un in sys/un.h contains a sun_len component */
#undef HAVE_SOCKADDR_UN_SUN_LEN

/* Define if the socklen_t typedef is in sys/socket.h */
#undef HAVE_SOCKLEN

/* Define if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define if you have the <stdlib.h> header file. */
#undef HAVE_STDLIB_H

/* Define if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define if you have the <string.h> header file. */
#undef HAVE_STRING_H

/* Define if you have the <sys/param.h> header file. */
#undef HAVE_SYS_PARAM_H

/* Define if you have the <sys/socket.h> header file. */
#undef HAVE_SYS_SOCKET_H

/* Define if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define if you have the <sys/time.h> header file. */
#undef HAVE_SYS_TIME_H

/* Define if you have the <sys/types.h> header file. */
#undef HAVE_SYS_TYPES_H

/* Define if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define if va_arg(arg, long double) crashes the compiler */
#undef HAVE_VA_ARG_LONG_DOUBLE_BUG

/* Define if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
#undef inline

/* Define to `int' if <sys/types.h> does not define. */
#undef ssize_t
