/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2015 The PHP Group                                |
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

#ifdef ZTS
#include "TSRM.h"
#endif

#ifndef pestrndup
#define pestrndup(s, length, persistent) ((persistent)?zend_strndup((s),(length)):estrndup((s),(length)))
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

void mysqlnd_upsert_status_init(MYSQLND_UPSERT_STATUS * const upsert_status);

#define UPSERT_STATUS_RESET(status)							(status)->m->reset((status))

#define UPSERT_STATUS_GET_SERVER_STATUS(status)				(status)->server_status
#define UPSERT_STATUS_SET_SERVER_STATUS(status, server_st)	(status)->server_status = (server_st)

#define UPSERT_STATUS_GET_WARNINGS(status)					(status)->warning_count
#define UPSERT_STATUS_SET_WARNINGS(status, warnings)		(status)->warning_count = (warnings)

#define UPSERT_STATUS_GET_AFFECTED_ROWS(status)				(status)->affected_rows
#define UPSERT_STATUS_SET_AFFECTED_ROWS(status, rows)		(status)->affected_rows = (rows)
#define UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(status)	(status)->m->set_affected_rows_to_error((status))

#define UPSERT_STATUS_GET_LAST_INSERT_ID(status)			(status)->last_insert_id
#define UPSERT_STATUS_SET_LAST_INSERT_ID(status, id)		(status)->last_insert_id = (id)


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


enum_func_status mysqlnd_error_info_init(MYSQLND_ERROR_INFO * const info, zend_bool persistent);

#define GET_CONNECTION_STATE(state_struct)		(state_struct)->m->get((state_struct))
#define SET_CONNECTION_STATE(state_struct, s)	(state_struct)->m->set((state_struct), (s))

void mysqlnd_connection_state_init(struct st_mysqlnd_connection_state * const state);

/* PS stuff */
typedef void (*ps_field_fetch_func)(zval * zv, const MYSQLND_FIELD * const field, unsigned int pack_len, zend_uchar ** row);
struct st_mysqlnd_perm_bind {
	ps_field_fetch_func func;
	/* should be signed int */
	int					pack_len;
	unsigned int		php_type;
	zend_bool			is_possibly_blob;
	zend_bool			can_ret_as_str_in_uni;
};

extern struct st_mysqlnd_perm_bind mysqlnd_ps_fetch_functions[MYSQL_TYPE_LAST + 1];

enum_func_status mysqlnd_stmt_fetch_row_buffered(MYSQLND_RES * result, void * param, unsigned int flags, zend_bool * fetched_anything);
enum_func_status mysqlnd_fetch_stmt_row_cursor(MYSQLND_RES * result, void * param, unsigned int flags, zend_bool * fetched_anything);


PHPAPI extern const char * const mysqlnd_old_passwd;
PHPAPI extern const char * const mysqlnd_out_of_sync;
PHPAPI extern const char * const mysqlnd_server_gone;
PHPAPI extern const char * const mysqlnd_out_of_memory;

PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_object_factory);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_conn);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_conn_data);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_res);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_result_unbuffered);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_result_buffered);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_protocol_payload_decoder_factory);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_protocol_packet_envelope_codec);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_vio);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_upsert_status);
PHPAPI extern MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(mysqlnd_error_info);

enum_func_status mysqlnd_handle_local_infile(MYSQLND_CONN_DATA * conn, const char * const filename, zend_bool * is_warning);



void _mysqlnd_init_ps_subsystem();/* This one is private, mysqlnd_library_init() will call it */
void _mysqlnd_init_ps_fetch_subsystem();

void ps_fetch_from_1_to_8_bytes(zval * zv, const MYSQLND_FIELD * const field, unsigned int pack_len, zend_uchar ** row, unsigned int byte_count);

void mysqlnd_plugin_subsystem_init(void);
void mysqlnd_plugin_subsystem_end(void);

void mysqlnd_register_builtin_authentication_plugins(void);

void mysqlnd_example_plugin_register(void);

struct st_mysqlnd_packet_greet;
struct st_mysqlnd_authentication_plugin;

enum_func_status
mysqlnd_auth_handshake(MYSQLND_CONN_DATA * conn,
						const char * const user,
						const char * const passwd,
						const size_t passwd_len,
						const char * const db,
						const size_t db_len,
						const MYSQLND_SESSION_OPTIONS * const session_options,
						zend_ulong mysql_flags,
						unsigned int server_charset_no,
						zend_bool use_full_blown_auth_packet,
						const char * const auth_protocol,
						const zend_uchar * const auth_plugin_data,
						const size_t auth_plugin_data_len,
						char ** switch_to_auth_protocol,
						size_t * switch_to_auth_protocol_len,
						zend_uchar ** switch_to_auth_protocol_data,
						size_t * switch_to_auth_protocol_data_len
						);

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
								);

extern func_mysqlnd__command_factory mysqlnd_command_factory;


enum_func_status
send_command_handle_response(
		const enum mysqlnd_packet_type ok_packet,
		const zend_bool silent,
		const enum php_mysqlnd_server_command command,
		const zend_bool ignore_upsert_status,

		MYSQLND_ERROR_INFO	* error_info,
		MYSQLND_UPSERT_STATUS * upsert_status,
		MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * payload_decoder_factory,
		MYSQLND_STRING * last_message,
		zend_bool last_message_persistent
	);

enum_func_status
mysqlnd_connect_run_authentication(
			MYSQLND_CONN_DATA * conn,
			const char * const user,
			const char * const passwd,
			const char * const db,
			size_t db_len,
			size_t passwd_len,
			MYSQLND_STRING authentication_plugin_data,
			const char * const authentication_protocol,
			const unsigned int charset_no,
			size_t server_capabilities,
			const MYSQLND_SESSION_OPTIONS * const session_options,
			zend_ulong mysql_flags
			);

#endif	/* MYSQLND_PRIV_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
