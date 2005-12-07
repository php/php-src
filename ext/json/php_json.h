/*
 * Copyright (c) 2005 Omar Kilani <omar@rmilk.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public (LGPL)
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details: http://www.gnu.org/
 *
 */

/* $Id$ */

#ifndef PHP_JSON_H
#define PHP_JSON_H

#define PHP_JSON_VERSION "1.1.0"

extern zend_module_entry json_module_entry;
#define phpext_json_ptr &json_module_entry

#ifdef PHP_WIN32
#define PHP_JSON_API __declspec(dllexport)
#else
#define PHP_JSON_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINFO_FUNCTION(json);

PHP_FUNCTION(json_encode);
PHP_FUNCTION(json_decode);

#ifdef ZTS
#define JSON_G(v) TSRMG(json_globals_id, zend_json_globals *, v)
#else
#define JSON_G(v) (json_globals.v)
#endif

#endif	/* PHP_JSON_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
