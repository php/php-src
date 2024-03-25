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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dateformat_data.h"

/* {{{ void dateformat_data_init( dateformat_data* datef_data )
 * Initialize internals of dateformat_data.
 */
void dateformat_data_init( dateformat_data* datef_data )
{
	if( !datef_data )
		return;

	datef_data->udatf = NULL;
	intl_error_reset( &datef_data->error );
}
/* }}} */

/* {{{ void dateformat_data_free( dateformat_data* datef_data )
 * Clean up memory allocated for dateformat_data
 */
void dateformat_data_free( dateformat_data* datef_data )
{
	if( !datef_data )
		return;

	if( datef_data->udatf )
		udat_close( datef_data->udatf );

	datef_data->udatf = NULL;
	intl_error_reset( &datef_data->error );
}
/* }}} */

/* {{{ dateformat_data* dateformat_data_create()
 * Allocate memory for dateformat_data and initialize it with default values.
 */
dateformat_data* dateformat_data_create( void )
{
	dateformat_data* datef_data = ecalloc( 1, sizeof(dateformat_data) );

	dateformat_data_init( datef_data );

	return datef_data;
}
/* }}} */
