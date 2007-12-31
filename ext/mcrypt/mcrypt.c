/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Derick Rethans <derick@derickrethans.nl>                    |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_LIBMCRYPT

#include "php_mcrypt.h"
#include "fcntl.h"

#define NON_FREE
#define MCRYPT2
#include "mcrypt.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/php_rand.h"

static int le_mcrypt;

typedef struct _php_mcrypt { 
	MCRYPT td;
	zend_bool init;
} php_mcrypt;

const zend_function_entry mcrypt_functions[] = { /* {{{ */
	PHP_FE(mcrypt_ecb, NULL)
	PHP_FE(mcrypt_cbc, NULL)
	PHP_FE(mcrypt_cfb, NULL)
	PHP_FE(mcrypt_ofb, NULL)
	PHP_FE(mcrypt_get_key_size, NULL)
	PHP_FE(mcrypt_get_block_size, NULL)
	PHP_FE(mcrypt_get_cipher_name, NULL)
	PHP_FE(mcrypt_create_iv, NULL)

	PHP_FE(mcrypt_list_algorithms, NULL)
	PHP_FE(mcrypt_list_modes, NULL)
	PHP_FE(mcrypt_get_iv_size, NULL)
	PHP_FE(mcrypt_encrypt, NULL)
	PHP_FE(mcrypt_decrypt, NULL)

	PHP_FE(mcrypt_module_open, NULL)
	PHP_FE(mcrypt_generic_init, NULL)
	PHP_FE(mcrypt_generic, NULL)
	PHP_FE(mdecrypt_generic, NULL)
	PHP_DEP_FALIAS(mcrypt_generic_end, mcrypt_generic_deinit, NULL)
	PHP_FE(mcrypt_generic_deinit, NULL)

	PHP_FE(mcrypt_enc_self_test, NULL)
	PHP_FE(mcrypt_enc_is_block_algorithm_mode, NULL)
	PHP_FE(mcrypt_enc_is_block_algorithm, NULL)
	PHP_FE(mcrypt_enc_is_block_mode, NULL)
	PHP_FE(mcrypt_enc_get_block_size, NULL)
	PHP_FE(mcrypt_enc_get_key_size, NULL)
	PHP_FE(mcrypt_enc_get_supported_key_sizes, NULL)
	PHP_FE(mcrypt_enc_get_iv_size, NULL)
	PHP_FE(mcrypt_enc_get_algorithms_name, NULL)
	PHP_FE(mcrypt_enc_get_modes_name, NULL)
	PHP_FE(mcrypt_module_self_test, NULL)

	PHP_FE(mcrypt_module_is_block_algorithm_mode, NULL)
	PHP_FE(mcrypt_module_is_block_algorithm, NULL)
	PHP_FE(mcrypt_module_is_block_mode, NULL)
	PHP_FE(mcrypt_module_get_algo_block_size, NULL)
	PHP_FE(mcrypt_module_get_algo_key_size, NULL)
	PHP_FE(mcrypt_module_get_supported_key_sizes, NULL)

	PHP_FE(mcrypt_module_close, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

static PHP_MINFO_FUNCTION(mcrypt);
static PHP_MINIT_FUNCTION(mcrypt);
static PHP_MSHUTDOWN_FUNCTION(mcrypt);

ZEND_DECLARE_MODULE_GLOBALS(mcrypt)

zend_module_entry mcrypt_module_entry = {
	STANDARD_MODULE_HEADER,
	"mcrypt", 
	mcrypt_functions,
	PHP_MINIT(mcrypt), PHP_MSHUTDOWN(mcrypt),
	NULL, NULL,
	PHP_MINFO(mcrypt),
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(mcrypt),
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_MCRYPT
ZEND_GET_MODULE(mcrypt)
#endif

#define MCRYPT_IV_WRONG_SIZE "The IV parameter must be as long as the blocksize"
#define MCRYPT_OPEN_MODULE_FAILED "Module initialization failed"

#define MCRYPT_ENTRY2_2_4(a,b) REGISTER_STRING_CONSTANT("MCRYPT_" #a, b, CONST_PERSISTENT)
#define MCRYPT_ENTRY2_4(a) MCRYPT_ENTRY_NAMED(a, a)

#define PHP_MCRYPT_INIT_CHECK	\
	if (!pm->init) {	\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Operation disallowed prior to mcrypt_generic_init()");	\
		RETURN_FALSE;	\
	}	\

#define MCRYPT_GET_INI											\
	cipher_dir_string = MCG(algorithms_dir); 					\
	module_dir_string = MCG(modes_dir);

#define MCRYPT_GET_TD_ARG										\
	zval *mcryptind;											\
	php_mcrypt *pm;												\
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &mcryptind)) {	\
		return;													\
	}															\
	ZEND_FETCH_RESOURCE (pm, php_mcrypt *, &mcryptind, -1, "MCrypt", le_mcrypt);

#define MCRYPT_GET_MODE_DIR_ARGS(DIRECTORY)								\
	char *dir = NULL;                                                   \
	int   dir_len;                                                      \
	char *module;                                                       \
	int   module_len;                                                   \
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&|s&",       \
		&module, &module_len, UG(ascii_conv),                           \
		&dir, &dir_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv))) == FAILURE) { \
		return;                                                         \
	}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("mcrypt.algorithms_dir", NULL, PHP_INI_ALL, OnUpdateString, algorithms_dir, zend_mcrypt_globals, mcrypt_globals)
	STD_PHP_INI_ENTRY("mcrypt.modes_dir",      NULL, PHP_INI_ALL, OnUpdateString, modes_dir, zend_mcrypt_globals, mcrypt_globals)
PHP_INI_END()

static void php_mcrypt_module_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	php_mcrypt *pm = (php_mcrypt *) rsrc->ptr;
	if (pm) {	
		mcrypt_generic_deinit(pm->td);
		mcrypt_module_close(pm->td);
		efree(pm);
		rsrc->ptr = NULL;
	}
}
/* }}} */
    
static PHP_MINIT_FUNCTION(mcrypt) /* {{{ */
{
	le_mcrypt = zend_register_list_destructors_ex(php_mcrypt_module_dtor, NULL, "mcrypt", module_number);
    
	/* modes for mcrypt_??? routines */
	REGISTER_LONG_CONSTANT("MCRYPT_ENCRYPT", PHP_MCRYPT_ENCRYPT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCRYPT_DECRYPT", PHP_MCRYPT_DECRYPT, CONST_PERSISTENT);
	
	/* sources for mcrypt_create_iv */
	REGISTER_LONG_CONSTANT("MCRYPT_DEV_RANDOM", PHP_MCRYPT_IV_SOURCE_RANDOM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCRYPT_DEV_URANDOM", PHP_MCRYPT_IV_SOURCE_URANDOM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCRYPT_RAND", PHP_MCRYPT_IV_SOURCE_RAND, CONST_PERSISTENT);
	
	/* ciphers */
	MCRYPT_ENTRY2_2_4(3DES, "tripledes");
	MCRYPT_ENTRY2_2_4(ARCFOUR_IV, "arcfour-iv");
	MCRYPT_ENTRY2_2_4(ARCFOUR, "arcfour");
	MCRYPT_ENTRY2_2_4(BLOWFISH, "blowfish");
	MCRYPT_ENTRY2_2_4(BLOWFISH_COMPAT, "blowfish-compat");
	MCRYPT_ENTRY2_2_4(CAST_128, "cast-128");
	MCRYPT_ENTRY2_2_4(CAST_256, "cast-256");
	MCRYPT_ENTRY2_2_4(CRYPT, "crypt");
	MCRYPT_ENTRY2_2_4(DES, "des");
	MCRYPT_ENTRY2_2_4(ENIGMA, "crypt");
	MCRYPT_ENTRY2_2_4(GOST, "gost");
	MCRYPT_ENTRY2_2_4(LOKI97, "loki97");
	MCRYPT_ENTRY2_2_4(PANAMA, "panama");
	MCRYPT_ENTRY2_2_4(RC2, "rc2");
	MCRYPT_ENTRY2_2_4(RIJNDAEL_128, "rijndael-128");
	MCRYPT_ENTRY2_2_4(RIJNDAEL_192, "rijndael-192");
	MCRYPT_ENTRY2_2_4(RIJNDAEL_256, "rijndael-256");
	MCRYPT_ENTRY2_2_4(SAFER64, "safer-sk64");
	MCRYPT_ENTRY2_2_4(SAFER128, "safer-sk128");
	MCRYPT_ENTRY2_2_4(SAFERPLUS, "saferplus");
	MCRYPT_ENTRY2_2_4(SERPENT, "serpent");
	MCRYPT_ENTRY2_2_4(THREEWAY, "threeway");
	MCRYPT_ENTRY2_2_4(TRIPLEDES, "tripledes");
	MCRYPT_ENTRY2_2_4(TWOFISH, "twofish");
	MCRYPT_ENTRY2_2_4(WAKE, "wake");
	MCRYPT_ENTRY2_2_4(XTEA, "xtea");

	MCRYPT_ENTRY2_2_4(IDEA, "idea");
	MCRYPT_ENTRY2_2_4(MARS, "mars");
	MCRYPT_ENTRY2_2_4(RC6, "rc6");
	MCRYPT_ENTRY2_2_4(SKIPJACK, "skipjack");
/* modes */
	MCRYPT_ENTRY2_2_4(MODE_CBC, "cbc");
	MCRYPT_ENTRY2_2_4(MODE_CFB, "cfb");
	MCRYPT_ENTRY2_2_4(MODE_ECB, "ecb");
	MCRYPT_ENTRY2_2_4(MODE_NOFB, "nofb");
	MCRYPT_ENTRY2_2_4(MODE_OFB, "ofb");
	MCRYPT_ENTRY2_2_4(MODE_STREAM, "stream");
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

static PHP_MSHUTDOWN_FUNCTION(mcrypt) /* {{{ */
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

#include "ext/standard/php_smart_str.h"

PHP_MINFO_FUNCTION(mcrypt) /* {{{ */
{
	char **modules;
	char mcrypt_api_no[16];
	int i, count;
	smart_str tmp1 = {0};
	smart_str tmp2 = {0};

	modules = mcrypt_list_algorithms(MCG(algorithms_dir), &count);
	if (count == 0) {
		smart_str_appends(&tmp1, "none");
	}
	for (i = 0; i < count; i++) {
		smart_str_appends(&tmp1, modules[i]);
		smart_str_appendc(&tmp1, ' ');
	}
	smart_str_0(&tmp1);
	mcrypt_free_p(modules, count);

	modules = mcrypt_list_modes(MCG(modes_dir), &count);
	if (count == 0) {
		smart_str_appends(&tmp2, "none");
	}
	for (i = 0; i < count; i++) {
		smart_str_appends(&tmp2, modules[i]);
		smart_str_appendc(&tmp2, ' ');
	}
	smart_str_0 (&tmp2);
	mcrypt_free_p (modules, count);

	snprintf (mcrypt_api_no, 16, "%d", MCRYPT_API_VERSION);

	php_info_print_table_start();
	php_info_print_table_header(2, "mcrypt support", "enabled");
	php_info_print_table_row(2, "Version", LIBMCRYPT_VERSION);
	php_info_print_table_row(2, "Api No", mcrypt_api_no);
	php_info_print_table_row(2, "Supported ciphers", tmp1.c);
	php_info_print_table_row(2, "Supported modes", tmp2.c);
	smart_str_free(&tmp1);
	smart_str_free(&tmp2);
	php_info_print_table_end();
	
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto resource mcrypt_module_open(string cipher, string cipher_directory, string mode, string mode_directory) U
   Opens the module of the algorithm and the mode to be used */
PHP_FUNCTION(mcrypt_module_open)
{
	char *cipher, *cipher_dir;
	char *mode,   *mode_dir;
	int   cipher_len, cipher_dir_len;
	int   mode_len,   mode_dir_len;
	MCRYPT td;
	php_mcrypt *pm;
   
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&s&s&",
		&cipher, &cipher_len, UG(ascii_conv),
		&cipher_dir, &cipher_dir_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)),
		&mode, &mode_len, UG(ascii_conv),
		&mode_dir, &mode_dir_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)))) {
		return;
	}
	
	td = mcrypt_module_open (
		cipher,
		cipher_dir_len > 0 ? cipher_dir : MCG(algorithms_dir),
		mode, 
		mode_dir_len > 0 ? mode_dir : MCG(modes_dir)
	);

	if (td == MCRYPT_FAILED) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not open encryption module");
		RETURN_FALSE;
	} else {
		pm = emalloc(sizeof(php_mcrypt));
		pm->td = td;
		pm->init = 0;
		ZEND_REGISTER_RESOURCE(return_value, pm, le_mcrypt);
	}
}
/* }}} */

/* {{{ proto int mcrypt_generic_init(resource td, binary key, binary iv) U
   This function initializes all buffers for the specific module */
PHP_FUNCTION(mcrypt_generic_init)
{
	zval *mcryptind;
	char *key, *iv;
	int max_key_size, req_iv_size, key_size, iv_size;
	php_mcrypt *pm;
	int result = 0;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rSS", &mcryptind, &key, &key_size, &iv, &iv_size)) {
		return;
	}
	ZEND_FETCH_RESOURCE(pm, php_mcrypt *, &mcryptind, -1, "MCrypt", le_mcrypt);

	max_key_size = mcrypt_enc_get_key_size(pm->td);
	req_iv_size = mcrypt_enc_get_iv_size(pm->td);

	if (!key_size) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key size is 0");
	}

	if (key_size > max_key_size) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key size too large; supplied length: %d, max: %d", key_size, max_key_size);
		key_size = max_key_size;
	}

	if (iv_size != req_iv_size) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Iv size incorrect; supplied length: %d, needed: %d", iv_size, req_iv_size);
	}

	mcrypt_generic_deinit(pm->td);
	result = mcrypt_generic_init(pm->td, key, key_size, iv);
	pm->init = 1;

	/* If this function fails, close the mcrypt module to prevent crashes
	 * when further functions want to access this resource */
	if (result < 0) {
		zend_list_delete(Z_LVAL_P(mcryptind));
		switch (result) {
			case -3:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key length incorrect");
				break;
			case -4:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Memory allocation error");
				break;
			case -1:
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown error");
				break;
		}
	}
	RETVAL_LONG(result);
}
/* }}} */

/* {{{ proto binary mcrypt_generic(resource td, binary data) U
   This function encrypts the plaintext */
PHP_FUNCTION(mcrypt_generic)
{
	zval *mcryptind;
	php_mcrypt *pm;
	char *data_copy, *data_str;
	int block_size, data_size, data_len;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rS", &mcryptind, &data_str, &data_len)) {
		return;
	}
	ZEND_FETCH_RESOURCE(pm, php_mcrypt *, &mcryptind, -1, "MCrypt", le_mcrypt);
	PHP_MCRYPT_INIT_CHECK
	
	if (!data_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An empty string was passed");
		RETURN_FALSE
	}

	/* Check blocksize */
	if (mcrypt_enc_is_block_mode(pm->td) == 1) {
		/* It's a block algorithm */
		block_size = mcrypt_enc_get_block_size(pm->td);
		data_size = (((data_len - 1) / block_size) + 1) * block_size;
		data_copy = ecalloc(1, data_size + 1);
		memcpy(data_copy, data_str, data_len);
	} else {
		/* It's not a block algorithm */
		data_copy = estrndup(data_str, data_size = data_len);
	}
	
	mcrypt_generic(pm->td, data_copy, data_size);
	data_copy[data_size] = '\0';

	RETVAL_STRINGL(data_copy, data_size, 0);
}
/* }}} */

/* {{{ proto binary mdecrypt_generic(resource td, binary data) U
   This function decrypts the plaintext */
PHP_FUNCTION(mdecrypt_generic)
{
	zval *mcryptind;
	php_mcrypt *pm;
	char *data_str, *data_copy;
	int block_size, data_len, data_size;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rS", &mcryptind, &data_str, &data_len)) {
		return;
	}
	ZEND_FETCH_RESOURCE(pm, php_mcrypt *, &mcryptind, -1, "MCrypt", le_mcrypt);
	PHP_MCRYPT_INIT_CHECK
	
	if (!data_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An empty string was passed");
		RETURN_FALSE
	}

	/* Check blocksize */
	if (mcrypt_enc_is_block_mode(pm->td) == 1) {
		/* It's a block algorithm */
		block_size = mcrypt_enc_get_block_size(pm->td);
		data_size = (((data_len - 1) / block_size) + 1) * block_size;
		data_copy = ecalloc(1, data_size + 1);
		memcpy(data_copy, data_str, data_len);
	} else {
		/* It's not a block algorithm */
		data_copy = estrndup(data_str, data_size = data_len);
	}
	
	mdecrypt_generic(pm->td, data_copy, data_size);

	RETVAL_STRINGL(data_copy, data_size, 0);
}
/* }}} */

/* {{{ proto array mcrypt_enc_get_supported_key_sizes(resource td) U
   This function decrypts the crypttext */
PHP_FUNCTION(mcrypt_enc_get_supported_key_sizes)
{
	int i, count = 0;
	int *key_sizes;
	
	MCRYPT_GET_TD_ARG
	array_init(return_value);

	key_sizes = mcrypt_enc_get_supported_key_sizes(pm->td, &count);

	for (i = 0; i < count; i++) {
		add_index_long(return_value, i, key_sizes[i]);
	}

	mcrypt_free(key_sizes);
}
/* }}} */

/* {{{ proto int mcrypt_enc_self_test(resource td) U
   This function runs the self test on the algorithm specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_self_test)
{
	MCRYPT_GET_TD_ARG
	RETURN_LONG(mcrypt_enc_self_test(pm->td));
}
/* }}} */

/* {{{ proto bool mcrypt_module_close(resource td) U
   Free the descriptor td */
PHP_FUNCTION(mcrypt_module_close)
{
	MCRYPT_GET_TD_ARG
	zend_list_delete(Z_LVAL_P(mcryptind));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mcrypt_generic_deinit(resource td) U
   This function terminates encrypt specified by the descriptor td */
PHP_FUNCTION(mcrypt_generic_deinit)
{
	MCRYPT_GET_TD_ARG

	if (mcrypt_generic_deinit(pm->td) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not terminate encryption specifier");
		RETURN_FALSE
	}
	RETURN_TRUE
}
/* }}} */

/* {{{ proto bool mcrypt_enc_is_block_algorithm_mode(resource td) U
   Returns TRUE if the mode is for use with block algorithms */
PHP_FUNCTION(mcrypt_enc_is_block_algorithm_mode)
{
	MCRYPT_GET_TD_ARG

	if (mcrypt_enc_is_block_algorithm_mode(pm->td) == 1) {
		RETURN_TRUE
	} else {
		RETURN_FALSE
	}
}
/* }}} */

/* {{{ proto bool mcrypt_enc_is_block_algorithm(resource td) U
   Returns TRUE if the alrogithm is a block algorithms */
PHP_FUNCTION(mcrypt_enc_is_block_algorithm)
{
	MCRYPT_GET_TD_ARG

	if (mcrypt_enc_is_block_algorithm(pm->td) == 1) {
		RETURN_TRUE 
	} else {
		RETURN_FALSE
	}
}
/* }}} */

/* {{{ proto bool mcrypt_enc_is_block_mode(resource td) U
   Returns TRUE if the mode outputs blocks */
PHP_FUNCTION(mcrypt_enc_is_block_mode)
{
	MCRYPT_GET_TD_ARG

	if (mcrypt_enc_is_block_mode(pm->td) == 1) {
		RETURN_TRUE
	} else {
		RETURN_FALSE
	}
}
/* }}} */

/* {{{ proto int mcrypt_enc_get_block_size(resource td) U
   Returns the block size of the cipher specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_block_size)
{
	MCRYPT_GET_TD_ARG
	RETURN_LONG(mcrypt_enc_get_block_size(pm->td));
}
/* }}} */

/* {{{ proto int mcrypt_enc_get_key_size(resource td) U
   Returns the maximum supported key size in bytes of the algorithm specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_key_size)
{
	MCRYPT_GET_TD_ARG
	RETURN_LONG(mcrypt_enc_get_key_size(pm->td));
}
/* }}} */

/* {{{ proto int mcrypt_enc_get_iv_size(resource td) U
   Returns the size of the IV in bytes of the algorithm specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_iv_size)
{
	MCRYPT_GET_TD_ARG
	RETURN_LONG(mcrypt_enc_get_iv_size(pm->td));
}
/* }}} */

/* {{{ proto string mcrypt_enc_get_algorithms_name(resource td) U
   Returns the name of the algorithm specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_algorithms_name)
{
	char *name;
	MCRYPT_GET_TD_ARG

	name = mcrypt_enc_get_algorithms_name(pm->td);
	RETVAL_ASCII_STRING(name, 1);
	mcrypt_free(name);
}
/* }}} */

/* {{{ proto string mcrypt_enc_get_modes_name(resource td) U
   Returns the name of the mode specified by the descriptor td */
PHP_FUNCTION(mcrypt_enc_get_modes_name)
{
	char *name;
	MCRYPT_GET_TD_ARG

	name = mcrypt_enc_get_modes_name(pm->td);
	RETVAL_ASCII_STRING(name, 1);
	mcrypt_free(name);
}
/* }}} */

/* {{{ proto bool mcrypt_module_self_test(string algorithm [, string lib_dir]) U
   Does a self test of the module "module" */
PHP_FUNCTION(mcrypt_module_self_test)
{
	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir);
	
	if (mcrypt_module_self_test(module, dir) == 0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool mcrypt_module_is_block_algorithm_mode(string mode [, string lib_dir]) U
   Returns TRUE if the mode is for use with block algorithms */
PHP_FUNCTION(mcrypt_module_is_block_algorithm_mode)
{
	MCRYPT_GET_MODE_DIR_ARGS(modes_dir)
	
	if (mcrypt_module_is_block_algorithm_mode(module, dir) == 1) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool mcrypt_module_is_block_algorithm(string algorithm [, string lib_dir]) U
   Returns TRUE if the algorithm is a block algorithm */
PHP_FUNCTION(mcrypt_module_is_block_algorithm)
{
	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir)
	
	if (mcrypt_module_is_block_algorithm(module, dir) == 1) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool mcrypt_module_is_block_mode(string mode [, string lib_dir]) U
   Returns TRUE if the mode outputs blocks of bytes */
PHP_FUNCTION(mcrypt_module_is_block_mode)
{
	MCRYPT_GET_MODE_DIR_ARGS(modes_dir)
	
	if (mcrypt_module_is_block_mode(module, dir) == 1) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int mcrypt_module_get_algo_block_size(string algorithm [, string lib_dir]) U
   Returns the block size of the algorithm */
PHP_FUNCTION(mcrypt_module_get_algo_block_size)
{
	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir)
	
	RETURN_LONG(mcrypt_module_get_algo_block_size(module, dir));
}
/* }}} */

/* {{{ proto int mcrypt_module_get_algo_key_size(string algorithm [, string lib_dir]) U
   Returns the maximum supported key size of the algorithm */
PHP_FUNCTION(mcrypt_module_get_algo_key_size)
{
	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir);
	
	RETURN_LONG(mcrypt_module_get_algo_key_size(module, dir));
}
/* }}} */

/* {{{ proto array mcrypt_module_get_supported_key_sizes(string algorithm [, string lib_dir]) U
   This function decrypts the crypttext */
PHP_FUNCTION(mcrypt_module_get_supported_key_sizes)
{
	int i, count = 0;
	int *key_sizes;
	
	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir)
	array_init(return_value);

	key_sizes = mcrypt_module_get_algo_supported_key_sizes(module, dir, &count);

	for (i = 0; i < count; i++) {
		add_index_long(return_value, i, key_sizes[i]);
	}
	mcrypt_free(key_sizes);
}
/* }}} */

/* {{{ proto array mcrypt_list_algorithms([string lib_dir]) U
   List all algorithms in "module_dir" */
PHP_FUNCTION(mcrypt_list_algorithms)
{
	char **modules;
	char *lib_dir = MCG(algorithms_dir);
	int   lib_dir_len;
	int   i, count;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s&",
		&lib_dir, &lib_dir_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv))) == FAILURE) {
		return;
	}
	
	array_init(return_value);
	modules = mcrypt_list_algorithms(lib_dir, &count);

	if (count == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No algorithms found in module dir");
	}
	for (i = 0; i < count; i++) {
		add_index_ascii_string(return_value, i, modules[i], ZSTR_DUPLICATE);
	}
	mcrypt_free_p(modules, count);
}
/* }}} */

/* {{{ proto array mcrypt_list_modes([string lib_dir]) U
   List all modes "module_dir" */
PHP_FUNCTION(mcrypt_list_modes)
{
	char **modules;
	char *lib_dir = MCG(modes_dir);
	int   lib_dir_len;
	int   i, count;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s&",
		&lib_dir, &lib_dir_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv))) == FAILURE) {
		return;
	}

	array_init(return_value);
	modules = mcrypt_list_modes(lib_dir, &count);

	if (count == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No modes found in module dir");
	}
	for (i = 0; i < count; i++) {
		add_index_ascii_string(return_value, i, modules[i], ZSTR_DUPLICATE);
	}
	mcrypt_free_p(modules, count);
}
/* }}} */

/* {{{ proto int mcrypt_get_key_size(string cipher, string module) U
   Get the key size of cipher */
PHP_FUNCTION(mcrypt_get_key_size)
{
	char *cipher;
	char *module;
	int   cipher_len, module_len; 
	char *cipher_dir_string;
	char *module_dir_string;
	MCRYPT td;

	MCRYPT_GET_INI

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&",
		&cipher, &cipher_len, UG(ascii_conv), &module, &module_len, UG(ascii_conv)) == FAILURE) {
		return;
	}
	
	td = mcrypt_module_open(cipher, cipher_dir_string, module, module_dir_string);
	if (td != MCRYPT_FAILED) {
		RETVAL_LONG(mcrypt_enc_get_key_size(td));
		mcrypt_module_close(td);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int mcrypt_get_block_size(string cipher, string module) U
   Get the key size of cipher */
PHP_FUNCTION(mcrypt_get_block_size)
{
	char *cipher;
	char *module;
	int   cipher_len, module_len; 
	char *cipher_dir_string;
	char *module_dir_string;
	MCRYPT td;

	MCRYPT_GET_INI

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&",
		&cipher, &cipher_len, UG(ascii_conv), &module, &module_len, UG(ascii_conv)) == FAILURE) {
		return;
	}
	
	td = mcrypt_module_open(cipher, cipher_dir_string, module, module_dir_string);
	if (td != MCRYPT_FAILED) {
		RETVAL_LONG(mcrypt_enc_get_block_size(td));
		mcrypt_module_close(td);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int mcrypt_get_iv_size(string cipher, string module) U
   Get the IV size of cipher (Usually the same as the blocksize) */
PHP_FUNCTION(mcrypt_get_iv_size)
{
	char *cipher;
	char *module;
	int   cipher_len, module_len; 
	char *cipher_dir_string;
	char *module_dir_string;
	MCRYPT td;

	MCRYPT_GET_INI

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&",
		&cipher, &cipher_len, UG(ascii_conv), &module, &module_len, UG(ascii_conv)) == FAILURE) {
		return;
	}
	
	td = mcrypt_module_open(cipher, cipher_dir_string, module, module_dir_string);
	if (td != MCRYPT_FAILED) {
		RETVAL_LONG(mcrypt_enc_get_iv_size(td));
		mcrypt_module_close(td);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string mcrypt_get_cipher_name(string cipher) U
   Get the name of cipher */
PHP_FUNCTION(mcrypt_get_cipher_name)
{
	char *cipher_dir_string;
	char *module_dir_string;
	char *cipher_name;
	char *cipher;
	int   cipher_len;
	MCRYPT td;

	MCRYPT_GET_INI

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&",
		&cipher, &cipher_len, UG(ascii_conv)) == FAILURE) {
		return;
	}

	/* The code below is actually not very nice, but I didn't see a better
	 * method */
	td = mcrypt_module_open(cipher, cipher_dir_string, "ecb", module_dir_string);
	if (td != MCRYPT_FAILED) {
		cipher_name = mcrypt_enc_get_algorithms_name(td);
		mcrypt_module_close(td);
		RETVAL_ASCII_STRING(cipher_name,1);
		mcrypt_free(cipher_name);
	} else {
		td = mcrypt_module_open(cipher, cipher_dir_string, "stream", module_dir_string);
		if (td != MCRYPT_FAILED) {
			cipher_name = mcrypt_enc_get_algorithms_name(td);
			mcrypt_module_close(td);
			RETVAL_ASCII_STRING(cipher_name,1);
			mcrypt_free(cipher_name);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ inline _php_mcrypt_func */
static inline void _php_mcrypt_func(INTERNAL_FUNCTION_PARAMETERS, long op, char *mode_str, int mode_len)
{
	char *cipher_str, *key_str, *iv_str = NULL, *data_str, *data_copy = NULL;
	int cipher_len, key_len, iv_len = 0, data_len, data_size = 0;
	
	if (mode_len) {
		if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&SSl|S", 
				&cipher_str, &cipher_len, UG(ascii_conv),
				&key_str, &key_len, &data_str, &data_len,
				&op, &iv_str, &iv_len)) {
			return;
		}
	} else {
		if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&SSs&|S", 
				&cipher_str, &cipher_len, UG(ascii_conv),
				&key_str, &key_len, &data_str, &data_len,
				&mode_str, &mode_len, UG(ascii_conv),
				&iv_str, &iv_len)) {
			return;
		}
	}
	
	if (SUCCESS != php_mcrypt_func(op, cipher_str, mode_str, key_str, key_len, iv_str, iv_len, data_str, data_len, &data_copy, &data_size TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(data_copy, data_size, 0);
}
/* }}} */

/* {{{ proto string mcrypt_encrypt(string cipher, string key, string data, string mode, string iv) U
   OFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_encrypt)
{
	_php_mcrypt_func(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_MCRYPT_ENCRYPT, NULL, 0);
}
/* }}} */

/* {{{ proto string mcrypt_decrypt(string cipher, string key, string data, string mode, string iv) U
   OFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_decrypt)
{
	_php_mcrypt_func(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_MCRYPT_DECRYPT, NULL, 0);
}
/* }}} */

/* {{{ proto string mcrypt_ecb(int cipher, string key, string data, int mode, string iv) U
   ECB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_ecb)
{
	_php_mcrypt_func(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, ZEND_STRL("ecb"));
}
/* }}} */

/* {{{ proto string mcrypt_cbc(int cipher, string key, string data, int mode, string iv) U
   CBC crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_cbc)
{
	_php_mcrypt_func(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, ZEND_STRL("cbc"));
}
/* }}} */

/* {{{ proto string mcrypt_cfb(int cipher, string key, string data, int mode, string iv) U
   CFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_cfb)
{
	_php_mcrypt_func(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, ZEND_STRL("cfb"));
}
/* }}} */

/* {{{ proto string mcrypt_ofb(int cipher, string key, string data, int mode, string iv) U
   OFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_ofb)
{
	_php_mcrypt_func(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, ZEND_STRL("ofb"));
}
/* }}} */

/* {{{ proto binary mcrypt_create_iv(int size, int source) U
   Create an initialization vector (IV) */
PHP_FUNCTION(mcrypt_create_iv)
{
	char *iv_str;
	int iv_len;
	long size, source = PHP_MCRYPT_IV_SOURCE_RANDOM;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &size, &source)) {
		return;
	}
	if (SUCCESS != php_mcrypt_iv(source, size, &iv_str, &iv_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(iv_str, iv_len, 0);
}
/* }}} */

/* {{{ php_mcrypt_iv */
int php_mcrypt_iv(php_mcrypt_iv_source source, int size, char **iv_str, int *iv_len TSRMLS_DC)
{
	int fd, n;
	size_t read_bytes;
	
	if (size <= 0 || size >= INT_MAX) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can not create an IV with a size of less then 1 or greater then %d", INT_MAX);
		return FAILURE;
	}
	
	*iv_str = ecalloc(size + 1, 1);
	
	switch (source) {
		case PHP_MCRYPT_IV_SOURCE_RANDOM:
		case PHP_MCRYPT_IV_SOURCE_URANDOM:
			read_bytes = 0;
			
			fd = open(source == PHP_MCRYPT_IV_SOURCE_RANDOM ? "/dev/random" : "/dev/urandom", O_RDONLY);
			if (fd < 0) {
				efree(*iv_str);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot open source device");
				return FAILURE;
			}
			
			while (read_bytes < size) {
				n = read(fd, *iv_str + read_bytes, size - read_bytes);
				if (n < 0) {
					break;
				}
				read_bytes += n;
			}
			*iv_len = read_bytes;
			close(fd);
			
			if (*iv_len < size) {
				efree(*iv_str);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not gather sufficient random data");
				return FAILURE;
			}
			break;
		case PHP_MCRYPT_IV_SOURCE_RAND:
				*iv_len = size;
				while (size) {
					(*iv_str)[--size] = 255.0 * php_rand(TSRMLS_C) / RAND_MAX;
				}
			break;
	}
	
	return SUCCESS;
}
/* }}} */

/* {{{ php_mcrypt */
int php_mcrypt_func(php_mcrypt_op op, char *cipher, char *mode, char *key_str, int key_len, char *iv_str, int iv_len, char *data_str, int data_len, char **data_copy, int *data_size TSRMLS_DC)
{
	MCRYPT td;
	char *cipher_dir_string, *module_dir_string, *key_copy, *iv_copy;
	int i, status = SUCCESS, count, *key_sizes, key_size, iv_size, block_size, iv_req;
	
	MCRYPT_GET_INI
	
	td = mcrypt_module_open(cipher, cipher_dir_string, mode, module_dir_string);
	if (td == MCRYPT_FAILED) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		return FAILURE;
	}
	
	if ((key_size = mcrypt_enc_get_key_size(td)) < key_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Size of key is too large for this algorithm");
	}
	
	key_sizes = mcrypt_enc_get_supported_key_sizes(td, &count);
	switch (count) {
		case 0:
			key_copy = estrndup(key_str, key_len);
			break;
		case 1:
			key_copy = ecalloc(1, key_sizes[0]);
			memcpy(key_copy, key_str, MIN(key_len, key_sizes[0]));
			key_len = key_sizes[0];
			break;
		default:
			for (i = 0; i < count; ++i) {
				if (key_sizes[i] >= key_len && key_sizes[i] <= key_size) {
					key_copy = ecalloc(1, key_sizes[i]);
					memcpy(key_copy, key_str, MIN(key_len, key_sizes[i]));
					key_len = key_sizes[i];
					break;
				}
			}
			break;
	}
	mcrypt_free(key_sizes);
	
	iv_size = mcrypt_enc_get_iv_size(td);
	iv_req = mcrypt_enc_mode_has_iv(td);
	if (iv_len) {
		if (iv_len == iv_size) {
			iv_copy = estrndup(iv_str, iv_len);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, MCRYPT_IV_WRONG_SIZE);
			iv_copy = ecalloc(1, iv_size);
			memcpy(iv_copy, iv_str, MIN(iv_len, iv_size));
		}
	} else {
		if (iv_req) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempt to use an empty IV, which is NOT recommended");
		}
		iv_copy = ecalloc(1, iv_size);
	}
	
	if (mcrypt_enc_is_block_mode(td) == 1) {
		block_size = mcrypt_enc_get_block_size(td);
		*data_size = (((data_len - 1) / block_size) + 1) * block_size;
		*data_copy = ecalloc(1, *data_size + 1);
		memcpy(*data_copy, data_str, data_len);
	} else {
		*data_copy = estrndup(data_str, *data_size = data_len);
	}
	
	if (mcrypt_generic_init(td, key_copy, key_len, iv_copy) >= 0) {
		switch (op) {
			case PHP_MCRYPT_ENCRYPT:
				if (mcrypt_generic(td, *data_copy, *data_size)) {
					efree(*data_copy);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "encryption failed");
					status = FAILURE;
				}
				break;
			case PHP_MCRYPT_DECRYPT:
				if (mdecrypt_generic(td, *data_copy, *data_size)) {
					efree(*data_copy);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "decryption failed");
					status = FAILURE;
				}
				break;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Mcrypt initialisation failed"); /* huh? error? */
		status = FAILURE;
	}
	
	efree(key_copy);
	efree(iv_copy);
	
	return status;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
