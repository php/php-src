/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Sæther Bakken <ssb@php.net>                             |
   |         Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

/*

Comparing: sprintf, snprintf, slprintf, spprintf

sprintf  offers the ability to make a lot of failures since it does not know
         the size of the buffer it uses. Therefore usage of sprintf often
         results in possible entries for buffer overrun attacks. So please
         use this version only if you are sure the call is safe. sprintf
         always terminates the buffer it writes to.

snprintf knows the buffers size and will not write behind it. But you will
         have to use either a static buffer or allocate a dynamic buffer
         before being able to call the function. In other words you must
         be sure that you really know the maximum size of the buffer required.
         A bad thing is having a big maximum while in most cases you would
         only need a small buffer. If the size of the resulting string is
         longer or equal to the buffer size than the buffer is not terminated.
         The function also returns the number of chars not including the
         terminating \0 that were needed to fully comply to the print request.

slprintf same as snprintf with the difference that it actually returns the
         length printed not including the terminating \0.

spprintf is the dynamical version of snprintf. It allocates the buffer in size
         as needed and allows a maximum setting as snprintf (turn this feature
         off by setting max_len to 0). spprintf is a little bit slower than
         snprintf and offers possible memory leaks if you miss freeing the
         buffer allocated by the function. Therefore this function should be
         used where either no maximum is known or the maximum is much bigger
         than normal size required. spprintf always terminates the buffer.

Example:

 #define MAX 1024              | #define MAX 1024               | #define MAX 1024
 char buffer[MAX]              | char buffer[MAX]               | char *buffer;
                               |                                |
                               |                                | // No need to initialize buffer:
                               |                                | // spprintf ignores value of buffer
 sprintf(buffer, "test");      | snprintf(buffer, MAX, "test"); | spprintf(&buffer, MAX, "text");
                               |                                | if (!buffer)
                               |                                |   return OUT_OF_MEMORY
 // sprintf always terminates | // manual termination of       | // spprintf allays terminates buffer
 // buffer                     | // buffer *IS* required        |
                               | buffer[MAX-1] = 0;             |
 action_with_buffer(buffer);   | action_with_buffer(buffer);    | action_with_buffer(buffer);
                               |                                | efree(buffer);
*/

#ifndef SNPRINTF_H
#define SNPRINTF_H

#include <stdbool.h>

BEGIN_EXTERN_C()
PHPAPI int ap_php_slprintf(char *buf, size_t len, const char *format,...) ZEND_ATTRIBUTE_FORMAT(printf, 3, 4);
PHPAPI int ap_php_vslprintf(char *buf, size_t len, const char *format, va_list ap);
PHPAPI int ap_php_snprintf(char *, size_t, const char *, ...) ZEND_ATTRIBUTE_FORMAT(printf, 3, 4);
PHPAPI int ap_php_vsnprintf(char *, size_t, const char *, va_list ap);
PHPAPI int ap_php_vasprintf(char **buf, const char *format, va_list ap);
PHPAPI int ap_php_asprintf(char **buf, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);
PHPAPI char * php_0cvt(double value, int ndigit, char dec_point, char exponent, char *buf);
PHPAPI char * php_conv_fp(char format, double num,
		 bool add_dp, int precision, char dec_point, bool * is_negative, char *buf, size_t *len);

END_EXTERN_C()

#define php_gcvt zend_gcvt

#ifdef slprintf
#undef slprintf
#endif
#define slprintf ap_php_slprintf

#ifdef vslprintf
#undef vslprintf
#endif
#define vslprintf ap_php_vslprintf

#ifdef snprintf
#undef snprintf
#endif
#define snprintf ap_php_snprintf

#ifdef vsnprintf
#undef vsnprintf
#endif
#define vsnprintf ap_php_vsnprintf

#ifndef HAVE_VASPRINTF
#define vasprintf ap_php_vasprintf
#endif

#ifndef HAVE_ASPRINTF
#define asprintf ap_php_asprintf
#endif

typedef enum {
	LM_STD = 0,
#if SIZEOF_INTMAX_T
	LM_INTMAX_T,
#endif
#if SIZEOF_PTRDIFF_T
	LM_PTRDIFF_T,
#endif
#if SIZEOF_LONG_LONG
	LM_LONG_LONG,
#endif
	LM_SIZE_T,
	LM_LONG,
	LM_LONG_DOUBLE,
} length_modifier_e;

PHPAPI char * ap_php_conv_10(int64_t num, bool is_unsigned,
	   bool * is_negative, char *buf_end, size_t *len);

PHPAPI char * ap_php_conv_p2(uint64_t num, int nbits,
		 char format, char *buf_end, size_t *len);

#endif /* SNPRINTF_H */
