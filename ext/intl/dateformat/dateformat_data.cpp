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

	datef_data->udatf = nullptr;
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

	datef_data->udatf = nullptr;
	intl_error_reset( &datef_data->error );
}
/* }}} */

/* {{{ dateformat_data* dateformat_data_create()
 * Allocate memory for dateformat_data and initialize it with default values.
 */
dateformat_data* dateformat_data_create( void )
{
	dateformat_data* datef_data = reinterpret_cast<dateformat_data *>(ecalloc( 1, sizeof(dateformat_data) ));

	dateformat_data_init( datef_data );

	return datef_data;
}
/* }}} */
