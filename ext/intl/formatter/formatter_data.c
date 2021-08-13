/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "formatter_data.h"

/* {{{ void formatter_data_init( formatter_data* nf_data )
 * Initialize internals of formatter_data.
 */
void formatter_data_init( formatter_data* nf_data )
{
	if( !nf_data )
		return;

	nf_data->unum                = NULL;
	intl_error_reset( &nf_data->error );
}
/* }}} */

/* {{{ void formatter_data_free( formatter_data* nf_data )
 * Clean up mem allocted by internals of formatter_data
 */
void formatter_data_free( formatter_data* nf_data )
{
	if( !nf_data )
		return;

	if( nf_data->unum )
		unum_close( nf_data->unum );

	nf_data->unum = NULL;
	intl_error_reset( &nf_data->error );
}
/* }}} */

/* {{{ formatter_data* formatter_data_create()
 * Alloc mem for formatter_data and initialize it with default values.
 */
formatter_data* formatter_data_create( void )
{
	formatter_data* nf_data = ecalloc( 1, sizeof(formatter_data) );

	formatter_data_init( nf_data );

	return nf_data;
}
/* }}} */
