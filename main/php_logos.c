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
   | Author: Hartmut Holzgraefe <hholzgra@php.net>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "logos.h"
#include "php_logos.h"
#include "ext/standard/info.h"
#include "SAPI.h"

typedef struct _php_info_logo { 
	char *mimetype;
	int mimelen;
	unsigned char *data; 
	int size; 
} php_info_logo;

HashTable phpinfo_logo_hash;

PHPAPI int php_register_info_logo(char *logo_string, char *mimetype, unsigned char *data, int size)
{
	php_info_logo info_logo;

	info_logo.mimetype = mimetype;
	info_logo.mimelen  = strlen(mimetype);
	info_logo.data     = data;
	info_logo.size     = size;

	return zend_hash_add(&phpinfo_logo_hash, logo_string, strlen(logo_string), &info_logo, sizeof(php_info_logo), NULL);
}

PHPAPI int php_unregister_info_logo(char *logo_string)
{
	return zend_hash_del(&phpinfo_logo_hash, logo_string, strlen(logo_string));
}

int php_init_info_logos(void)
{
	if(zend_hash_init(&phpinfo_logo_hash, 0, NULL, NULL, 1)==FAILURE) 
		return FAILURE;

	php_register_info_logo(PHP_LOGO_GUID    , "image/gif", php_logo    , sizeof(php_logo));
	php_register_info_logo(PHP_EGG_LOGO_GUID, "image/gif", php_egg_logo, sizeof(php_egg_logo));
	php_register_info_logo(ZEND_LOGO_GUID   , "image/gif", zend_logo   , sizeof(zend_logo));

	return SUCCESS;
}

int php_shutdown_info_logos(void)
{
	zend_hash_destroy(&phpinfo_logo_hash);
	return SUCCESS;
}

#define CONTENT_TYPE_HEADER "Content-Type: "
int php_info_logos(const char *logo_string TSRMLS_DC)
{
	php_info_logo *logo_image;
	char *content_header;
	int len;

	if(FAILURE==zend_hash_find(&phpinfo_logo_hash, (char *) logo_string, strlen(logo_string), (void **)&logo_image))
		return 0;

	len=strlen(CONTENT_TYPE_HEADER)+logo_image->mimelen;
	content_header=malloc(len+1);
	if(!content_header) return 0;
	strcpy(content_header, CONTENT_TYPE_HEADER);
	strcat(content_header, logo_image->mimetype);
	sapi_add_header(content_header, len, 1);
	free(content_header);

	PHPWRITE(logo_image->data, logo_image->size);
	return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
