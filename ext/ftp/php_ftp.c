/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrew Skalski      <askalski@chek.com>                     |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_FTP

#include "ext/standard/info.h"
#include "ext/standard/file.h"

#include "php_ftp.h"
#include "ftp.h"

static int	le_ftpbuf;
#define le_ftpbuf_name "FTP Buffer"

function_entry php_ftp_functions[] = {
	PHP_FE(ftp_connect,			NULL)
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
	le_ftpbuf = zend_register_list_destructors_ex(ftp_destructor_ftpbuf, NULL, "ftp", module_number);
	REGISTER_LONG_CONSTANT("FTP_ASCII", FTPTYPE_ASCII,
		CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_BINARY", FTPTYPE_IMAGE,
		CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_IMAGE", FTPTYPE_IMAGE,
		CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("FTP_TEXT", FTPTYPE_ASCII,
		CONST_PERSISTENT | CONST_CS);
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
									php_error(E_WARNING, "%s(): mode must be FTP_ASCII or FTP_IMAGE", get_active_function_name(TSRMLS_C)); \
									RETURN_FALSE; \
								} \
								xtype = mode; \
							}

#define	FILEP(fp, pval)		ZEND_FETCH_RESOURCE(fp, FILE *, &pval, -1, "File-Handle", php_file_le_fopen());

/* {{{ proto resource ftp_connect(string host [, int port])
   Opens a FTP stream */
PHP_FUNCTION(ftp_connect)
{
	ftpbuf_t	*ftp;
	char		*host;
	int			host_len, port = 21;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &host, &host_len, &port) == FAILURE) {
		return;
	}

	/* connect */
	ftp = ftp_open(host, htons((short)port));
	if (ftp == NULL) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, ftp, le_ftpbuf);
}
/* }}} */

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
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
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
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
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
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
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
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_exec(resource stream, string command)
   Requests execution of a program on the ftp server */
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
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed ftp_mkdir(resource stream, string directory)
   Creates a directory. Returns the absolute path for the new directory or false on error. */
PHP_FUNCTION(ftp_mkdir)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*dir, *tmp;
	int			dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* create directorie */
	if (NULL == (tmp = ftp_mkdir(ftp, dir))) {
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_STRING(tmp, 1);
}
/* }}} */

/* {{{ proto bool ftp_rmdir(resource stream, string directory)
   Removes a directory */
PHP_FUNCTION(ftp_rmdir)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*dir;
	int			dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* remove directorie */
	if (!ftp_rmdir(ftp, dir)) {
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
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
	int			dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* get list of files */
	if (NULL == (nlist = ftp_nlist(ftp, dir))) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = nlist; *ptr; ptr++)
		add_next_index_string(return_value, *ptr, 1);
	free(nlist);
}
/* }}} */

/* {{{ proto array ftp_rawlist(resource stream, string directory)
   Returns a detailed listing of a directory as an array of output lines */
PHP_FUNCTION(ftp_rawlist)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		**llist, **ptr, *dir;
	int			dir_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &dir, &dir_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* get raw directory listing */
	if (NULL == (llist = ftp_list(ftp, dir))) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = llist; *ptr; ptr++)
		add_next_index_string(return_value, *ptr, 1);
	free(llist);
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

	syst = ftp_syst(ftp);
	if (NULL == (syst = ftp_syst(ftp))) {
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_STRING((char*) syst, 1);
}
/* }}} */

/* {{{ proto bool ftp_fget(resource stream, resource fp, string remote_file, int mode)
   Retrieves a file from the FTP server and writes it to an open file */
PHP_FUNCTION(ftp_fget)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	FILE		*fp;
	char		*file;
	int			file_len, mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrsl", &z_ftp, &z_file, &file, &file_len, &mode) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	FILEP(fp, z_file);
	XTYPE(xtype, mode);

	if (!ftp_get(ftp, fp, file, xtype) || ferror(fp)) {
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
		RETURN_FALSE;
	}

	if (ferror(fp)) {
		php_error(E_WARNING, "%s(): error writing %s", get_active_function_name(TSRMLS_C), Z_STRVAL_P(z_file));
		RETURN_FALSE;
	}

	RETURN_TRUE;
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

/* {{{ proto bool ftp_get(resource stream, string local_file, string remote_file, int mode)
   Retrieves a file from the FTP server and writes it to a local file */
PHP_FUNCTION(ftp_get)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	FILE		*outfp, *tmpfp;
	int			ch;
	char		*local, *remote;
	int			local_len, remote_len, mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssl", &z_ftp, &local, &local_len, &remote, &remote_len, &mode) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	XTYPE(xtype, mode);

	/* get to temporary file, so if there is an error, no existing
	 * file gets clobbered
	 */
	if ((tmpfp = tmpfile()) == NULL) {
		php_error(E_WARNING, "%s(): error opening tmpfile", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	if (!ftp_get(ftp, tmpfp, remote, xtype) || ferror(tmpfp)) {
		fclose(tmpfp);
		php_error(E_WARNING, "ftp_get: %s", ftp->inbuf);
		RETURN_FALSE;
	}

#ifdef PHP_WIN32
	if ((outfp = VCWD_FOPEN(local, "wb")) == NULL) {
#else
	if ((outfp = VCWD_FOPEN(local, "w")) == NULL) {
#endif
		fclose(tmpfp);
		php_error(E_WARNING, "%s(): error opening %s", get_active_function_name(TSRMLS_C), local);
		RETURN_FALSE;
	}

	rewind(tmpfp);
	while ((ch = getc(tmpfp)) != EOF)
		putc(ch, outfp);

	if (ferror(tmpfp) || ferror(outfp)) {
		fclose(tmpfp);
		fclose(outfp);
		php_error(E_WARNING, "%s(): error writing %s", get_active_function_name(TSRMLS_C), local);
		RETURN_FALSE;
	}

	fclose(tmpfp);
	fclose(outfp);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_fput(resource stream, string remote_file, resource local_file, int mode)
   Stores a file from an open file to the FTP server */
PHP_FUNCTION(ftp_fput)
{
	zval		*z_ftp, *z_file;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	int			type, mode, remote_len;
	void		*rsrc;
	char		*remote;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsrl", &z_ftp, &remote, &remote_len, &z_file, &mode) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
   	rsrc = zend_fetch_resource(&z_file TSRMLS_CC, -1, "File-Handle", &type, 3, php_file_le_fopen(), php_file_le_popen(), php_file_le_socket());
	ZEND_VERIFY_RESOURCE(rsrc);   
	XTYPE(xtype, mode);

	if (!ftp_put(ftp, remote, (FILE*)rsrc, *(int*) rsrc, (type==php_file_le_socket()), xtype)) {
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ftp_put(resource stream, string remote_file, string local_file, int mode)
   Stores a file on the FTP server */
PHP_FUNCTION(ftp_put)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	FILE		*infp;
	char		*remote, *local;
	int			remote_len, local_len, mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssl", &z_ftp, &remote, &remote_len, &local, &local_len, &mode) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);
	XTYPE(xtype, mode);

#ifdef PHP_WIN32
	if ((infp = VCWD_FOPEN(local, "rb")) == NULL) {
#else
	if ((infp = VCWD_FOPEN(local, "r")) == NULL) {
#endif
		php_error(E_WARNING, "%s(): error opening %s", get_active_function_name(TSRMLS_C), local);
		RETURN_FALSE;
	}
	if (!ftp_put(ftp, remote, infp, 0, 0, xtype) || ferror(infp)) {
		fclose(infp);
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
		RETURN_FALSE;
	}
	fclose(infp);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_size(resource stream, string filename)
   Returns the size of the file, or -1 on error */
PHP_FUNCTION(ftp_size)
{
	zval		*z_ftp;
	ftpbuf_t	*ftp;
	char		*file;
	int			file_len;

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
	int			file_len;

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
	int			src_len, dest_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &z_ftp, &src, &src_len, &dest, &dest_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* rename the file */
	if (!ftp_rename(ftp, src, dest)) {
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
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
	int			file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &file, &file_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* delete the file */
	if (!ftp_delete(ftp, file)) {
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
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
	int			cmd_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ftp, &cmd, &cmd_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ftp, ftpbuf_t*, &z_ftp, -1, le_ftpbuf_name, le_ftpbuf);

	/* send the site command */
	if (!ftp_site(ftp, cmd)) {
		php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), ftp->inbuf);
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

	zend_list_delete(Z_LVAL_P(z_ftp));
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
