/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mysqli_structs.h"

/* {{{ proto bool mysqli_embedded_server_start(bool start, array arguments, array groups)
   initialize and start embedded server */
PHP_FUNCTION(mysqli_embedded_server_start)
{
#ifdef HAVE_EMBEDDED_MYSQLI
	long start;
	zval *args;
	zval *grps;

	int	argc = 0;
	char **arguments;
	char **groups;
	HashPosition pos;
	int index, rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "laa", &start, &args, &grps) == FAILURE) {
		return;
	}

	if (!start) {
		mysql_server_init(-1,NULL, NULL);
		RETURN_TRUE;
	}
	/* get arguments */
	if ((argc = zend_hash_num_elements(HASH_OF(args)))) {
		arguments = safe_emalloc(sizeof(char *), argc + 1, 0);
		arguments[0] = NULL;

		zend_hash_internal_pointer_reset_ex(HASH_OF(args), &pos);

		for (index = 0;; zend_hash_move_forward_ex(HASH_OF(args), &pos))	{
			zval **item;

			if (zend_hash_get_current_data_ex(HASH_OF(args), (void **) &item, &pos) == FAILURE) {
				break;
			}

			convert_to_string_ex(item);

			arguments[++index] = Z_STRVAL_PP(item);
		}
		argc++;
	}

	/* get groups */
	if ((zend_hash_num_elements(HASH_OF(grps)))) {
		groups = safe_emalloc(sizeof(char *), zend_hash_num_elements(HASH_OF(grps)) + 1, 0);
		groups[0] = NULL;

		zend_hash_internal_pointer_reset_ex(HASH_OF(grps), &pos);

		for (index = 0;; zend_hash_move_forward_ex(HASH_OF(grps), &pos))	{
			zval ** item;

			if (zend_hash_get_current_data_ex(HASH_OF(grps), (void **) &item, &pos) == FAILURE) {
				break;
			}

			convert_to_string_ex(item);

			groups[++index] = Z_STRVAL_PP(item);
		}
		groups[index] = NULL;
	} else {
		groups = safe_emalloc(sizeof(char *), 1, 0);
		groups[0] = NULL;
	}

	rc = mysql_server_init(argc, arguments, groups);

	if (argc) {
		efree(arguments);
	}
	efree(groups);

	if (rc) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
#endif
}
/* }}} */

/* {{{ proto void mysqli_embedded_server_end(void)
*/
PHP_FUNCTION(mysqli_embedded_server_end)
{
#ifdef HAVE_MYSQLI_EMBEDDED
	mysql_server_end();
#endif
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
