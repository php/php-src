#ifndef ZEND_OBJECTS_H
#define ZEND_OBJECTS_H

#include "zend.h"

zend_object_value zend_objects_new(zend_object **object, zend_class_entry *class_type);
ZEND_API zend_object *zend_objects_get_address(zval *object);
zend_object_value zend_objects_clone_obj(zval *object TSRMLS_DC);

#endif /* ZEND_OBJECTS_H */
