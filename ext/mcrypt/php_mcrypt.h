#ifndef PHP_MCRYPT_H
#define PHP_MCRYPT_H

#if HAVE_LIBMCRYPT

#if PHP_API_VERSION < 19990421
#define  zend_module_entry php3_module_entry
#include "modules.h"
#include "internal_functions.h"
#endif

extern zend_module_entry mcrypt_module_entry;
#define mcrypt_module_ptr &mcrypt_module_entry

PHP_FUNCTION(mcrypt_ecb);
PHP_FUNCTION(mcrypt_cbc);
PHP_FUNCTION(mcrypt_cfb);
PHP_FUNCTION(mcrypt_ofb);
PHP_FUNCTION(mcrypt_get_cipher_name);
PHP_FUNCTION(mcrypt_get_block_size);
PHP_FUNCTION(mcrypt_get_key_size);
PHP_FUNCTION(mcrypt_create_iv);

#else
#define mcrypt_module_ptr NULL
#endif

#define phpext_mcrypt_ptr mcrypt_module_ptr

#endif
