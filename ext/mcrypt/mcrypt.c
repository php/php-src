/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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
   +----------------------------------------------------------------------+
 */

#include "php.h"

#if HAVE_LIBMCRYPT

#include "php_mcrypt.h"
#include "fcntl.h"

#define NON_FREE
#define MCRYPT2
#include "mcrypt.h"

function_entry mcrypt_functions[] = {
	PHP_FE(mcrypt_ecb, NULL)
	PHP_FE(mcrypt_cbc, NULL)
	PHP_FE(mcrypt_cfb, NULL)
	PHP_FE(mcrypt_ofb, NULL)
	PHP_FE(mcrypt_get_cipher_name, NULL)
	PHP_FE(mcrypt_get_block_size, NULL)
	PHP_FE(mcrypt_get_key_size, NULL)
	PHP_FE(mcrypt_create_iv, NULL)
	{0},
};

static PHP_MINIT_FUNCTION(mcrypt);

zend_module_entry mcrypt_module_entry = {
	"mcrypt", 
	mcrypt_functions,
	PHP_MINIT(mcrypt), NULL,
	NULL, NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES,
};

#ifdef COMPILE_DL_MCRYPT
ZEND_GET_MODULE(mcrypt)
#endif

#if 0

typedef struct mcrypt_global_struct {
	int le_h;
} mcrypt_global_struct;

static mcrypt_global_struct mcryptg;

#define MCRYPTG(x) mcryptg.x

#endif

#define MCRYPT_ARGS2 											\
	pval **cipher, **data, **key, **mode; 						\
	int td; 													\
	char *ndata; 												\
	size_t bsize; 												\
	size_t nr; 													\
	size_t nsize

#define MCRYPT_ARGS 											\
	MCRYPT_ARGS2; 												\
	pval **iv

#define MCRYPT_CONVERT 											\
	convert_to_long_ex(cipher); 								\
	convert_to_long_ex(mode); 									\
	convert_to_string_ex(data); 								\
	convert_to_string_ex(key)

#define MCRYPT_SIZE 											\
	bsize = mcrypt_get_block_size((*cipher)->value.lval); 		\
	nr = ((*data)->value.str.len + bsize - 1) / bsize; 			\
	nsize = nr * bsize

#define MCRYPT_CHECK_TD_CPY 									\
	if(td == -1) { 												\
		php_error(E_WARNING, MCRYPT_FAILED); 					\
		RETURN_FALSE; 											\
	} 															\
	ndata = ecalloc(nr, bsize); 								\
	memcpy(ndata, (*data)->value.str.val, (*data)->value.str.len)

#define MCRYPT_CHECK_IV 										\
	convert_to_string_ex(iv);	 								\
	if((*iv)->value.str.len != bsize) { 						\
		php_error(E_WARNING, MCRYPT_IV_WRONG_SIZE); 			\
		RETURN_FALSE; 											\
	}

#define MCRYPT_ACTION(x) 										\
	if((*mode)->value.lval == 0) 								\
		mcrypt_##x(td, ndata, nsize); 							\
	else 														\
		mdecrypt_##x(td, ndata, nsize); 						\
	end_mcrypt_##x(td)

#define MCRYPT_IV_WRONG_SIZE "The IV paramater must be as long as the blocksize"
#define MCRYPT_FAILED "mcrypt initialization failed"

#define MCRYPT_ENTRY_NAMED(a,b) REGISTER_LONG_CONSTANT("MCRYPT_" #a, b, CONST_PERSISTENT)
#define MCRYPT_ENTRY2(a) MCRYPT_ENTRY_NAMED(a, MCRYPT_##a)
#define MCRYPT_ENTRY(a) MCRYPT_ENTRY_NAMED(a, a)

static PHP_MINIT_FUNCTION(mcrypt)
{
	/* modes for mcrypt_??? routines */
	REGISTER_LONG_CONSTANT("MCRYPT_ENCRYPT", 0, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCRYPT_DECRYPT", 1, CONST_PERSISTENT);
	
	/* sources for mcrypt_create_iv */
	REGISTER_LONG_CONSTANT("MCRYPT_DEV_RANDOM", 0, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCRYPT_DEV_URANDOM", 1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCRYPT_RAND", 2, CONST_PERSISTENT);
	
	/* ciphers */
#if defined(MCRYPT_API_VERSION) && MCRYPT_API_VERSION >= 19991015
	MCRYPT_ENTRY2(BLOWFISH_448);
	MCRYPT_ENTRY2(DES);
	MCRYPT_ENTRY2(3DES);
	MCRYPT_ENTRY2(3WAY);
	MCRYPT_ENTRY2(GOST);
	MCRYPT_ENTRY2(SAFER_64);
	MCRYPT_ENTRY2(SAFER_128);
	MCRYPT_ENTRY2(CAST_128);
	MCRYPT_ENTRY2(XTEA);
	MCRYPT_ENTRY2(RC2_1024);
	MCRYPT_ENTRY2(TWOFISH_128);
	MCRYPT_ENTRY2(TWOFISH_192);
	MCRYPT_ENTRY2(TWOFISH_256);
	MCRYPT_ENTRY2(BLOWFISH_128);
	MCRYPT_ENTRY2(BLOWFISH_192);
	MCRYPT_ENTRY2(BLOWFISH_256);
	MCRYPT_ENTRY2(CAST_256);
	MCRYPT_ENTRY2(SAFERPLUS);
	MCRYPT_ENTRY2(LOKI97);
	MCRYPT_ENTRY2(SERPENT_128);
	MCRYPT_ENTRY2(SERPENT_192);
	MCRYPT_ENTRY2(SERPENT_256);
	MCRYPT_ENTRY2(RIJNDAEL_128);
	MCRYPT_ENTRY2(RIJNDAEL_192);
	MCRYPT_ENTRY2(RIJNDAEL_256);
	MCRYPT_ENTRY2(RC2_256);
	MCRYPT_ENTRY2(RC2_128);
	MCRYPT_ENTRY2(RC6_256);
	MCRYPT_ENTRY2(IDEA);
	MCRYPT_ENTRY2(RC6_128);
	MCRYPT_ENTRY2(RC6_192);
	MCRYPT_ENTRY2(RC4);
#else
#error Please update your mcrypt library
#endif
	
	return SUCCESS;
}

typedef enum {
	RANDOM = 0,
	URANDOM,
	RAND
} iv_source;

/* {{{ proto string mcrypt_create_iv(int size, int source)
   Create an initializing vector (IV) */
PHP_FUNCTION(mcrypt_create_iv)
{
	pval **size, **psource;
	char *iv;
	iv_source source;
	int i;
	int n = 0;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &size, &psource) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(size);
	convert_to_long_ex(psource);
	
	source = (*psource)->value.lval;
	i = (*size)->value.lval;
	
	if(i <= 0) {
		php_error(E_WARNING, "illegal size input parameter");
		RETURN_FALSE;
	}
	
	iv = ecalloc(i, 1);
	
	if(source == RANDOM || source == URANDOM) {
		int fd;

		fd = open(source == RANDOM ? "/dev/random" : "/dev/urandom",
				O_RDONLY);
		if(fd < 0) {
			efree(iv);
			php_error(E_WARNING, "cannot open source device");
			RETURN_FALSE;
		}
		n = read(fd, iv, i);
		close(fd);
	} else {
		while(i) {
			iv[--i] = 255.0 * rand() / RAND_MAX;
		}
		n = (*size)->value.lval;
	}
	RETURN_STRINGL(iv, n, 0);
}
/* }}} */

/* {{{ proto string mcrypt_get_cipher_name(int cipher)
   Get the name of cipher */
PHP_FUNCTION(mcrypt_get_cipher_name)
{
	pval **cipher;
	char *str, *nstr;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(cipher);

	str = mcrypt_get_algorithms_name((*cipher)->value.lval);
	if (str) {
		nstr = estrdup(str);
		free(str);
		RETURN_STRING(nstr, 0);
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int mcrypt_get_key_size(int cipher)
   Get the key size of cipher */
PHP_FUNCTION(mcrypt_get_key_size)
{
	pval **cipher;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(cipher);

	RETURN_LONG(mcrypt_get_key_size((*cipher)->value.lval));
}
/* }}} */

/* {{{ proto int mcrypt_get_block_size(int cipher)
   Get the block size of cipher */
PHP_FUNCTION(mcrypt_get_block_size)
{
	pval **cipher;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(cipher);

	RETURN_LONG(mcrypt_get_block_size((*cipher)->value.lval));
}
/* }}} */

/* {{{ proto string mcrypt_ofb(int cipher, string key, string data, int mode, string iv)
   OFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_ofb)
{
	MCRYPT_ARGS;
	
	if(ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5, &cipher, &key, &data, &mode, &iv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	MCRYPT_CHECK_IV;

	td = init_mcrypt_ofb((*cipher)->value.lval, (*key)->value.str.val, (*key)->value.str.len, (*iv)->value.str.val);
	MCRYPT_CHECK_TD_CPY;
	MCRYPT_ACTION(ofb);

	RETURN_STRINGL(ndata, nsize, 0);
}
/* }}} */

/* {{{ proto string mcrypt_cfb(int cipher, string key, string data, int mode, string iv)
   CFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_cfb)
{
	MCRYPT_ARGS;

	if(ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5, &cipher, &key, &data, &mode, &iv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	MCRYPT_CHECK_IV;

	td = init_mcrypt_cfb((*cipher)->value.lval, (*key)->value.str.val, (*key)->value.str.len, (*iv)->value.str.val);
	MCRYPT_CHECK_TD_CPY;
	MCRYPT_ACTION(cfb);

	RETURN_STRINGL(ndata, nsize, 0);
}
/* }}} */


/* {{{ proto string mcrypt_cbc(int cipher, string key, string data, int mode [, string iv])
   CBC crypt/decrypt data using key key with cipher cipher using optional iv */
PHP_FUNCTION(mcrypt_cbc)
{
	MCRYPT_ARGS;
	int ac = ZEND_NUM_ARGS();

	if(ac < 4 || ac > 5 || 
			zend_get_parameters_ex(ac, &cipher, &key, &data, &mode, &iv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	if(ac > 4) {
		MCRYPT_CHECK_IV;
	}
	
	td = init_mcrypt_cbc((*cipher)->value.lval, (*key)->value.str.val, (*key)->value.str.len);
	MCRYPT_CHECK_TD_CPY;
	
	if(ac > 4) {
		mcrypt(td, (*iv)->value.str.val);
	}
	
	MCRYPT_ACTION(cbc);

	RETURN_STRINGL(ndata, nsize, 0);
}
/* }}} */

/* {{{ proto string mcrypt_ecb(int cipher, string key, string data, int mode)
   ECB crypt/decrypt data using key key with cipher cipher */
PHP_FUNCTION(mcrypt_ecb)
{
	MCRYPT_ARGS2;

	if(ZEND_NUM_ARGS() != 4 || 
			zend_get_parameters_ex(4, &cipher, &key, &data, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	
	td = init_mcrypt_ecb((*cipher)->value.lval, (*key)->value.str.val, (*key)->value.str.len);
	MCRYPT_CHECK_TD_CPY;
	MCRYPT_ACTION(ecb);

	RETURN_STRINGL(ndata, nsize, 0);
}
/* }}} */

#endif
