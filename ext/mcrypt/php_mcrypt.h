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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Derick Rethans <d.rethans@jdimedia.nl>                      |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef PHP_MCRYPT_H
#define PHP_MCRYPT_H

#if HAVE_LIBMCRYPT

#ifdef ZTS
#include "TSRM.h"
#endif

#if PHP_API_VERSION < 19990421
#define  zend_module_entry zend_module_entry
#include "zend_modules.h"
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
#if HAVE_MCRYPT_GENERIC_DEINIT
PHP_FUNCTION(mcrypt_generic_deinit);
#endif
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
# define MCG(v) TSRMG(mcrypt_globals_id, zend_mcrypt_globals *, v)
#else
# define MCG(v)        (mcrypt_globals.v)
#endif

#endif

#else
#define mcrypt_module_ptr NULL
#endif

#define phpext_mcrypt_ptr mcrypt_module_ptr

#endif
