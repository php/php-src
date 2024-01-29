/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 072486274a3361dee3655cfd046a293cfb8a2757 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_ftp_connect, 0, 1, FTP\\Connection, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 0, "21")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "90")
ZEND_END_ARG_INFO()

#if defined(HAVE_FTP_SSL)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_ftp_ssl_connect, 0, 1, FTP\\Connection, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 0, "21")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "90")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_login, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_pwd, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_cdup, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_chdir, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_exec, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_raw, 0, 2, IS_ARRAY, 1)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_mkdir, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ftp_rmdir arginfo_ftp_chdir

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_chmod, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, permissions, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_alloc, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, response, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_nlist, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_rawlist, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, recursive, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_ftp_mlsd arginfo_ftp_nlist

#define arginfo_ftp_systype arginfo_ftp_pwd

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_fget, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_nb_fget, 0, 3, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_pasv, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_get, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_nb_get, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_nb_continue, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_fput, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_nb_fput, 0, 3, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_put, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_append, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_nb_put, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_size, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ftp_mdtm arginfo_ftp_size

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_rename, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_delete, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ftp_site arginfo_ftp_exec

#define arginfo_ftp_close arginfo_ftp_cdup

#define arginfo_ftp_quit arginfo_ftp_cdup

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_set_option, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_get_option, 0, 2, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, ftp, FTP\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(ftp_connect);
#if defined(HAVE_FTP_SSL)
ZEND_FUNCTION(ftp_ssl_connect);
#endif
ZEND_FUNCTION(ftp_login);
ZEND_FUNCTION(ftp_pwd);
ZEND_FUNCTION(ftp_cdup);
ZEND_FUNCTION(ftp_chdir);
ZEND_FUNCTION(ftp_exec);
ZEND_FUNCTION(ftp_raw);
ZEND_FUNCTION(ftp_mkdir);
ZEND_FUNCTION(ftp_rmdir);
ZEND_FUNCTION(ftp_chmod);
ZEND_FUNCTION(ftp_alloc);
ZEND_FUNCTION(ftp_nlist);
ZEND_FUNCTION(ftp_rawlist);
ZEND_FUNCTION(ftp_mlsd);
ZEND_FUNCTION(ftp_systype);
ZEND_FUNCTION(ftp_fget);
ZEND_FUNCTION(ftp_nb_fget);
ZEND_FUNCTION(ftp_pasv);
ZEND_FUNCTION(ftp_get);
ZEND_FUNCTION(ftp_nb_get);
ZEND_FUNCTION(ftp_nb_continue);
ZEND_FUNCTION(ftp_fput);
ZEND_FUNCTION(ftp_nb_fput);
ZEND_FUNCTION(ftp_put);
ZEND_FUNCTION(ftp_append);
ZEND_FUNCTION(ftp_nb_put);
ZEND_FUNCTION(ftp_size);
ZEND_FUNCTION(ftp_mdtm);
ZEND_FUNCTION(ftp_rename);
ZEND_FUNCTION(ftp_delete);
ZEND_FUNCTION(ftp_site);
ZEND_FUNCTION(ftp_close);
ZEND_FUNCTION(ftp_set_option);
ZEND_FUNCTION(ftp_get_option);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("ftp_connect", zif_ftp_connect, arginfo_ftp_connect, 0, NULL)
#if defined(HAVE_FTP_SSL)
	ZEND_RAW_FENTRY("ftp_ssl_connect", zif_ftp_ssl_connect, arginfo_ftp_ssl_connect, 0, NULL)
#endif
	ZEND_RAW_FENTRY("ftp_login", zif_ftp_login, arginfo_ftp_login, 0, NULL)
	ZEND_RAW_FENTRY("ftp_pwd", zif_ftp_pwd, arginfo_ftp_pwd, 0, NULL)
	ZEND_RAW_FENTRY("ftp_cdup", zif_ftp_cdup, arginfo_ftp_cdup, 0, NULL)
	ZEND_RAW_FENTRY("ftp_chdir", zif_ftp_chdir, arginfo_ftp_chdir, 0, NULL)
	ZEND_RAW_FENTRY("ftp_exec", zif_ftp_exec, arginfo_ftp_exec, 0, NULL)
	ZEND_RAW_FENTRY("ftp_raw", zif_ftp_raw, arginfo_ftp_raw, 0, NULL)
	ZEND_RAW_FENTRY("ftp_mkdir", zif_ftp_mkdir, arginfo_ftp_mkdir, 0, NULL)
	ZEND_RAW_FENTRY("ftp_rmdir", zif_ftp_rmdir, arginfo_ftp_rmdir, 0, NULL)
	ZEND_RAW_FENTRY("ftp_chmod", zif_ftp_chmod, arginfo_ftp_chmod, 0, NULL)
	ZEND_RAW_FENTRY("ftp_alloc", zif_ftp_alloc, arginfo_ftp_alloc, 0, NULL)
	ZEND_RAW_FENTRY("ftp_nlist", zif_ftp_nlist, arginfo_ftp_nlist, 0, NULL)
	ZEND_RAW_FENTRY("ftp_rawlist", zif_ftp_rawlist, arginfo_ftp_rawlist, 0, NULL)
	ZEND_RAW_FENTRY("ftp_mlsd", zif_ftp_mlsd, arginfo_ftp_mlsd, 0, NULL)
	ZEND_RAW_FENTRY("ftp_systype", zif_ftp_systype, arginfo_ftp_systype, 0, NULL)
	ZEND_RAW_FENTRY("ftp_fget", zif_ftp_fget, arginfo_ftp_fget, 0, NULL)
	ZEND_RAW_FENTRY("ftp_nb_fget", zif_ftp_nb_fget, arginfo_ftp_nb_fget, 0, NULL)
	ZEND_RAW_FENTRY("ftp_pasv", zif_ftp_pasv, arginfo_ftp_pasv, 0, NULL)
	ZEND_RAW_FENTRY("ftp_get", zif_ftp_get, arginfo_ftp_get, 0, NULL)
	ZEND_RAW_FENTRY("ftp_nb_get", zif_ftp_nb_get, arginfo_ftp_nb_get, 0, NULL)
	ZEND_RAW_FENTRY("ftp_nb_continue", zif_ftp_nb_continue, arginfo_ftp_nb_continue, 0, NULL)
	ZEND_RAW_FENTRY("ftp_fput", zif_ftp_fput, arginfo_ftp_fput, 0, NULL)
	ZEND_RAW_FENTRY("ftp_nb_fput", zif_ftp_nb_fput, arginfo_ftp_nb_fput, 0, NULL)
	ZEND_RAW_FENTRY("ftp_put", zif_ftp_put, arginfo_ftp_put, 0, NULL)
	ZEND_RAW_FENTRY("ftp_append", zif_ftp_append, arginfo_ftp_append, 0, NULL)
	ZEND_RAW_FENTRY("ftp_nb_put", zif_ftp_nb_put, arginfo_ftp_nb_put, 0, NULL)
	ZEND_RAW_FENTRY("ftp_size", zif_ftp_size, arginfo_ftp_size, 0, NULL)
	ZEND_RAW_FENTRY("ftp_mdtm", zif_ftp_mdtm, arginfo_ftp_mdtm, 0, NULL)
	ZEND_RAW_FENTRY("ftp_rename", zif_ftp_rename, arginfo_ftp_rename, 0, NULL)
	ZEND_RAW_FENTRY("ftp_delete", zif_ftp_delete, arginfo_ftp_delete, 0, NULL)
	ZEND_RAW_FENTRY("ftp_site", zif_ftp_site, arginfo_ftp_site, 0, NULL)
	ZEND_RAW_FENTRY("ftp_close", zif_ftp_close, arginfo_ftp_close, 0, NULL)
	ZEND_RAW_FENTRY("ftp_quit", zif_ftp_close, arginfo_ftp_quit, 0, NULL)
	ZEND_RAW_FENTRY("ftp_set_option", zif_ftp_set_option, arginfo_ftp_set_option, 0, NULL)
	ZEND_RAW_FENTRY("ftp_get_option", zif_ftp_get_option, arginfo_ftp_get_option, 0, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_FTP_Connection_methods[] = {
	ZEND_FE_END
};

static void register_ftp_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("FTP_ASCII", FTPTYPE_ASCII, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_TEXT", FTPTYPE_ASCII, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_BINARY", FTPTYPE_IMAGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_IMAGE", FTPTYPE_IMAGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_AUTORESUME", PHP_FTP_AUTORESUME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_TIMEOUT_SEC", PHP_FTP_OPT_TIMEOUT_SEC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_AUTOSEEK", PHP_FTP_OPT_AUTOSEEK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_USEPASVADDRESS", PHP_FTP_OPT_USEPASVADDRESS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_FAILED", PHP_FTP_FAILED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_FINISHED", PHP_FTP_FINISHED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FTP_MOREDATA", PHP_FTP_MOREDATA, CONST_PERSISTENT);


	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "ftp_login", sizeof("ftp_login") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
}

static zend_class_entry *register_class_FTP_Connection(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FTP", "Connection", class_FTP_Connection_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
