/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have a working `mmap' system call.  */
#define HAVE_MMAP 1

/* Define if system calls automatically restart after interruption
   by a signal.  */
/* #undef HAVE_RESTARTABLE_SYSCALLS */

/* Define if your struct stat has st_rdev.  */
#define HAVE_ST_RDEV 1

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
#define HAVE_UTIME_NULL 1

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
#define STACK_DIRECTION -1

/* Define if the `S_IS*' macros in <sys/stat.h> do not work properly.  */
/* #undef STAT_MACROS_BROKEN */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
#define WORDS_BIGENDIAN 1

/* Version of .frm files */
#define DOT_FRM_VERSION 6

/* READLINE: */
/* #undef FIONREAD_IN_SYS_IOCTL */

/* READLINE: Define if your system defines TIOCGWINSZ in sys/ioctl.h.  */
/* #undef GWINSZ_IN_SYS_IOCTL */

/* Do we have FIONREAD */
/* #undef FIONREAD_IN_SYS_IOCTL */

/* bool is not defined by all C++ compilators */
#define HAVE_BOOL 1

/* DSB style signals ? */
/* #undef HAVE_BSD_SIGNALS */

/* Can netinet be included */
/* #undef HAVE_BROKEN_NETINET_INCLUDES */

/* READLINE: */
/* #undef HAVE_BSD_SIGNALS */

/* Define if we are using OSF1 DEC threads */
/* #undef HAVE_DEC_THREADS */

/* Define if we are using OSF1 DEC threads on 3.2 */
/* #undef HAVE_DEC_3_2_THREADS */

/* fp_except from ieeefp.h */
#define HAVE_FP_EXCEPT 1

/* READLINE: */
/* #undef HAVE_GETPW_DECLS */

/* Solaris define gethostbyname_r with 5 arguments. glibc2 defines
   this with 6 arguments */
/* #undef HAVE_GLIBC2_STYLE_GETHOSTBYNAME_R */

/* Define if have -lwrap */
/* #undef HAVE_LIBWRAP */

/* Define if we are using Xavier Leroy's LinuxThreads */
/* #undef HAVE_LINUXTHREADS */

/* Do we use user level threads */
#define HAVE_mit_thread 1

/* For some non posix threads */
/* #undef HAVE_NONPOSIX_PTHREAD_GETSPECIFIC */

/* For some non posix threads */
/* #undef HAVE_NONPOSIX_PTHREAD_MUTEX_INIT */

/* READLINE: */
#define HAVE_POSIX_SIGNALS 1

/* sigwait with one argument */
/* #undef HAVE_NONPOSIX_SIGWAIT */

/* pthread_attr_setscope */
/* #undef HAVE_PTHREAD_ATTR_SETSCOPE */

/* POSIX readdir_r */
#define HAVE_READDIR_R 1

/* POSIX sigwait */
#define HAVE_SIGWAIT 1

/* Solaris define gethostbyaddr_r with 7 arguments. glibc2 defines
   this with 8 arguments */
#define HAVE_SOLARIS_STYLE_GETHOST 1

/* MIT pthreads does not support connecting with unix sockets */
#define HAVE_THREADS_WITHOUT_SOCKETS 1

/* Timespec has a ts_sec instead of tv_sev  */
/* #undef HAVE_TIMESPEC_TS_SEC */

/* Define if the system files define uchar */
/* #undef HAVE_UCHAR */

/* Define if the system files define uint */
#define HAVE_UINT 1

/* Define if the system files define ulong */
#define HAVE_ULONG 1

/* UNIXWARE7 threads are not posix */
/* #undef HAVE_UNIXWARE7_THREADS */

/* new UNIXWARE7 threads that are not yet posix */
/* #undef HAVE_UNIXWARE7_POSIX */

/* READLINE: */
/* #undef HAVE_USG_SIGHOLD */

/* Define if want -lwrap */
/* #undef LIBWRAP */

/* Define to machine type name eg sun10 */
#define MACHINE_TYPE "sparc"

/* Defined to used character set */
#define MY_CHARSET_CURRENT MY_CHARSET_LATIN1

/* mysql client protocoll version */
#define PROTOCOL_VERSION 10

/* Define if qsort returns void */
#define QSORT_TYPE_IS_VOID 1

/* Define as the return type of qsort (int or void). */
#define RETQSORTTYPE void

/* Size of off_t */
#define SIZEOF_OFF_T 4

/* Define as the base type of the last arg to accept */
#define SOCKET_SIZE_TYPE int

/* Needed to get large file supportat HPUX 10.20 */
#define __STDC_EXT__ 1

/* #undef STRUCT_DIRENT_HAS_D_FILENO */
#define STRUCT_DIRENT_HAS_D_INO 1

/* Define to name of system eg solaris*/
#define SYSTEM_TYPE "sun-solaris2.5.1"

/* Define if you want to have threaded code. This may be undef on client code */
#define THREAD 1

/* Should be client be thread safe */
/* #undef THREAD_SAFE_CLIENT */

/* READLINE: */
/* #undef TIOCSTAT_IN_SYS_IOCTL */

/* Use MySQL RAID */
/* #undef USE_RAID */

/* READLINE: */
#define VOID_SIGHANDLER 1

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a long long.  */
#define SIZEOF_LONG_LONG 8

/* Define if you have the alarm function.  */
#define HAVE_ALARM 1

/* Define if you have the atod function.  */
/* #undef HAVE_ATOD */

/* Define if you have the bcmp function.  */
#define HAVE_BCMP 1

/* Define if you have the bfill function.  */
/* #undef HAVE_BFILL */

/* Define if you have the bmove function.  */
/* #undef HAVE_BMOVE */

/* Define if you have the bzero function.  */
#define HAVE_BZERO 1

/* Define if you have the chsize function.  */
/* #undef HAVE_CHSIZE */

/* Define if you have the crypt function.  */
#define HAVE_CRYPT 1

/* Define if you have the cuserid function.  */
#define HAVE_CUSERID 1

/* Define if you have the dlerror function.  */
#define HAVE_DLERROR 1

/* Define if you have the dlopen function.  */
#define HAVE_DLOPEN 1

/* Define if you have the fchmod function.  */
#define HAVE_FCHMOD 1

/* Define if you have the fcntl function.  */
#define HAVE_FCNTL 1

/* Define if you have the fconvert function.  */
#define HAVE_FCONVERT 1

/* Define if you have the finite function.  */
#define HAVE_FINITE 1

/* Define if you have the fpresetsticky function.  */
/* #undef HAVE_FPRESETSTICKY */

/* Define if you have the fpsetmask function.  */
#define HAVE_FPSETMASK 1

/* Define if you have the fseeko function.  */
/* #undef HAVE_FSEEKO */

/* Define if you have the ftruncate function.  */
#define HAVE_FTRUNCATE 1

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the gethostbyaddr_r function.  */
#define HAVE_GETHOSTBYADDR_R 1

/* Define if you have the gethostbyname_r function.  */
#define HAVE_GETHOSTBYNAME_R 1

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the getpass function.  */
#define HAVE_GETPASS 1

/* Define if you have the getpassphrase function.  */
/* #undef HAVE_GETPASSPHRASE */

/* Define if you have the getpwnam function.  */
#define HAVE_GETPWNAM 1

/* Define if you have the getpwuid function.  */
#define HAVE_GETPWUID 1

/* Define if you have the getrlimit function.  */
#define HAVE_GETRLIMIT 1

/* Define if you have the getrusage function.  */
#define HAVE_GETRUSAGE 1

/* Define if you have the getwd function.  */
#define HAVE_GETWD 1

/* Define if you have the index function.  */
#define HAVE_INDEX 1

/* Define if you have the localtime_r function.  */
#define HAVE_LOCALTIME_R 1

/* Define if you have the locking function.  */
/* #undef HAVE_LOCKING */

/* Define if you have the longjmp function.  */
#define HAVE_LONGJMP 1

/* Define if you have the lrand48 function.  */
#define HAVE_LRAND48 1

/* Define if you have the lstat function.  */
#define HAVE_LSTAT 1

/* Define if you have the madvise function.  */
#define HAVE_MADVISE 1

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the perror function.  */
#define HAVE_PERROR 1

/* Define if you have the pread function.  */
#define HAVE_PREAD 1

/* Define if you have the pthread_attr_create function.  */
/* #undef HAVE_PTHREAD_ATTR_CREATE */

/* Define if you have the pthread_attr_setprio function.  */
/* #undef HAVE_PTHREAD_ATTR_SETPRIO */

/* Define if you have the pthread_attr_setschedparam function.  */
/* #undef HAVE_PTHREAD_ATTR_SETSCHEDPARAM */

/* Define if you have the pthread_attr_setstacksize function.  */
/* #undef HAVE_PTHREAD_ATTR_SETSTACKSIZE */

/* Define if you have the pthread_condattr_create function.  */
/* #undef HAVE_PTHREAD_CONDATTR_CREATE */

/* Define if you have the pthread_getsequence_np function.  */
/* #undef HAVE_PTHREAD_GETSEQUENCE_NP */

/* Define if you have the pthread_init function.  */
/* #undef HAVE_PTHREAD_INIT */

/* Define if you have the pthread_setprio function.  */
/* #undef HAVE_PTHREAD_SETPRIO */

/* Define if you have the pthread_setprio_np function.  */
/* #undef HAVE_PTHREAD_SETPRIO_NP */

/* Define if you have the pthread_setschedparam function.  */
/* #undef HAVE_PTHREAD_SETSCHEDPARAM */

/* Define if you have the pthread_sigmask function.  */
/* #undef HAVE_PTHREAD_SIGMASK */

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the realpath function.  */
#define HAVE_REALPATH 1

/* Define if you have the rename function.  */
#define HAVE_RENAME 1

/* Define if you have the rint function.  */
#define HAVE_RINT 1

/* Define if you have the rwlock_init function.  */
#define HAVE_RWLOCK_INIT 1

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the setenv function.  */
/* #undef HAVE_SETENV */

/* Define if you have the setlocale function.  */
#define HAVE_SETLOCALE 1

/* Define if you have the setupterm function.  */
/* #undef HAVE_SETUPTERM */

/* Define if you have the sighold function.  */
#define HAVE_SIGHOLD 1

/* Define if you have the sigset function.  */
#define HAVE_SIGSET 1

/* Define if you have the sigthreadmask function.  */
/* #undef HAVE_SIGTHREADMASK */

/* Define if you have the snprintf function.  */
/* #undef HAVE_SNPRINTF */

/* Define if you have the socket function.  */
#define HAVE_SOCKET 1

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strcoll function.  */
#define HAVE_STRCOLL 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strnlen function.  */
/* #undef HAVE_STRNLEN */

/* Define if you have the strpbrk function.  */
#define HAVE_STRPBRK 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the strtok_r function.  */
#define HAVE_STRTOK_R 1

/* Define if you have the strtoul function.  */
#define HAVE_STRTOUL 1

/* Define if you have the strtoull function.  */
#define HAVE_STRTOULL 1

/* Define if you have the tcgetattr function.  */
#define HAVE_TCGETATTR 1

/* Define if you have the tell function.  */
#define HAVE_TELL 1

/* Define if you have the tempnam function.  */
#define HAVE_TEMPNAM 1

/* Define if you have the thr_setconcurrency function.  */
#define HAVE_THR_SETCONCURRENCY 1

/* Define if you have the vidattr function.  */
/* #undef HAVE_VIDATTR */

/* Define if you have the <alloca.h> header file.  */
#define HAVE_ALLOCA_H 1

/* Define if you have the <asm/termbits.h> header file.  */
/* #undef HAVE_ASM_TERMBITS_H */

/* Define if you have the <crypt.h> header file.  */
#define HAVE_CRYPT_H 1

/* Define if you have the <curses.h> header file.  */
#define HAVE_CURSES_H 1

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <float.h> header file.  */
#define HAVE_FLOAT_H 1

/* Define if you have the <floatingpoint.h> header file.  */
#define HAVE_FLOATINGPOINT_H 1

/* Define if you have the <ieeefp.h> header file.  */
#define HAVE_IEEEFP_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <pwd.h> header file.  */
#define HAVE_PWD_H 1

/* Define if you have the <sched.h> header file.  */
#define HAVE_SCHED_H 1

/* Define if you have the <select.h> header file.  */
/* #undef HAVE_SELECT_H */

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <synch.h> header file.  */
#define HAVE_SYNCH_H 1

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/mman.h> header file.  */
#define HAVE_SYS_MMAN_H 1

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/pte.h> header file.  */
/* #undef HAVE_SYS_PTE_H */

/* Define if you have the <sys/ptem.h> header file.  */
#define HAVE_SYS_PTEM_H 1

/* Define if you have the <sys/select.h> header file.  */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/socket.h> header file.  */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/stream.h> header file.  */
#define HAVE_SYS_STREAM_H 1

/* Define if you have the <sys/timeb.h> header file.  */
#define HAVE_SYS_TIMEB_H 1

/* Define if you have the <sys/types.h> header file.  */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/un.h> header file.  */
#define HAVE_SYS_UN_H 1

/* Define if you have the <sys/utime.h> header file.  */
#define HAVE_SYS_UTIME_H 1

/* Define if you have the <sys/vadvise.h> header file.  */
/* #undef HAVE_SYS_VADVISE_H */

/* Define if you have the <sys/wait.h> header file.  */
#define HAVE_SYS_WAIT_H 1

/* Define if you have the <term.h> header file.  */
#define HAVE_TERM_H 1

/* Define if you have the <termbits.h> header file.  */
/* #undef HAVE_TERMBITS_H */

/* Define if you have the <termcap.h> header file.  */
/* #undef HAVE_TERMCAP_H */

/* Define if you have the <termio.h> header file.  */
#define HAVE_TERMIO_H 1

/* Define if you have the <termios.h> header file.  */
#define HAVE_TERMIOS_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the <utime.h> header file.  */
#define HAVE_UTIME_H 1

/* Define if you have the <varargs.h> header file.  */
#define HAVE_VARARGS_H 1

/* Define if you have the bind library (-lbind).  */
/* #undef HAVE_LIBBIND */

/* Define if you have the c_r library (-lc_r).  */
/* #undef HAVE_LIBC_R */

/* Define if you have the compat library (-lcompat).  */
/* #undef HAVE_LIBCOMPAT */

/* Define if you have the crypt library (-lcrypt).  */
#define HAVE_LIBCRYPT 1

/* Define if you have the dl library (-ldl).  */
#define HAVE_LIBDL 1

/* Define if you have the gen library (-lgen).  */
#define HAVE_LIBGEN 1

/* Define if you have the m library (-lm).  */
#define HAVE_LIBM 1

/* Define if you have the nsl library (-lnsl).  */
#define HAVE_LIBNSL 1

/* Define if you have the nsl_r library (-lnsl_r).  */
/* #undef HAVE_LIBNSL_R */

/* Define if you have the pthread library (-lpthread).  */
#define HAVE_LIBPTHREAD 1

/* Define if you have the socket library (-lsocket).  */
#define HAVE_LIBSOCKET 1

/* Name of package */
#define PACKAGE "mysql"

/* Version number of package */
#define VERSION "3.23.10-alpha"

