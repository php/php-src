/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f6424cef1dc6ce6c28ec7ecff00fbbb41d1e6b70 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Url_Url___construct, 0, 0, 8)
	ZEND_ARG_TYPE_INFO(0, scheme, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fragment, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Url_Url___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Url_UrlParser_parseUrl, 0, 1, Url\\\125rl, 0)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Url_UrlParser_parseUrlArray, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Url_UrlParser_parseUrlComponent, 0, 2, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, component, Url\\\125rlComponent, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Url_Url, __construct);
ZEND_METHOD(Url_Url, __toString);
ZEND_METHOD(Url_UrlParser, parseUrl);
ZEND_METHOD(Url_UrlParser, parseUrlArray);
ZEND_METHOD(Url_UrlParser, parseUrlComponent);


static const zend_function_entry class_Url_UrlComponent_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_Url_Url_methods[] = {
	ZEND_ME(Url_Url, __construct, arginfo_class_Url_Url___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Url_Url, __toString, arginfo_class_Url_Url___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Url_UrlParser_methods[] = {
	ZEND_ME(Url_UrlParser, parseUrl, arginfo_class_Url_UrlParser_parseUrl, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Url_UrlParser, parseUrlArray, arginfo_class_Url_UrlParser_parseUrlArray, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Url_UrlParser, parseUrlComponent, arginfo_class_Url_UrlParser_parseUrlComponent, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Url_UrlComponent(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Url\\UrlComponent", IS_LONG, class_Url_UrlComponent_methods);

	zval enum_case_Scheme_value;
	ZVAL_LONG(&enum_case_Scheme_value, 0);
	zend_enum_add_case_cstr(class_entry, "Scheme", &enum_case_Scheme_value);

	zval enum_case_Host_value;
	ZVAL_LONG(&enum_case_Host_value, 1);
	zend_enum_add_case_cstr(class_entry, "Host", &enum_case_Host_value);

	zval enum_case_Port_value;
	ZVAL_LONG(&enum_case_Port_value, 2);
	zend_enum_add_case_cstr(class_entry, "Port", &enum_case_Port_value);

	zval enum_case_User_value;
	ZVAL_LONG(&enum_case_User_value, 3);
	zend_enum_add_case_cstr(class_entry, "User", &enum_case_User_value);

	zval enum_case_Password_value;
	ZVAL_LONG(&enum_case_Password_value, 4);
	zend_enum_add_case_cstr(class_entry, "Password", &enum_case_Password_value);

	zval enum_case_Path_value;
	ZVAL_LONG(&enum_case_Path_value, 5);
	zend_enum_add_case_cstr(class_entry, "Path", &enum_case_Path_value);

	zval enum_case_Query_value;
	ZVAL_LONG(&enum_case_Query_value, 6);
	zend_enum_add_case_cstr(class_entry, "Query", &enum_case_Query_value);

	zval enum_case_Fragment_value;
	ZVAL_LONG(&enum_case_Fragment_value, 7);
	zend_enum_add_case_cstr(class_entry, "Fragment", &enum_case_Fragment_value);

	return class_entry;
}

static zend_class_entry *register_class_Url_Url(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Url", "Url", class_Url_Url_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_READONLY_CLASS;
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	zval property_scheme_default_value;
	ZVAL_UNDEF(&property_scheme_default_value);
	zend_string *property_scheme_name = zend_string_init("scheme", sizeof("scheme") - 1, 1);
	zend_declare_typed_property(class_entry, property_scheme_name, &property_scheme_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_scheme_name);

	zval property_host_default_value;
	ZVAL_UNDEF(&property_host_default_value);
	zend_string *property_host_name = zend_string_init("host", sizeof("host") - 1, 1);
	zend_declare_typed_property(class_entry, property_host_name, &property_host_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_host_name);

	zval property_port_default_value;
	ZVAL_UNDEF(&property_port_default_value);
	zend_string *property_port_name = zend_string_init("port", sizeof("port") - 1, 1);
	zend_declare_typed_property(class_entry, property_port_name, &property_port_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_port_name);

	zval property_user_default_value;
	ZVAL_UNDEF(&property_user_default_value);
	zend_string *property_user_name = zend_string_init("user", sizeof("user") - 1, 1);
	zend_declare_typed_property(class_entry, property_user_name, &property_user_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_user_name);

	zval property_password_default_value;
	ZVAL_UNDEF(&property_password_default_value);
	zend_string *property_password_name = zend_string_init("password", sizeof("password") - 1, 1);
	zend_declare_typed_property(class_entry, property_password_name, &property_password_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_password_name);

	zval property_path_default_value;
	ZVAL_UNDEF(&property_path_default_value);
	zend_string *property_path_name = zend_string_init("path", sizeof("path") - 1, 1);
	zend_declare_typed_property(class_entry, property_path_name, &property_path_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_path_name);

	zval property_query_default_value;
	ZVAL_UNDEF(&property_query_default_value);
	zend_string *property_query_name = zend_string_init("query", sizeof("query") - 1, 1);
	zend_declare_typed_property(class_entry, property_query_name, &property_query_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_query_name);

	zval property_fragment_default_value;
	ZVAL_UNDEF(&property_fragment_default_value);
	zend_string *property_fragment_name = zend_string_init("fragment", sizeof("fragment") - 1, 1);
	zend_declare_typed_property(class_entry, property_fragment_name, &property_fragment_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_fragment_name);

	return class_entry;
}

static zend_class_entry *register_class_Url_UrlParser(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Url", "UrlParser", class_Url_UrlParser_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
