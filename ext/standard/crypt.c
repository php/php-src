/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Stig Bakken <ssb@gaurdian.no>                               |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */
#include <stdlib.h>

#include "php.h"

#if HAVE_CRYPT

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_CRYPT_H
#include <crypt.h>
#endif
#if TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#if MSVC5
#include <process.h>
extern char *crypt(char *__key,char *__salt);
#endif

#include "php3_crypt.h"

function_entry crypt_functions[] = {
	{"crypt",	php3_crypt,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry crypt_module_entry = {
	"Crypt", crypt_functions, php_minit_crypt, NULL, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};


/* 
   The capabilities of the crypt() function is determined by the test programs
   run by configure from aclocal.m4.  They will set PHP3_STD_DES_CRYPT,
   PHP3_EXT_DES_CRYPT, PHP3_MD5_CRYPT and PHP3_BLOWFISH_CRYPT as appropriate 
   for the target platform
*/
#if PHP3_STD_DES_CRYPT
#define PHP3_MAX_SALT_LEN 2
#endif
#if PHP3_EXT_DES_CRYPT
#undef PHP3_MAX_SALT_LEN
#define PHP3_MAX_SALT_LEN 9
#endif
#if PHP3_MD5_CRYPT
#undef PHP3_MAX_SALT_LEN
#define PHP3_MAX_SALT_LEN 12
#endif
#if PHP3_BLOWFISH_CRYPT
#undef PHP3_MAX_SALT_LEN
#define PHP3_MAX_SALT_LEN 17
#endif

#if HAVE_LRAND48
#define PHP3_CRYPT_RAND lrand48()
#else
#if HAVE_RANDOM
#define PHP3_CRYPT_RAND random()
#else
#define PHP3_CRYPT_RAND rand()
#endif
#endif

int php_minit_crypt(INIT_FUNC_ARGS)
{
	ELS_FETCH();

#if PHP3_STD_DES_CRYPT
    REGISTER_LONG_CONSTANT("CRYPT_SALT_LENGTH", 2, CONST_CS | CONST_PERSISTENT);
#else
#if PHP3_MD5_CRYPT
    REGISTER_LONG_CONSTANT("CRYPT_SALT_LENGTH", 12, CONST_CS | CONST_PERSISTENT);
#endif
#endif
    REGISTER_LONG_CONSTANT("CRYPT_STD_DES", PHP3_STD_DES_CRYPT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("CRYPT_EXT_DES", PHP3_EXT_DES_CRYPT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("CRYPT_MD5", PHP3_MD5_CRYPT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("CRYPT_BLOWFISH", PHP3_BLOWFISH_CRYPT, CONST_CS | CONST_PERSISTENT);
    return SUCCESS;
}

static unsigned char itoa64[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void php3i_to64(char *s, long v, int n)	{
	while (--n >= 0) {
		*s++ = itoa64[v&0x3f]; 		
		v >>= 6;
	} 
} 

PHP_FUNCTION(crypt)
{
	char salt[PHP3_MAX_SALT_LEN];
	pval *arg1, *arg2;

	salt[0]='\0';

	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &arg1)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (getParameters(ht, 2, &arg1, &arg2)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(arg2);
			memcpy(salt, arg2->value.str.val, MIN(PHP3_MAX_SALT_LEN,arg2->value.str.len));
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	convert_to_string(arg1);

	/* The automatic salt generation only covers standard DES and md5-crypt */
	if(!*salt) {
#if HAVE_SRAND48
		srand48((unsigned int) time(0) * getpid());
#else
#if HAVE_SRANDOM
		srandom((unsigned int) time(0) * getpid());
#else
		srand((unsigned int) time(0) * getpid());
#endif
#endif

#if PHP3_STD_DES_CRYPT
		php3i_to64(&salt[0], PHP3_CRYPT_RAND, 2);
		salt[2] = '\0';
#else
#if PHP3_MD5_CRYPT
		strcpy(salt, "$1$");
		php3i_to64(&salt[3], PHP3_CRYPT_RAND, 4);
		php3i_to64(&salt[7], PHP3_CRYPT_RAND, 4);
		strcpy(&salt[11], "$");
#endif
#endif
	}

	return_value->value.str.val = (char *) crypt(arg1->value.str.val, salt);
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->type = IS_STRING;
	pval_copy_constructor(return_value);
}

#endif



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
