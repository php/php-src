/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Authors: Mark Randall <marandall@php.net>                            |
   +----------------------------------------------------------------------+
 */

#include "gd_image_object.h"
#include "gd_arginfo.h"
#include "zend.h"
#include "zend_interfaces.h"
#include "../../Zend/zend.h"

/*
 * List of unique methods implemented by GdImage
 */

static zend_object_handlers php_gd_image_object_handlers;
static PHP_METHOD(GdImage, __wakeup);

static const zend_function_entry gd_image_object_methods[] = {
	PHP_FE_END
};

static zend_function *php_gd_image_object_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "You cannot initialize a GdImage object except through helper functions");
	return NULL;
}

zend_object *php_gd_image_object_create(zend_class_entry *class_type) {
	size_t block_len = sizeof(php_gd_image_object) + zend_object_properties_size(class_type);
	php_gd_image_object *intern = emalloc(block_len);
	memset(intern, 0, block_len);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &php_gd_image_object_handlers;

	return &intern->std;
};

static void php_gd_image_object_free(zend_object *intern) {
	php_gd_image_object *img_obj_ptr = php_gd_exgdimage_from_zobj_p(intern);
	gdImageDestroy(img_obj_ptr->image);
	img_obj_ptr->image = NULL;

	zend_object_std_dtor(intern);
};

void php_gd_assign_libgdimageptr_as_extgdimage(zval *val, gdImagePtr image) {
	object_init_ex(val, gd_image_ce);
	php_gd_exgdimage_from_zobj_p(Z_OBJ_P(val))->image = image;
}

void php_gd_object_minit_helper() {
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "GdImage", gd_image_object_methods);
	gd_image_ce = zend_register_internal_class(&ce);
	gd_image_ce->ce_flags |= ZEND_ACC_FINAL;
	gd_image_ce->create_object = php_gd_image_object_create;
	gd_image_ce->serialize = zend_class_serialize_deny;
	gd_image_ce->unserialize = zend_class_unserialize_deny;

	/* setting up the object handlers for the GdImage class */
	memcpy(&php_gd_image_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_gd_image_object_handlers.clone_obj = NULL;
	php_gd_image_object_handlers.free_obj = php_gd_image_object_free;
	php_gd_image_object_handlers.get_constructor = php_gd_image_object_get_constructor;
	php_gd_image_object_handlers.offset = XtOffsetOf(php_gd_image_object, std);
}
