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

#include "php.h"
#include "Zend/zend_exceptions.h"

#include "php_time.h"
#include "time_arginfo.h"

zend_class_entry *php_date_ce_time_duration;
zend_class_entry *php_date_ce_time_timeexception;

static zend_object_handlers time_duration_object_handlers;

static zend_object *time_duration_object_create(zend_class_entry *ce)
{
	php_date_time_duration *obj = zend_object_alloc(sizeof(*obj), ce);

	zend_object_std_init(&obj->std, ce);
	object_properties_init(&obj->std, ce);

	timelib_duration_ctor_static(&obj->duration, /* seconds */ 0, /* nanoseconds */ 0, /* negative */ false);

	return &obj->std;
}

static zend_object *time_duration_object_clone(zend_object *object)
{
	const php_date_time_duration *obj = php_date_time_duration_from_obj(object);

	php_date_time_duration *new_obj = php_date_time_duration_from_obj(object->ce->create_object(object->ce));

	new_obj->duration = obj->duration;
	zend_objects_clone_members(&new_obj->std, &obj->std);

	return &new_obj->std;
}

PHP_MINIT_FUNCTION(date_time)
{
	/* Time\TimeException */
	php_date_ce_time_timeexception = register_class_Time_TimeException(zend_ce_exception);

	/* Time\Duration */
	memcpy(&time_duration_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	time_duration_object_handlers.offset = offsetof(php_date_time_duration, std);
	time_duration_object_handlers.clone_obj = time_duration_object_clone;
	php_date_ce_time_duration = register_class_Time_Duration();
	php_date_ce_time_duration->create_object = time_duration_object_create;
	php_date_ce_time_duration->default_object_handlers = &time_duration_object_handlers;

	return SUCCESS;
}
