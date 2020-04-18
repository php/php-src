/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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
  |          Dmitry Stogov <dmitry@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_SCHEMA_H
#define PHP_SCHEMA_H

int load_schema(sdlCtx *ctx, xmlNodePtr schema);
void schema_pass2(sdlCtx *ctx);

void delete_model(zval *zv);
void delete_model_persistent(zval *zv);
void delete_type(zval *zv);
void delete_type_persistent(zval *zv);
void delete_extra_attribute(zval *zv);
void delete_extra_attribute_persistent(zval *zv);
void delete_attribute(zval *zv);
void delete_attribute_persistent(zval *zv);
void delete_restriction_var_int(sdlRestrictionIntPtr ptr);
void delete_restriction_var_int_persistent(sdlRestrictionIntPtr ptr);
void delete_restriction_var_char(zval *zv);
void delete_restriction_var_char_int(sdlRestrictionCharPtr ptr);
void delete_restriction_var_char_persistent(zval *zv);
void delete_restriction_var_char_persistent_int(sdlRestrictionCharPtr ptr);
#endif
