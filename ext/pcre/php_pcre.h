/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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

#ifndef PHP_PCRE_H
#define PHP_PCRE_H

#if HAVE_PCRE || HAVE_BUNDLED_PCRE

#if HAVE_BUNDLED_PCRE
#include "pcre2lib/pcre2.h"
#else
#include "pcre2.h"
#endif

#if HAVE_LOCALE_H
#include <locale.h>
#endif

PHPAPI zend_string *php_pcre_replace(zend_string *regex, zend_string *subject_str, char *subject, size_t subject_len, zend_string *replace_str, size_t limit, size_t *replace_count);
PHPAPI pcre2_code* pcre_get_compiled_regex(zend_string *regex, uint32_t *capture_count, uint32_t *options);
PHPAPI pcre2_code* pcre_get_compiled_regex_ex(zend_string *regex, uint32_t *capture_count, uint32_t *preg_options, uint32_t *coptions);

extern zend_module_entry pcre_module_entry;
#define pcre_module_ptr &pcre_module_entry

#include "php_version.h"
#define PHP_PCRE_VERSION PHP_VERSION

typedef struct _pcre_cache_entry pcre_cache_entry;

PHPAPI pcre_cache_entry* pcre_get_compiled_regex_cache(zend_string *regex);

PHPAPI void  php_pcre_match_impl(  pcre_cache_entry *pce, char *subject, size_t subject_len, zval *return_value,
	zval *subpats, int global, int use_flags, zend_long flags, zend_off_t start_offset);

PHPAPI zend_string *php_pcre_replace_impl(pcre_cache_entry *pce, zend_string *subject_str, char *subject, size_t subject_len, zend_string *replace_str,
	size_t limit, size_t *replace_count);

PHPAPI void  php_pcre_split_impl(  pcre_cache_entry *pce, zend_string *subject_str, zval *return_value,
	zend_long limit_val, zend_long flags);

PHPAPI void  php_pcre_grep_impl(   pcre_cache_entry *pce, zval *input, zval *return_value,
	zend_long flags);

PHPAPI pcre2_match_context *php_pcre_mctx(void);
PHPAPI pcre2_general_context *php_pcre_gctx(void);
PHPAPI pcre2_compile_context *php_pcre_cctx(void);
PHPAPI void php_pcre_pce_incref(pcre_cache_entry *);
PHPAPI void php_pcre_pce_decref(pcre_cache_entry *);
PHPAPI pcre2_code *php_pcre_pce_re(pcre_cache_entry *);
/* capture_count can be ignored, re is required. */
PHPAPI pcre2_match_data *php_pcre_create_match_data(uint32_t, pcre2_code *);
PHPAPI void php_pcre_free_match_data(pcre2_match_data *);

ZEND_BEGIN_MODULE_GLOBALS(pcre)
	HashTable pcre_cache;
	zend_long backtrack_limit;
	zend_long recursion_limit;
#ifdef HAVE_PCRE_JIT_SUPPORT
	zend_bool jit;
#endif
	int  error_code;
ZEND_END_MODULE_GLOBALS(pcre)

PHPAPI ZEND_EXTERN_MODULE_GLOBALS(pcre)
#define PCRE_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(pcre, v)

#else

#define pcre_module_ptr NULL

#endif /* HAVE_PCRE || HAVE_BUNDLED_PCRE */

#define phpext_pcre_ptr pcre_module_ptr

#endif /* PHP_PCRE_H */
