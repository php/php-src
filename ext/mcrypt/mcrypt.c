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

#include "lcrypt.h"

function_entry mcrypt_functions[] = {
	PHP_FE(mcrypt_ecb, NULL)
	PHP_FE(mcrypt_cbc, NULL)
	PHP_FE(mcrypt_get_block_size, NULL)
	PHP_FE(mcrypt_get_key_size, NULL)
	{0},
};

static int php3_minit_mcrypt(INIT_FUNC_ARGS);

zend_module_entry mcrypt_module_entry = {
	"mcrypt", 
	mcrypt_functions,
	php3_minit_mcrypt, NULL,
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

#define MCRYPT_ENTRY(a) REGISTER_LONG_CONSTANT("MCRYPT_" #a, a, 0)

static int php3_minit_mcrypt(INIT_FUNC_ARGS)
{
	MCRYPT_ENTRY(BLOWFISH);
	MCRYPT_ENTRY(DES);
	MCRYPT_ENTRY(TripleDES);
	MCRYPT_ENTRY(ThreeWAY);
	MCRYPT_ENTRY(GOST);
	MCRYPT_ENTRY(SAFER64);
	MCRYPT_ENTRY(SAFER128);
	MCRYPT_ENTRY(CAST128);
	MCRYPT_ENTRY(TEAN);
	MCRYPT_ENTRY(TWOFISH);
	MCRYPT_ENTRY(RC2);
#ifdef CRYPT
	MCRYPT_ENTRY(CRYPT);
#endif
	return SUCCESS;
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

/* proto mcrypt_cbc(int cipher, string key, string data, int mode)
   CBC crypt/decrypt data using key key with cipher cipher */
PHP_FUNCTION(mcrypt_cbc)
{
	pval *cipher, *data, *key, *mode;
	int td;
	char *ndata;
	size_t bsize;
	size_t nr;
	size_t nsize;

	if(ARG_COUNT(ht) != 4 || 
			getParameters(ht, 4, &cipher, &key, &data, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(cipher);
	convert_to_long(mode);
	convert_to_string(data);
	convert_to_string(key);

	bsize = get_block_size(cipher->value.lval);
	nr = (data->value.str.len + bsize - 1) / bsize;
	nsize = nr * bsize;

	td = init_mcrypt_cbc(cipher->value.lval, key->value.str.val, key->value.str.len);
	if(td == -1) {
		php3_error(E_WARNING, "mcrypt initialization failed");
		RETURN_FALSE;
	}
	
	ndata = ecalloc(nr, bsize);
	memcpy(ndata, data->value.str.val, data->value.str.len);
	
	if(mode->value.lval == 0)
		mcrypt_cbc(td, ndata, nsize);
	else
		mdecrypt_cbc(td, ndata, nsize);
	
	end_mcrypt_cbc(td);

	RETURN_STRINGL(ndata, nsize, 0);
}

/* proto mcrypt_ecb(int cipher, string key, string data, int mode)
   ECB crypt/decrypt data using key key with cipher cipher */
PHP_FUNCTION(mcrypt_ecb)
{
	pval *cipher, *data, *key, *mode;
	int td;
	char *ndata;
	size_t bsize;
	size_t nr;
	size_t nsize;

	if(ARG_COUNT(ht) != 4 || 
			getParameters(ht, 4, &cipher, &key, &data, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(cipher);
	convert_to_long(mode);
	convert_to_string(data);
	convert_to_string(key);

	bsize = get_block_size(cipher->value.lval);
	nr = (data->value.str.len + bsize - 1) / bsize;
	nsize = nr * bsize;

	td = init_mcrypt_ecb(cipher->value.lval, key->value.str.val, key->value.str.len);
	if(td == -1) {
		php3_error(E_WARNING, "mcrypt initialization failed");
		RETURN_FALSE;
	}
	
	ndata = ecalloc(nr, bsize);
	memcpy(ndata, data->value.str.val, data->value.str.len);
	
	if(mode->value.lval == 0)
		mcrypt_ecb(td, ndata, nsize);
	else
		mdecrypt_ecb(td, ndata, nsize);
	
	end_mcrypt_ecb(td);

	RETURN_STRINGL(ndata, nsize, 0);
}

#endif
