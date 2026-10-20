/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
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

xmlChar *schema_location_construct_uri(const xmlAttr *attribute);

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
