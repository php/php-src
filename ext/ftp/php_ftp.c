/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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
	PHP_FE(ftp_quit,			NULL)
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
	REGISTER_MAIN_LONG_CONSTANT("FTP_ASCII", FTPTYPE_ASCII,
		CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("FTP_BINARY", FTPTYPE_IMAGE,
		CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("FTP_IMAGE", FTPTYPE_IMAGE,
		CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("FTP_TEXT", FTPTYPE_ASCII,
		CONST_PERSISTENT | CONST_CS);
	return SUCCESS;
}

PHP_MINFO_FUNCTION(ftp)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "FTP support", "enabled");
	php_info_print_table_end();
}


#define	FTPBUF(ftp, pval) { \
	int	id, type; \
	convert_to_long(pval); \
	id = Z_LVAL_P(pval); \
	(ftp) = zend_list_find(id, &type); \
	if (!(ftp) || type != le_ftpbuf) { \
		php_error(E_WARNING, "Unable to find ftpbuf %d", id); \
		RETURN_FALSE; \
	} \
	}

#define	XTYPE(xtype, pval) { \
	convert_to_long(pval); \
	if (	Z_LVAL_P(pval) != FTPTYPE_ASCII && \
		Z_LVAL_P(pval) != FTPTYPE_IMAGE) \
	{ \
		php_error(E_WARNING, "arg4 must be FTP_ASCII or FTP_IMAGE"); \
		RETURN_FALSE; \
	} \
	(xtype) = Z_LVAL_P(pval); \
	}

#define	FILEP(fp, pval) { \
	ZEND_FETCH_RESOURCE(fp, FILE *, &pval, -1, "File-Handle", php_file_le_fopen()); \
	}

/* {{{ proto int ftp_connect(string host [, int port])
   Opens a FTP stream */
PHP_FUNCTION(ftp_connect)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;
	short		port = 0;

	/* arg1 - hostname
	 * arg2 - [port]
	 */
	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (getParameters(ht, 1, &arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(arg2);
		port = (short) Z_LVAL_P(arg2);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg1);

	/* connect */
	ftp = ftp_open(Z_STRVAL_P(arg1), htons(port));
	if (ftp == NULL)
		RETURN_FALSE;

	RETURN_LONG(zend_list_insert(ftp, le_ftpbuf));
}
/* }}} */

/* {{{ proto int ftp_login(int stream, string username, string password)
   Logs into the FTP server */
PHP_FUNCTION(ftp_login)
{
	pval		*arg1, *arg2, *arg3;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - username
	 * arg3 - password
	 */
	if (	ZEND_NUM_ARGS() != 3 ||
		getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg2);
	convert_to_string(arg3);

	FTPBUF(ftp, arg1);

	/* log in */
	if (!ftp_login(ftp, Z_STRVAL_P(arg2), Z_STRVAL_P(arg3))) {
		php_error(E_WARNING, "ftp_login: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string ftp_pwd(int stream)
   Returns the present working directory */
PHP_FUNCTION(ftp_pwd)
{
	pval		*arg1;
	ftpbuf_t	*ftp;
	const char	*pwd;

	/* arg1 - ftp
	 */
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);

	pwd = ftp_pwd(ftp);
	if (pwd == NULL) {
		php_error(E_WARNING, "ftp_pwd: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_STRING((char*) pwd, 1);
}
/* }}} */

/* {{{ proto int ftp_cdup(int stream)
   Changes to the parent directory */
PHP_FUNCTION(ftp_cdup)
{
	pval		*arg1;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 */
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);

	if (!ftp_cdup(ftp)) {
		php_error(E_WARNING, "ftp_cdup: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_chdir(int stream, string directory)
   Changes directories */
PHP_FUNCTION(ftp_chdir)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg2);

	FTPBUF(ftp, arg1);

	/* change directories */
	if (!ftp_chdir(ftp, Z_STRVAL_P(arg2))) {
		php_error(E_WARNING, "ftp_chdir: %s", ftp->inbuf);
			RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_exec(int stream, string command)
   Changes directories */
PHP_FUNCTION(ftp_exec)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - command
	 */
	if (ARG_COUNT(ht) != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg2);
	
	FTPBUF(ftp, arg1);

	/* change directories */
	if (!ftp_exec(ftp, Z_STRVAL_P(arg2))) {
		php_error(E_WARNING, "ftp_exec: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string ftp_mkdir(int stream, string directory)
   Creates a directory */
PHP_FUNCTION(ftp_mkdir)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;
	char		*ret, *tmp;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg2);

	FTPBUF(ftp, arg1);

	/* change directories */
	tmp = ftp_mkdir(ftp, Z_STRVAL_P(arg2));
	if (tmp == NULL) {
		php_error(E_WARNING, "ftp_mkdir: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	if ((ret = estrdup(tmp)) == NULL) {
		free(tmp);
		php_error(E_WARNING, "estrdup failed");
		RETURN_FALSE;
	}

	RETURN_STRING(ret, 0);
}
/* }}} */

/* {{{ proto int ftp_rmdir(int stream, string directory)
   Removes a directory */
PHP_FUNCTION(ftp_rmdir)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg2);

	FTPBUF(ftp, arg1);

	/* change directories */
	if (!ftp_rmdir(ftp, Z_STRVAL_P(arg2))) {
		php_error(E_WARNING, "ftp_rmdir: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array ftp_nlist(int stream, string directory)
   Returns an array of filenames in the given directory */
PHP_FUNCTION(ftp_nlist)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;
	char		**nlist, **ptr;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg2);

	FTPBUF(ftp, arg1);

	/* get list of files */
	nlist = ftp_nlist(ftp, Z_STRVAL_P(arg2));
	if (nlist == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = nlist; *ptr; ptr++)
		add_next_index_string(return_value, *ptr, 1);
	free(nlist);
}
/* }}} */

/* {{{ proto array ftp_rawlist(int stream, string directory)
   Returns a detailed listing of a directory as an array of output lines */
PHP_FUNCTION(ftp_rawlist)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;
	char		**llist, **ptr;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg2);

	FTPBUF(ftp, arg1);

	/* get directory listing */
	llist = ftp_list(ftp, Z_STRVAL_P(arg2));
	if (llist == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = llist; *ptr; ptr++)
		add_next_index_string(return_value, *ptr, 1);
	free(llist);
}
/* }}} */

/* {{{ proto string ftp_systype(int stream)
   Returns the system type identifier */
PHP_FUNCTION(ftp_systype)
{
	pval		*arg1;
	ftpbuf_t	*ftp;
	const char	*syst;


	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);

	syst = ftp_syst(ftp);
	if (syst == NULL) {
		php_error(E_WARNING, "ftp_syst: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_STRING((char*) syst, 1);
}
/* }}} */

/* {{{ proto int ftp_fget(int stream, int fp, string remote_file, int mode)
   Retrieves a file from the FTP server and writes it to an open file */
PHP_FUNCTION(ftp_fget)
{
	pval		*arg1, *arg2, *arg3, *arg4;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	FILE		*fp;

	/* arg1 - ftp
	 * arg2 - fp
	 * arg3 - remote file
	 * arg4 - transfer mode
	 */
	if (	ZEND_NUM_ARGS() != 4 ||
		getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
	{
			WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	FILEP(fp, arg2);
	convert_to_string(arg3);
	XTYPE(xtype, arg4);

	if (!ftp_get(ftp, fp, Z_STRVAL_P(arg3), xtype) || ferror(fp)) {
		php_error(E_WARNING, "ftp_get: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	if (ferror(fp)) {
		php_error(E_WARNING, "error writing %s", Z_STRVAL_P(arg2));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_pasv(int stream, int pasv)
   Turns passive mode on or off */
PHP_FUNCTION(ftp_pasv)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - pasv
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	convert_to_long(arg2);

	if (!ftp_pasv(ftp, (Z_LVAL_P(arg2)) ? 1 : 0))
		RETURN_FALSE;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_get(int stream, string local_file, string remote_file, int mode)
   Retrieves a file from the FTP server and writes it to a local file */
PHP_FUNCTION(ftp_get)
{
	pval		*arg1, *arg2, *arg3, *arg4;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	FILE		*outfp, *tmpfp;
	int		ch;


	/* arg1 - ftp
	 * arg2 - destination (local) file
	 * arg3 - source (remote) file
	 * arg4 - transfer mode
	 */
	if (	ZEND_NUM_ARGS() != 4 ||
		getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	XTYPE(xtype, arg4);

	/* get to temporary file, so if there is an error, no existing
	 * file gets clobbered
	 */
	if ((tmpfp = tmpfile()) == NULL) {
		php_error(E_WARNING, "error opening tmpfile");
		RETURN_FALSE;
	}

	if (	!ftp_get(ftp, tmpfp, Z_STRVAL_P(arg3), xtype) ||
		ferror(tmpfp))
	{
		fclose(tmpfp);
		php_error(E_WARNING, "ftp_get: %s", ftp->inbuf);
		RETURN_FALSE;
	}

#ifdef PHP_WIN32
	if ((outfp = VCWD_FOPEN(Z_STRVAL_P(arg2), "wb")) == NULL) {
#else
	if ((outfp = VCWD_FOPEN(Z_STRVAL_P(arg2), "w")) == NULL) {
#endif
		fclose(tmpfp);
		php_error(E_WARNING, "error opening %s", Z_STRVAL_P(arg2));
		RETURN_FALSE;
	}

	rewind(tmpfp);
	while ((ch = getc(tmpfp)) != EOF)
		putc(ch, outfp);

	if (ferror(tmpfp) || ferror(outfp)) {
		fclose(tmpfp);
		fclose(outfp);
		php_error(E_WARNING, "error writing %s", Z_STRVAL_P(arg2));
		RETURN_FALSE;
	}

	fclose(tmpfp);
	fclose(outfp);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_fput(int stream, string local_file, string remote_file, int mode)
   Stores a file from an open file to the FTP server */
PHP_FUNCTION(ftp_fput)
{
	pval		*arg1, *arg2, *arg3, *arg4;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	int		type;
	void		*rsrc;

	/* arg1 - ftp
	 * arg2 - remote file
	 * arg3 - fp
	 * arg4 - transfer mode
	 */
	if (	ZEND_NUM_ARGS() != 4 ||
		getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	convert_to_string(arg2);
   	rsrc = zend_fetch_resource(&arg3 TSRMLS_CC,-1,"File-Handle", &type, 3, php_file_le_fopen(), php_file_le_popen(), php_file_le_socket());
	ZEND_VERIFY_RESOURCE(rsrc);   
	XTYPE(xtype, arg4);

	if (!ftp_put(ftp, Z_STRVAL_P(arg2), (FILE*)rsrc, *(int*) rsrc, (type==php_file_le_socket()), xtype)) {
		php_error(E_WARNING, "ftp_put: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_put(int stream, string remote_file, string local_file, int mode)
   Stores a file on the FTP server */
PHP_FUNCTION(ftp_put)
{
	pval		*arg1, *arg2, *arg3, *arg4;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	FILE		*infp;


	/* arg1 - ftp
	 * arg2 - destination (remote) file
	 * arg3 - source (local) file
	 * arg4 - transfer mode
	 */
	if (	ZEND_NUM_ARGS() != 4 ||
		getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	XTYPE(xtype, arg4);

#ifdef PHP_WIN32
	if ((infp = VCWD_FOPEN(Z_STRVAL_P(arg3), "rb")) == NULL) {
#else
	if ((infp = VCWD_FOPEN(Z_STRVAL_P(arg3), "r")) == NULL) {
#endif
		php_error(E_WARNING, "error opening %s", Z_STRVAL_P(arg3));
		RETURN_FALSE;
	}
	if (	!ftp_put(ftp, Z_STRVAL_P(arg2), infp, 0, 0, xtype) ||
		ferror(infp))
	{
		fclose(infp);
		php_error(E_WARNING, "ftp_put: %s", ftp->inbuf);
		RETURN_FALSE;
	}
	fclose(infp);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_size(int stream, string path)
   Returns the size of the file, or -1 on error */
PHP_FUNCTION(ftp_size)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - path
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	convert_to_string(arg2);

	/* get file size */
	RETURN_LONG(ftp_size(ftp, Z_STRVAL_P(arg2)));
}
/* }}} */

/* {{{ proto int ftp_mdtm(int stream, string path)
   Returns the last modification time of the file, or -1 on error */
PHP_FUNCTION(ftp_mdtm)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - path
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	convert_to_string(arg2);

	/* get file mod time */
	RETURN_LONG(ftp_mdtm(ftp, Z_STRVAL_P(arg2)));
}
/* }}} */

/* {{{ proto int ftp_rename(int stream, string src, string dest)
   Renames the given file to a new path */
PHP_FUNCTION(ftp_rename)
{
	pval		*arg1, *arg2, *arg3;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - src
	 * arg3 - dest
	 */
	if (	ZEND_NUM_ARGS() != 3 ||
		getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);

	/* rename the file */
	if (!ftp_rename(ftp, Z_STRVAL_P(arg2), Z_STRVAL_P(arg3))) {
		php_error(E_WARNING, "ftp_rename: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_delete(int stream, string path)
   Deletes a file */
PHP_FUNCTION(ftp_delete)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - path
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	convert_to_string(arg2);

	/* delete the file */
	if (!ftp_delete(ftp, Z_STRVAL_P(arg2))) {
		php_error(E_WARNING, "ftp_delete: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_site(int stream, string cmd)
   Sends a SITE command to the server */
PHP_FUNCTION(ftp_site)
{
	pval		*arg1, *arg2;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - cmd
	 */
	if (	ZEND_NUM_ARGS() != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	FTPBUF(ftp, arg1);
	convert_to_string(arg2);

	/* send the site command */
	if (!ftp_site(ftp, Z_STRVAL_P(arg2))) {
		php_error(E_WARNING, "ftp_site: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_quit(int stream)
   Closes the FTP stream */
PHP_FUNCTION(ftp_quit)
{
	pval		*arg1;
	int		id, type;

	/* arg1 - ftp
	 */
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	id = Z_LVAL_P(arg1);
	if (zend_list_find(id, &type) && type == le_ftpbuf)
		zend_list_delete(id);

	RETURN_TRUE;
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
