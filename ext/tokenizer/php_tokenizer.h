/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_TOKENIZER_H
#define PHP_TOKENIZER_H

extern zend_module_entry tokenizer_module_entry;
#define phpext_tokenizer_ptr &tokenizer_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

void tokenizer_register_constants(INIT_FUNC_ARGS);
char *get_token_type_name(int token_type);


PHP_MINIT_FUNCTION(tokenizer);
PHP_MINFO_FUNCTION(tokenizer);

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
