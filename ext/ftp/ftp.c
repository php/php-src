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

#include <ftplib.h>
#include "ftp.h"


#if HAVE_FTP

static int	le_netbuf;


function_entry php3_ftp_functions[] = {
	PHP_FE(ftp_connect,			NULL)
	PHP_FE(ftp_login,			NULL)
	PHP_FE(ftp_chdir,			NULL)
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

static void ftp_destructor_netbuf(netbuf *net)
{
	if (net) {
		FtpQuit(net);
	}
}

PHP_MINIT_FUNCTION(ftp)
{
	le_netbuf = register_list_destructors(ftp_destructor_netbuf, NULL);
	REGISTER_MAIN_LONG_CONSTANT("FTP_ASCII", FTPLIB_ASCII,
		CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("FTP_BINARY", FTPLIB_BINARY,
		CONST_PERSISTENT | CONST_CS);
	return SUCCESS;
}

PHP_FUNCTION(ftp_connect)
{
	pval		*arg1;
	int		id;
	netbuf		*net;

	/* arg1 - hostname
	 */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg1);

	/* connect */
	if (!FtpConnect(arg1->value.str.val, &net)) {
		php_error(E_WARNING, "FtpConnect: %s", FtpLastResponse(net));
		RETURN_FALSE;
	}

	id = php3_list_insert(net, le_netbuf);
	RETURN_LONG(id);
}

PHP_FUNCTION(ftp_login)
{
	pval		*arg1, *arg2, *arg3;
	int		id, type;
	netbuf		*net;

	/* arg1 - netbuf
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
	net = php3_list_find(id, &type);
	if (!net || type != le_netbuf) {
		php_error(E_WARNING, "Unable to find netbuf %d", id);
		RETURN_FALSE;
	}

	/* log in */
	if (!FtpLogin(arg2->value.str.val, arg3->value.str.val, net)) {
		php_error(E_WARNING, "FtpLogin: %s", FtpLastResponse(net));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

PHP_FUNCTION(ftp_chdir)
{
	pval		*arg1, *arg2;
	int		id, type;
	netbuf		*net;

	/* arg1 - netbuf
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
	net = php3_list_find(id, &type);
	if (!net || type != le_netbuf) {
		php_error(E_WARNING, "Unable to find netbuf %d", id);
		RETURN_FALSE;
	}

	/* change directories */
	if (!FtpChdir(arg2->value.str.val, net)) {
		php_error(E_WARNING, "FtpChdir: %s", FtpLastResponse(net));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

PHP_FUNCTION(ftp_nlist)
{
	pval		*arg1, *arg2;
	int		id, type;
	netbuf		*net;
	FILE		*outfp;
	char		*entry = NULL;
	char		*ptr;
	long		size;
	char		ch;


	/* arg1 - netbuf
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
	net = php3_list_find(id, &type);
	if (!net || type != le_netbuf) {
		php_error(E_WARNING, "Unable to find netbuf %d", id);
		RETURN_FALSE;
	}

	/* set up a temporary output file */
	if ((outfp = tmpfile()) == NULL) {
		php_error(E_WARNING, "error opening tmpfile");
		RETURN_FALSE;
	}

	/* list to the temporary file */
	if (!FtpNlst(outfp, arg2->value.str.val, net) || ferror(outfp)) {
		fclose(outfp);
		RETURN_FALSE;
	}

	array_init(return_value);
	rewind(outfp);

	/* Pluck out each file name and save to the return array. */
	do {
		/* scan for end of line */
		size = 1;
		while ((ch = getc(outfp)) != '\n') {
			if (ch == EOF) {
				size = -1;
				break;
			}
			size++;
		}

		if (size > 0) {
			/* seek back to the start of file name and copy
			 * to a buffer.  add the buffer to the array.
			 */
			fseek(outfp, -size, SEEK_CUR);
			entry = emalloc(size);
			ptr = entry;
			while (--size)
				*ptr++ = getc(outfp);
			*ptr = 0;

			add_next_index_string(return_value, entry, 0);
		}

		/* eat the \n */
		(void) getc(outfp);
	} while (size != -1);
	fclose(outfp);
}

PHP_FUNCTION(ftp_listraw)
{
	pval		*arg1, *arg2;
	int		id, type;
	netbuf		*net;
	FILE		*outfp;
	char		*entry = NULL;
	char		*ptr;
	long		size;
	char		ch;


	/* arg1 - netbuf
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
	net = php3_list_find(id, &type);
	if (!net || type != le_netbuf) {
		php_error(E_WARNING, "Unable to find netbuf %d", id);
		RETURN_FALSE;
	}

	/* set up a temporary output file */
	if ((outfp = tmpfile()) == NULL) {
		php_error(E_WARNING, "error opening tmpfile");
		RETURN_FALSE;
	}

	/* list to the temporary file */
	if (!FtpDir(outfp, arg2->value.str.val, net) || ferror(outfp)) {
		fclose(outfp);
		RETURN_FALSE;
	}

	array_init(return_value);
	rewind(outfp);

	/* Pluck out each file name and save to the return array. */
	do {
		/* scan for end of line */
		size = 1;
		while ((ch = getc(outfp)) != '\n') {
			if (ch == EOF) {
				size = -1;
				break;
			}
			size++;
		}

		if (size > 0) {
			/* seek back to the start of file name and copy
			 * to a buffer.  add the buffer to the array.
			 */
			fseek(outfp, -size, SEEK_CUR);
			entry = emalloc(size);
			ptr = entry;
			while (--size)
				*ptr++ = getc(outfp);
			*ptr = 0;

			add_next_index_string(return_value, entry, 0);
		}

		/* eat the \n */
		(void) getc(outfp);
	} while (size != -1);
	fclose(outfp);
}

PHP_FUNCTION(ftp_systype)
{
	pval		*arg1;
	int		id, type;
	netbuf		*net;
	char		buf[64];


	/* arg1 - netbuf
	 * arg2 - directory
	 */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);

	id = arg1->value.lval;
	net = php3_list_find(id, &type);
	if (!net || type != le_netbuf) {
		php_error(E_WARNING, "Unable to find netbuf %d", id);
		RETURN_FALSE;
	}

	if (!FtpSysType(buf, sizeof(buf), net)) {
		RETURN_FALSE;
	}

	RETURN_STRING(buf, 1);
}

PHP_FUNCTION(ftp_get)
{
	pval		*arg1, *arg2, *arg3, *arg4;
	int		id, type;
	netbuf		*net;
	FILE		*outfp, *tmpfp;
	char		*entry = NULL;
	char		*ptr;
	long		size;
	char		ch;


	/* arg1 - netbuf
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
	net = php3_list_find(id, &type);
	if (!net || type != le_netbuf) {
		php_error(E_WARNING, "Unable to find netbuf %d", id);
		RETURN_FALSE;
	}

	if (	arg4->value.lval != FTPLIB_ASCII &&
		arg4->value.lval != FTPLIB_BINARY)
	{
		php_error(E_WARNING, "arg4 must be FTP_ASCII or FTP_BINARY");
		RETURN_FALSE;
	}


	/* get to temporary file, so if there is an error, no existing
	 * file gets clobbered
	 */
	if ((tmpfp = tmpfile()) == NULL) {
		php_error(E_WARNING, "error opening tmpfile");
		RETURN_FALSE;
	}

	if (	!FtpGet(tmpfp, arg3->value.str.val, arg4->value.lval, net) ||
		ferror(tmpfp))
	{
		fclose(tmpfp);
		php_error(E_WARNING, "FtpGet: %s", FtpLastResponse(net));
		RETURN_FALSE;
	}

	rewind(tmpfp);

	if ((outfp = fopen(arg2->value.str.val, "w")) == NULL) {
		fclose(tmpfp);
		php_error(E_WARNING, "error opening %s", arg2->value.str.val);
		RETURN_FALSE;
	}

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

PHP_FUNCTION(ftp_put)
{
	pval		*arg1, *arg2, *arg3, *arg4;
	int		id, type;
	netbuf		*net;
	FILE		*infp;
	char		*entry = NULL;
	char		*ptr;
	long		size;
	char		ch;


	/* arg1 - netbuf
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
	net = php3_list_find(id, &type);
	if (!net || type != le_netbuf) {
		php_error(E_WARNING, "Unable to find netbuf %d", id);
		RETURN_FALSE;
	}

	if (	arg4->value.lval != FTPLIB_ASCII &&
		arg4->value.lval != FTPLIB_BINARY)
	{
		php_error(E_WARNING, "arg4 must be FTP_ASCII or FTP_BINARY");
		RETURN_FALSE;
	}

	if ((infp = fopen(arg3->value.str.val, "r")) == NULL) {
		php_error(E_WARNING, "error opening %s", arg3->value.str.val);
		RETURN_FALSE;
	}
	if (	!FtpPut(infp, arg2->value.str.val, arg4->value.lval, net) ||
		ferror(infp))
	{
		fclose(infp);
		php_error(E_WARNING, "FtpPut: %s", FtpLastResponse(net));
		RETURN_FALSE;
	}
	fclose(infp);

	RETURN_TRUE;
}

PHP_FUNCTION(ftp_quit)
{
	pval		*arg1;
	int		id, type;
	netbuf		*net;

	/* arg1 - netbuf
	 */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id = arg1->value.lval;
	net = php3_list_find(id, &type);
	if (!net || type != le_netbuf) {
		php_error(E_WARNING, "Unable to find netbuf %d", id);
		RETURN_FALSE;
	}

	php3_list_delete(id);

	RETURN_TRUE;
}

#endif /* HAVE_FTP */
