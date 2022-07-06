/*
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
   | Authors: Andrew Skalski <askalski@chek.com>                          |
   |          Stefan Esser <sesser@php.net> (resume functions)            |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef HAVE_FTP_SSL
# include <openssl/ssl.h>
#endif

#ifdef HAVE_FTP

#include "ext/standard/info.h"
#include "ext/standard/file.h"

#include "php_ftp.h"
#include "ftp.h"
#include "ftp_arginfo.h"

static int le_ftpbuf;
#define le_ftpbuf_name "FTP Buffer"

zend_module_entry php_ftp_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	NULL,
	"ftp",
	ext_functions,
	PHP_MINIT(ftp),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(ftp),
	PHP_FTP_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FTP
ZEND_GET_MODULE(php_ftp)
#endif

static void ftp_destructor_ftpbuf(zend_resource *rsrc)
{
	ftpbuf_t *ftp = (ftpbuf_t *)rsrc->ptr;

	ftp_close(ftp);
}

PHP_MINIT_FUNCTION(ftp)
{
#ifdef HAVE_FTP_SSL
#if OPENSSL_VERSION_NUMBER < 0x10101000 && !defined(LIBRESSL_VERSION_NUMBER)
	SSL_library_init();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	OpenSSL_add_all_algorithms();

	SSL_load_error_strings();
#endif
#endif

	le_ftpbuf = zend_register_list_destructors_ex(ftp_destructor_ftpbuf, NULL, le_ftpbuf_name, module_number);
	REGISTER_LONG_CONSTANT("FTP_ASCII",  FTPTYPE_ASCII, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_TEXT",   FTPTYPE_ASCII, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_BINARY", FTPTYPE_IMAGE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_IMAGE",  FTPTYPE_IMAGE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_AUTORESUME", PHP_FTP_AUTORESUME, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_TIMEOUT_SEC", PHP_FTP_OPT_TIMEOUT_SEC, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_AUTOSEEK", PHP_FTP_OPT_AUTOSEEK, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_USEPASVADDRESS", PHP_FTP_OPT_USEPASVADDRESS, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_FAILED", PHP_FTP_FAILED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_FINISHED", PHP_FTP_FINISHED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_MOREDATA", PHP_FTP_MOREDATA, CONST_PERSISTENT | CONST_CS);
	return SUCCESS;
}

PHP_MINFO_FUNCTION(ftp)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "FTP support", "enabled");
#ifdef HAVE_FTP_SSL
	php_info_print_table_row(2, "FTPS support", "enabled");
#else
	php_info_print_table_row(2, "FTPS support", "disabled");
#endif
	php_info_print_table_end();
}

#define	XTYPE(xtype, mode)	{ \
	if (mode != FTPTYPE_ASCII && mode != FTPTYPE_IMAGE) { \
		zend_argument_value_error(4, "must be either FTP_ASCII or FTP_BINARY"); \
		RETURN_THROWS(); \
	} \
	xtype = mode; \
}


/* {{{ Opens a FTP stream */
PHP_FUNCTION(ftp_connect)
{
	ftpbuf_t	*ftp;
	char		*host;
	size_t		host_len;
	zend_long 		port = 0;
	zend_long		timeout_sec = FTP_DEFAULT_TIMEOUT;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|ll", &host, &host_len, &port, &timeout_sec) == FAILURE) {
		RETURN_THROWS();
	}

	if (timeout_sec <= 0) {
		zend_argument_value_error(3, "must be greater than 0");
		RETURN_THROWS();
	}

	/* connect */
	if (!(ftp = ftp_open(host, (short)port, timeout_sec))) {
		RETURN_FALSE;
	}

	/* autoseek for resuming */
	ftp->autoseek = FTP_DEFAULT_AUTOSEEK;
	ftp->usepasvaddress = FTP_DEFAULT_USEPASVADDRESS;
#ifdef HAVE_FTP_SSL
	/* disable ssl */
	ftp->use_ssl = 0;
#endif

	RETURN_RES(zend_register_resource(ftp, le_ftpbuf));
}
/* }}} */

#ifdef HAVE_FTP_SSL
/* {{{ Opens a FTP-SSL stream */
PHP_FUNCTION(ftp_ssl_connect)
{
	ftpbuf_t	*ftp;
	char		*host;
	size_t		host_len;
	zend_long		port = 0;
	zend_long		timeout_sec = FTP_DEFAULT_TIMEOUT;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|ll", &host, &host_len, &port, &timeout_sec) == FAILURE) {
		RETURN_THROWS();
	}

	if (timeout_sec <= 0) {
		zend_argument_value_error(3, "must be greater than 0");
		RETURN_THROWS();
	}

	/* connect */
	if (!(ftp = ftp_open(host, (short)port, timeout_sec))) {
		RETURN_FALSE;
	}

	/* autoseek for resuming */
	ftp->autoseek = FTP_DEFAULT_AUTOSEEK;
	ftp->usepasvaddress = FTP_DEFAULT_USEPASVADDRESS;
	/* enable ssl */
	ftp->use_ssl = 1;

	RETURN_RES(zend_register_resource(ftp, le_ftpbuf));
}
/* }}} */
#endif

/* {{{ Logs into the FTP server */
PHP_FUNCTION(ftp_login)
{
	zval 		*z_ftp;
	ftpbuf_t	*ftp;
	char *user, *pass;
	size_t user_len, pass_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss", &z_ftp, &user, &user_len, &pass, &pass_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* log in */
	if (!ftp_login(ftp, user, user_len, pass, pass_len)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns the present working directory */
PHP_FUNCTION(ftp_pwd)
{
	zval 		*z_ftp;
	ftpbuf_t	*ftp;
	const char	*pwd;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_ftp) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	if (!(pwd = ftp_pwd(ftp))) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_STRING((char*) pwd);
}
/* }}} */

/* {{{ Changes to the parent directory */
PHP_FUNCTION(ftp_cdup)
{
	zval 		*z_ftp;
	ftpbuf_t	*ftp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_ftp) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	if (!ftp_cdup(ftp)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Changes directories */
PHP_FUNCTION(ftp_chdir)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*dir;
	size_t			dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* change directories */
	if (!ftp_chdir(ftp, dir, dir_len)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Requests execution of a program on the FTP server */
PHP_FUNCTION(ftp_exec)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*cmd;
	size_t			cmd_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &z_ftp, &cmd, &cmd_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* execute serverside command */
	if (!ftp_exec(ftp, cmd, cmd_len)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sends a literal command to the FTP server */
PHP_FUNCTION(ftp_raw)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*cmd;
	size_t			cmd_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &z_ftp, &cmd, &cmd_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* execute arbitrary ftp command */
	ftp_raw(ftp, cmd, cmd_len, return_value);
}
/* }}} */

/* {{{ Creates a directory and returns the absolute path for the new directory or false on error */
PHP_FUNCTION(ftp_mkdir)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*dir;
	zend_string *tmp;
	size_t		dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* create directory */
	if (NULL == (tmp = ftp_mkdir(ftp, dir, dir_len))) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_STR(tmp);
}
/* }}} */

/* {{{ Removes a directory */
PHP_FUNCTION(ftp_rmdir)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*dir;
	size_t		dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* remove directorie */
	if (!ftp_rmdir(ftp, dir, dir_len)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sets permissions on a file */
PHP_FUNCTION(ftp_chmod)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*filename;
	size_t		filename_len;
	zend_long		mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlp", &z_ftp, &mode, &filename, &filename_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	if (!ftp_chmod(ftp, mode, filename, filename_len)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_LONG(mode);
}
/* }}} */

/* {{{ Attempt to allocate space on the remote FTP server */
PHP_FUNCTION(ftp_alloc)
{
	zval		*z_ftp, *zresponse = NULL;
	ftpbuf_t	*ftp;
	zend_long		size, ret;
	zend_string	*response = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl|z", &z_ftp, &size, &zresponse) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	ret = ftp_alloc(ftp, size, zresponse ? &response : NULL);

	if (response) {
		ZEND_TRY_ASSIGN_REF_STR(zresponse, response);
	}

	if (!ret) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns an array of filenames in the given directory */
PHP_FUNCTION(ftp_nlist)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		**nlist, **ptr, *dir;
	size_t		dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rp", &z_ftp, &dir, &dir_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* get list of files */
	if (NULL == (nlist = ftp_nlist(ftp, dir, dir_len))) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = nlist; *ptr; ptr++) {
		add_next_index_string(return_value, *ptr);
	}
	efree(nlist);
}
/* }}} */

/* {{{ Returns a detailed listing of a directory as an array of output lines */
PHP_FUNCTION(ftp_rawlist)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		**llist, **ptr, *dir;
	size_t		dir_len;
	zend_bool	recursive = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|b", &z_ftp, &dir, &dir_len, &recursive) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* get raw directory listing */
	if (NULL == (llist = ftp_list(ftp, dir, dir_len, recursive))) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = llist; *ptr; ptr++) {
		add_next_index_string(return_value, *ptr);
	}
	efree(llist);
}
/* }}} */

/* {{{ Returns a detailed listing of a directory as an array of parsed output lines */
PHP_FUNCTION(ftp_mlsd)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		**llist, **ptr, *dir;
	size_t		dir_len;
	zval		entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* get raw directory listing */
	if (NULL == (llist = ftp_mlsd(ftp, dir, dir_len))) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = llist; *ptr; ptr++) {
		array_init(&entry);
		if (ftp_mlsd_parse_line(Z_ARRVAL_P(&entry), *ptr) == SUCCESS) {
			zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &entry);
		} else {
			zval_ptr_dtor(&entry);
		}
	}

	efree(llist);
}
/* }}} */

/* {{{ Returns the system type identifier */
PHP_FUNCTION(ftp_systype)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	const char	*syst;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_ftp) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	if (NULL == (syst = ftp_syst(ftp))) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_STRING((char*) syst);
}
/* }}} */

/* {{{ Retrieves a file from the FTP server and writes it to an open file */
PHP_FUNCTION(ftp_fget)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	php_stream	*stream;
	char		*file;
	size_t		file_len;
	zend_long		mode=FTPTYPE_IMAGE, resumepos=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rrs|ll", &z_ftp, &z_file, &file, &file_len, &mode, &resumepos) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}
	php_stream_from_res(stream, Z_RES_P(z_file));
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && resumepos == PHP_FTP_AUTORESUME) {
		resumepos = 0;
	}

	if (ftp->autoseek && resumepos) {
		/* if autoresume is wanted seek to end */
		if (resumepos == PHP_FTP_AUTORESUME) {
			php_stream_seek(stream, 0, SEEK_END);
			resumepos = php_stream_tell(stream);
		} else {
			php_stream_seek(stream, resumepos, SEEK_SET);
		}
	}

	if (!ftp_get(ftp, stream, file, file_len, xtype, resumepos)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Retrieves a file from the FTP server asynchronly and writes it to an open file */
PHP_FUNCTION(ftp_nb_fget)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	php_stream	*stream;
	char		*file;
	size_t		file_len;
	zend_long		mode=FTPTYPE_IMAGE, resumepos=0, ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rrs|ll", &z_ftp, &z_file, &file, &file_len, &mode, &resumepos) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}
	php_stream_from_res(stream, Z_RES_P(z_file));
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && resumepos == PHP_FTP_AUTORESUME) {
		resumepos = 0;
	}

	if (ftp->autoseek && resumepos) {
		/* if autoresume is wanted seek to end */
		if (resumepos == PHP_FTP_AUTORESUME) {
			php_stream_seek(stream, 0, SEEK_END);
			resumepos = php_stream_tell(stream);
		} else {
			php_stream_seek(stream, resumepos, SEEK_SET);
		}
	}

	/* configuration */
	ftp->direction = 0;   /* recv */
	ftp->closestream = 0; /* do not close */

	if ((ret = ftp_nb_get(ftp, stream, file, file_len, xtype, resumepos)) == PHP_FTP_FAILED) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_LONG(ret);
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ Turns passive mode on or off */
PHP_FUNCTION(ftp_pasv)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	zend_bool	pasv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rb", &z_ftp, &pasv) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	if (!ftp_pasv(ftp, pasv ? 1 : 0)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Retrieves a file from the FTP server and writes it to a local file */
PHP_FUNCTION(ftp_get)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	php_stream	*outstream;
	char		*local, *remote;
	size_t		local_len, remote_len;
	zend_long		mode=FTPTYPE_IMAGE, resumepos=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rpp|ll", &z_ftp, &local, &local_len, &remote, &remote_len, &mode, &resumepos) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && resumepos == PHP_FTP_AUTORESUME) {
		resumepos = 0;
	}

#ifdef PHP_WIN32
	mode = FTPTYPE_IMAGE;
#endif

	if (ftp->autoseek && resumepos) {
		outstream = php_stream_open_wrapper(local, mode == FTPTYPE_ASCII ? "rt+" : "rb+", REPORT_ERRORS, NULL);
		if (outstream == NULL) {
			outstream = php_stream_open_wrapper(local, mode == FTPTYPE_ASCII ? "wt" : "wb", REPORT_ERRORS, NULL);
		}
		if (outstream != NULL) {
			/* if autoresume is wanted seek to end */
			if (resumepos == PHP_FTP_AUTORESUME) {
				php_stream_seek(outstream, 0, SEEK_END);
				resumepos = php_stream_tell(outstream);
			} else {
				php_stream_seek(outstream, resumepos, SEEK_SET);
			}
		}
	} else {
		outstream = php_stream_open_wrapper(local, mode == FTPTYPE_ASCII ? "wt" : "wb", REPORT_ERRORS, NULL);
	}

	if (outstream == NULL)	{
		php_error_docref(NULL, E_WARNING, "Error opening %s", local);
		RETURN_FALSE;
	}

	if (!ftp_get(ftp, outstream, remote, remote_len, xtype, resumepos)) {
		php_stream_close(outstream);
		VCWD_UNLINK(local);
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	php_stream_close(outstream);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Retrieves a file from the FTP server nbhronly and writes it to a local file */
PHP_FUNCTION(ftp_nb_get)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	php_stream	*outstream;
	char		*local, *remote;
	size_t		local_len, remote_len;
	int ret;
	zend_long		mode=FTPTYPE_IMAGE, resumepos=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss|ll", &z_ftp, &local, &local_len, &remote, &remote_len, &mode, &resumepos) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && resumepos == PHP_FTP_AUTORESUME) {
		resumepos = 0;
	}
#ifdef PHP_WIN32
	mode = FTPTYPE_IMAGE;
#endif
	if (ftp->autoseek && resumepos) {
		outstream = php_stream_open_wrapper(local, mode == FTPTYPE_ASCII ? "rt+" : "rb+", REPORT_ERRORS, NULL);
		if (outstream == NULL) {
			outstream = php_stream_open_wrapper(local, mode == FTPTYPE_ASCII ? "wt" : "wb", REPORT_ERRORS, NULL);
		}
		if (outstream != NULL) {
			/* if autoresume is wanted seek to end */
			if (resumepos == PHP_FTP_AUTORESUME) {
				php_stream_seek(outstream, 0, SEEK_END);
				resumepos = php_stream_tell(outstream);
			} else {
				php_stream_seek(outstream, resumepos, SEEK_SET);
			}
		}
	} else {
		outstream = php_stream_open_wrapper(local, mode == FTPTYPE_ASCII ? "wt" : "wb", REPORT_ERRORS, NULL);
	}

	if (outstream == NULL)	{
		php_error_docref(NULL, E_WARNING, "Error opening %s", local);
		RETURN_FALSE;
	}

	/* configuration */
	ftp->direction = 0;   /* recv */
	ftp->closestream = 1; /* do close */

	if ((ret = ftp_nb_get(ftp, outstream, remote, remote_len, xtype, resumepos)) == PHP_FTP_FAILED) {
		php_stream_close(outstream);
		ftp->stream = NULL;
		VCWD_UNLINK(local);
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_LONG(PHP_FTP_FAILED);
	}

	if (ret == PHP_FTP_FINISHED){
		php_stream_close(outstream);
		ftp->stream = NULL;
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ Continues retrieving/sending a file nbronously */
PHP_FUNCTION(ftp_nb_continue)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	zend_long		ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_ftp) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	if (!ftp->nb) {
		php_error_docref(NULL, E_WARNING, "No nbronous transfer to continue");
		RETURN_LONG(PHP_FTP_FAILED);
	}

	if (ftp->direction) {
		ret=ftp_nb_continue_write(ftp);
	} else {
		ret=ftp_nb_continue_read(ftp);
	}

	if (ret != PHP_FTP_MOREDATA && ftp->closestream) {
		php_stream_close(ftp->stream);
		ftp->stream = NULL;
	}

	if (ret == PHP_FTP_FAILED) {
		php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ Stores a file from an open file to the FTP server */
PHP_FUNCTION(ftp_fput)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	size_t		remote_len;
	zend_long		mode=FTPTYPE_IMAGE, startpos=0;
	php_stream	*stream;
	char		*remote;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsr|ll", &z_ftp, &remote, &remote_len, &z_file, &mode, &startpos) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}
	php_stream_from_zval(stream, z_file);
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && startpos == PHP_FTP_AUTORESUME) {
		startpos = 0;
	}

	if (ftp->autoseek && startpos) {
		/* if autoresume is wanted ask for remote size */
		if (startpos == PHP_FTP_AUTORESUME) {
			startpos = ftp_size(ftp, remote, remote_len);
			if (startpos < 0) {
				startpos = 0;
			}
		}
		if (startpos) {
			php_stream_seek(stream, startpos, SEEK_SET);
		}
	}

	if (!ftp_put(ftp, remote, remote_len, stream, xtype, startpos)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Stores a file from an open file to the FTP server nbronly */
PHP_FUNCTION(ftp_nb_fput)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	size_t		remote_len;
	int             ret;
	zend_long	mode=FTPTYPE_IMAGE, startpos=0;
	php_stream	*stream;
	char		*remote;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsr|ll", &z_ftp, &remote, &remote_len, &z_file, &mode, &startpos) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}
	php_stream_from_res(stream, Z_RES_P(z_file));
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && startpos == PHP_FTP_AUTORESUME) {
		startpos = 0;
	}

	if (ftp->autoseek && startpos) {
		/* if autoresume is wanted ask for remote size */
		if (startpos == PHP_FTP_AUTORESUME) {
			startpos = ftp_size(ftp, remote, remote_len);
			if (startpos < 0) {
				startpos = 0;
			}
		}
		if (startpos) {
			php_stream_seek(stream, startpos, SEEK_SET);
		}
	}

	/* configuration */
	ftp->direction = 1;   /* send */
	ftp->closestream = 0; /* do not close */

	if (((ret = ftp_nb_put(ftp, remote, remote_len, stream, xtype, startpos)) == PHP_FTP_FAILED)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_LONG(ret);
	}

	RETURN_LONG(ret);
}
/* }}} */


/* {{{ Stores a file on the FTP server */
PHP_FUNCTION(ftp_put)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	char		*remote, *local;
	size_t		remote_len, local_len;
	zend_long		mode=FTPTYPE_IMAGE, startpos=0;
	php_stream 	*instream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rpp|ll", &z_ftp, &remote, &remote_len, &local, &local_len, &mode, &startpos) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}
	XTYPE(xtype, mode);

	if (!(instream = php_stream_open_wrapper(local, mode == FTPTYPE_ASCII ? "rt" : "rb", REPORT_ERRORS, NULL))) {
		RETURN_FALSE;
	}

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && startpos == PHP_FTP_AUTORESUME) {
		startpos = 0;
	}

	if (ftp->autoseek && startpos) {
		/* if autoresume is wanted ask for remote size */
		if (startpos == PHP_FTP_AUTORESUME) {
			startpos = ftp_size(ftp, remote, remote_len);
			if (startpos < 0) {
				startpos = 0;
			}
		}
		if (startpos) {
			php_stream_seek(instream, startpos, SEEK_SET);
		}
	}

	if (!ftp_put(ftp, remote, remote_len, instream, xtype, startpos)) {
		php_stream_close(instream);
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}
	php_stream_close(instream);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Append content of a file a another file on the FTP server */
PHP_FUNCTION(ftp_append)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	char		*remote, *local;
	size_t		remote_len, local_len;
	zend_long		mode=FTPTYPE_IMAGE;
	php_stream 	*instream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rpp|l", &z_ftp, &remote, &remote_len, &local, &local_len, &mode) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}
	XTYPE(xtype, mode);

	if (!(instream = php_stream_open_wrapper(local, mode == FTPTYPE_ASCII ? "rt" : "rb", REPORT_ERRORS, NULL))) {
		RETURN_FALSE;
	}

	if (!ftp_append(ftp, remote, remote_len, instream, xtype)) {
		php_stream_close(instream);
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}
	php_stream_close(instream);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Stores a file on the FTP server */
PHP_FUNCTION(ftp_nb_put)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	char		*remote, *local;
	size_t		remote_len, local_len;
	zend_long		mode=FTPTYPE_IMAGE, startpos=0, ret;
	php_stream 	*instream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rpp|ll", &z_ftp, &remote, &remote_len, &local, &local_len, &mode, &startpos) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}
	XTYPE(xtype, mode);

	if (!(instream = php_stream_open_wrapper(local, mode == FTPTYPE_ASCII ? "rt" : "rb", REPORT_ERRORS, NULL))) {
		RETURN_FALSE;
	}

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && startpos == PHP_FTP_AUTORESUME) {
		startpos = 0;
	}

	if (ftp->autoseek && startpos) {
		/* if autoresume is wanted ask for remote size */
		if (startpos == PHP_FTP_AUTORESUME) {
			startpos = ftp_size(ftp, remote, remote_len);
			if (startpos < 0) {
				startpos = 0;
			}
		}
		if (startpos) {
			php_stream_seek(instream, startpos, SEEK_SET);
		}
	}

	/* configuration */
	ftp->direction = 1;   /* send */
	ftp->closestream = 1; /* do close */

	ret = ftp_nb_put(ftp, remote, remote_len, instream, xtype, startpos);

	if (ret != PHP_FTP_MOREDATA) {
		php_stream_close(instream);
		ftp->stream = NULL;
	}

	if (ret == PHP_FTP_FAILED) {
		php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ Returns the size of the file, or -1 on error */
PHP_FUNCTION(ftp_size)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*file;
	size_t		file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rp", &z_ftp, &file, &file_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* get file size */
	RETURN_LONG(ftp_size(ftp, file, file_len));
}
/* }}} */

/* {{{ Returns the last modification time of the file, or -1 on error */
PHP_FUNCTION(ftp_mdtm)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*file;
	size_t		file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rp", &z_ftp, &file, &file_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* get file mod time */
	RETURN_LONG(ftp_mdtm(ftp, file, file_len));
}
/* }}} */

/* {{{ Renames the given file to a new path */
PHP_FUNCTION(ftp_rename)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*src, *dest;
	size_t		src_len, dest_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss", &z_ftp, &src, &src_len, &dest, &dest_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* rename the file */
	if (!ftp_rename(ftp, src, src_len, dest, dest_len)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Deletes a file */
PHP_FUNCTION(ftp_delete)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*file;
	size_t		file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &z_ftp, &file, &file_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* delete the file */
	if (!ftp_delete(ftp, file, file_len)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sends a SITE command to the server */
PHP_FUNCTION(ftp_site)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*cmd;
	size_t		cmd_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &z_ftp, &cmd, &cmd_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	/* send the site command */
	if (!ftp_site(ftp, cmd, cmd_len)) {
		if (*ftp->inbuf) {
			php_error_docref(NULL, E_WARNING, "%s", ftp->inbuf);
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Closes the FTP stream */
PHP_FUNCTION(ftp_close)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_ftp) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	ftp_quit(ftp);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sets an FTP option */
PHP_FUNCTION(ftp_set_option)
{
	zval		*z_ftp, *z_value;
	zend_long		option;
	ftpbuf_t	*ftp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlz", &z_ftp, &option, &z_value) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	switch (option) {
		case PHP_FTP_OPT_TIMEOUT_SEC:
			if (Z_TYPE_P(z_value) != IS_LONG) {
				zend_argument_type_error(3, "must be of type int for the FTP_TIMEOUT_SEC option, %s given", zend_zval_type_name(z_value));
				RETURN_THROWS();
			}
			if (Z_LVAL_P(z_value) <= 0) {
				zend_argument_value_error(3, "must be greater than 0 for the FTP_TIMEOUT_SEC option");
				RETURN_THROWS();
			}
			ftp->timeout_sec = Z_LVAL_P(z_value);
			RETURN_TRUE;
			break;
		case PHP_FTP_OPT_AUTOSEEK:
			if (Z_TYPE_P(z_value) != IS_TRUE && Z_TYPE_P(z_value) != IS_FALSE) {
				zend_argument_type_error(3, "must be of type bool for the FTP_AUTOSEEK option, %s given", zend_zval_type_name(z_value));
				RETURN_THROWS();
			}
			ftp->autoseek = Z_TYPE_P(z_value) == IS_TRUE ? 1 : 0;
			RETURN_TRUE;
			break;
		case PHP_FTP_OPT_USEPASVADDRESS:
			if (Z_TYPE_P(z_value) != IS_TRUE && Z_TYPE_P(z_value) != IS_FALSE) {
				zend_argument_type_error(3, "must be of type bool for the FTP_USEPASVADDRESS option, %s given", zend_zval_type_name(z_value));
				RETURN_THROWS();
			}
			ftp->usepasvaddress = Z_TYPE_P(z_value) == IS_TRUE ? 1 : 0;
			RETURN_TRUE;
			break;
		default:
			zend_argument_value_error(2, "must be one of FTP_TIMEOUT_SEC, FTP_AUTOSEEK, or FTP_USEPASVADDRESS");
			RETURN_THROWS();
			break;
	}
}
/* }}} */

/* {{{ Gets an FTP option */
PHP_FUNCTION(ftp_get_option)
{
	zval		*z_ftp;
	zend_long		option;
	ftpbuf_t	*ftp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &z_ftp, &option) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ftp = (ftpbuf_t *)zend_fetch_resource(Z_RES_P(z_ftp), le_ftpbuf_name, le_ftpbuf)) == NULL) {
		RETURN_THROWS();
	}

	switch (option) {
		case PHP_FTP_OPT_TIMEOUT_SEC:
			RETURN_LONG(ftp->timeout_sec);
			break;
		case PHP_FTP_OPT_AUTOSEEK:
			RETURN_BOOL(ftp->autoseek);
			break;
		case PHP_FTP_OPT_USEPASVADDRESS:
			RETURN_BOOL(ftp->usepasvaddress);
			break;
		default:
			zend_argument_value_error(2, "must be one of FTP_TIMEOUT_SEC, FTP_AUTOSEEK, or FTP_USEPASVADDRESS");
			RETURN_THROWS();
	}
}
/* }}} */

#endif /* HAVE_FTP */
