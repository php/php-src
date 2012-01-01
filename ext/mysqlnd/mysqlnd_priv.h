/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  |          Georg Richter <georg@mysql.com>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef MYSQLND_PRIV_H
#define MYSQLND_PRIV_H

#ifndef Z_ADDREF_P
/* PHP 5.2, old GC */
#define Z_ADDREF_P(pz)				(++(pz)->refcount)
#define Z_DELREF_P(pz)				(--(pz)->refcount)
#define Z_REFCOUNT_P(pz)			((pz)->refcount)
#define Z_SET_REFCOUNT_P(pz, rc)	((pz)->refcount = rc)
#define Z_REFCOUNT_PP(ppz)			Z_REFCOUNT_P(*(ppz))
#define Z_DELREF_PP(ppz)			Z_DELREF_P(*(ppz))
#endif

#if PHP_MAJOR_VERSION >= 6
#define MYSQLND_UNICODE 1
#else
#define MYSQLND_UNICODE 0
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifndef pestrndup
#define pestrndup(s, length, persistent) ((persistent)?zend_strndup((s),(length)):estrndup((s),(length)))
#endif

#if MYSQLND_UNICODE
#define mysqlnd_array_init(arg, field_count) \
{ \
	ALLOC_HASHTABLE_REL(Z_ARRVAL_P(arg));\
	zend_u_hash_init(Z_ARRVAL_P(arg), (field_count), NULL, ZVAL_PTR_DTOR, 0, 0);\
	Z_TYPE_P(arg) = IS_ARRAY;\
}
#else
#define mysqlnd_array_init(arg, field_count) \
{ \
	ALLOC_HASHTABLE_REL(Z_ARRVAL_P(arg));\
	zend_hash_init(Z_ARRVAL_P(arg), (field_count), NULL, ZVAL_PTR_DTOR, 0); \
	Z_TYPE_P(arg) = IS_ARRAY;\
}
#endif

#define MYSQLND_STR_W_LEN(str)  str, (sizeof(str) - 1)

#define MYSQLND_DEBUG_DUMP_TIME				1
#define MYSQLND_DEBUG_DUMP_TRACE			2
#define MYSQLND_DEBUG_DUMP_PID				4
#define MYSQLND_DEBUG_DUMP_LINE				8
#define MYSQLND_DEBUG_DUMP_FILE				16
#define MYSQLND_DEBUG_DUMP_LEVEL			32
#define MYSQLND_DEBUG_APPEND				64
#define MYSQLND_DEBUG_FLUSH					128
#define MYSQLND_DEBUG_TRACE_MEMORY_CALLS	256
#define MYSQLND_DEBUG_PROFILE_CALLS			512


/* Client Error codes */
#define CR_UNKNOWN_ERROR		2000
#define CR_CONNECTION_ERROR		2002
#define CR_SERVER_GONE_ERROR	2006
#define CR_OUT_OF_MEMORY		2008
#define CR_SERVER_LOST			2013
#define CR_COMMANDS_OUT_OF_SYNC	2014
#define CR_CANT_FIND_CHARSET	2019
#define CR_MALFORMED_PACKET		2027
#define CR_NOT_IMPLEMENTED		2054
#define CR_NO_PREPARE_STMT		2030
#define CR_PARAMS_NOT_BOUND		2031
#define CR_INVALID_PARAMETER_NO	2034
#define CR_INVALID_BUFFER_USE	2035

#define MYSQLND_EE_FILENOTFOUND	 7890

#define UNKNOWN_SQLSTATE		"HY000"

#define MAX_CHARSET_LEN			32


#define SET_ERROR_AFF_ROWS(s)	(s)->upsert_status->affected_rows = (uint64_t) ~0

/* Error handling */
#define SET_NEW_MESSAGE(buf, buf_len, message, len, persistent) \
	{\
		if ((buf)) { \
			mnd_pefree((buf), (persistent)); \
		} \
		if ((message)) { \
			(buf) = mnd_pestrndup((message), (len), (persistent)); \
		} else { \
			(buf) = NULL; \
		} \
		(buf_len) = (len); \
	}

#define SET_EMPTY_MESSAGE(buf, buf_len, persistent) \
	{\
		if ((buf)) { \
			mnd_pefree((buf), (persistent)); \
			(buf) = NULL; \
		} \
		(buf_len) = 0; \
	}


#define SET_EMPTY_ERROR(error_info) \
	{ \
		(error_info).error_no = 0; \
		(error_info).error[0] = '\0'; \
		strlcpy((error_info).sqlstate, "00000", sizeof((error_info).sqlstate)); \
		if ((error_info).error_list) { \
			zend_llist_clean((error_info).error_list); \
		} \
	}


#define SET_CLIENT_ERROR(error_info, a, b, c) \
{ \
	if (0 == (a)) { \
		SET_EMPTY_ERROR((error_info)); \
	} else { \
		(error_info).error_no = (a); \
		strlcpy((error_info).sqlstate, (b), sizeof((error_info).sqlstate)); \
		strlcpy((error_info).error, (c), sizeof((error_info).error)); \
		if ((error_info).error_list) {\
			MYSQLND_ERROR_LIST_ELEMENT error_for_the_list = {0}; \
																	\
			error_for_the_list.error_no = (a); \
			strlcpy(error_for_the_list.sqlstate, (b), sizeof(error_for_the_list.sqlstate)); \
			error_for_the_list.error = mnd_pestrdup((c), TRUE); \
			if (error_for_the_list.error) { \
				DBG_INF_FMT("adding error [%s] to the list", error_for_the_list.error); \
				zend_llist_add_element((error_info).error_list, &error_for_the_list); \
			} \
		} \
	} \
}


#define COPY_CLIENT_ERROR(error_info_to, error_info_from) \
	{ \
		SET_CLIENT_ERROR((error_info_to), (error_info_from).error_no, (error_info_from).sqlstate, (error_info_from).error); \
	}


#define SET_OOM_ERROR(error_info) SET_CLIENT_ERROR((error_info), CR_OUT_OF_MEMORY, UNKNOWN_SQLSTATE, mysqlnd_out_of_memory)


#define SET_STMT_ERROR(stmt, a, b, c)	SET_CLIENT_ERROR(*(stmt)->error_info, a, b, c)

#define CONN_GET_STATE(c)		(c)->m->get_state((c) TSRMLS_CC)
#define CONN_SET_STATE(c, s)	(c)->m->set_state((c), (s) TSRMLS_CC)

/* PS stuff */
typedef void (*ps_field_fetch_func)(zval *zv, const MYSQLND_FIELD * const field,
									unsigned int pack_len, zend_uchar **row,
									zend_bool everything_as_unicode TSRMLS_DC);
struct st_mysqlnd_perm_bind {
	ps_field_fetch_func func;
	/* should be signed int */
	int					pack_len;
	unsigned int		php_type;
	zend_bool			is_possibly_blob;
	zend_bool			can_ret_as_str_in_uni;
};

extern struct st_mysqlnd_perm_bind mysqlnd_ps_fetch_functions[MYSQL_TYPE_LAST + 1];

PHPAPI extern const char * const mysqlnd_old_passwd;
PHPAPI extern const char * const mysqlnd_out_of_sync;
PHPAPI extern const char * const mysqlnd_server_gone;
PHPAPI extern const char * const mysqlnd_out_of_memory;

extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_object_factory);
extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_conn);
extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_conn_data);
extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_res);
extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_protocol);
extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_net);

enum_func_status mysqlnd_handle_local_infile(MYSQLND_CONN_DATA * conn, const char *filename, zend_bool *is_warning TSRMLS_DC);



void _mysqlnd_init_ps_subsystem();/* This one is private, mysqlnd_library_init() will call it */
void _mysqlnd_init_ps_fetch_subsystem();

void ps_fetch_from_1_to_8_bytes(zval *zv, const MYSQLND_FIELD * const field,
								unsigned int pack_len, zend_uchar **row, zend_bool as_unicode,
								unsigned int byte_count TSRMLS_DC);

void mysqlnd_plugin_subsystem_init(TSRMLS_D);
void mysqlnd_plugin_subsystem_end(TSRMLS_D);

void mysqlnd_register_builtin_authentication_plugins(TSRMLS_D);

void mysqlnd_example_plugin_register(TSRMLS_D);

struct st_mysqlnd_packet_greet;
struct st_mysqlnd_authentication_plugin;

enum_func_status
mysqlnd_auth_handshake(MYSQLND_CONN_DATA * conn,
						const char * const user,
						const char * const passwd,
						const size_t passwd_len,
						const char * const db,
						const size_t db_len,
						const MYSQLND_OPTIONS * const options,
						unsigned long mysql_flags,
						unsigned int server_charset_no,
						zend_bool use_full_blown_auth_packet,
						const char * const auth_protocol,
						const zend_uchar * const auth_plugin_data,
						const size_t auth_plugin_data_len,
						char ** switch_to_auth_protocol,
						size_t * switch_to_auth_protocol_len,
						zend_uchar ** switch_to_auth_protocol_data,
						size_t * switch_to_auth_protocol_data_len
						TSRMLS_DC);

enum_func_status
mysqlnd_auth_change_user(MYSQLND_CONN_DATA * const conn,
								const char * const user,
								const size_t user_len,
								const char * const passwd,
								const size_t passwd_len,
								const char * const db,
								const size_t db_len,
								const zend_bool silent,
								zend_bool use_full_blown_auth_packet,
								const char * const auth_protocol,
								zend_uchar * auth_plugin_data,
								size_t auth_plugin_data_len,
								char ** switch_to_auth_protocol,
								size_t * switch_to_auth_protocol_len,
								zend_uchar ** switch_to_auth_protocol_data,
								size_t * switch_to_auth_protocol_data_len
								TSRMLS_DC);

#endif	/* MYSQLND_PRIV_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
