/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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
