/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |          Andrew Skalski      <askalski@chek.com>                     |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_globals.h"

#if HAVE_FTP

#include "php_ftp.h"
#include "ftp.h"

static int	le_ftpbuf;


function_entry php3_ftp_functions[] = {
	PHP_FE(ftp_connect,			NULL)
	PHP_FE(ftp_login,			NULL)
	PHP_FE(ftp_pwd,				NULL)
	PHP_FE(ftp_cdup,			NULL)
	PHP_FE(ftp_chdir,			NULL)
	PHP_FE(ftp_mkdir,			NULL)
	PHP_FE(ftp_rmdir,			NULL)
	PHP_FE(ftp_nlist,			NULL)
	PHP_FE(ftp_listraw,			NULL)
	PHP_FE(ftp_systype,			NULL)
	PHP_FE(ftp_get,				NULL)
	PHP_FE(ftp_put,				NULL)
	PHP_FE(ftp_quit,			NULL)
	{NULL, NULL, NULL}
};

php3_module_entry php3_ftp_module_entry = {
	"FTP Functions",
	php3_ftp_functions,
	PHP_MINIT(ftp),
	NULL,
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES
};

static void ftp_destructor_ftpbuf(ftpbuf_t *ftp)
{
	ftp_close(ftp);
}

PHP_MINIT_FUNCTION(ftp)
{
	le_ftpbuf = register_list_destructors(ftp_destructor_ftpbuf, NULL);
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

/* {{{ proto int ftp_connect(string host)
   Open a FTP stream */
PHP_FUNCTION(ftp_connect)
{
	pval		*arg1;
	int		id;
	ftpbuf_t	*ftp;

	/* arg1 - hostname
	 */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg1);

	/* connect */
	ftp = ftp_open(arg1->value.str.val, 0);
	if (ftp == NULL) {
		php_error(E_WARNING, "ftp_connect: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	id = php3_list_insert(ftp, le_ftpbuf);
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int ftp_login(int stream, string username, string password)
   Logs into the FTP server. */
PHP_FUNCTION(ftp_login)
{
	pval		*arg1, *arg2, *arg3;
	int		id, type;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - username
	 * arg3 - password
	 */
	if (	ARG_COUNT(ht) != 3 ||
		getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	/* log in */
	if (!ftp_login(ftp, arg2->value.str.val, arg3->value.str.val)) {
		php_error(E_WARNING, "ftp_login: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string ftp_pwd(int stream)
   Returns the present working directory. */
PHP_FUNCTION(ftp_pwd)
{
	pval		*arg1;
	int		id, type;
	ftpbuf_t	*ftp;
	const char	*pwd;

	/* arg1 - ftp
	 */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

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
	int		id, type;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

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
	int		id, type;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ARG_COUNT(ht) != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	/* change directories */
	if (!ftp_chdir(ftp, arg2->value.str.val)) {
		php_error(E_WARNING, "ftp_chdir: %s", ftp->inbuf);
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
	int		id, type;
	ftpbuf_t	*ftp;
	char		*ret, *tmp;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ARG_COUNT(ht) != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	/* change directories */
	tmp = ftp_mkdir(ftp, arg2->value.str.val);
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
	int		id, type;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ARG_COUNT(ht) != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	/* change directories */
	if (!ftp_rmdir(ftp, arg2->value.str.val)) {
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
	int		id, type;
	ftpbuf_t	*ftp;
	char		**nlist, **ptr;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ARG_COUNT(ht) != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	/* get list of files */
	nlist = ftp_nlist(ftp, arg2->value.str.val);
	if (nlist == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (ptr = nlist; *ptr; ptr++)
		add_next_index_string(return_value, *ptr, 1);
	free(nlist);
}
/* }}} */

/* {{{ proto array ftp_listraw(int stream, string directory)
   Returns a detailed listing of a directory as an array of output lines */
PHP_FUNCTION(ftp_listraw)
{
	pval		*arg1, *arg2;
	int		id, type;
	ftpbuf_t	*ftp;
	char		**llist, **ptr;

	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (	ARG_COUNT(ht) != 2 ||
		getParameters(ht, 2, &arg1, &arg2) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	/* get directory listing */
	llist = ftp_list(ftp, arg2->value.str.val);
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
	int		id, type;
	ftpbuf_t	*ftp;
	const char	*syst;


	/* arg1 - ftp
	 * arg2 - directory
	 */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	syst = ftp_syst(ftp);
	if (syst == NULL) {
		php_error(E_WARNING, "ftp_syst: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	RETURN_STRING((char*) syst, 1);
}
/* }}} */

/* {{{ proto int ftp_get(int stream, string local_file, string remote_file, int mode)
   Retrieves a file from the FTP server. */
PHP_FUNCTION(ftp_get)
{
	pval		*arg1, *arg2, *arg3, *arg4;
	int		id, type;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	FILE		*outfp, *tmpfp;
	int		ch;


	/* arg1 - ftp
	 * arg2 - destination (local) file
	 * arg3 - source (remote) file
	 * arg4 - transfer mode
	 */
	if (	ARG_COUNT(ht) != 4 ||
		getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	convert_to_long(arg4);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	if (	arg4->value.lval != FTPTYPE_ASCII &&
		arg4->value.lval != FTPTYPE_IMAGE)
	{
		php_error(E_WARNING, "arg4 must be FTP_ASCII or FTP_IMAGE");
		RETURN_FALSE;
	}

	xtype = arg4->value.lval;

	/* get to temporary file, so if there is an error, no existing
	 * file gets clobbered
	 */
	if ((tmpfp = tmpfile()) == NULL) {
		php_error(E_WARNING, "error opening tmpfile");
		RETURN_FALSE;
	}

	if (	!ftp_get(ftp, tmpfp, arg3->value.str.val, xtype) ||
		ferror(tmpfp))
	{
		fclose(tmpfp);
		php_error(E_WARNING, "ftp_get: %s", ftp->inbuf);
		RETURN_FALSE;
	}

	if ((outfp = fopen(arg2->value.str.val, "w")) == NULL) {
		fclose(tmpfp);
		php_error(E_WARNING, "error opening %s", arg2->value.str.val);
		RETURN_FALSE;
	}

	rewind(tmpfp);
	while ((ch = getc(tmpfp)) != EOF)
		putc(ch, outfp);

	if (ferror(tmpfp) || ferror(outfp)) {
		fclose(tmpfp);
		fclose(outfp);
		php_error(E_WARNING, "error writing %s", arg2->value.str.val);
		RETURN_FALSE;
	}

	fclose(tmpfp);
	fclose(outfp);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftp_put(int stream, string remote_file, string local_file, int mode)
   Stores a file on the FTP server */
PHP_FUNCTION(ftp_put)
{
	pval		*arg1, *arg2, *arg3, *arg4;
	int		id, type;
	ftpbuf_t	*ftp;
	ftptype_t	xtype;
	FILE		*infp;


	/* arg1 - ftp
	 * arg2 - destination (remote) file
	 * arg3 - source (local) file
	 * arg4 - transfer mode
	 */
	if (	ARG_COUNT(ht) != 4 ||
		getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	convert_to_long(arg4);

	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	if (	arg4->value.lval != FTPTYPE_ASCII &&
		arg4->value.lval != FTPTYPE_IMAGE)
	{
		php_error(E_WARNING, "arg4 must be FTP_ASCII or FTP_IMAGE");
		RETURN_FALSE;
	}

	xtype = arg4->value.lval;

	if ((infp = fopen(arg3->value.str.val, "r")) == NULL) {
		php_error(E_WARNING, "error opening %s", arg3->value.str.val);
		RETURN_FALSE;
	}
	if (	!ftp_put(ftp, arg2->value.str.val, infp, xtype) ||
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

/* {{{ proto int ftp_quit(int stream)
   Closes the FTP stream */
PHP_FUNCTION(ftp_quit)
{
	pval		*arg1;
	int		id, type;
	ftpbuf_t	*ftp;

	/* arg1 - ftp
	 */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id = arg1->value.lval;
	ftp = php3_list_find(id, &type);
	if (!ftp || type != le_ftpbuf) {
		php_error(E_WARNING, "Unable to find ftpbuf %d", id);
		RETURN_FALSE;
	}

	php3_list_delete(id);

	RETURN_TRUE;
}
/* }}} */

#endif /* HAVE_FTP */
