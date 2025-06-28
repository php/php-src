/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
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
