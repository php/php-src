/*
  +----------------------------------------------------------------------+
  | simdjson_php                                                         |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  +----------------------------------------------------------------------+
  | Author: Jinxi Wang  <1054636713@qq.com>                              |
  | Author: Máté Kocsis <kocsismate@woohoolabs.com>                      |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_SIMDJSON_H
#define PHP_SIMDJSON_H

extern zend_module_entry simdjson_module_entry;
#define phpext_simdjson_ptr &simdjson_module_entry

#include "php_version.h"
#define PHP_SIMDJSON_VERSION PHP_VERSION

#define SIMDJSON_PARSE_FAIL                   0
#define SIMDJSON_PARSE_SUCCESS                1
#define SIMDJSON_PARSE_KEY_EXISTS             2
#define SIMDJSON_PARSE_KEY_NOEXISTS           3

#define SIMDJSON_PARSE_DEFAULT_DEPTH          512

#define SIMDJSON_RESOUCE_PJH_TYPE             3
#define SIMDJSON_RESOUCE_PJ_TYPE              4

ZEND_BEGIN_MODULE_GLOBALS(simdjson)
ZEND_END_MODULE_GLOBALS(simdjson)

PHP_MINIT_FUNCTION(simdjson);
PHP_MSHUTDOWN_FUNCTION(simdjson);
PHP_RINIT_FUNCTION(simdjson);
PHP_RSHUTDOWN_FUNCTION(simdjson);
PHP_MINFO_FUNCTION(simdjson);

#endif
