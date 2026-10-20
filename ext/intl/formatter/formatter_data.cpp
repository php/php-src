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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "formatter_data.h"

/* {{{ void formatter_data_init( formatter_data* nf_data )
 * Initialize internals of formatter_data.
 */
void formatter_data_init( formatter_data* nf_data )
{
	if( !nf_data )
		return;

	nf_data->unum                = nullptr;
	intl_error_reset( &nf_data->error );
}
/* }}} */

/* {{{ void formatter_data_free( formatter_data* nf_data )
 * Clean up mem allocated by internals of formatter_data
 */
void formatter_data_free( formatter_data* nf_data )
{
	if( !nf_data )
		return;

	delete nf_data->unum;
	nf_data->unum = nullptr;
	intl_error_reset( &nf_data->error );
}
/* }}} */
