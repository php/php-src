/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "php_sysvmsg.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str.h"

/* In order to detect MSG_EXCEPT use at run time; we have no way
 * of knowing what the bit definitions are, so we can't just define
 * out own MSG_EXCEPT value. */
#define PHP_MSG_IPC_NOWAIT	1
#define PHP_MSG_NOERROR		2
#define PHP_MSG_EXCEPT		4

/* True global resources - no need for thread safety here */
static int le_sysvmsg;

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_msg_get_queue, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, perms)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msg_send, 0, 0, 3)
	ZEND_ARG_INFO(0, queue)
	ZEND_ARG_INFO(0, msgtype)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, serialize)
	ZEND_ARG_INFO(0, blocking)
	ZEND_ARG_INFO(1, errorcode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msg_receive, 0, 0, 5)
	ZEND_ARG_INFO(0, queue)
	ZEND_ARG_INFO(0, desiredmsgtype)
	ZEND_ARG_INFO(1, msgtype)
	ZEND_ARG_INFO(0, maxsize)
	ZEND_ARG_INFO(1, message)
	ZEND_ARG_INFO(0, unserialize)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(1, errorcode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msg_remove_queue, 0, 0, 1)
	ZEND_ARG_INFO(0, queue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msg_stat_queue, 0, 0, 1)
	ZEND_ARG_INFO(0, queue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msg_set_queue, 0, 0, 2)
	ZEND_ARG_INFO(0, queue)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msg_queue_exists, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ sysvmsg_functions[]
 *
 * Every user visible function must have an entry in sysvmsg_functions[].
 */
static const zend_function_entry sysvmsg_functions[] = {
	PHP_FE(msg_get_queue,				arginfo_msg_get_queue)
	PHP_FE(msg_send,					arginfo_msg_send)
	PHP_FE(msg_receive,					arginfo_msg_receive)
	PHP_FE(msg_remove_queue,			arginfo_msg_remove_queue)
	PHP_FE(msg_stat_queue,				arginfo_msg_stat_queue)
	PHP_FE(msg_set_queue,				arginfo_msg_set_queue)
	PHP_FE(msg_queue_exists,			arginfo_msg_queue_exists)
	PHP_FE_END
};
/* }}} */

/* {{{ sysvmsg_module_entry
 */
zend_module_entry sysvmsg_module_entry = {
	STANDARD_MODULE_HEADER,
	"sysvmsg",
	sysvmsg_functions,
	PHP_MINIT(sysvmsg),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(sysvmsg),
	PHP_SYSVMSG_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SYSVMSG
ZEND_GET_MODULE(sysvmsg)
#endif

static void sysvmsg_release(zend_resource *rsrc)
{
	sysvmsg_queue_t *mq = (sysvmsg_queue_t *) rsrc->ptr;
	efree(mq);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sysvmsg)
{
	le_sysvmsg = zend_register_list_destructors_ex(sysvmsg_release, NULL, "sysvmsg queue", module_number);
	REGISTER_LONG_CONSTANT("MSG_IPC_NOWAIT", PHP_MSG_IPC_NOWAIT, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("MSG_EAGAIN",	 EAGAIN, 	     CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("MSG_ENOMSG",	 ENOMSG, 	     CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("MSG_NOERROR",    PHP_MSG_NOERROR,    CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("MSG_EXCEPT",     PHP_MSG_EXCEPT,     CONST_PERSISTENT|CONST_CS);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(sysvmsg)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "sysvmsg support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto bool msg_set_queue(resource queue, array data)
   Set information for a message queue */
PHP_FUNCTION(msg_set_queue)
{
	zval *queue, *data;
	sysvmsg_queue_t *mq = NULL;
	struct msqid_ds stat;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ra", &queue, &data) == FAILURE) {
		return;
	}

	if ((mq = (sysvmsg_queue_t *)zend_fetch_resource(Z_RES_P(queue), "sysvmsg queue", le_sysvmsg)) == NULL) {
		RETURN_FALSE;
	}

	if (msgctl(mq->id, IPC_STAT, &stat) == 0) {
		zval *item;

		/* now pull out members of data and set them in the stat buffer */
		if ((item = zend_hash_str_find(Z_ARRVAL_P(data), "msg_perm.uid", sizeof("msg_perm.uid") - 1)) != NULL) {
			stat.msg_perm.uid = zval_get_long(item);
		}
		if ((item = zend_hash_str_find(Z_ARRVAL_P(data), "msg_perm.gid", sizeof("msg_perm.gid") - 1)) != NULL) {
			stat.msg_perm.gid = zval_get_long(item);
		}
		if ((item = zend_hash_str_find(Z_ARRVAL_P(data), "msg_perm.mode", sizeof("msg_perm.mode") - 1)) != NULL) {
			stat.msg_perm.mode = zval_get_long(item);
		}
		if ((item = zend_hash_str_find(Z_ARRVAL_P(data), "msg_qbytes", sizeof("msg_qbytes") - 1)) != NULL) {
			stat.msg_qbytes = zval_get_long(item);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &queue) == FAILURE) {
		return;
	}

	if ((mq = (sysvmsg_queue_t *)zend_fetch_resource(Z_RES_P(queue), "sysvmsg queue", le_sysvmsg)) == NULL) {
		RETURN_FALSE;
	}

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

/* {{{ proto bool msg_queue_exists(int key)
   Check whether a message queue exists */
PHP_FUNCTION(msg_queue_exists)
{
	zend_long key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &key) == FAILURE)	{
		return;
	}

	if (msgget(key, 0) < 0) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource msg_get_queue(int key [, int perms])
   Attach to a message queue */
PHP_FUNCTION(msg_get_queue)
{
	zend_long key;
	zend_long perms = 0666;
	sysvmsg_queue_t *mq;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &key, &perms) == FAILURE)	{
		return;
	}

	mq = (sysvmsg_queue_t *) emalloc(sizeof(sysvmsg_queue_t));

	mq->key = key;
	mq->id = msgget(key, 0);
	if (mq->id < 0)	{
		/* doesn't already exist; create it */
		mq->id = msgget(key, IPC_CREAT | IPC_EXCL | perms);
		if (mq->id < 0)	{
			php_error_docref(NULL, E_WARNING, "failed for key 0x" ZEND_XLONG_FMT ": %s", key, strerror(errno));
			efree(mq);
			RETURN_FALSE;
		}
	}
	ZVAL_COPY_VALUE(return_value, zend_list_insert(mq, le_sysvmsg));
}
/* }}} */

/* {{{ proto bool msg_remove_queue(resource queue)
   Destroy the queue */
PHP_FUNCTION(msg_remove_queue)
{
	zval *queue;
	sysvmsg_queue_t *mq = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &queue) == FAILURE) {
		return;
	}

	if ((mq = (sysvmsg_queue_t *)zend_fetch_resource(Z_RES_P(queue), "sysvmsg queue", le_sysvmsg)) == NULL) {
		RETURN_FALSE;
	}

	if (msgctl(mq->id, IPC_RMID, NULL) == 0) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto mixed msg_receive(resource queue, int desiredmsgtype, int &msgtype, int maxsize, mixed message [, bool unserialize=true [, int flags=0 [, int errorcode]]])
   Send a message of type msgtype (must be > 0) to a message queue */
PHP_FUNCTION(msg_receive)
{
	zval *out_message, *queue, *out_msgtype, *zerrcode = NULL;
	zend_long desiredmsgtype, maxsize, flags = 0;
	zend_long realflags = 0;
	zend_bool do_unserialize = 1;
	sysvmsg_queue_t *mq = NULL;
	struct php_msgbuf *messagebuffer = NULL; /* buffer to transmit */
	int result;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlz/lz/|blz/",
				&queue, &desiredmsgtype, &out_msgtype, &maxsize,
				&out_message, &do_unserialize, &flags, &zerrcode) == FAILURE) {
		return;
	}

	if (maxsize <= 0) {
		php_error_docref(NULL, E_WARNING, "maximum size of the message has to be greater than zero");
		return;
	}

	if (flags != 0) {
		if (flags & PHP_MSG_EXCEPT) {
#ifndef MSG_EXCEPT
			php_error_docref(NULL, E_WARNING, "MSG_EXCEPT is not supported on your system");
			RETURN_FALSE;
#else
			realflags |= MSG_EXCEPT;
#endif
		}
		if (flags & PHP_MSG_NOERROR) {
			realflags |= MSG_NOERROR;
		}
		if (flags & PHP_MSG_IPC_NOWAIT) {
			realflags |= IPC_NOWAIT;
		}
	}

	if ((mq = (sysvmsg_queue_t *)zend_fetch_resource(Z_RES_P(queue), "sysvmsg queue", le_sysvmsg)) == NULL) {
		RETURN_FALSE;
	}

	messagebuffer = (struct php_msgbuf *) safe_emalloc(maxsize, 1, sizeof(struct php_msgbuf));

	result = msgrcv(mq->id, messagebuffer, maxsize, desiredmsgtype, realflags);

	zval_ptr_dtor(out_msgtype);
	zval_ptr_dtor(out_message);
	ZVAL_LONG(out_msgtype, 0);
	ZVAL_FALSE(out_message);

	if (zerrcode) {
		zval_ptr_dtor(zerrcode);
		ZVAL_LONG(zerrcode, 0);
	}

	if (result >= 0) {
		/* got it! */
		ZVAL_LONG(out_msgtype, messagebuffer->mtype);

		RETVAL_TRUE;
		if (do_unserialize)	{
			php_unserialize_data_t var_hash;
			zval tmp;
			const unsigned char *p = (const unsigned char *) messagebuffer->mtext;

			PHP_VAR_UNSERIALIZE_INIT(var_hash);
			if (!php_var_unserialize(&tmp, &p, p + result, &var_hash)) {
				php_error_docref(NULL, E_WARNING, "message corrupted");
				RETVAL_FALSE;
			} else {
				ZVAL_COPY_VALUE(out_message, &tmp);
			}
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		} else {
			ZVAL_STRINGL(out_message, messagebuffer->mtext, result);
		}
	} else if (zerrcode) {
		ZVAL_LONG(zerrcode, errno);
	}
	efree(messagebuffer);
}
/* }}} */

/* {{{ proto bool msg_send(resource queue, int msgtype, mixed message [, bool serialize=true [, bool blocking=true [, int errorcode]]])
   Send a message of type msgtype (must be > 0) to a message queue */
PHP_FUNCTION(msg_send)
{
	zval *message, *queue, *zerror=NULL;
	zend_long msgtype;
	zend_bool do_serialize = 1, blocking = 1;
	sysvmsg_queue_t * mq = NULL;
	struct php_msgbuf * messagebuffer = NULL; /* buffer to transmit */
	int result;
	int message_len = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlz|bbz/",
				&queue, &msgtype, &message, &do_serialize, &blocking, &zerror) == FAILURE) {
		return;
	}

	if ((mq = (sysvmsg_queue_t *)zend_fetch_resource(Z_RES_P(queue), "sysvmsg queue", le_sysvmsg)) == NULL) {
		RETURN_FALSE;
	}

	if (do_serialize) {
		smart_str msg_var = {0};
		php_serialize_data_t var_hash;

		PHP_VAR_SERIALIZE_INIT(var_hash);
		php_var_serialize(&msg_var, message, &var_hash);
		PHP_VAR_SERIALIZE_DESTROY(var_hash);

		/* NB: php_msgbuf is 1 char bigger than a long, so there is no need to
		 * allocate the extra byte. */
		messagebuffer = safe_emalloc(ZSTR_LEN(msg_var.s), 1, sizeof(struct php_msgbuf));
		memcpy(messagebuffer->mtext, ZSTR_VAL(msg_var.s), ZSTR_LEN(msg_var.s) + 1);
		message_len = ZSTR_LEN(msg_var.s);
		smart_str_free(&msg_var);
	} else {
		char *p;
		switch (Z_TYPE_P(message)) {
			case IS_STRING:
				p = Z_STRVAL_P(message);
				message_len = Z_STRLEN_P(message);
				break;

			case IS_LONG:
				message_len = spprintf(&p, 0, ZEND_LONG_FMT, Z_LVAL_P(message));
				break;
			case IS_FALSE:
				message_len = spprintf(&p, 0, "0");
				break;
			case IS_TRUE:
				message_len = spprintf(&p, 0, "1");
				break;
			case IS_DOUBLE:
				message_len = spprintf(&p, 0, "%F", Z_DVAL_P(message));
				break;
			default:
				php_error_docref(NULL, E_WARNING, "Message parameter must be either a string or a number.");
				RETURN_FALSE;
		}

		messagebuffer = safe_emalloc(message_len, 1, sizeof(struct php_msgbuf));
		memcpy(messagebuffer->mtext, p, message_len + 1);

		if (Z_TYPE_P(message) != IS_STRING) {
			efree(p);
		}
	}

	/* set the message type */
	messagebuffer->mtype = msgtype;

	result = msgsnd(mq->id, messagebuffer, message_len, blocking ? 0 : IPC_NOWAIT);

	efree(messagebuffer);

	if (result == -1) {
		php_error_docref(NULL, E_WARNING, "msgsnd failed: %s", strerror(errno));
		if (zerror) {
			zval_ptr_dtor(zerror);
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
