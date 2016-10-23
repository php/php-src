/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB
This file is public domain and comes with NO WARRANTY of any kind */

/*
  Parts of the original, which are not applicable to mysqlnd have been removed.

  With small modifications, mostly casting but adding few more macros by
  Andrey Hristov <andrey@php.net> . The additions are in the public domain and
  were added to improve the header file, to get it more consistent.
*/

#ifndef MYSQLND_PORTABILITY_H
#define MYSQLND_PORTABILITY_H



/* Comes from global.h as OFFSET, renamed to STRUCT_OFFSET */
#define STRUCT_OFFSET(t, f)   ((size_t)(char *)&((t *)0)->f)

#ifndef __attribute
#if !defined(__GNUC__)
#define __attribute(A)
#endif
#endif

#ifdef __CYGWIN__
/* We use a Unix API, so pretend it's not Windows */
#undef WIN
#undef WIN32
#undef _WIN
#undef _WIN32
#undef _WIN64
#undef __WIN__
#undef __WIN32__
#endif /* __CYGWIN__ */

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
#  include "ext/mysqlnd/config-win.h"
#endif /* _WIN32... */

#if __STDC_VERSION__ < 199901L && !defined(atoll)
  /* "inline" is a keyword */
  #define atoll atol
#endif

#include "php_stdint.h"

#if SIZEOF_LONG_LONG > 4 && !defined(_LONG_LONG)
#define _LONG_LONG 1        /* For AIX string library */
#endif


/* Go around some bugs in different OS and compilers */
#if defined(_HPUX_SOURCE) && defined(HAVE_SYS_STREAM_H)
#include <sys/stream.h>        /* HPUX 10.20 defines ulong here. UGLY !!! */
#define HAVE_ULONG
#endif


#if SIZEOF_LONG_LONG > 4
#define HAVE_LONG_LONG 1
#endif

#ifdef PHP_WIN32
#define MYSQLND_LLU_SPEC "%I64u"
#define MYSQLND_LL_SPEC "%I64d"
#define MYSQLND_SZ_T_SPEC "%Id"
#ifndef L64
#define L64(x) x##i64
#endif
#else

#if __i386__
#define MYSQLND_LL_SPEC	"%lli"
#define MYSQLND_LLU_SPEC "%llu"
#endif

#if __ia64__
#define MYSQLND_LL_SPEC	"%li"
#define MYSQLND_LLU_SPEC "%lu"
#endif

#if __powerpc64__ || __ppc64__
#define MYSQLND_LL_SPEC	"%li"
#define MYSQLND_LLU_SPEC "%lu"
#endif

#if (__powerpc__ || __ppc__ ) && !(__powerpc64__ || __ppc64__)
#define MYSQLND_LL_SPEC	"%lli"
#define MYSQLND_LLU_SPEC "%llu"
#endif

#if __x86_64__
#define MYSQLND_LL_SPEC	"%li"
#define MYSQLND_LLU_SPEC "%lu"
#endif

#if __s390x__
#define MYSQLND_LL_SPEC	"%li"
#define MYSQLND_LLU_SPEC "%lu"
#endif

#if __s390__ && !__s390x__
#define MYSQLND_LL_SPEC	"%lli"
#define MYSQLND_LLU_SPEC "%llu"
#endif

#ifdef _AIX
#define MYSQLND_LL_SPEC "%lli"
#define MYSQLND_LLU_SPEC "%llu"
#endif

#ifndef MYSQLND_LL_SPEC
  #if SIZEOF_LONG == 8
    #define MYSQLND_LL_SPEC "%li"
  #elif SIZEOF_LONG == 4
    #define MYSQLND_LL_SPEC "%lli"
  #endif
#endif

#ifndef MYSQLND_LLU_SPEC
  #if SIZEOF_LONG == 8
    #define MYSQLND_LLU_SPEC "%lu"
  #elif SIZEOF_LONG == 4
    #define MYSQLND_LLU_SPEC "%llu"
   #endif
#endif /* MYSQLND_LLU_SPEC*/


#define MYSQLND_SZ_T_SPEC "%zd"
#ifndef L64
#define L64(x) x##LL
#endif
#endif


#define int1store(T,A)	do { *((int8_t*) (T)) = (A); } while(0)
#define uint1korr(A)	(*(((uint8_t*)(A))))

/* Bit values are sent in reverted order of bytes, compared to normal !!! */
#define bit_uint2korr(A) ((uint16_t) (((uint16_t) (((unsigned char*) (A))[1])) +\
                                   ((uint16_t) (((unsigned char*) (A))[0]) << 8)))
#define bit_uint3korr(A) ((uint32_t) (((uint32_t) (((unsigned char*) (A))[2])) +\
                                   (((uint32_t) (((unsigned char*) (A))[1])) << 8) +\
                                   (((uint32_t) (((unsigned char*) (A))[0])) << 16)))
#define bit_uint4korr(A) ((uint32_t) (((uint32_t) (((unsigned char*) (A))[3])) +\
                                   (((uint32_t) (((unsigned char*) (A))[2])) << 8) +\
                                   (((uint32_t) (((unsigned char*) (A))[1])) << 16) +\
                                   (((uint32_t) (((unsigned char*) (A))[0])) << 24)))
#define bit_uint5korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[4])) +\
                                    (((uint32_t) (((unsigned char*) (A))[3])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[2])) << 16) +\
                                   (((uint32_t) (((unsigned char*) (A))[1])) << 24)) +\
                                    (((uint64_t) (((unsigned char*) (A))[0])) << 32))
#define bit_uint6korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[5])) +\
                                    (((uint32_t) (((unsigned char*) (A))[4])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[3])) << 16) +\
                                    (((uint32_t) (((unsigned char*) (A))[2])) << 24)) +\
                        (((uint64_t) (((uint32_t) (((unsigned char*) (A))[1])) +\
                                    (((uint32_t) (((unsigned char*) (A))[0]) << 8)))) <<\
                                     32))
#define bit_uint7korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[6])) +\
                                    (((uint32_t) (((unsigned char*) (A))[5])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[4])) << 16) +\
                                   (((uint32_t) (((unsigned char*) (A))[3])) << 24)) +\
                        (((uint64_t) (((uint32_t) (((unsigned char*) (A))[2])) +\
                                    (((uint32_t) (((unsigned char*) (A))[1])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[0])) << 16))) <<\
                                     32))
#define bit_uint8korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[7])) +\
                                    (((uint32_t) (((unsigned char*) (A))[6])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[5])) << 16) +\
                                    (((uint32_t) (((unsigned char*) (A))[4])) << 24)) +\
                        (((uint64_t) (((uint32_t) (((unsigned char*) (A))[3])) +\
                                    (((uint32_t) (((unsigned char*) (A))[2])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[1])) << 16) +\
                                    (((uint32_t) (((unsigned char*) (A))[0])) << 24))) <<\
                                    32))


/*
** Define-funktions for reading and storing in machine independent format
**  (low byte first)
*/

/* Optimized store functions for Intel x86, non-valid for WIN64. __i386__ is GCC */
#if defined(__i386__) && !defined(_WIN64)
#define sint2korr(A)    (*((int16_t *) (A)))
#define sint3korr(A)    ((int32_t) ((((zend_uchar) (A)[2]) & 128) ? \
                   (((uint32_t) 255L << 24) | \
                   (((uint32_t) (zend_uchar) (A)[2]) << 16) |\
                   (((uint32_t) (zend_uchar) (A)[1]) << 8) | \
                    ((uint32_t) (zend_uchar) (A)[0])) : \
                   (((uint32_t) (zend_uchar) (A)[2]) << 16) |\
                   (((uint32_t) (zend_uchar) (A)[1]) << 8) | \
                    ((uint32_t) (zend_uchar) (A)[0])))
#define sint4korr(A)  (*((zend_long *) (A)))

#define uint2korr(A)  (*((uint16_t *) (A)))
#define uint3korr(A)  (uint32_t) (((uint32_t) ((zend_uchar) (A)[0])) +\
                               (((uint32_t) ((zend_uchar) (A)[1])) << 8) +\
                               (((uint32_t) ((zend_uchar) (A)[2])) << 16))
#define uint4korr(A)  (*((zend_ulong *) (A)))



#define uint8korr(A)    (*((uint64_t *) (A)))
#define sint8korr(A)    (*((int64_t *) (A)))
#define int2store(T,A)    *((uint16_t*) (T))= (uint16_t) (A)
#define int3store(T,A)   { \
                  *(T)=  (zend_uchar) ((A));\
                  *(T+1)=(zend_uchar) (((uint32_t) (A) >> 8));\
                  *(T+2)=(zend_uchar) (((A) >> 16)); }
#define int4store(T,A)    *((zend_long *) (T))= (zend_long) (A)
#define int5store(T,A)    { \
              *((zend_uchar *)(T))= (zend_uchar)((A));\
              *(((zend_uchar *)(T))+1)=(zend_uchar) (((A) >> 8));\
              *(((zend_uchar *)(T))+2)=(zend_uchar) (((A) >> 16));\
              *(((zend_uchar *)(T))+3)=(zend_uchar) (((A) >> 24)); \
              *(((zend_uchar *)(T))+4)=(zend_uchar) (((A) >> 32)); }

/* From Andrey Hristov, based on int5store() */
#define int6store(T,A)    { \
              *(((zend_uchar *)(T)))= (zend_uchar)((A));\
              *(((zend_uchar *)(T))+1))=(zend_uchar) (((A) >> 8));\
              *(((zend_uchar *)(T))+2))=(zend_uchar) (((A) >> 16));\
              *(((zend_uchar *)(T))+3))=(zend_uchar) (((A) >> 24)); \
              *(((zend_uchar *)(T))+4))=(zend_uchar) (((A) >> 32)); \
              *(((zend_uchar *)(T))+5))=(zend_uchar) (((A) >> 40)); }

#define int8store(T,A)    *((uint64_t *) (T))= (uint64_t) (A)

typedef union {
  double v;
  zend_long m[2];
} float8get_union;
#define float8get(V,M)    { ((float8get_union *)&(V))->m[0] = *((zend_long*) (M)); \
                            ((float8get_union *)&(V))->m[1] = *(((zend_long*) (M))+1); }
#define float8store(T,V) { *((zend_long *) (T))     = ((float8get_union *)&(V))->m[0]; \
                           *(((zend_long *) (T))+1) = ((float8get_union *)&(V))->m[1]; }
#define float4get(V,M)	{ *((float *) &(V)) = *((float*) (M)); }
/* From Andrey Hristov based on float8get */
#define floatget(V,M)    memcpy((char*) &(V),(char*) (M),sizeof(float))
#endif /* __i386__ */


/* If we haven't defined sint2korr, which is because the platform is not x86 or it's WIN64 */
#ifndef sint2korr
#define sint2korr(A)    (int16_t) (((int16_t) ((zend_uchar) (A)[0])) +\
                                 ((int16_t) ((int16_t) (A)[1]) << 8))
#define sint3korr(A)    ((int32_t) ((((zend_uchar) (A)[2]) & 128) ? \
                  (((uint32_t) 255L << 24) | \
                  (((uint32_t) (zend_uchar) (A)[2]) << 16) |\
                  (((uint32_t) (zend_uchar) (A)[1]) << 8) | \
                   ((uint32_t) (zend_uchar) (A)[0])) : \
                  (((uint32_t) (zend_uchar) (A)[2]) << 16) |\
                  (((uint32_t) (zend_uchar) (A)[1]) << 8) | \
                  ((uint32_t) (zend_uchar) (A)[0])))
#define sint4korr(A)  (int32_t) (((int32_t) ((zend_uchar) (A)[0])) +\
                              (((int32_t) ((zend_uchar) (A)[1]) << 8)) +\
                              (((int32_t) ((zend_uchar) (A)[2]) << 16)) +\
                              (((int32_t) ((int16_t) (A)[3]) << 24)))

#define sint8korr(A)  (int64_t) uint8korr(A)
#define uint2korr(A)  (uint16_t) (((uint16_t) ((zend_uchar) (A)[0])) +\
                               ((uint16_t) ((zend_uchar) (A)[1]) << 8))
#define uint3korr(A)  (uint32_t) (((uint32_t) ((zend_uchar) (A)[0])) +\
                               (((uint32_t) ((zend_uchar) (A)[1])) << 8) +\
                               (((uint32_t) ((zend_uchar) (A)[2])) << 16))
#define uint4korr(A)  (uint32_t) (((uint32_t) ((zend_uchar) (A)[0])) +\
                               (((uint32_t) ((zend_uchar) (A)[1])) << 8) +\
                               (((uint32_t) ((zend_uchar) (A)[2])) << 16) +\
                               (((uint32_t) ((zend_uchar) (A)[3])) << 24))

#define uint8korr(A)	((uint64_t)(((uint32_t) ((zend_uchar) (A)[0])) +\
									(((uint32_t) ((zend_uchar) (A)[1])) << 8) +\
									(((uint32_t) ((zend_uchar) (A)[2])) << 16) +\
									(((uint32_t) ((zend_uchar) (A)[3])) << 24)) +\
									(((uint64_t) (((uint32_t) ((zend_uchar) (A)[4])) +\
									(((uint32_t) ((zend_uchar) (A)[5])) << 8) +\
									(((uint32_t) ((zend_uchar) (A)[6])) << 16) +\
									(((uint32_t) ((zend_uchar) (A)[7])) << 24))) << 32))


#define int2store(T,A)  do { uint32_t def_temp= (uint32_t) (A) ;\
                  *((zend_uchar*) (T))  =  (zend_uchar)(def_temp); \
                  *((zend_uchar*) (T+1)) = (zend_uchar)((def_temp >> 8)); } while (0)
#define int3store(T,A)  do { /*lint -save -e734 */\
                  *(((char *)(T)))   = (char) ((A));\
                  *(((char *)(T))+1) = (char) (((A) >> 8));\
                  *(((char *)(T))+2) = (char) (((A) >> 16)); \
                  /*lint -restore */} while (0)
#define int4store(T,A)  do { \
                  *(((char *)(T)))   = (char) ((A));\
                  *(((char *)(T))+1) = (char) (((A) >> 8));\
                  *(((char *)(T))+2) = (char) (((A) >> 16));\
                  *(((char *)(T))+3) = (char) (((A) >> 24)); } while (0)
#define int5store(T,A)  do { \
                  *(((char *)(T)))   = (char)((A));\
                  *(((char *)(T))+1) = (char)(((A) >> 8));\
                  *(((char *)(T))+2) = (char)(((A) >> 16));\
                  *(((char *)(T))+3) = (char)(((A) >> 24)); \
                  *(((char *)(T))+4) = (char)(((A) >> 32)); } while (0)
/* Based on int5store() from Andrey Hristov */
#define int6store(T,A)  do { \
                  *(((char *)(T)))   = (char)((A));\
                  *(((char *)(T))+1) = (char)(((A) >> 8));\
                  *(((char *)(T))+2) = (char)(((A) >> 16));\
                  *(((char *)(T))+3) = (char)(((A) >> 24)); \
                  *(((char *)(T))+4) = (char)(((A) >> 32)); \
                  *(((char *)(T))+5) = (char)(((A) >> 40)); } while (0)
#define int8store(T,A)        { uint32_t def_temp= (uint32_t) (A), def_temp2= (uint32_t) ((A) >> 32); \
                  int4store((T),def_temp); \
                  int4store((T+4),def_temp2); \
                }
#ifdef WORDS_BIGENDIAN
#define float4get(V,M)   do { float def_temp;\
                          ((char*) &def_temp)[0] = (M)[3];\
                          ((char*) &def_temp)[1] = (M)[2];\
                          ((char*) &def_temp)[2] = (M)[1];\
                          ((char*) &def_temp)[3] = (M)[0];\
                          (V)=def_temp; } while (0)
#define float8store(T,V)  do { \
                           *(((char *)(T)))   = (char) ((char *) &(V))[7];\
                           *(((char *)(T))+1) = (char) ((char *) &(V))[6];\
                           *(((char *)(T))+2) = (char) ((char *) &(V))[5];\
                           *(((char *)(T))+3) = (char) ((char *) &(V))[4];\
                           *(((char *)(T))+4) = (char) ((char *) &(V))[3];\
                           *(((char *)(T))+5) = (char) ((char *) &(V))[2];\
                           *(((char *)(T))+6) = (char) ((char *) &(V))[1];\
                           *(((char *)(T))+7) = (char) ((char *) &(V))[0]; } while (0)

#define float8get(V,M)   do { double def_temp;\
                          ((char*) &def_temp)[0] = (M)[7];\
                          ((char*) &def_temp)[1] = (M)[6];\
                          ((char*) &def_temp)[2] = (M)[5];\
                          ((char*) &def_temp)[3] = (M)[4];\
                          ((char*) &def_temp)[4] = (M)[3];\
                          ((char*) &def_temp)[5] = (M)[2];\
                          ((char*) &def_temp)[6] = (M)[1];\
                          ((char*) &def_temp)[7] = (M)[0];\
                          (V) = def_temp; \
                         } while (0)
#else
#define float4get(V,M)   memcpy((char*) &(V),(char*) (M),sizeof(float))

#if defined(__FLOAT_WORD_ORDER) && (__FLOAT_WORD_ORDER == __BIG_ENDIAN)
#define float8store(T,V)  do { \
                         *(((char *)(T)))= ((char *) &(V))[4];\
                         *(((char *)(T))+1)=(char) ((char *) &(V))[5];\
                         *(((char *)(T))+2)=(char) ((char *) &(V))[6];\
                         *(((char *)(T))+3)=(char) ((char *) &(V))[7];\
                         *(((char *)(T))+4)=(char) ((char *) &(V))[0];\
                         *(((char *)(T))+5)=(char) ((char *) &(V))[1];\
                         *(((char *)(T))+6)=(char) ((char *) &(V))[2];\
                         *(((char *)(T))+7)=(char) ((char *) &(V))[3];} while (0)
#define float8get(V,M) do { double def_temp;\
                         ((char*) &def_temp)[0]=(M)[4];\
                         ((char*) &def_temp)[1]=(M)[5];\
                         ((char*) &def_temp)[2]=(M)[6];\
                         ((char*) &def_temp)[3]=(M)[7];\
                         ((char*) &def_temp)[4]=(M)[0];\
                         ((char*) &def_temp)[5]=(M)[1];\
                         ((char*) &def_temp)[6]=(M)[2];\
                         ((char*) &def_temp)[7]=(M)[3];\
                         (V) = def_temp; } while (0)
#endif /* __FLOAT_WORD_ORDER */

#endif /* WORDS_BIGENDIAN */

#endif /* sint2korr */
/* To here if the platform is not x86 or it's WIN64 */


/* Define-funktions for reading and storing in machine format from/to
   short/long to/from some place in memory V should be a (not
   register) variable, M is a pointer to byte */

#ifndef float8get

#ifdef WORDS_BIGENDIAN
#define float8get(V,M)		memcpy((char*) &(V),(char*)  (M), sizeof(double))
#define float8store(T,V)	memcpy((char*)  (T),(char*) &(V), sizeof(double))
#else
#define float8get(V,M)    memcpy((char*) &(V),(char*) (M),sizeof(double))
#define float8store(T,V)  memcpy((char*) (T),(char*) &(V),sizeof(double))
#endif /* WORDS_BIGENDIAN */

#endif /* float8get */

#endif /* MYSQLND_PORTABILITY_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
