/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@zend.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id$ */

#ifndef PHP_SCHEMA_H
#define PHP_SCHEMA_H

int load_schema(sdlCtx *ctx, xmlNodePtr schema TSRMLS_DC);
void schema_pass2(sdlCtx *ctx);

void delete_model(void *handle);
void delete_model_persistent(void *handle);
void delete_type(void *data);
void delete_type_persistent(void *data);
void delete_extra_attribute(void *attribute);
void delete_extra_attribute_persistent(void *attribute);
void delete_attribute(void *attribute);
void delete_attribute_persistent(void *attribute);
void delete_restriction_var_int(void *rvi);
void delete_restriction_var_int_persistent(void *rvi);
void delete_restriction_var_char(void *srvc);
void delete_restriction_var_char_persistent(void *srvc);
#endif
