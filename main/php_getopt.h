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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_GETOPT_H
#define PHP_GETOPT_H

#include "php.h"

/* Define structure for one recognized option (both single char and long name).
 * If short_open is '-' this is the last option. */
typedef struct _opt_struct {
	char opt_char;
	int  need_param;
	char * opt_name;
} opt_struct;

BEGIN_EXTERN_C()
/* holds the index of the latest fetched element from the opts array */
extern PHPAPI int php_optidx;
PHPAPI int php_getopt(int argc, char* const *argv, const opt_struct opts[], char **optarg, int *optind, int show_err, int arg_start);
END_EXTERN_C()

/* php_getopt will return this value if there is an error in arguments */
#define PHP_GETOPT_INVALID_ARG (-2)

#endif
