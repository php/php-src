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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Parts based on Apache 1.3 SAPI module by                    |
   |          Rasmus Lerdorf and Zeev Suraski                             |
   +----------------------------------------------------------------------+
 */

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#include "php.h"
#ifdef strcasecmp
# undef strcasecmp
#endif
#ifdef strncasecmp
# undef strncasecmp
#endif
#include "php_apache.h"

AP_MODULE_DECLARE_DATA module php_module = {
	STANDARD20_MODULE_STUFF,
	create_php_config,		/* create per-directory config structure */
	merge_php_config,		/* merge per-directory config structures */
	NULL,					/* create per-server config structure */
	NULL,					/* merge per-server config structures */
	php_dir_cmds,			/* command apr_table_t */
	php_ap2_register_hook,	/* register hooks */
	0						/* flags */
};
