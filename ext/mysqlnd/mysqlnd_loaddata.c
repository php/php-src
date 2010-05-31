/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_globals.h"
#include "mysqlnd.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"


#define ALLOC_CALLBACK_ARGS(a, b, c)\
if (c) {\
	a = (zval ***)safe_emalloc(c, sizeof(zval **), 0);\
	for (i = b; i < c; i++) {\
		a[i] = mnd_emalloc(sizeof(zval *));\
		MAKE_STD_ZVAL(*a[i]);\
	}\
}

#define FREE_CALLBACK_ARGS(a, b, c)\
if (a) {\
	for (i = b; i < c; i++) {\
		zval_ptr_dtor(a[i]);\
		mnd_efree(a[i]);\
	}\
	mnd_efree(a);\
}

/* {{{ mysqlnd_local_infile_init */
static
int mysqlnd_local_infile_init(void **ptr, char *filename, void **userdata TSRMLS_DC)
{
	MYSQLND_INFILE_INFO	*info;
	php_stream_context	*context = NULL;

	DBG_ENTER("mysqlnd_local_infile_init");

	info = ((MYSQLND_INFILE_INFO *)mnd_ecalloc(1, sizeof(MYSQLND_INFILE_INFO)));
	if (!info) {
		DBG_RETURN(1);
	}
	
	*ptr = info;

	/* check open_basedir */
	if (PG(open_basedir)) {
		if (php_check_open_basedir_ex(filename, 0 TSRMLS_CC) == -1) {
			strcpy(info->error_msg, "open_basedir restriction in effect. Unable to open file");
			info->error_no = CR_UNKNOWN_ERROR;
			DBG_RETURN(1);
		}
	}

	info->filename = filename;
	info->fd = php_stream_open_wrapper_ex((char *)filename, "r", 0, NULL, context);

	if (info->fd == NULL) {
		snprintf((char *)info->error_msg, sizeof(info->error_msg), "Can't find file '%-.64s'.", filename);
		info->error_no = MYSQLND_EE_FILENOTFOUND;
		DBG_RETURN(1);
	}

	DBG_RETURN(0);
}
/* }}} */


/* {{{ mysqlnd_local_infile_read */
static
int mysqlnd_local_infile_read(void *ptr, char *buf, unsigned int buf_len TSRMLS_DC)
{
	MYSQLND_INFILE_INFO	*info = (MYSQLND_INFILE_INFO *)ptr;
	int count;

	DBG_ENTER("mysqlnd_local_infile_read");

	count = (int)php_stream_read(info->fd, buf, buf_len);

	if (count < 0) {
		strcpy(info->error_msg, "Error reading file");
		info->error_no = CR_UNKNOWN_ERROR;
	}

	DBG_RETURN(count);
}
/* }}} */


/* {{{ mysqlnd_local_infile_error */
static
int	mysqlnd_local_infile_error(void *ptr, char *error_buf, unsigned int error_buf_len TSRMLS_DC)
{
	MYSQLND_INFILE_INFO	*info = (MYSQLND_INFILE_INFO *)ptr;

	DBG_ENTER("mysqlnd_local_infile_error");

	if (info) {
		strlcpy(error_buf, info->error_msg, error_buf_len);
		DBG_INF_FMT("have info, %d", info->error_no);
		DBG_RETURN(info->error_no);
	}

	strlcpy(error_buf, "Unknown error", error_buf_len);
	DBG_INF_FMT("no info, %d", CR_UNKNOWN_ERROR);
	DBG_RETURN(CR_UNKNOWN_ERROR);
}
/* }}} */


/* {{{ mysqlnd_local_infile_end */
static
void mysqlnd_local_infile_end(void *ptr TSRMLS_DC)
{
	MYSQLND_INFILE_INFO	*info = (MYSQLND_INFILE_INFO *)ptr;

	if (info) {
		/* php_stream_close segfaults on NULL */
		if (info->fd) {
			php_stream_close(info->fd);
			info->fd = NULL;
		}
		mnd_efree(info);
	}
}
/* }}} */


/* {{{ mysqlnd_local_infile_default */
PHPAPI void mysqlnd_local_infile_default(MYSQLND *conn)
{
	conn->infile.local_infile_init = mysqlnd_local_infile_init;
	conn->infile.local_infile_read = mysqlnd_local_infile_read;
	conn->infile.local_infile_error = mysqlnd_local_infile_error;
	conn->infile.local_infile_end = mysqlnd_local_infile_end;
}
/* }}} */

/* {{{ mysqlnd_set_local_infile_handler */
PHPAPI void mysqlnd_set_local_infile_handler(MYSQLND * const conn, const char * const funcname)
{
	if (!conn->infile.callback) {
		MAKE_STD_ZVAL(conn->infile.callback);
	} else {
		zval_dtor(conn->infile.callback);
	}
	ZVAL_STRING(conn->infile.callback, (char*) funcname, 1);
}
/* }}} */


static const char *lost_conn = "Lost connection to MySQL server during LOAD DATA of local file";


/* {{{ mysqlnd_handle_local_infile */
enum_func_status
mysqlnd_handle_local_infile(MYSQLND *conn, const char *filename, zend_bool *is_warning TSRMLS_DC)
{
	char				*buf;
	char				empty_packet[MYSQLND_HEADER_SIZE];
	enum_func_status	result = FAIL;
	unsigned int		buflen = 4096;
	void				*info = NULL;
	int					bufsize;
	size_t				ret;
	MYSQLND_INFILE		infile;

	DBG_ENTER("mysqlnd_handle_local_infile");

	if (!(conn->options.flags & CLIENT_LOCAL_FILES)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "LOAD DATA LOCAL INFILE forbidden");
		/* write empty packet to server */
		ret = conn->net->m.send(conn, empty_packet, 0 TSRMLS_CC);
		*is_warning = TRUE;
		goto infile_error;
	}

	infile = conn->infile;
	/* allocate buffer for reading data */
	buf = (char *)mnd_ecalloc(1, buflen);

	*is_warning = FALSE;

	/* init handler: allocate read buffer and open file */
	if (infile.local_infile_init(&info, (char *)filename, conn->infile.userdata TSRMLS_CC)) {
		*is_warning = TRUE;
		/* error occured */
		strcpy(conn->error_info.sqlstate, UNKNOWN_SQLSTATE);
		conn->error_info.error_no =
				infile.local_infile_error(info, conn->error_info.error, sizeof(conn->error_info.error) TSRMLS_CC);
		/* write empty packet to server */
		ret = conn->net->m.send(conn, empty_packet, 0 TSRMLS_CC);
		goto infile_error;
	}

	/* read data */
	while ((bufsize = infile.local_infile_read (info, buf + MYSQLND_HEADER_SIZE, buflen - MYSQLND_HEADER_SIZE TSRMLS_CC)) > 0) {
		if ((ret = conn->net->m.send(conn, buf, bufsize TSRMLS_CC)) < 0) {
			DBG_ERR_FMT("Error during read : %d %s %s", CR_SERVER_LOST, UNKNOWN_SQLSTATE, lost_conn);
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_LOST, UNKNOWN_SQLSTATE, lost_conn);
			goto infile_error;
		}
	}

	/* send empty packet for eof */
	if ((ret = conn->net->m.send(conn, empty_packet, 0 TSRMLS_CC)) < 0) {
		SET_CLIENT_ERROR(conn->error_info, CR_SERVER_LOST, UNKNOWN_SQLSTATE, lost_conn);
		goto infile_error;
	}

	/* error during read occured */
	if (bufsize < 0) {
		*is_warning = TRUE;
		DBG_ERR_FMT("Bufsize < 0, warning,  %d %s %s", CR_SERVER_LOST, UNKNOWN_SQLSTATE, lost_conn);
		strcpy(conn->error_info.sqlstate, UNKNOWN_SQLSTATE);
		conn->error_info.error_no =
				infile.local_infile_error(info, conn->error_info.error, sizeof(conn->error_info.error) TSRMLS_CC);
		goto infile_error;
	}

	result = PASS;

infile_error:
	/* get response from server and update upsert values */
	if (FAIL == conn->m->simple_command_handle_response(conn, PROT_OK_PACKET, FALSE, COM_QUERY, FALSE TSRMLS_CC)) {
		result = FAIL;
	}

	(*conn->infile.local_infile_end)(info TSRMLS_CC);
	mnd_efree(buf);
	DBG_INF_FMT("%s", result == PASS? "PASS":"FAIL");
	DBG_RETURN(result);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
