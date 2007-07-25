/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_statistics.h"


typedef struct st_mysqlnd_string
{
	char	*s;
	size_t	l;
} MYSQLND_STRING;


#define STR_W_LEN(str)  str, (sizeof(str) - 1)

/* {{{ mysqlnd_stats_values_names 
 */
static
const MYSQLND_STRING mysqlnd_stats_values_names[STAT_LAST] =
{
	{ STR_W_LEN("bytes_sent") },
	{ STR_W_LEN("bytes_received") }, 
	{ STR_W_LEN("packets_sent") },
	{ STR_W_LEN("packets_received") },
	{ STR_W_LEN("protocol_overhead_in") },
	{ STR_W_LEN("protocol_overhead_out") },
	{ STR_W_LEN("result_set_queries") },
	{ STR_W_LEN("non_result_set_queries") },
	{ STR_W_LEN("no_index_used") },
	{ STR_W_LEN("bad_index_used") },
	{ STR_W_LEN("buffered_sets") },
	{ STR_W_LEN("unbuffered_sets") },
	{ STR_W_LEN("ps_buffered_sets") },
	{ STR_W_LEN("ps_unbuffered_sets") },
	{ STR_W_LEN("flushed_normal_sets") },
	{ STR_W_LEN("flushed_ps_sets") },
	{ STR_W_LEN("rows_fetched_from_server") },
	{ STR_W_LEN("rows_fetched_from_client") },
	{ STR_W_LEN("rows_skipped") },
	{ STR_W_LEN("copy_on_write_saved") },
	{ STR_W_LEN("copy_on_write_performed") },
	{ STR_W_LEN("command_buffer_too_small") },
	{ STR_W_LEN("connect_success") },
	{ STR_W_LEN("connect_failure") },
	{ STR_W_LEN("connection_reused") },
	{ STR_W_LEN("explicit_close") },
	{ STR_W_LEN("implicit_close") },
	{ STR_W_LEN("disconnect_close") },
	{ STR_W_LEN("in_middle_of_command_close") },
	{ STR_W_LEN("explicit_free_result") },
	{ STR_W_LEN("implicit_free_result") },
	{ STR_W_LEN("explicit_stmt_close") },
	{ STR_W_LEN("implicit_stmt_close") },
};
/* }}} */


void
mysqlnd_fill_stats_hash(const MYSQLND_STATS * const stats, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	unsigned int i;

	mysqlnd_array_init(return_value, STAT_LAST);
	for (i = 0; i < STAT_LAST; i++) {
		char tmp[22];
		
		sprintf((char *)&tmp, MYSQLND_LLU_SPEC, stats->values[i]);
#if PHP_MAJOR_VERSION >= 6
		if (UG(unicode)) {
			UChar *ustr, *tstr;
			int ulen, tlen;

			zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, mysqlnd_stats_values_names[i].s,
									mysqlnd_stats_values_names[i].l + 1 TSRMLS_CC);
			zend_string_to_unicode(UG(utf8_conv), &tstr, &tlen, tmp, strlen(tmp) + 1 TSRMLS_CC);
			add_u_assoc_unicode_ex(return_value, IS_UNICODE, ZSTR(ustr), ulen, tstr, 1);
			efree(ustr);
			efree(tstr);
		} else
#endif		
		{	
			add_assoc_string_ex(return_value, mysqlnd_stats_values_names[i].s,
								mysqlnd_stats_values_names[i].l + 1, tmp, 1);
		}
	}
}


/* {{{ _mysqlnd_get_client_stats */
PHPAPI void _mysqlnd_get_client_stats(zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	MYSQLND_STATS stats, *stats_ptr = mysqlnd_global_stats;

	if (!stats_ptr) {
		memset(&stats, 0, sizeof(stats));
		stats_ptr = &stats;
	}
	mysqlnd_fill_stats_hash(stats_ptr, return_value TSRMLS_CC ZEND_FILE_LINE_CC);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
