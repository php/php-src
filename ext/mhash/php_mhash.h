#ifndef PHP_MHASH_H
#define PHP_MHASH_H

#if HAVE_LIBMHASH

#if PHP_API_VERSION < 19990421
#define  zend_module_entry zend_module_entry
#include "zend_modules.h"
#include "internal_functions.h"
#endif

extern zend_module_entry mhash_module_entry;
#define mhash_module_ptr &mhash_module_entry

static PHP_MINIT_FUNCTION(mhash);
PHP_MINFO_FUNCTION(mhash);
PHP_FUNCTION(mhash_get_block_size);
PHP_FUNCTION(mhash_get_hash_name);
PHP_FUNCTION(mhash_count);
PHP_FUNCTION(mhash_keygen_s2k);
PHP_FUNCTION(mhash);

#else
#define mhash_module_ptr NULL
#endif

#define phpext_mhash_ptr mhash_module_ptr

#endif
