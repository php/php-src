/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
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
   | Authors: Wez Furlong <wez@thebrainroom.com                           |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_sysvmsg.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_smart_str.h"

/* In order to detect MSG_EXCEPT use at run time; we have no way
 * of knowing what the bit definitions are, so we can't just define
 * out own MSG_EXCEPT value. */
#define PHP_MSG_IPC_NOWAIT	1
#define PHP_MSG_NOERROR		2
#define PHP_MSG_EXCEPT		4

/* True global resources - no need for thread safety here */
static int le_sysvmsg;

static unsigned char sixth_arg_force_ref[] = { 6, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
static unsigned char msg_receive_args_force_ref[] = { 8, BYREF_NONE, BYREF_NONE, BYREF_FORCE,
	BYREF_NONE, BYREF_FORCE, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

/* {{{ sysvmsg_functions[]
 *
 * Every user visible function must have an entry in sysvmsg_functions[].
 */
function_entry sysvmsg_functions[] = {
	PHP_FE(msg_get_queue,				NULL)
	PHP_FE(msg_send,					sixth_arg_force_ref)
	PHP_FE(msg_receive,					msg_receive_args_force_ref)
	PHP_FE(msg_remove_queue,			NULL)
	PHP_FE(msg_stat_queue,				NULL)
	PHP_FE(msg_set_queue,				NULL)
	{NULL, NULL, NULL}	/* Must be the last line in sysvmsg_functions[] */
};
/* }}} */

/* {{{ sysvmsg_module_entry
 */
zend_module_entry sysvmsg_module_entry = {
	STANDARD_MODULE_HEADER,
	"sysvmsg",
	sysvmsg_functions,
	PHP_MINIT(sysvmsg),
	PHP_MSHUTDOWN(sysvmsg),
	NULL,
	NULL,
	PHP_MINFO(sysvmsg),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SYSVMSG
ZEND_GET_MODULE(sysvmsg)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("sysvmsg.value",      "42", PHP_INI_ALL, OnUpdateInt, global_value, zend_sysvmsg_globals, sysvmsg_globals)
    STD_PHP_INI_ENTRY("sysvmsg.string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_sysvmsg_globals, sysvmsg_globals)
PHP_INI_END()
*/
/* }}} */

static void sysvmsg_release(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	sysvmsg_queue_t * mq = (sysvmsg_queue_t*)rsrc->ptr;
	efree(mq);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sysvmsg)
{
	le_sysvmsg = zend_register_list_destructors_ex(sysvmsg_release, NULL, "sysvmsg queue", module_number);
	REGISTER_LONG_CONSTANT("MSG_IPC_NOWAIT", PHP_MSG_IPC_NOWAIT, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("MSG_NOERROR", PHP_MSG_NOERROR, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("MSG_EXCEPT", PHP_MSG_EXCEPT, CONST_PERSISTENT|CONST_CS);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(sysvmsg)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(sysvmsg)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "sysvmsg support", "enabled");
	php_info_print_table_row(2, "Revision", "$Revision$");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto array msg_set_queue(resource queue, array data)
   Set information for a message queue */
PHP_FUNCTION(msg_set_queue)
{
	zval *queue, *data;
	sysvmsg_queue_t *mq = NULL;
	struct msqid_ds stat;
			
	RETVAL_FALSE;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &queue, &data) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(mq, sysvmsg_queue_t *, &queue, -1, "sysvmsg queue", le_sysvmsg);

	if (msgctl(mq->id, IPC_STAT, &stat) == 0) {
		zval **item;
		
		/* now pull out members of data and set them in the stat buffer */
		if (zend_hash_find(Z_ARRVAL_P(data), "msg_perm.uid",
					sizeof("msg_perm.uid"), (void**)&item) == SUCCESS) {
			convert_to_long_ex(item);
			stat.msg_perm.uid = Z_LVAL_PP(item);
		}
		if (zend_hash_find(Z_ARRVAL_P(data), "msg_perm.gid",
					sizeof("msg_perm.gid"), (void**)&item) == SUCCESS) {
			convert_to_long_ex(item);
			stat.msg_perm.gid = Z_LVAL_PP(item);
		}
		if (zend_hash_find(Z_ARRVAL_P(data), "msg_perm.mode",
					sizeof("msg_perm.mode"), (void**)&item) == SUCCESS) {
			convert_to_long_ex(item);
			stat.msg_perm.mode = Z_LVAL_PP(item);
		}
		if (zend_hash_find(Z_ARRVAL_P(data), "msg_qbytes",
					sizeof("msg_qbytes"), (void**)&item) == SUCCESS) {
			convert_to_long_ex(item);
			stat.msg_qbytes = Z_LVAL_PP(item);
		}
		if (msgctl(mq->id, IPC_SET, &stat) == 0) {
			RETVAL_TRUE;
		}
	}
}
/* }}} */

/* {{{ proto array msg_stat_queue(resource queue)
   Returns information about a message queue */
PHP_FUNCTION(msg_stat_queue)
{
	zval *queue;
	sysvmsg_queue_t *mq = NULL;
	struct msqid_ds stat;
	
	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &queue) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(mq, sysvmsg_queue_t *, &queue, -1, "sysvmsg queue", le_sysvmsg);

	if (msgctl(mq->id, IPC_STAT, &stat) == 0) {
		array_init(return_value);
		
		add_assoc_long(return_value, "msg_perm.uid", stat.msg_perm.uid);
		add_assoc_long(return_value, "msg_perm.gid", stat.msg_perm.gid);
		add_assoc_long(return_value, "msg_perm.mode", stat.msg_perm.mode);
		add_assoc_long(return_value, "msg_stime",  stat.msg_stime);
		add_assoc_long(return_value, "msg_rtime",  stat.msg_rtime);
		add_assoc_long(return_value, "msg_ctime",  stat.msg_ctime);
		add_assoc_long(return_value, "msg_qnum",   stat.msg_qnum);
		add_assoc_long(return_value, "msg_qbytes", stat.msg_qbytes);
		add_assoc_long(return_value, "msg_lspid",  stat.msg_lspid);
		add_assoc_long(return_value, "msg_lrpid",  stat.msg_lrpid);
	}
}
/* }}} */

/* {{{ proto resource msg_get_queue(long key [, long perms])
   Attach to a message queue */
PHP_FUNCTION(msg_get_queue)
{
	long key;
	long perms = 0666;
	sysvmsg_queue_t *mq;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &key, &perms) == FAILURE)	{
		return;
	}

	mq = (sysvmsg_queue_t *)emalloc(sizeof(sysvmsg_queue_t));

	mq->key = key;
	mq->id = msgget(key, 0);
	if (mq->id < 0)	{
		/* doesn't already exist; create it */
		mq->id = msgget(key, IPC_CREAT|IPC_EXCL|perms);
		if (mq->id < 0)	{
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed for key 0x%x: %s", key, strerror(errno));
			efree(mq);
			RETURN_FALSE;
		}
	}
	RETVAL_RESOURCE(zend_list_insert(mq, le_sysvmsg));	
}
/* }}} */

/* {{{ proto bool msg_remove_queue(resource queue)
   Destroy the queue */
PHP_FUNCTION(msg_remove_queue)
{
	zval *queue;
	sysvmsg_queue_t *mq = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &queue) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(mq, sysvmsg_queue_t *, &queue, -1, "sysvmsg queue", le_sysvmsg);

	if (msgctl(mq->id, IPC_RMID, NULL) == 0) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto mixed msg_receive(resource queue, long desiredmsgtype, long &msgtype, long maxsize, mixed message [[, bool unserialize=true][, long flags=0[, long errorcode]]]
   Send a message of type msgtype (must be > 0) to a message queue */
PHP_FUNCTION(msg_receive)
{
	zval *out_message, *queue, *out_msgtype, *zerrcode = NULL;
	long desiredmsgtype, maxsize, flags = 0;
	long realflags = 0;
	zend_bool do_unserialize = 1;
	sysvmsg_queue_t *mq = NULL;
	struct php_msgbuf *messagebuffer = NULL; /* buffer to transmit */
	int result;
	
	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlzlz|blz",
				&queue, &desiredmsgtype, &out_msgtype, &maxsize,
				&out_message, &do_unserialize, &flags, &zerrcode) == FAILURE)
		return;

	if (flags != 0) {
		if (flags & PHP_MSG_EXCEPT) {
#ifndef MSG_EXCEPT
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "MSG_EXCEPT is not supported on your system");
			RETURN_FALSE;
#else
			realflags |= MSG_EXCEPT;
#endif
		}
		if (flags & PHP_MSG_NOERROR)
			realflags |= MSG_NOERROR;
		if (flags & PHP_MSG_IPC_NOWAIT)
			realflags |= IPC_NOWAIT;
	}
	
	ZEND_FETCH_RESOURCE(mq, sysvmsg_queue_t *, &queue, -1, "sysvmsg queue", le_sysvmsg);

	messagebuffer = (struct php_msgbuf*)emalloc(sizeof(struct php_msgbuf) + maxsize);
	
	result = msgrcv(mq->id, messagebuffer, maxsize, desiredmsgtype, realflags);
		
	zval_dtor(out_msgtype);
	zval_dtor(out_message);	
	ZVAL_LONG(out_msgtype, 0);
	ZVAL_FALSE(out_message);
	
	if (zerrcode) {
		zval_dtor(zerrcode);
		ZVAL_LONG(zerrcode, 0);
	}
	
	if (result >= 0) {
		/* got it! */
		ZVAL_LONG(out_msgtype, messagebuffer->mtype);

		if (do_unserialize)	{
			php_unserialize_data_t var_hash;
			zval *tmp = NULL;
			const char *p = (const char*)messagebuffer->mtext;

			MAKE_STD_ZVAL(tmp);
			PHP_VAR_UNSERIALIZE_INIT(var_hash);
			if (!php_var_unserialize(&tmp, &p, p + result, &var_hash TSRMLS_CC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "message corrupted");
				RETVAL_FALSE;
			}
			REPLACE_ZVAL_VALUE(&out_message, tmp, 0);
			FREE_ZVAL(tmp);
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		} else {
			ZVAL_STRINGL(out_message, messagebuffer->mtext, result, 1);
		}
		RETVAL_TRUE;
	} else if (zerrcode) {
		ZVAL_LONG(zerrcode, errno);
	}
	efree(messagebuffer);
}
/* }}} */

/* {{{ proto bool msg_send(resource queue, long msgtype, mixed message [[, bool serialize=true][, bool blocking=true][, long errorcode]])
   Send a message of type msgtype (must be > 0) to a message queue */
PHP_FUNCTION(msg_send)
{
	zval *message, *queue, *zerror=NULL;
	long msgtype;
	zend_bool do_serialize = 1, blocking = 1;
	sysvmsg_queue_t * mq = NULL;
	struct php_msgbuf * messagebuffer = NULL; /* buffer to transmit */
	int result;
	int message_len = 0;
	
	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlz|bbz",
				&queue, &msgtype, &message, &do_serialize, &blocking, &zerror) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(mq, sysvmsg_queue_t*, &queue, -1, "sysvmsg queue", le_sysvmsg);

	if (do_serialize) {
		smart_str msg_var = {0};
		php_serialize_data_t var_hash;

		PHP_VAR_SERIALIZE_INIT(var_hash);
		php_var_serialize(&msg_var, &message, &var_hash TSRMLS_CC);
		PHP_VAR_SERIALIZE_DESTROY(var_hash);
		
		/* NB: php_msgbuf is 1 char bigger than a long, so there is no need to
		 * allocate the extra byte. */
		messagebuffer = emalloc(sizeof(struct php_msgbuf) + msg_var.len);
		memcpy(messagebuffer->mtext, msg_var.c, msg_var.len + 1);
		message_len = msg_var.len;
		smart_str_free(&msg_var);
	} else {
		convert_to_string_ex(&message);
		messagebuffer = emalloc(sizeof(struct php_msgbuf) + Z_STRLEN_P(message));
		memcpy(messagebuffer->mtext, Z_STRVAL_P(message), Z_STRLEN_P(message) + 1);
		message_len = Z_STRLEN_P(message);
	}
	
	/* set the message type */
	messagebuffer->mtype = msgtype;

	result = msgsnd(mq->id, messagebuffer, message_len, blocking ? 0 : IPC_NOWAIT);
	
	efree(messagebuffer);

	if (result == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "msgsnd failed: %s", strerror(errno));
		if (zerror) {
			ZVAL_LONG(zerror, errno);
		}
	} else {
		RETVAL_TRUE;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 tw=78 fdm=marker
 * vim<600: noet sw=4 ts=4 tw=78
 */
