/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrew Skalski <askalski@chek.com>                          |
   |          Stefan Esser <sesser@php.net> (resume functions)            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef NETWARE
#ifdef USE_WINSOCK
#include <novsock2.h>
#else
#ifndef NEW_LIBC
#include <sys/socket.h>
#endif
#endif
#endif

#if HAVE_FTP

#include "ext/standard/info.h"
#include "ext/standard/file.h"

#include "php_ftp.h"
#include "ftp.h"

static int	le_ftpbuf;
#define le_ftpbuf_name "FTP Buffer"

function_entry php_ftp_functions[] = {
	PHP_FE(ftp_connect,			NULL)
#if HAVE_OPENSSL_EXT
	PHP_FE(ftp_ssl_connect,		NULL)
#endif	
	PHP_FE(ftp_login,			NULL)
	PHP_FE(ftp_pwd,				NULL)
	PHP_FE(ftp_cdup,			NULL)
	PHP_FE(ftp_chdir,			NULL)
	PHP_FE(ftp_exec,			NULL)
	PHP_FE(ftp_mkdir,			NULL)
	PHP_FE(ftp_rmdir,			NULL)
	PHP_FE(ftp_nlist,			NULL)
	PHP_FE(ftp_rawlist,			NULL)
	PHP_FE(ftp_systype,			NULL)
	PHP_FE(ftp_pasv,			NULL)
	PHP_FE(ftp_get,				NULL)
	PHP_FE(ftp_fget,			NULL)
	PHP_FE(ftp_put,				NULL)
	PHP_FE(ftp_fput,			NULL)
	PHP_FE(ftp_size,			NULL)
	PHP_FE(ftp_mdtm,			NULL)
	PHP_FE(ftp_rename,			NULL)
	PHP_FE(ftp_delete,			NULL)
	PHP_FE(ftp_site,			NULL)
	PHP_FE(ftp_close,			NULL)
	PHP_FE(ftp_set_option,		NULL)
	PHP_FE(ftp_get_option,		NULL)
	PHP_FE(ftp_nb_fget,			NULL)
	PHP_FE(ftp_nb_get,			NULL)
	PHP_FE(ftp_nb_continue,		NULL)
	PHP_FE(ftp_nb_put,			NULL)
	PHP_FE(ftp_nb_fput,			NULL)
	PHP_FALIAS(ftp_quit, ftp_close, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry php_ftp_module_entry = {
    STANDARD_MODULE_HEADER,
	"ftp",
	php_ftp_functions,
	PHP_MINIT(ftp),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(ftp),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FTP
ZEND_GET_MODULE(php_ftp)
#endif

static void ftp_destructor_ftpbuf(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ftpbuf_t *ftp = (ftpbuf_t *)rsrc->ptr;

	ftp_close(ftp);
}

PHP_MINIT_FUNCTION(ftp)
{
	le_ftpbuf = zend_register_list_destructors_ex(ftp_destructor_ftpbuf, NULL, le_ftpbuf_name, module_number);
	REGISTER_LONG_CONSTANT("FTP_ASCII",  FTPTYPE_ASCII, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_TEXT",   FTPTYPE_ASCII, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_BINARY", FTPTYPE_IMAGE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_IMAGE",  FTPTYPE_IMAGE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_AUTORESUME", PHP_FTP_AUTORESUME, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_TIMEOUT_SEC", PHP_FTP_OPT_TIMEOUT_SEC, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_AUTOSEEK", PHP_FTP_OPT_AUTOSEEK, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_FAILED", PHP_FTP_FAILED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_FINISHED", PHP_FTP_FINISHED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_MOREDATA", PHP_FTP_MOREDATA, CONST_PERSISTENT | CONST_CS);
	return SUCCESS;
}

PHP_MINFO_FUNCTION(ftp)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "FTP support", "enabled");
	php_info_print_table_end();
}

#define	XTYPE(xtype, mode)	{ \
								if (mode != FTPTYPE_ASCII && mode != FTPTYPE_IMAGE) { \
									php_error_docref(NULL TSRMLS_CC, E_WARNING, "Mode must be FTP_ASCII or FTP_BINARY"); \
									RETURN_FALSE; \
								} \
								xtype = mode; \
							}


/* {{{ proto resource ftp_connect(string host [, int port [, int timeout)]])
   Opens a FTP stream */
PHP_FUNCTION(ftp_connect)
{
	ftpbuf_t	*ftp;
	char		*host;
	int			host_len, port = 0;
	long		timeout_sec = FTP_DEFAULT_TIMEOUT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &host, &host_len, &port, &timeout_sec) == FAILURE) {
		return;
	}

	if (timeout_sec <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Timeout has to be greater than 0");
		RETURN_FALSE;
	}

	/* connect */
	ftp = ftp_open(host, (short)port, timeout_sec TSRMLS_CC);
	if (ftp == NULL) {
		RETURN_FALSE;
	}

	/* autoseek for resuming */
	ftp->autoseek = FTP_DEFAULT_AUTOSEEK;
#if HAVE_OPENSSL_EXT
	/* disable ssl */
	ftp->use_ssl = 0;
#endif

	ZEND_REGISTER_RESOURCE(return_value, ftp, le_ftpbuf);
}
/* }}} */

#if HAVE_OPENSSL_EXT
/* {{{ proto resource ftp_ssl_connect(string host [, int port [, int timeout)]])
   Opens a FTP-SSL stream */
PHP_FUNCTION(ftp_ssl_connect)
{
	ftpbuf_t	*ftp;
	char		*host;
	int			host_len, port = 0;
	long		timeout_sec = FTP_DEFAULT_TIMEOUT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &host, &host_len, &port, &timeout_sec) == FAILURE) {
		return;
	}

	if (timeout_sec <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Timeout has to be greater than 0");
		RETURN_FALSE;
	}

	/* connect */
	ftp = ftp_open(host, (short)port, timeout_sec TSRMLS_CC);
	if (ftp == NULL) {
		RETURN_FALSE;
	}

	/* autoseek for resuming */
	ftp->autoseek = FTP_DEFAULT_AUTOSEEK;
	/* enable ssl */
	ftp->use_ssl = 1;

	ZEND_REGISTER_RESOURCE(return_value, ftp, le_ftpbuf);
}
/* }}} */
#endif

/* {{{ proto bool ftp_login(resource stream, string username, string password)
   Logs into the FTP server */
PHP_FUNCTION(ftp_login)
{
	zval 		*z_ftp;
	ftpbuf_t	*ftp;
	char *user, *pass;
	int user_len, pass_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &z_ftp, &user, &user_len, &pass, &pass_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* log in */
	if (!ftp_login(ftp, user, pass)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string ftp_pwd(resource stream)
   Returns the present working directory */
PHP_FUNCTION(ftp_pwd)
{
	zval 		*z_ftp;
	ftpbuf_t	*ftp;
	const char	*pwd;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ftp) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	pwd = ftp_pwd(ftp);
	if (pwd == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_STRING((char*) pwd, 1);
}
/* }}} */

/* {{{ proto bool ftp_cdup(resource stream)
   Changes to the parent directory */
PHP_FUNCTION(ftp_cdup)
{
	zval 		*z_ftp;
	ftpbuf_t	*ftp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ftp) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	if (!ftp_cdup(ftp)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_chdir(resource stream, string directory)
   Changes directories */
PHP_FUNCTION(ftp_chdir)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*dir;
	int			dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* change directories */
	if (!ftp_chdir(ftp, dir)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_exec(resource stream, string command)
   Requests execution of a program on the FTP server */
PHP_FUNCTION(ftp_exec)
{
	pval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*cmd;
	int			cmd_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &cmd, &cmd_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* execute serverside command */
	if (!ftp_exec(ftp, cmd)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string ftp_mkdir(resource stream, string directory)
   Creates a directory and returns the absolute path for the new directory or false on error */
PHP_FUNCTION(ftp_mkdir)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*dir, *tmp;
	int		dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* create directorie */
	if (NULL == (tmp = ftp_mkdir(ftp, dir))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_STRING(tmp, 0);
}
/* }}} */

/* {{{ proto bool ftp_rmdir(resource stream, string directory)
   Removes a directory */
PHP_FUNCTION(ftp_rmdir)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*dir;
	int		dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* remove directorie */
	if (!ftp_rmdir(ftp, dir)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array ftp_nlist(resource stream, string directory)
   Returns an array of filenames in the given directory */
PHP_FUNCTION(ftp_nlist)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		**nlist, **ptr, *dir;
	int		dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* get list of files */
	if (NULL == (nlist = ftp_nlist(ftp, dir))) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = nlist; *ptr; ptr++) {
		add_next_index_string(return_value, *ptr, 1);
	}
	efree(nlist);
}
/* }}} */

/* {{{ proto array ftp_rawlist(resource stream, string directory [, bool recursive])
   Returns a detailed listing of a directory as an array of output lines */
PHP_FUNCTION(ftp_rawlist)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		**llist, **ptr, *dir;
	int		dir_len;
	zend_bool	recursive = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|b", &z_ftp, &dir, &dir_len, &recursive) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* get raw directory listing */
	if (NULL == (llist = ftp_list(ftp, dir, recursive))) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = llist; *ptr; ptr++) {
		add_next_index_string(return_value, *ptr, 1);
	}
	efree(llist);
}
/* }}} */

/* {{{ proto string ftp_systype(resource stream)
   Returns the system type identifier */
PHP_FUNCTION(ftp_systype)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	const char	*syst;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ftp) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	if (NULL == (syst = ftp_syst(ftp))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_STRING((char*) syst, 1);
}
/* }}} */

/* {{{ proto bool ftp_fget(resource stream, resource fp, string remote_file, int mode[, int resumepos])
   Retrieves a file from the FTP server and writes it to an open file */
PHP_FUNCTION(ftp_fget)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	php_stream	*stream;
	char		*file;
	int		file_len, mode, resumepos=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrsl|l", &z_ftp, &z_file, &file, &file_len, &mode, &resumepos) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	php_stream_from_zval(stream, &z_file);
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

	if (!ftp_get(ftp, stream, file, xtype, resumepos)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_nb_fget(resource stream, resource fp, string remote_file, int mode[, int resumepos])
   Retrieves a file from the FTP server asynchronly and writes it to an open file */
PHP_FUNCTION(ftp_nb_fget)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	php_stream	*stream;
	char		*file;
	int		file_len, mode, resumepos=0, ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrsl|l", &z_ftp, &z_file, &file, &file_len, &mode, &resumepos) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	php_stream_from_zval(stream, &z_file);
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

	if ((ret = ftp_nb_get(ftp, stream, file, xtype, resumepos)) == PHP_FTP_FAILED) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_LONG(ret);
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto bool ftp_pasv(resource stream, bool pasv)
   Turns passive mode on or off */
PHP_FUNCTION(ftp_pasv)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	zend_bool	pasv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rb", &z_ftp, &pasv) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	if (!ftp_pasv(ftp, pasv ? 1 : 0)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_get(resource stream, string local_file, string remote_file, int mode[, int resume_pos])
   Retrieves a file from the FTP server and writes it to a local file */
PHP_FUNCTION(ftp_get)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	php_stream	*outstream;
	char		*local, *remote;
	int		local_len, remote_len, mode, resumepos=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssl|l", &z_ftp, &local, &local_len, &remote, &remote_len, &mode, &resumepos) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && resumepos == PHP_FTP_AUTORESUME) {
		resumepos = 0;
	}

	if (ftp->autoseek && resumepos) {
		outstream = php_stream_fopen(local, "rb+", NULL);
		if (outstream == NULL) {
			outstream = php_stream_fopen(local, "wb", NULL);
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
		outstream = php_stream_fopen(local, "wb", NULL);
	}

	if (outstream == NULL)	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error opening %s", local);
		RETURN_FALSE;
	}

	if (!ftp_get(ftp, outstream, remote, xtype, resumepos)) {
		php_stream_close(outstream);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	php_stream_close(outstream);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_nb_get(resource stream, string local_file, string remote_file, int mode[, int resume_pos])
   Retrieves a file from the FTP server nbhronly and writes it to a local file */
PHP_FUNCTION(ftp_nb_get)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	php_stream	*outstream;
	char		*local, *remote;
	int		local_len, remote_len, mode, resumepos=0, ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssl|l", &z_ftp, &local, &local_len, &remote, &remote_len, &mode, &resumepos) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && resumepos == PHP_FTP_AUTORESUME) {
		resumepos = 0;
	}

	if (ftp->autoseek && resumepos) {
		outstream = php_stream_fopen(local, "rb+", NULL);
		if (outstream == NULL) {
			outstream = php_stream_fopen(local, "wb", NULL);
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
		outstream = php_stream_fopen(local, "wb", NULL);
	}

	if (outstream == NULL)	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error opening %s", local);
		RETURN_FALSE;
	}

	/* configuration */
	ftp->direction = 0;   /* recv */
	ftp->closestream = 1; /* do close */

	if ((ret = ftp_nb_get(ftp, outstream, remote, xtype, resumepos)) == PHP_FTP_FAILED) {
		php_stream_close(outstream);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_LONG(PHP_FTP_FAILED);
	}

	if (ret == PHP_FTP_FINISHED) {
		php_stream_close(outstream);
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int ftp_nb_continue(resource stream)
   Continues retrieving/sending a file nbronously */
PHP_FUNCTION(ftp_nb_continue)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	int		ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ftp) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	if (!ftp->nb) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "no nbronous transfer to continue.");
		RETURN_LONG(PHP_FTP_FAILED);
	}

	if (ftp->direction) {
		ret=ftp_nb_continue_write(ftp);
	} else {
		ret=ftp_nb_continue_read(ftp);
	}

	if (ret != PHP_FTP_MOREDATA && ftp->closestream) {
		php_stream_close(ftp->stream);
	}

	if (ret == PHP_FTP_FAILED) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto bool ftp_fput(resource stream, string remote_file, resource fp, int mode[, int startpos])
   Stores a file from an open file to the FTP server */
PHP_FUNCTION(ftp_fput)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	int		mode, remote_len, startpos=0;
	php_stream	*stream;
	char		*remote;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsrl|l", &z_ftp, &remote, &remote_len, &z_file, &mode, &startpos) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	php_stream_from_zval(stream, &z_file);
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && startpos == PHP_FTP_AUTORESUME) {
		startpos = 0;
	}

	if (ftp->autoseek && startpos) {
		/* if autoresume is wanted ask for remote size */
		if (startpos == PHP_FTP_AUTORESUME) {
			startpos = ftp_size(ftp, remote);
			if (startpos < 0) {
				startpos = 0;
			}
		}
		if (startpos) {
			php_stream_seek(stream, startpos, SEEK_SET);
		}
	}

	if (!ftp_put(ftp, remote, stream, xtype, startpos)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_nb_fput(resource stream, string remote_file, resource fp, int mode[, int startpos])
   Stores a file from an open file to the FTP server nbronly */
PHP_FUNCTION(ftp_nb_fput)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	int		mode, remote_len, startpos=0, ret;
	php_stream	*stream;
	char		*remote;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsrl|l", &z_ftp, &remote, &remote_len, &z_file, &mode, &startpos) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	php_stream_from_zval(stream, &z_file);
	XTYPE(xtype, mode);

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && startpos == PHP_FTP_AUTORESUME) {
		startpos = 0;
	}

	if (ftp->autoseek && startpos) {
		/* if autoresume is wanted ask for remote size */
		if (startpos == PHP_FTP_AUTORESUME) {
			startpos = ftp_size(ftp, remote);
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

	if (((ret = ftp_nb_put(ftp, remote, stream, xtype, startpos)) == PHP_FTP_FAILED)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_LONG(ret);
	}

	RETURN_LONG(ret);
}
/* }}} */


/* {{{ proto bool ftp_put(resource stream, string remote_file, string local_file, int mode[, int startpos])
   Stores a file on the FTP server */
PHP_FUNCTION(ftp_put)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	char		*remote, *local;
	int		remote_len, local_len, mode, startpos=0;
	php_stream 	*instream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssl|l", &z_ftp, &remote, &remote_len, &local, &local_len, &mode, &startpos) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	XTYPE(xtype, mode);

	instream = php_stream_fopen(local, "rb", NULL);

	if (instream == NULL) {
		RETURN_FALSE;
	}

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && startpos == PHP_FTP_AUTORESUME) {
		startpos = 0;
	}

	if (ftp->autoseek && startpos) {
		/* if autoresume is wanted ask for remote size */
		if (startpos == PHP_FTP_AUTORESUME) {
			startpos = ftp_size(ftp, remote);
			if (startpos < 0) {
				startpos = 0;
			}
		}
		if (startpos) {
			php_stream_seek(instream, startpos, SEEK_SET);
		}
	}

	if (!ftp_put(ftp, remote, instream, xtype, startpos)) {
		php_stream_close(instream);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}
	php_stream_close(instream);

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool ftp_nb_put(resource stream, string remote_file, string local_file, int mode[, int startpos])
   Stores a file on the FTP server */
PHP_FUNCTION(ftp_nb_put)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	char		*remote, *local;
	int		remote_len, local_len, mode, startpos=0, ret;
	php_stream 	*instream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssl|l", &z_ftp, &remote, &remote_len, &local, &local_len, &mode, &startpos) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	XTYPE(xtype, mode);

	instream = php_stream_fopen(local, "rb", NULL);

	if (instream == NULL)	{
		RETURN_FALSE;
	}

	/* ignore autoresume if autoseek is switched off */
	if (!ftp->autoseek && startpos == PHP_FTP_AUTORESUME) {
		startpos = 0;
	}

	if (ftp->autoseek && startpos) {
		/* if autoresume is wanted ask for remote size */
		if (startpos == PHP_FTP_AUTORESUME) {
			startpos = ftp_size(ftp, remote);
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

	ret = ftp_nb_put(ftp, remote, instream, xtype, startpos);

	if (ret != PHP_FTP_MOREDATA) {
		php_stream_close(instream);
	}

	if (ret == PHP_FTP_FAILED) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int ftp_size(resource stream, string filename)
   Returns the size of the file, or -1 on error */
PHP_FUNCTION(ftp_size)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*file;
	int		file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &file, &file_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* get file size */
	RETURN_LONG(ftp_size(ftp, file));
}
/* }}} */

/* {{{ proto int ftp_mdtm(resource stream, string filename)
   Returns the last modification time of the file, or -1 on error */
PHP_FUNCTION(ftp_mdtm)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*file;
	int		file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &file, &file_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* get file mod time */
	RETURN_LONG(ftp_mdtm(ftp, file));
}
/* }}} */

/* {{{ proto bool ftp_rename(resource stream, string src, string dest)
   Renames the given file to a new path */
PHP_FUNCTION(ftp_rename)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*src, *dest;
	int		src_len, dest_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &z_ftp, &src, &src_len, &dest, &dest_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* rename the file */
	if (!ftp_rename(ftp, src, dest)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_delete(resource stream, string file)
   Deletes a file */
PHP_FUNCTION(ftp_delete)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*file;
	int		file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &file, &file_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* delete the file */
	if (!ftp_delete(ftp, file)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_site(resource stream, string cmd)
   Sends a SITE command to the server */
PHP_FUNCTION(ftp_site)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*cmd;
	int		cmd_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &cmd, &cmd_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* send the site command */
	if (!ftp_site(ftp, cmd)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ftp_close(resource stream)
   Closes the FTP stream */
PHP_FUNCTION(ftp_close)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ftp) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	RETURN_BOOL(zend_list_delete(Z_LVAL_P(z_ftp)) == SUCCESS);
}
/* }}} */

/* {{{ proto bool ftp_set_option(resource stream, int option, mixed value)
   Sets an FTP option */
PHP_FUNCTION(ftp_set_option)
{
	zval		*z_ftp, *z_value;
	long		option;
	ftpbuf_t	*ftp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlz", &z_ftp, &option, &z_value) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	switch (option) {
		case PHP_FTP_OPT_TIMEOUT_SEC:
			if (Z_TYPE_P(z_value) != IS_LONG) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Option TIMEOUT_SEC expects value of type long, %s given",
					zend_zval_type_name(z_value));
				RETURN_FALSE;
			}
			if (Z_LVAL_P(z_value) <= 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Timeout has to be greater than 0");
				RETURN_FALSE;
			}
			ftp->timeout_sec = Z_LVAL_P(z_value);
			RETURN_TRUE;
			break;
		case PHP_FTP_OPT_AUTOSEEK:
			if (Z_TYPE_P(z_value) != IS_BOOL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Option AUTOSEEK expects value of type boolean, %s given",
					zend_zval_type_name(z_value));
				RETURN_FALSE;
			}
			ftp->autoseek = Z_LVAL_P(z_value);
			RETURN_TRUE;
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown option '%d'", option);
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ proto mixed ftp_get_option(resource stream, int option)
   Gets an FTP option */
PHP_FUNCTION(ftp_get_option)
{
	zval		*z_ftp;
	long		option;
	ftpbuf_t	*ftp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &z_ftp, &option) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	switch (option) {
		case PHP_FTP_OPT_TIMEOUT_SEC:
			RETURN_LONG(ftp->timeout_sec);
			break;
		case PHP_FTP_OPT_AUTOSEEK:
			RETURN_BOOL(ftp->autoseek);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown option '%d'", option);
			RETURN_FALSE;
			break;
	}
}
/* }}} */

#endif /* HAVE_FTP */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
