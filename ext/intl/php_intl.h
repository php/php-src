/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_INTL_H
#define PHP_INTL_H

#include <php.h>

/* Even if we're included from C++, don't introduce C++ definitions
 * because we were included with extern "C". The effect would be that
 * when the headers defined any method, they would do so with C linkage */
#undef U_SHOW_CPLUSPLUS_API
#define U_SHOW_CPLUSPLUS_API 0
#include "collator/collator_sort.h"
#include <unicode/ubrk.h>
#include "intl_error.h"

extern zend_module_entry intl_module_entry;
#define phpext_intl_ptr &intl_module_entry

#ifdef PHP_WIN32
#define PHP_INTL_API __declspec(dllexport)
#else
#define PHP_INTL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(intl)
	zval current_collator;
	char* default_locale;
	collator_compare_func_t compare_func;
	UBreakIterator* grapheme_iterator;
	intl_error g_error;
	zend_long error_level;
	zend_bool use_exceptions;
ZEND_END_MODULE_GLOBALS(intl)

/* Macro to access request-wide global variables. */
#ifdef ZTS
#define INTL_G(v) ZEND_TSRMG(intl_globals_id, zend_intl_globals *, v)
#ifdef COMPILE_DL_INTL
ZEND_TSRMLS_CACHE_EXTERN;
#endif
#else
#define INTL_G(v) (intl_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(intl)

PHP_MINIT_FUNCTION(intl);
PHP_MSHUTDOWN_FUNCTION(intl);
PHP_RINIT_FUNCTION(intl);
PHP_RSHUTDOWN_FUNCTION(intl);
PHP_MINFO_FUNCTION(intl);

const char *intl_locale_get_default( void );

#define PHP_INTL_VERSION "1.1.0"

#endif  /* PHP_INTL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
