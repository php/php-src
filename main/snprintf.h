/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Sæther Bakken <ssb@fast.no>                             |
   +----------------------------------------------------------------------+
 */

/*

Comparing: sprintf, snprintf, spprintf 

sprintf  offers the ability to make a lot of failures since it does not know
         the size of the buffer it uses. Therefore usage of sprintf often
         results in possible entries for buffer overrun attacks. So please
         use this version only if you are sure the call is safe. sprintf
         allways terminstes the buffer it writes to.

snprintf knows the buffers size and will not write behind it. But you will
         have to use either a static buffer or allocate a dynamic buffer
         before beeing able to call the function. In other words you must
         be sure that you really know the maximum size of the buffer required.
         A bad thing is having a big maximum while in most cases you would
         only need a small buffer. If the size of the resulting string is 
         longer or equal to the buffer size than the buffer is not terminated.

spprintf is the dynamical version of snprintf. It allocates the buffer in size
         as needed and allows a maximum setting as snprintf (turn this feature
         off by setting max_len to 0). spprintf is a little bit slower than
         snprintf and offers possible memory leakes if you miss freeing the 
         buffer allocated by the function. Therfore this function should be 
         used where either no maximum is known or the maximum is much bigger
         than normal size required. spprintf allways terminates the buffer.

Example:

 #define MAX 1024              | #define MAX 1024               | #define MAX 1024
 char buffer[MAX]              | char buffer[MAX]               | char *buffer;
                               |                                |
                               |                                | // No need to initialize buffer:
                               |                                | // spprintf ignores value of buffer
 sprintf(buffer, "test");      | snprintf(buffer, MAX, "test"); | spprintf(&buffer, MAX, "text");
                               |                                | if (!buffer)
                               |                                |   return OUT_OF_MEMORY
 // sprintf allways terminates | // manual termination of       | // spprintf allays terminates buffer
 // buffer                     | // buffer *IS* required        |   
                               | buffer[MAX-1] = 0;             | 
 action_with_buffer(buffer);   | action_with_buffer(buffer);    | action_with_buffer(buffer);
                               |                                | efree(buffer);
*/

#ifndef SNPRINTF_H
#define SNPRINTF_H

#if !defined(HAVE_SNPRINTF) || PHP_BROKEN_SNPRINTF
extern int ap_php_snprintf(char *, size_t, const char *, ...);
#define snprintf ap_php_snprintf
#endif

#if !defined(HAVE_VSNPRINTF) || PHP_BROKEN_VSNPRINTF
extern int ap_php_vsnprintf(char *, size_t, const char *, va_list ap);
#define vsnprintf ap_php_vsnprintf
#endif

#if PHP_BROKEN_SPRINTF
int php_sprintf (char* s, const char* format, ...);
#define sprintf php_sprintf
#endif

typedef enum {
	NO = 0, YES = 1
} boolean_e;

extern char * ap_php_cvt(double arg, int ndigits, int *decpt, int *sign, int eflag, char *buf);
extern char * ap_php_ecvt(double arg, int ndigits, int *decpt, int *sign, char *buf);
extern char * ap_php_fcvt(double arg, int ndigits, int *decpt, int *sign, char *buf);
extern char * ap_php_gcvt(double number, int ndigit, char *buf, boolean_e altform);

#define WIDE_INT		long
typedef WIDE_INT wide_int;
typedef unsigned WIDE_INT u_wide_int;

typedef int bool_int;

extern char * ap_php_conv_10(register wide_int num, register bool_int is_unsigned,
	   register bool_int * is_negative, char *buf_end, register int *len);

extern char * ap_php_conv_fp(register char format, register double num,
		 boolean_e add_dp, int precision, bool_int * is_negative, char *buf, int *len);

extern char * ap_php_conv_p2(register u_wide_int num, register int nbits,
		 char format, char *buf_end, register int *len);


#endif /* SNPRINTF_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
