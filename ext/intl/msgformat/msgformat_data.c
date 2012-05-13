/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#include <unicode/ustring.h>
#include "msgformat_data.h"

/* {{{ void msgformat_data_init( msgformat_data* mf_data )
 * Initialize internals of msgformat_data.
 */
void msgformat_data_init( msgformat_data* mf_data TSRMLS_DC )
{
	if( !mf_data )
		return;

	mf_data->umsgf = NULL;
	mf_data->orig_format = NULL;
	mf_data->arg_types = NULL;
	intl_error_reset( &mf_data->error TSRMLS_CC );
}
/* }}} */

/* {{{ void msgformat_data_free( msgformat_data* mf_data )
 * Clean up memory allocated for msgformat_data
 */
void msgformat_data_free(msgformat_data* mf_data TSRMLS_DC)
{
	if (!mf_data)
		return;

	if (mf_data->umsgf)
		umsg_close(mf_data->umsgf);

	if (mf_data->orig_format) {
		efree(mf_data->orig_format);
		mf_data->orig_format = NULL;
	}

	if (mf_data->arg_types) {
		zend_hash_destroy(mf_data->arg_types);
		efree(mf_data->arg_types);
		mf_data->arg_types = NULL;
	}

	mf_data->umsgf = NULL;
	intl_error_reset(&mf_data->error TSRMLS_CC);
}
/* }}} */

/* {{{ msgformat_data* msgformat_data_create()
 * Allocate memory for msgformat_data and initialize it with default values.
 */
msgformat_data* msgformat_data_create( TSRMLS_D )
{
	msgformat_data* mf_data = ecalloc( 1, sizeof(msgformat_data) );

	msgformat_data_init( mf_data TSRMLS_CC );

	return mf_data;
}
/* }}} */

int msgformat_fix_quotes(UChar **spattern, uint32_t *spattern_len, UErrorCode *ec) 
{
	if(*spattern && *spattern_len && u_strchr(*spattern, (UChar)'\'')) {
		UChar *npattern = emalloc(sizeof(UChar)*(2*(*spattern_len)+1));
		uint32_t npattern_len;
		npattern_len = umsg_autoQuoteApostrophe(*spattern, *spattern_len, npattern, 2*(*spattern_len)+1, ec);
		efree(*spattern);
		if( U_FAILURE(*ec) )
		{
			return FAILURE;
		}
		npattern = erealloc(npattern, sizeof(UChar)*(npattern_len+1));
		*spattern = npattern;
		*spattern_len = npattern_len;
	}
	return SUCCESS;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
