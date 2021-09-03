/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: b3890e798e609e306027b4717ce0c5e782884087 */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Directory_close, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Directory_rewind arginfo_class_Directory_close

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Directory_read, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(closedir);
ZEND_FUNCTION(rewinddir);
ZEND_FUNCTION(readdir);


static const zend_function_entry class_Directory_methods[] = {
	ZEND_ME_MAPPING(close, closedir, arginfo_class_Directory_close, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(rewind, rewinddir, arginfo_class_Directory_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(read, readdir, arginfo_class_Directory_read, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Directory(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Directory", class_Directory_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval property_path_default_value;
	ZVAL_UNDEF(&property_path_default_value);
	zend_string *property_path_name = zend_string_init("path", sizeof("path") - 1, 1);
	zend_declare_typed_property(class_entry, property_path_name, &property_path_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_path_name);

	zval property_handle_default_value;
	ZVAL_UNDEF(&property_handle_default_value);
	zend_string *property_handle_name = zend_string_init("handle", sizeof("handle") - 1, 1);
	zend_declare_typed_property(class_entry, property_handle_name, &property_handle_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release(property_handle_name);

	return class_entry;
}
