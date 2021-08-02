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
  | Authors: Keyur Govande <kgovande@gmail.com>                          |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQL_FLOAT_TO_DOUBLE_H
#define MYSQL_FLOAT_TO_DOUBLE_H

#include "main/php.h"
#include <float.h>
#include "main/snprintf.h"

#define MAX_CHAR_BUF_LEN 255

#ifndef FLT_DIG
# define FLT_DIG 6
#endif

/*
 * Convert from a 4-byte float to a 8-byte decimal by first converting
 * the float to a string (ignoring localization), and then the string to a double.
 * The decimals argument specifies the precision of the output. If decimals
 * is less than zero, then a gcvt(3) like logic is used with the significant
 * digits set to FLT_DIG i.e. 6.
 */
static inline double mysql_float_to_double(float fp4, int decimals) {
	char num_buf[MAX_CHAR_BUF_LEN]; /* Over allocated */

	if (decimals < 0) {
		zend_gcvt(fp4, FLT_DIG, '.', 'e', num_buf);
	} else {
		snprintf(num_buf, MAX_CHAR_BUF_LEN, "%.*F", decimals, fp4);
	}

	return zend_strtod(num_buf, NULL);
}

#endif /* MYSQL_FLOAT_TO_DOUBLE_H */
