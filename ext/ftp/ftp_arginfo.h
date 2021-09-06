/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f47161a8575ef53f7c0fa6037c30e0c70462a281 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_ftp_connect, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 0, "21")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "90")
ZEND_END_ARG_INFO()

#if defined(HAVE_FTP_SSL)
ZEND_BEGIN_ARG_INFO_EX(arginfo_ftp_ssl_connect, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 0, "21")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "90")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_login, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_pwd, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, ftp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_cdup, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_chdir, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_exec, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_raw, 0, 2, IS_ARRAY, 1)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_mkdir, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ftp_rmdir arginfo_ftp_chdir

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_chmod, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, permissions, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_alloc, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, response, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_nlist, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_rawlist, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, recursive, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_ftp_mlsd arginfo_ftp_nlist

#define arginfo_ftp_systype arginfo_ftp_pwd

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_fget, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_nb_fget, 0, 3, IS_LONG, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_pasv, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_get, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_nb_get, 0, 3, IS_LONG, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_nb_continue, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, ftp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_fput, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_nb_fput, 0, 3, IS_LONG, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_put, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_append, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_nb_put, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, remote_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, local_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "FTP_BINARY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_size, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ftp_mdtm arginfo_ftp_size

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_rename, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_delete, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ftp_site arginfo_ftp_exec

#define arginfo_ftp_close arginfo_ftp_cdup

#define arginfo_ftp_quit arginfo_ftp_cdup

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftp_set_option, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, ftp)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftp_get_option, 0, 2, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, ftp)
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
	ZEND_FE(ftp_connect, arginfo_ftp_connect)
#if defined(HAVE_FTP_SSL)
	ZEND_FE(ftp_ssl_connect, arginfo_ftp_ssl_connect)
#endif
	ZEND_FE(ftp_login, arginfo_ftp_login)
	ZEND_FE(ftp_pwd, arginfo_ftp_pwd)
	ZEND_FE(ftp_cdup, arginfo_ftp_cdup)
	ZEND_FE(ftp_chdir, arginfo_ftp_chdir)
	ZEND_FE(ftp_exec, arginfo_ftp_exec)
	ZEND_FE(ftp_raw, arginfo_ftp_raw)
	ZEND_FE(ftp_mkdir, arginfo_ftp_mkdir)
	ZEND_FE(ftp_rmdir, arginfo_ftp_rmdir)
	ZEND_FE(ftp_chmod, arginfo_ftp_chmod)
	ZEND_FE(ftp_alloc, arginfo_ftp_alloc)
	ZEND_FE(ftp_nlist, arginfo_ftp_nlist)
	ZEND_FE(ftp_rawlist, arginfo_ftp_rawlist)
	ZEND_FE(ftp_mlsd, arginfo_ftp_mlsd)
	ZEND_FE(ftp_systype, arginfo_ftp_systype)
	ZEND_FE(ftp_fget, arginfo_ftp_fget)
	ZEND_FE(ftp_nb_fget, arginfo_ftp_nb_fget)
	ZEND_FE(ftp_pasv, arginfo_ftp_pasv)
	ZEND_FE(ftp_get, arginfo_ftp_get)
	ZEND_FE(ftp_nb_get, arginfo_ftp_nb_get)
	ZEND_FE(ftp_nb_continue, arginfo_ftp_nb_continue)
	ZEND_FE(ftp_fput, arginfo_ftp_fput)
	ZEND_FE(ftp_nb_fput, arginfo_ftp_nb_fput)
	ZEND_FE(ftp_put, arginfo_ftp_put)
	ZEND_FE(ftp_append, arginfo_ftp_append)
	ZEND_FE(ftp_nb_put, arginfo_ftp_nb_put)
	ZEND_FE(ftp_size, arginfo_ftp_size)
	ZEND_FE(ftp_mdtm, arginfo_ftp_mdtm)
	ZEND_FE(ftp_rename, arginfo_ftp_rename)
	ZEND_FE(ftp_delete, arginfo_ftp_delete)
	ZEND_FE(ftp_site, arginfo_ftp_site)
	ZEND_FE(ftp_close, arginfo_ftp_close)
	ZEND_FALIAS(ftp_quit, ftp_close, arginfo_ftp_quit)
	ZEND_FE(ftp_set_option, arginfo_ftp_set_option)
	ZEND_FE(ftp_get_option, arginfo_ftp_get_option)
	ZEND_FE_END
};
