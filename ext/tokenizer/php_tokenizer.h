/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_TOKENIZER_H
#define PHP_TOKENIZER_H

extern zend_module_entry tokenizer_module_entry;
#define phpext_tokenizer_ptr &tokenizer_module_entry

#ifdef PHP_WIN32
#define PHP_TOKENIZER_API __declspec(dllexport)
#else
#define PHP_TOKENIZER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(tokenizer);
PHP_MSHUTDOWN_FUNCTION(tokenizer);
PHP_RINIT_FUNCTION(tokenizer);
PHP_RSHUTDOWN_FUNCTION(tokenizer);
PHP_MINFO_FUNCTION(tokenizer);

PHP_FUNCTION(confirm_tokenizer_compiled);	/* For testing, remove later. */
PHP_FUNCTION(token_get_all);
PHP_FUNCTION(token_name);

#ifdef ZTS
#define TOKENIZER_G(v) TSRMG(tokenizer_globals_id, zend_tokenizer_globals *, v)
#else
#define TOKENIZER_G(v) (tokenizer_globals.v)
#endif

#endif	/* PHP_TOKENIZER_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
