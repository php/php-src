/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 05487a0b7668688c1885a14a8ce02493b96f63bb */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Pdo_Mysql_getWarningCount, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(Pdo_Mysql, getWarningCount);

static const zend_function_entry class_Pdo_Mysql_methods[] = {
	ZEND_ME(Pdo_Mysql, getWarningCount, arginfo_class_Pdo_Mysql_getWarningCount, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Pdo_Mysql(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Pdo", "Mysql", class_Pdo_Mysql_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_PDO, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zval const_ATTR_USE_BUFFERED_QUERY_value;
	ZVAL_LONG(&const_ATTR_USE_BUFFERED_QUERY_value, PDO_MYSQL_ATTR_USE_BUFFERED_QUERY);
	zend_string *const_ATTR_USE_BUFFERED_QUERY_name = zend_string_init_interned("ATTR_USE_BUFFERED_QUERY", strlen("ATTR_USE_BUFFERED_QUERY"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_USE_BUFFERED_QUERY_name, &const_ATTR_USE_BUFFERED_QUERY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_USE_BUFFERED_QUERY_name);

	zval const_ATTR_LOCAL_INFILE_value;
	ZVAL_LONG(&const_ATTR_LOCAL_INFILE_value, PDO_MYSQL_ATTR_LOCAL_INFILE);
	zend_string *const_ATTR_LOCAL_INFILE_name = zend_string_init_interned("ATTR_LOCAL_INFILE", strlen("ATTR_LOCAL_INFILE"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_LOCAL_INFILE_name, &const_ATTR_LOCAL_INFILE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_LOCAL_INFILE_name);

	zval const_ATTR_INIT_COMMAND_value;
	ZVAL_LONG(&const_ATTR_INIT_COMMAND_value, PDO_MYSQL_ATTR_INIT_COMMAND);
	zend_string *const_ATTR_INIT_COMMAND_name = zend_string_init_interned("ATTR_INIT_COMMAND", strlen("ATTR_INIT_COMMAND"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_INIT_COMMAND_name, &const_ATTR_INIT_COMMAND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_INIT_COMMAND_name);
#if !defined(PDO_USE_MYSQLND)

	zval const_ATTR_MAX_BUFFER_SIZE_value;
	ZVAL_LONG(&const_ATTR_MAX_BUFFER_SIZE_value, PDO_MYSQL_ATTR_MAX_BUFFER_SIZE);
	zend_string *const_ATTR_MAX_BUFFER_SIZE_name = zend_string_init_interned("ATTR_MAX_BUFFER_SIZE", strlen("ATTR_MAX_BUFFER_SIZE"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_MAX_BUFFER_SIZE_name, &const_ATTR_MAX_BUFFER_SIZE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_MAX_BUFFER_SIZE_name);
#endif
#if !defined(PDO_USE_MYSQLND)

	zval const_ATTR_READ_DEFAULT_FILE_value;
	ZVAL_LONG(&const_ATTR_READ_DEFAULT_FILE_value, PDO_MYSQL_ATTR_READ_DEFAULT_FILE);
	zend_string *const_ATTR_READ_DEFAULT_FILE_name = zend_string_init_interned("ATTR_READ_DEFAULT_FILE", strlen("ATTR_READ_DEFAULT_FILE"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_READ_DEFAULT_FILE_name, &const_ATTR_READ_DEFAULT_FILE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_READ_DEFAULT_FILE_name);
#endif
#if !defined(PDO_USE_MYSQLND)

	zval const_ATTR_READ_DEFAULT_GROUP_value;
	ZVAL_LONG(&const_ATTR_READ_DEFAULT_GROUP_value, PDO_MYSQL_ATTR_READ_DEFAULT_GROUP);
	zend_string *const_ATTR_READ_DEFAULT_GROUP_name = zend_string_init_interned("ATTR_READ_DEFAULT_GROUP", strlen("ATTR_READ_DEFAULT_GROUP"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_READ_DEFAULT_GROUP_name, &const_ATTR_READ_DEFAULT_GROUP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_READ_DEFAULT_GROUP_name);
#endif

	zval const_ATTR_COMPRESS_value;
	ZVAL_LONG(&const_ATTR_COMPRESS_value, PDO_MYSQL_ATTR_COMPRESS);
	zend_string *const_ATTR_COMPRESS_name = zend_string_init_interned("ATTR_COMPRESS", strlen("ATTR_COMPRESS"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_COMPRESS_name, &const_ATTR_COMPRESS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_COMPRESS_name);

	zval const_ATTR_DIRECT_QUERY_value;
	ZVAL_LONG(&const_ATTR_DIRECT_QUERY_value, PDO_MYSQL_ATTR_DIRECT_QUERY);
	zend_string *const_ATTR_DIRECT_QUERY_name = zend_string_init_interned("ATTR_DIRECT_QUERY", strlen("ATTR_DIRECT_QUERY"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_DIRECT_QUERY_name, &const_ATTR_DIRECT_QUERY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_DIRECT_QUERY_name);

	zval const_ATTR_FOUND_ROWS_value;
	ZVAL_LONG(&const_ATTR_FOUND_ROWS_value, PDO_MYSQL_ATTR_FOUND_ROWS);
	zend_string *const_ATTR_FOUND_ROWS_name = zend_string_init_interned("ATTR_FOUND_ROWS", strlen("ATTR_FOUND_ROWS"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_FOUND_ROWS_name, &const_ATTR_FOUND_ROWS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_FOUND_ROWS_name);

	zval const_ATTR_IGNORE_SPACE_value;
	ZVAL_LONG(&const_ATTR_IGNORE_SPACE_value, PDO_MYSQL_ATTR_IGNORE_SPACE);
	zend_string *const_ATTR_IGNORE_SPACE_name = zend_string_init_interned("ATTR_IGNORE_SPACE", strlen("ATTR_IGNORE_SPACE"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_IGNORE_SPACE_name, &const_ATTR_IGNORE_SPACE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_IGNORE_SPACE_name);

	zval const_ATTR_SSL_KEY_value;
	ZVAL_LONG(&const_ATTR_SSL_KEY_value, PDO_MYSQL_ATTR_SSL_KEY);
	zend_string *const_ATTR_SSL_KEY_name = zend_string_init_interned("ATTR_SSL_KEY", strlen("ATTR_SSL_KEY"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SSL_KEY_name, &const_ATTR_SSL_KEY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SSL_KEY_name);

	zval const_ATTR_SSL_CERT_value;
	ZVAL_LONG(&const_ATTR_SSL_CERT_value, PDO_MYSQL_ATTR_SSL_CERT);
	zend_string *const_ATTR_SSL_CERT_name = zend_string_init_interned("ATTR_SSL_CERT", strlen("ATTR_SSL_CERT"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SSL_CERT_name, &const_ATTR_SSL_CERT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SSL_CERT_name);

	zval const_ATTR_SSL_CA_value;
	ZVAL_LONG(&const_ATTR_SSL_CA_value, PDO_MYSQL_ATTR_SSL_CA);
	zend_string *const_ATTR_SSL_CA_name = zend_string_init_interned("ATTR_SSL_CA", strlen("ATTR_SSL_CA"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SSL_CA_name, &const_ATTR_SSL_CA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SSL_CA_name);

	zval const_ATTR_SSL_CAPATH_value;
	ZVAL_LONG(&const_ATTR_SSL_CAPATH_value, PDO_MYSQL_ATTR_SSL_CAPATH);
	zend_string *const_ATTR_SSL_CAPATH_name = zend_string_init_interned("ATTR_SSL_CAPATH", strlen("ATTR_SSL_CAPATH"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SSL_CAPATH_name, &const_ATTR_SSL_CAPATH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SSL_CAPATH_name);

	zval const_ATTR_SSL_CIPHER_value;
	ZVAL_LONG(&const_ATTR_SSL_CIPHER_value, PDO_MYSQL_ATTR_SSL_CIPHER);
	zend_string *const_ATTR_SSL_CIPHER_name = zend_string_init_interned("ATTR_SSL_CIPHER", strlen("ATTR_SSL_CIPHER"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SSL_CIPHER_name, &const_ATTR_SSL_CIPHER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SSL_CIPHER_name);
#if MYSQL_VERSION_ID > 50605 || defined(PDO_USE_MYSQLND)

	zval const_ATTR_SERVER_PUBLIC_KEY_value;
	ZVAL_LONG(&const_ATTR_SERVER_PUBLIC_KEY_value, PDO_MYSQL_ATTR_SERVER_PUBLIC_KEY);
	zend_string *const_ATTR_SERVER_PUBLIC_KEY_name = zend_string_init_interned("ATTR_SERVER_PUBLIC_KEY", strlen("ATTR_SERVER_PUBLIC_KEY"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SERVER_PUBLIC_KEY_name, &const_ATTR_SERVER_PUBLIC_KEY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SERVER_PUBLIC_KEY_name);
#endif

	zval const_ATTR_MULTI_STATEMENTS_value;
	ZVAL_LONG(&const_ATTR_MULTI_STATEMENTS_value, PDO_MYSQL_ATTR_MULTI_STATEMENTS);
	zend_string *const_ATTR_MULTI_STATEMENTS_name = zend_string_init_interned("ATTR_MULTI_STATEMENTS", strlen("ATTR_MULTI_STATEMENTS"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_MULTI_STATEMENTS_name, &const_ATTR_MULTI_STATEMENTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_MULTI_STATEMENTS_name);
#if defined(PDO_USE_MYSQLND)

	zval const_ATTR_SSL_VERIFY_SERVER_CERT_value;
	ZVAL_LONG(&const_ATTR_SSL_VERIFY_SERVER_CERT_value, PDO_MYSQL_ATTR_SSL_VERIFY_SERVER_CERT);
	zend_string *const_ATTR_SSL_VERIFY_SERVER_CERT_name = zend_string_init_interned("ATTR_SSL_VERIFY_SERVER_CERT", strlen("ATTR_SSL_VERIFY_SERVER_CERT"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SSL_VERIFY_SERVER_CERT_name, &const_ATTR_SSL_VERIFY_SERVER_CERT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SSL_VERIFY_SERVER_CERT_name);
#endif
#if MYSQL_VERSION_ID >= 80021 || defined(PDO_USE_MYSQLND)

	zval const_ATTR_LOCAL_INFILE_DIRECTORY_value;
	ZVAL_LONG(&const_ATTR_LOCAL_INFILE_DIRECTORY_value, PDO_MYSQL_ATTR_LOCAL_INFILE_DIRECTORY);
	zend_string *const_ATTR_LOCAL_INFILE_DIRECTORY_name = zend_string_init_interned("ATTR_LOCAL_INFILE_DIRECTORY", strlen("ATTR_LOCAL_INFILE_DIRECTORY"), 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_LOCAL_INFILE_DIRECTORY_name, &const_ATTR_LOCAL_INFILE_DIRECTORY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_LOCAL_INFILE_DIRECTORY_name);
#endif

	return class_entry;
}
