/* acconfig.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */


/* Solaris define gethostbyname_r with 5 arguments. glibc2 defines
   this with 6 arguments */
#undef HAVE_GLIBC2_STYLE_GETHOSTBYNAME_R

/* For some non posix threads */
#undef HAVE_NONPOSIX_PTHREAD_GETSPECIFIC

/* For some non posix threads */
#undef HAVE_NONPOSIX_PTHREAD_MUTEX_INIT

/* sigwait with one argument */
#undef HAVE_NONPOSIX_SIGWAIT

/* POSIX readdir_r */
#undef HAVE_READDIR_R

/* POSIX sigwait */
#undef HAVE_SIGWAIT

/* Define if the system files define uchar */
#undef HAVE_UCHAR

/* Define if the system files define uint */
#undef HAVE_UINT

/* Define if the system files define ulong */
#undef HAVE_ULONG

/* Define as the base type of the last arg to accept */
#undef SOCKET_SIZE_TYPE
