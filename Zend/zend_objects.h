#ifndef ZEND_OBJECTS_H
#define ZEND_OBJECTS_H

#include "zend.h"

typedef struct _zend_object_bucket {
	zend_bool valid;
	union _bucket {
		struct {
			zend_object object;
			zend_uint refcount;
		} obj;
		struct {
			int next;
		} free_list;
	} bucket;
} zend_object_bucket;

typedef struct _zend_objects {
	zend_object_bucket *object_buckets;
	zend_uint top;
	zend_uint size;
	int free_list_head;
} zend_objects;

void zend_objects_init(zend_objects *objects, zend_uint init_size);
void zend_objects_destroy(zend_objects *objects);
zend_object_value zend_objects_new(zend_object **object);
zend_object *zend_objects_get_address(zend_object_handle handle);
void zend_objects_add_ref(zend_object_handle handle);
void zend_objects_del_ref(zend_object_handle handle);
void zend_objects_delete_obj(zend_object_handle handle);

#endif /* ZEND_OBJECTS_H */