#ifndef ZEND_ATTRIBUTES_H
#define ZEND_ATTRIBUTES_H

#define ZEND_ATTRIBUTE_TARGET_CLASS			(1<<0)
#define ZEND_ATTRIBUTE_TARGET_FUNCTION		(1<<1)
#define ZEND_ATTRIBUTE_TARGET_METHOD		(1<<2)
#define ZEND_ATTRIBUTE_TARGET_PROPERTY		(1<<3)
#define ZEND_ATTRIBUTE_TARGET_CLASS_CONST	(1<<4)
#define ZEND_ATTRIBUTE_TARGET_PARAMETER		(1<<5)
#define ZEND_ATTRIBUTE_TARGET_ALL			(1<<6)

#define ZEND_ATTRIBUTE_SIZE(argc) (sizeof(zend_attribute) + sizeof(zval) * (argc) - sizeof(zval))

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_php_attribute;

typedef struct _zend_attribute {
	zend_string *name;
	zend_string *lcname;
	/* Parameter offsets start at 1, everything else uses 0. */
	uint32_t offset;
	uint32_t argc;
	zval argv[1];
} zend_attribute;

typedef void (*zend_attributes_internal_validator)(zend_attribute *attr, int target);

ZEND_API void zend_attribute_free(zend_attribute *attr, int persistent);

ZEND_API zend_attribute *zend_get_attribute(HashTable *attributes, zend_string *lcname);
ZEND_API zend_attribute *zend_get_attribute_str(HashTable *attributes, const char *str, size_t len);

ZEND_API zend_attribute *zend_get_parameter_attribute(HashTable *attributes, zend_string *lcname, uint32_t offset);
ZEND_API zend_attribute *zend_get_parameter_attribute_str(HashTable *attributes, const char *str, size_t len, uint32_t offset);

ZEND_API void zend_compiler_attribute_register(zend_class_entry *ce, zend_attributes_internal_validator validator);
ZEND_API zend_attributes_internal_validator zend_attribute_get_validator(zend_string *lcname);

void zend_register_attribute_ce(void);

END_EXTERN_C()

#endif
