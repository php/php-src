/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "php_sysvmsg.h"
#include "sysvmsg_arginfo.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

PHP_MINIT_FUNCTION(sysvmsg);
PHP_MINFO_FUNCTION(sysvmsg);

typedef struct {
	key_t key;
	zend_long id;
	zend_object std;
} sysvmsg_queue_t;

struct php_msgbuf {
	zend_long mtype;
	char mtext[1];
};

/* {{{ sysvmsg_module_entry */
zend_module_entry sysvmsg_module_entry = {
	STANDARD_MODULE_HEADER,
	"sysvmsg",
	ext_functions,
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

/* SysvMessageQueue class */

zend_class_entry *sysvmsg_queue_ce;
static zend_object_handlers sysvmsg_queue_object_handlers;

static inline sysvmsg_queue_t *sysvmsg_queue_from_obj(zend_object *obj) {
	return (sysvmsg_queue_t *)((char *)(obj) - XtOffsetOf(sysvmsg_queue_t, std));
}

#define Z_SYSVMSG_QUEUE_P(zv) sysvmsg_queue_from_obj(Z_OBJ_P(zv))

static zend_object *sysvmsg_queue_create_object(zend_class_entry *class_type) {
	sysvmsg_queue_t *intern = zend_object_alloc(sizeof(sysvmsg_queue_t), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

static zend_function *sysvmsg_queue_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct SysvMessageQueue, use msg_get_queue() instead");
	return NULL;
}

static void sysvmsg_queue_free_obj(zend_object *object)
{
	sysvmsg_queue_t *sysvmsg_queue = sysvmsg_queue_from_obj(object);

	zend_object_std_dtor(&sysvmsg_queue->std);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(sysvmsg)
{
	sysvmsg_queue_ce = register_class_SysvMessageQueue();
	sysvmsg_queue_ce->create_object = sysvmsg_queue_create_object;
	sysvmsg_queue_ce->default_object_handlers = &sysvmsg_queue_object_handlers;

	memcpy(&sysvmsg_queue_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	sysvmsg_queue_object_handlers.offset = XtOffsetOf(sysvmsg_queue_t, std);
	sysvmsg_queue_object_handlers.free_obj = sysvmsg_queue_free_obj;
	sysvmsg_queue_object_handlers.get_constructor = sysvmsg_queue_get_constructor;
	sysvmsg_queue_object_handlers.clone_obj = NULL;
	sysvmsg_queue_object_handlers.compare = zend_objects_not_comparable;

	register_sysvmsg_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(sysvmsg)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "sysvmsg support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ Set information for a message queue */
PHP_FUNCTION(msg_set_queue)
{
	zval *queue, *data;
	sysvmsg_queue_t *mq = NULL;
	struct msqid_ds stat;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oa", &queue, sysvmsg_queue_ce, &data) == FAILURE) {
		RETURN_THROWS();
	}

	mq = Z_SYSVMSG_QUEUE_P(queue);

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

/* {{{ Returns information about a message queue */
PHP_FUNCTION(msg_stat_queue)
{
	zval *queue;
	sysvmsg_queue_t *mq = NULL;
	struct msqid_ds stat;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &queue, sysvmsg_queue_ce) == FAILURE) {
		RETURN_THROWS();
	}

	mq = Z_SYSVMSG_QUEUE_P(queue);

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

/* {{{ Check whether a message queue exists */
PHP_FUNCTION(msg_queue_exists)
{
	zend_long key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &key) == FAILURE)	{
		RETURN_THROWS();
	}

	if (msgget(key, 0) < 0) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Attach to a message queue */
PHP_FUNCTION(msg_get_queue)
{
	zend_long key;
	zend_long perms = 0666;
	sysvmsg_queue_t *mq;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &key, &perms) == FAILURE)	{
		RETURN_THROWS();
	}

	object_init_ex(return_value, sysvmsg_queue_ce);
	mq = Z_SYSVMSG_QUEUE_P(return_value);

	mq->key = key;
	mq->id = msgget(key, 0);
	if (mq->id < 0)	{
		/* doesn't already exist; create it */
		mq->id = msgget(key, IPC_CREAT | IPC_EXCL | perms);
		if (mq->id < 0)	{
			php_error_docref(NULL, E_WARNING, "Failed for key 0x" ZEND_XLONG_FMT ": %s", key, strerror(errno));
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ Destroy the queue */
PHP_FUNCTION(msg_remove_queue)
{
	zval *queue;
	sysvmsg_queue_t *mq = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &queue, sysvmsg_queue_ce) == FAILURE) {
		RETURN_THROWS();
	}

	mq = Z_SYSVMSG_QUEUE_P(queue);

	if (msgctl(mq->id, IPC_RMID, NULL) == 0) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ Send a message of type msgtype (must be > 0) to a message queue */
PHP_FUNCTION(msg_receive)
{
	zval *out_message, *queue, *out_msgtype, *zerrcode = NULL;
	zend_long desiredmsgtype, maxsize, flags = 0;
	zend_long realflags = 0;
	bool do_unserialize = 1;
	sysvmsg_queue_t *mq = NULL;
	struct php_msgbuf *messagebuffer = NULL; /* buffer to transmit */
	int result;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olzlz|blz",
				&queue, sysvmsg_queue_ce, &desiredmsgtype, &out_msgtype, &maxsize,
				&out_message, &do_unserialize, &flags, &zerrcode) == FAILURE) {
		RETURN_THROWS();
	}

	if (maxsize <= 0) {
		zend_argument_value_error(4, "must be greater than 0");
		RETURN_THROWS();
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

	mq = Z_SYSVMSG_QUEUE_P(queue);

	messagebuffer = (struct php_msgbuf *) safe_emalloc(maxsize, 1, sizeof(struct php_msgbuf));

	result = msgrcv(mq->id, messagebuffer, maxsize, desiredmsgtype, realflags);

	if (result >= 0) {
		/* got it! */
		ZEND_TRY_ASSIGN_REF_LONG(out_msgtype, messagebuffer->mtype);
		if (zerrcode) {
			ZEND_TRY_ASSIGN_REF_LONG(zerrcode, 0);
		}

		RETVAL_TRUE;
		if (do_unserialize)	{
			php_unserialize_data_t var_hash;
			zval tmp;
			const unsigned char *p = (const unsigned char *) messagebuffer->mtext;

			PHP_VAR_UNSERIALIZE_INIT(var_hash);
			if (!php_var_unserialize(&tmp, &p, p + result, &var_hash)) {
				php_error_docref(NULL, E_WARNING, "Message corrupted");
				ZEND_TRY_ASSIGN_REF_FALSE(out_message);
				RETVAL_FALSE;
			} else {
				ZEND_TRY_ASSIGN_REF_TMP(out_message, &tmp);
			}
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		} else {
			ZEND_TRY_ASSIGN_REF_STRINGL(out_message, messagebuffer->mtext, result);
		}
	} else {
		ZEND_TRY_ASSIGN_REF_LONG(out_msgtype, 0);
		ZEND_TRY_ASSIGN_REF_FALSE(out_message);
		if (zerrcode) {
			ZEND_TRY_ASSIGN_REF_LONG(zerrcode, errno);
		}
	}
	efree(messagebuffer);
}
/* }}} */

/* {{{ Send a message of type msgtype (must be > 0) to a message queue */
PHP_FUNCTION(msg_send)
{
	zval *message, *queue, *zerror=NULL;
	zend_long msgtype;
	bool do_serialize = 1, blocking = 1;
	sysvmsg_queue_t * mq = NULL;
	struct php_msgbuf * messagebuffer = NULL; /* buffer to transmit */
	int result;
	size_t message_len = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olz|bbz",
				&queue, sysvmsg_queue_ce, &msgtype, &message, &do_serialize, &blocking, &zerror) == FAILURE) {
		RETURN_THROWS();
	}

	mq = Z_SYSVMSG_QUEUE_P(queue);

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
				zend_argument_type_error(3, "must be of type string|int|float|bool, %s given", zend_zval_value_name(message));
				RETURN_THROWS();
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
			ZEND_TRY_ASSIGN_REF_LONG(zerror, errno);
		}
	} else {
		RETVAL_TRUE;
	}
}
/* }}} */
