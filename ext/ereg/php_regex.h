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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_REGEX_H
#define PHP_REGEX_H

/*
 * REGEX means:
 * 0.. system regex
 * 1.. bundled regex
 */

#if REGEX
/* get aliases */
#include "regex/regex_extra.h"
#include "regex/regex.h"

/* get rid of aliases */
#define PHP_NO_ALIASES
#include "regex/regex_extra.h"
#undef PHP_NO_ALIASES

#undef _PCREPOSIX_H
#define _PCREPOSIX_H 1

#ifndef _REGEX_H
#define _REGEX_H 1				/* this should stop Apache from loading the system version of regex.h */
#endif
#ifndef _REGEX_H_
#define _REGEX_H_ 1
#endif
#ifndef _RX_H
#define _RX_H 1				  	/* Try defining these for Linux to	*/
#endif
#ifndef __REGEXP_LIBRARY_H__
#define __REGEXP_LIBRARY_H__ 1 	/* avoid Apache including regex.h	*/
#endif
#ifndef _H_REGEX
#define _H_REGEX 1              /* This one is for AIX */
#endif
#elif REGEX == 0
#include <regex.h>
#ifndef _REGEX_H_
#define _REGEX_H_ 1
#endif
#endif

#endif /* PHP_REGEX_H */
