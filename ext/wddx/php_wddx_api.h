/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:  Andrei Zmievski <andrei@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_WDDX_API_H
#define PHP_WDDX_API_H

#include "ext/standard/php_smart_str.h"

#define WDDX_ARRAY_S			"<array length='%d'>"
#define WDDX_ARRAY_E			"</array>"
#define WDDX_BINARY_S			"<binary>"
#define WDDX_BINARY_E			"</binary>"
#define WDDX_BOOLEAN_TRUE		"<boolean value='true'/>"
#define WDDX_BOOLEAN_FALSE		"<boolean value='false'/>"
#define WDDX_CHAR				"<char code='%02X'/>"
#define WDDX_COMMENT_S			"<comment>"
#define WDDX_COMMENT_E			"</comment>"
#define WDDX_DATA_S				"<data>"
#define WDDX_DATA_E				"</data>"
#define WDDX_HEADER				"<header/>"
#define WDDX_HEADER_S			"<header>"
#define WDDX_HEADER_E			"</header>"
#define WDDX_NULL				"<null/>"
#define WDDX_NUMBER				"<number>%s</number>"
#define WDDX_PACKET_S			"<wddxPacket version='1.0'>"
#define WDDX_PACKET_E			"</wddxPacket>"
#define WDDX_STRING_S			"<string>"
#define WDDX_STRING_E			"</string>"
#define WDDX_STRUCT_S			"<struct>"
#define WDDX_STRUCT_E			"</struct>"
#define WDDX_VAR_S				"<var name='%s'>"
#define WDDX_VAR_E				"</var>"

#define php_wddx_add_chunk(packet, str)	smart_str_appends(packet, str)
#define php_wddx_add_chunk_ex(packet, str, len)	smart_str_appendl(packet, str, len)
#define php_wddx_add_chunk_static(packet, str) smart_str_appendl(packet, str, sizeof(str)-1)

typedef smart_str wddx_packet;

wddx_packet* php_wddx_constructor(void);
void		 php_wddx_destructor(wddx_packet *packet);

void 		 php_wddx_packet_start(wddx_packet *packet, char *comment, int comment_len);
void 		 php_wddx_packet_end(wddx_packet *packet);

void 		 php_wddx_serialize_var(wddx_packet *packet, zval *var, char *name, int name_len TSRMLS_DC);
int 		 php_wddx_deserialize_ex(char *, int, zval *return_value);
#define php_wddx_gather(packet) estrndup(packet->c, packet->len)

#endif /* PHP_WDDX_API_H */
