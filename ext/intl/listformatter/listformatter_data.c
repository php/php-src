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
   | Authors: Bogdan Ungureanu <bogdanungureanu21@gmail.com>              |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "listformatter_data.h"

/* {{{ void listformatter_data_init( listformatter_data* lf_data )
 * Initialize internals of listformatter_data.
 */
void listformatter_data_init( listformatter_data* lf_data )
{
    if( !lf_data )
        return;

    lf_data->ulistfmt = NULL;
    intl_error_reset( &lf_data->error );
}
/* }}} */

/* {{{ void listformatter_data_free( listformatter_data* lf_data )
 * Clean up mem allocated by internals of listformatter_data
 */
void listformatter_data_free( listformatter_data* lf_data )
{
    if( !lf_data )
        return;

    if( lf_data->ulistfmt )
        ulistfmt_close( lf_data->ulistfmt );

    lf_data->ulistfmt = NULL;
    intl_error_reset( &lf_data->error );
}
/* }}} */
