/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef NETWARE

/* ------------------------------------------------------------------
 * These functions are to be called when the shared NLM starts and
 * stops.  By using these functions instead of defining a main()
 * and calling ExitThread(TSR_THREAD, 0), the load time of the
 * shared NLM is faster and memory size reduced.
 *
 * You may also want to override these in your own Apache module
 * to do any cleanup other than the mechanism Apache modules provide.
 * ------------------------------------------------------------------
 */


#ifdef __GNUC__
#include <string.h>        /* memset */
extern char _edata, _end ; /* end of DATA (start of BSS), end of BSS */
#endif

int _lib_start()
{
/*	printf("Inside _lib_start\n");*/
#ifdef __GNUC__
    memset (&_edata, 0, &_end - &_edata);
#endif
    return 0;
}

int _lib_stop()
{
/*	printf("Inside _lib_stop\n");*/
    return 0;
}

#endif	/* NETWARE */
