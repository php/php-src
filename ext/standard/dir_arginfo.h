/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5792bd58c595d77cbea19d844db98a05c05a54cc */

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

static void register_dir_symbols(int module_number)
{
#if (defined(HAVE_GLOB) && defined(GLOB_BRACE))
	REGISTER_LONG_CONSTANT("GLOB_BRACE", GLOB_BRACE, CONST_PERSISTENT);
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_ERR))
	REGISTER_LONG_CONSTANT("GLOB_ERR", GLOB_ERR, CONST_PERSISTENT);
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_MARK))
	REGISTER_LONG_CONSTANT("GLOB_MARK", GLOB_MARK, CONST_PERSISTENT);
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_NOCHECK))
	REGISTER_LONG_CONSTANT("GLOB_NOCHECK", GLOB_NOCHECK, CONST_PERSISTENT);
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_NOESCAPE))
	REGISTER_LONG_CONSTANT("GLOB_NOESCAPE", GLOB_NOESCAPE, CONST_PERSISTENT);
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_NOSORT))
	REGISTER_LONG_CONSTANT("GLOB_NOSORT", GLOB_NOSORT, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SCANDIR_SORT_ASCENDING", PHP_SCANDIR_SORT_ASCENDING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SCANDIR_SORT_DESCENDING", PHP_SCANDIR_SORT_DESCENDING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SCANDIR_SORT_NONE", PHP_SCANDIR_SORT_NONE, CONST_PERSISTENT);
}

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
