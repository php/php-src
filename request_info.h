/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead (jimw@php.net)                                  |
   +----------------------------------------------------------------------+
 */

#ifndef _REQUEST_INFO_H_
#define _REQUEST_INFO_H_

typedef struct {
	char *script_name;
	char *current_user;
	int current_user_length;
	const char *script_filename;
	char *php_argv0;
} php_request_info;

#ifndef THREAD_SAFE
PHPAPI extern php_request_info request_info;
#endif

extern int php_init_request_info(void *conf);
extern int php_destroy_request_info(void *conf);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
