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
   | Authors: Derick Rethans <derick@derickrethans.nl>                    |
   |          Tim Düsterhus <timwolla@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_DATE_TIME_H
# define PHP_DATE_TIME_H

# include "php.h"
# include "lib/timelib.h"

typedef struct php_date_time_duration {
	timelib_duration  duration;
	zend_object       std;
} php_date_time_duration;

# define php_date_time_duration_from_obj(obj) ZEND_CONTAINER_OF(obj, php_date_time_duration, std)

# define Z_DATE_TIME_DURATION_P(zv)  php_date_time_duration_from_obj(Z_OBJ_P((zv)))

# define Z_PARAM_DATE_TIME_DURATION(d) { \
		zend_object *__##d; \
		Z_PARAM_OBJ_OF_CLASS(__##d, php_date_ce_time_duration); \
		d = php_date_time_duration_from_obj(__##d); \
	}

# define Z_PARAM_DATE_TIME_DURATION_OR_NULL(d) { \
		zend_object *__##d; \
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(__##d, php_date_ce_time_duration); \
		d = __##d ? php_date_time_duration_from_obj(__##d) : NULL; \
	}

PHPAPI extern zend_class_entry *php_date_ce_time_duration;
PHPAPI extern zend_class_entry *php_date_ce_time_timeexception;

PHP_MINIT_FUNCTION(date_time);

#endif /* PHP_DATE_TIME_H */
