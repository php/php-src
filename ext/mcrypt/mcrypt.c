/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 4.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1999 PHP Development Team (See Credits file)           |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.2ns.de>                    |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#if HAVE_LIBMCRYPT

#include "php_mcrypt.h"
#include "fcntl.h"

/* we should find a way to figure out whether RC6/IDEA are available */
#define NON_FREE
#include "mcrypt.h"

function_entry mcrypt_functions[] = {
	PHP_FE(mcrypt_ecb, NULL)
	PHP_FE(mcrypt_cbc, NULL)
	PHP_FE(mcrypt_cfb, NULL)
	PHP_FE(mcrypt_ofb, NULL)
	PHP_FE(mcrypt_get_algorithms_name, NULL)
	PHP_FE(mcrypt_get_block_size, NULL)
	PHP_FE(mcrypt_get_key_size, NULL)
	PHP_FE(mcrypt_create_iv, NULL)
	{0},
};

static int php_minit_mcrypt(INIT_FUNC_ARGS);

zend_module_entry mcrypt_module_entry = {
	"mcrypt", 
	mcrypt_functions,
	php_minit_mcrypt, NULL,
	NULL, NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES,
};

#if 0

typedef struct mcrypt_global_struct {
	int le_h;
} mcrypt_global_struct;

static mcrypt_global_struct mcryptg;

#define MCRYPTG(x) mcryptg.x

#endif

#define MCRYPT_ARGS2 \
	pval *cipher, *data, *key, *mode; \
	int td; \
	char *ndata; \
	size_t bsize; \
	size_t nr; \
	size_t nsize

#define MCRYPT_ARGS MCRYPT_ARGS2; pval *iv

#define MCRYPT_CONVERT \
	convert_to_long(cipher); \
	convert_to_long(mode); \
	convert_to_string(data); \
	convert_to_string(key)

#define MCRYPT_SIZE \
	bsize = get_block_size(cipher->value.lval); \
	nr = (data->value.str.len + bsize - 1) / bsize; \
	nsize = nr * bsize

#define MCRYPT_CHECK_TD_CPY \
	if(td == -1) { \
		php3_error(E_WARNING, MCRYPT_FAILED); \
		RETURN_FALSE; \
	} \
	ndata = ecalloc(nr, bsize); \
	memcpy(ndata, data->value.str.val, data->value.str.len)

#define MCRYPT_CHECK_IV \
	convert_to_string(iv); \
	if(iv->value.str.len != bsize) { \
		php3_error(E_WARNING, MCRYPT_IV_WRONG_SIZE); \
		RETURN_FALSE; \
	}

#define MCRYPT_ACTION(x) \
	if(mode->value.lval == 0) \
		mcrypt_##x(td, ndata, nsize); \
	else \
		mdecrypt_##x(td, ndata, nsize); \
	end_mcrypt_##x(td)

#define MCRYPT_IV_WRONG_SIZE "The IV paramater must be as long as the blocksize"
#define MCRYPT_FAILED "mcrypt initialization failed"

#define MCRYPT_ENTRY(a) REGISTER_LONG_CONSTANT("MCRYPT_" #a, a, 0)

static int php_minit_mcrypt(INIT_FUNC_ARGS)
{
	/* modes for mcrypt_??? routines */
	REGISTER_LONG_CONSTANT("MCRYPT_ENCRYPT", 0, 0);
	REGISTER_LONG_CONSTANT("MCRYPT_DECRYPT", 1, 0);
	
	/* sources for mcrypt_create_iv */
	REGISTER_LONG_CONSTANT("MCRYPT_DEV_RANDOM", 0, 0);
	REGISTER_LONG_CONSTANT("MCRYPT_DEV_URANDOM", 1, 0);
	REGISTER_LONG_CONSTANT("MCRYPT_RAND", 2, 0);
	
	/* ciphers */
	MCRYPT_ENTRY(BLOWFISH);
	MCRYPT_ENTRY(DES);
	MCRYPT_ENTRY(TripleDES);
	MCRYPT_ENTRY(ThreeWAY);
	MCRYPT_ENTRY(GOST);
#ifdef MCRYPT2
	MCRYPT_ENTRY(CRYPT);
	MCRYPT_ENTRY(DES_COMPAT);
#endif
	MCRYPT_ENTRY(SAFER64);
	MCRYPT_ENTRY(SAFER128);
	MCRYPT_ENTRY(CAST128);
	MCRYPT_ENTRY(TEAN);
	MCRYPT_ENTRY(RC2);
#ifdef TWOFISH
	MCRYPT_ENTRY(TWOFISH);
#elif defined(TWOFISH128)
	MCRYPT_ENTRY(TWOFISH128);
	MCRYPT_ENTRY(TWOFISH192);
	MCRYPT_ENTRY(TWOFISH256);
#endif
#ifdef RC6
	MCRYPT_ENTRY(RC6);
#endif
#ifdef IDEA
	MCRYPT_ENTRY(IDEA);
#endif
	
	return SUCCESS;
}

typedef enum {
	RANDOM = 0,
	URANDOM,
	RAND
} iv_source;

/* proto mcrypt_create_iv(int size, int source)
   create an initializing vector (IV) */
PHP_FUNCTION(mcrypt_create_iv)
{
	pval *size, *psource;
	char *iv;
	iv_source source;
	int i;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &size, &psource) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(size);
	convert_to_long(psource);
	source = psource->value.lval;

	i = size->value.lval;
	if(i <= 0) {
		php3_error(E_WARNING, "illegal size input parameter");
		RETURN_FALSE;
	}
	
	iv = ecalloc(i, 1);
	
	if(source == RANDOM || source == URANDOM) {
		int fd;

		fd = open(source == RANDOM ? "/dev/random" : "/dev/urandom",
				O_RDONLY);
		if(fd < 0) {
			efree(iv);
			php3_error(E_WARNING, "cannot open source device");
			RETURN_FALSE;
		}
		read(fd, iv, i);
		close(fd);
	} else {
		while(i) {
			iv[--i] = rand();
		}
	}
	RETURN_STRINGL(iv, size->value.lval, 0);
}

/* proto mcrypt_get_algorithms_name(int cipher)
   get the name of cipher */
PHP_FUNCTION(mcrypt_get_algorithms_name)
{
	pval *cipher;
	char *str, *nstr;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(cipher);

	str = get_algorithms_name(cipher->value.lval);
	nstr = estrdup(str);
	free(str);

	RETURN_STRING(nstr, 0);
}

/* proto mcrypt_get_key_size(int cipher)
   get the key size of cipher */
PHP_FUNCTION(mcrypt_get_key_size)
{
	pval *cipher;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(cipher);

	RETURN_LONG(get_key_size(cipher->value.lval));
}

/* proto mcrypt_get_block_size(int cipher)
   get the block size of cipher */
PHP_FUNCTION(mcrypt_get_block_size)
{
	pval *cipher;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &cipher) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(cipher);

	RETURN_LONG(get_block_size(cipher->value.lval));
}

/* proto mcrypt_ofb(int cipher, string key, string data, int mode, string iv)
   OFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_ofb)
{
	MCRYPT_ARGS;
	
	if(ARG_COUNT(ht) != 5 || 
			getParameters(ht, 5, &cipher, &key, &data, &mode, &iv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	MCRYPT_CHECK_IV;

	td = init_mcrypt_ofb(cipher->value.lval, key->value.str.val, key->value.str.len, iv->value.str.val);
	MCRYPT_CHECK_TD_CPY;
	MCRYPT_ACTION(ofb);

	RETURN_STRINGL(ndata, nsize, 0);
}

/* proto mcrypt_cfb(int cipher, string key, string data, int mode, string iv)
   CFB crypt/decrypt data using key key with cipher cipher starting with iv */
PHP_FUNCTION(mcrypt_cfb)
{
	MCRYPT_ARGS;

	if(ARG_COUNT(ht) != 5 || 
			getParameters(ht, 5, &cipher, &key, &data, &mode, &iv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	MCRYPT_CHECK_IV;

	td = init_mcrypt_cfb(cipher->value.lval, key->value.str.val, key->value.str.len, iv->value.str.val);
	MCRYPT_CHECK_TD_CPY;
	MCRYPT_ACTION(cfb);

	RETURN_STRINGL(ndata, nsize, 0);
}


/* proto mcrypt_cbc(int cipher, string key, string data, int mode [,string iv])
   CBC crypt/decrypt data using key key with cipher cipher using optional iv */
PHP_FUNCTION(mcrypt_cbc)
{
	MCRYPT_ARGS;
	int ac = ARG_COUNT(ht);

	if(ac < 4 || ac > 5 || 
			getParameters(ht, ac, &cipher, &key, &data, &mode, &iv) == 
			FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	if(ac > 4 && mode == 0) {
		MCRYPT_CHECK_IV;
	}
	
	td = init_mcrypt_cbc(cipher->value.lval, key->value.str.val, key->value.str.len);
	MCRYPT_CHECK_TD_CPY;
	
	/* iv may be only used in encryption */
	if(ac > 4 && mode == 0) {
		mcrypt(td, iv->value.str.val);
	}
	
	MCRYPT_ACTION(cbc);

	RETURN_STRINGL(ndata, nsize, 0);
}

/* proto mcrypt_ecb(int cipher, string key, string data, int mode)
   ECB crypt/decrypt data using key key with cipher cipher */
PHP_FUNCTION(mcrypt_ecb)
{
	MCRYPT_ARGS2;

	if(ARG_COUNT(ht) != 4 || 
			getParameters(ht, 4, &cipher, &key, &data, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MCRYPT_CONVERT;
	MCRYPT_SIZE;
	
	td = init_mcrypt_ecb(cipher->value.lval, key->value.str.val, key->value.str.len);
	MCRYPT_CHECK_TD_CPY;
	MCRYPT_ACTION(ecb);

	RETURN_STRINGL(ndata, nsize, 0);
}

#endif
