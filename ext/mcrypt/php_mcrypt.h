#ifndef PHP_MCRYPT_H
#define PHP_MCRYPT_H

#if HAVE_LIBMCRYPT

#ifdef ZTS
#include "TSRM.h"
#endif

#if PHP_API_VERSION < 19990421
#define  zend_module_entry zend_module_entry
#include "modules.h"
#include "internal_functions.h"
#endif

extern zend_module_entry mcrypt_module_entry;

#define mcrypt_module_ptr &mcrypt_module_entry

/* Functions for both old and new API */
PHP_FUNCTION(mcrypt_ecb);
PHP_FUNCTION(mcrypt_cbc);
PHP_FUNCTION(mcrypt_cfb);
PHP_FUNCTION(mcrypt_ofb);
PHP_FUNCTION(mcrypt_get_cipher_name);
PHP_FUNCTION(mcrypt_get_block_size);
PHP_FUNCTION(mcrypt_get_key_size);
PHP_FUNCTION(mcrypt_create_iv);

#if HAVE_LIBMCRYPT24
/* Support functions for old API */
PHP_FUNCTION(mcrypt_list_algorithms);
PHP_FUNCTION(mcrypt_list_modes);
PHP_FUNCTION(mcrypt_get_iv_size);
PHP_FUNCTION(mcrypt_encrypt);
PHP_FUNCTION(mcrypt_decrypt);

/* Functions for new API */
PHP_FUNCTION(mcrypt_module_open);
PHP_FUNCTION(mcrypt_generic_init);
PHP_FUNCTION(mcrypt_generic);
PHP_FUNCTION(mdecrypt_generic);
PHP_FUNCTION(mcrypt_generic_end);
PHP_FUNCTION(mcrypt_enc_self_test);
PHP_FUNCTION(mcrypt_enc_is_block_algorithm_mode);
PHP_FUNCTION(mcrypt_enc_is_block_algorithm);
PHP_FUNCTION(mcrypt_enc_is_block_mode);
PHP_FUNCTION(mcrypt_enc_get_block_size);
PHP_FUNCTION(mcrypt_enc_get_key_size);
PHP_FUNCTION(mcrypt_enc_get_supported_key_sizes);
PHP_FUNCTION(mcrypt_enc_get_iv_size);
PHP_FUNCTION(mcrypt_enc_get_algorithms_name);
PHP_FUNCTION(mcrypt_enc_get_modes_name);
PHP_FUNCTION(mcrypt_module_self_test);
PHP_FUNCTION(mcrypt_module_is_block_algorithm_mode);
PHP_FUNCTION(mcrypt_module_is_block_algorithm);
PHP_FUNCTION(mcrypt_module_is_block_mode);
PHP_FUNCTION(mcrypt_module_get_algo_block_size);
PHP_FUNCTION(mcrypt_module_get_algo_key_size);
PHP_FUNCTION(mcrypt_module_get_supported_key_sizes);
PHP_FUNCTION(mcrypt_module_close);

ZEND_BEGIN_MODULE_GLOBALS(mcrypt)
	int le_h;
	char *modes_dir;
	char *algorithms_dir;
ZEND_END_MODULE_GLOBALS(mcrypt)

#ifdef ZTS
# define MCLS_D        zend_mcrypt_globals *mcrypt_globals
# define MCLS_DC       , MCLS_D
# define MCLS_C        mcrypt_globals
# define MCLS_CC       , MCLS_C
# define MCG(v)        (mcrypt_globals->v)
# define MCLS_FETCH()  zend_mcrypt_globals *mcrypt_globals = ts_resource(mcrypt_globals_id)
#else
# define MCLS_D
# define MCLS_DC
# define MCLS_C
# define MCLS_CC
# define MCG(v)        (mcrypt_globals.v)
# define MCLS_FETCH()
#endif

#endif

#else
#define mcrypt_module_ptr NULL
#endif

#define phpext_mcrypt_ptr mcrypt_module_ptr

#endif
