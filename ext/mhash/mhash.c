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

#if HAVE_LIBMHASH

#include "php_mhash.h"

#include "mhash.h"

function_entry mhash_functions[] = {
	PHP_FE(mhash_get_block_size, NULL)
	PHP_FE(mhash, NULL)
	{0},
};

static int php_minit_mhash(INIT_FUNC_ARGS);

zend_module_entry mhash_module_entry = {
	"mhash", 
	mhash_functions,
	php_minit_mhash, NULL,
	NULL, NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES,
};

#define MHASH_FAILED "mhash initialization failed"

#define MHASH_ENTRY(a) REGISTER_LONG_CONSTANT("MHASH_" #a, a, 0)

static int php_minit_mhash(INIT_FUNC_ARGS)
{
	/* hashes */
	MHASH_ENTRY(CRC32);
	MHASH_ENTRY(MD5);
	MHASH_ENTRY(SHA1);
	MHASH_ENTRY(HAVAL);
	MHASH_ENTRY(RIPEMD128);
	MHASH_ENTRY(RIPEMD160);
	MHASH_ENTRY(TIGER);
	MHASH_ENTRY(SNEFRU);
	MHASH_ENTRY(GOST);
	
	return SUCCESS;
}

/* proto mhash_get_block_size(int hash)
   get the block size of hash */
PHP_FUNCTION(mhash_get_block_size)
{
	pval *hash;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &hash) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(hash);

	RETURN_LONG(mhash_get_block_size(hash->value.lval));
}

/* proto mhash(int hash, string data)
   hash data with hash */
PHP_FUNCTION(mhash)
{
	pval *hash, *data;
	int td;
	int bsize;
	unsigned char *hash_data;
	int i;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &hash, &data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(hash);
	convert_to_string(data);

	bsize = mhash_get_block_size(hash->value.lval);
	td = init_mhash(hash->value.lval);
	if(td == -1) {
		php3_error(E_WARNING, MHASH_FAILED);
		RETURN_FALSE;
	}

	mhash(td, data->value.str.val, data->value.str.len);

	hash_data = (char *) end_mhash(td);
	
	RETVAL_STRINGL(hash_data, bsize, 1);
	
	free(hash_data);
}

#endif
