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
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_TOKENIZER_H
#define PHP_TOKENIZER_H

extern zend_module_entry tokenizer_module_entry;
#define phpext_tokenizer_ptr &tokenizer_module_entry

#include "php_version.h"
#define PHP_TOKENIZER_VERSION PHP_VERSION

#define TOKEN_PARSE (1 << 0)

#ifdef ZTS
#include "TSRM.h"
#endif

char *get_token_type_name(int token_type);


PHP_MINIT_FUNCTION(tokenizer);
PHP_MINFO_FUNCTION(tokenizer);

#endif	/* PHP_TOKENIZER_H */
