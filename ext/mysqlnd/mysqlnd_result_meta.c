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

/* $Id$ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_debug.h"
#include "ext/standard/basic_functions.h"


/* {{{ php_mysqlnd_free_field_metadata */
static void
php_mysqlnd_free_field_metadata(MYSQLND_FIELD *meta, zend_bool persistent)
{
	if (meta) {
		if (meta->root) {
			mnd_pefree(meta->root, persistent);
			meta->root = NULL;
		}
		if (meta->def) {
			mnd_pefree(meta->def, persistent);
			meta->def = NULL;
		}
		if (meta->sname) {
			zend_string_release(meta->sname);
		}
	}
}
/* }}} */

/* {{{ mysqlnd_res_meta::read_metadata */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res_meta, read_metadata)(MYSQLND_RES_METADATA * const meta, MYSQLND_CONN_DATA * conn)
{
	unsigned int i = 0;
	MYSQLND_PACKET_RES_FIELD * field_packet;

	DBG_ENTER("mysqlnd_res_meta::read_metadata");

	field_packet = conn->protocol->m.get_result_field_packet(conn->protocol, FALSE);
	if (!field_packet) {
		SET_OOM_ERROR(*conn->error_info);
		DBG_RETURN(FAIL);
	}
	field_packet->persistent_alloc = meta->persistent;
	for (;i < meta->field_count; i++) {
		zend_ulong idx;

		if (meta->fields[i].root) {
			/* We re-read metadata for PS */
			mnd_pefree(meta->fields[i].root, meta->persistent);
			meta->fields[i].root = NULL;
		}

		field_packet->metadata = &(meta->fields[i]);
		if (FAIL == PACKET_READ(field_packet, conn)) {
			PACKET_FREE(field_packet);
			DBG_RETURN(FAIL);
		}
		if (field_packet->error_info.error_no) {
			COPY_CLIENT_ERROR(*conn->error_info, field_packet->error_info);
			/* Return back from CONN_QUERY_SENT */
			PACKET_FREE(field_packet);
			DBG_RETURN(FAIL);
		}

		if (field_packet->stupid_list_fields_eof == TRUE) {
			meta->field_count = i;
			break;
		}

		if (mysqlnd_ps_fetch_functions[meta->fields[i].type].func == NULL) {
			DBG_ERR_FMT("Unknown type %u sent by the server.  Please send a report to the developers",
						meta->fields[i].type);
			php_error_docref(NULL, E_WARNING,
							 "Unknown type %u sent by the server. "
							 "Please send a report to the developers",
							 meta->fields[i].type);
			PACKET_FREE(field_packet);
			DBG_RETURN(FAIL);
		}
		if (meta->fields[i].type == MYSQL_TYPE_BIT) {
			size_t field_len;
			DBG_INF("BIT");
			++meta->bit_fields_count;
			/* .length is in bits */
			field_len = meta->fields[i].length / 8;
			/*
			  If there is rest, add one byte :
			  8 bits = 1 byte but 9 bits = 2 bytes
			*/
			if (meta->fields[i].length % 8) {
				++field_len;
			}
			switch (field_len) {
				case 8:
				case 7:
				case 6:
				case 5:
					meta->bit_fields_total_len += 20;/* 21 digis, no sign*/
					break;
				case 4:
					meta->bit_fields_total_len += 10;/* 2 000 000 000*/
					break;
				case 3:
					meta->bit_fields_total_len += 8;/*  12 000 000*/
					break;
				case 2:
					meta->bit_fields_total_len += 5;/* 32 500 */
					break;
				case 1:
					meta->bit_fields_total_len += 3;/* 120 */
					break;
			}
		}

		/* For BC we have to check whether the key is numeric and use it like this */
		if ((meta->zend_hash_keys[i].is_numeric = ZEND_HANDLE_NUMERIC(field_packet->metadata->sname, idx))) {
			meta->zend_hash_keys[i].key = idx;
		}
	}
	PACKET_FREE(field_packet);

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res_meta::free */
static void
MYSQLND_METHOD(mysqlnd_res_meta, free)(MYSQLND_RES_METADATA * meta)
{
	int i;
	MYSQLND_FIELD *fields;
	DBG_ENTER("mysqlnd_res_meta::free");
	DBG_INF_FMT("persistent=%u", meta->persistent);

	if ((fields = meta->fields)) {
		DBG_INF("Freeing fields metadata");
		i = meta->field_count;
		while (i--) {
			php_mysqlnd_free_field_metadata(fields++, meta->persistent);
		}
		mnd_pefree(meta->fields, meta->persistent);
		meta->fields = NULL;
	}

	if (meta->zend_hash_keys) {
		DBG_INF("Freeing zend_hash_keys");
		mnd_pefree(meta->zend_hash_keys, meta->persistent);
		meta->zend_hash_keys = NULL;
	}
	DBG_INF("Freeing metadata structure");
	mnd_pefree(meta, meta->persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::clone_metadata */
static MYSQLND_RES_METADATA *
MYSQLND_METHOD(mysqlnd_res_meta, clone_metadata)(const MYSQLND_RES_METADATA * const meta, zend_bool persistent)
{
	unsigned int i;
	/* +1 is to have empty marker at the end */
	MYSQLND_RES_METADATA * new_meta = NULL;
	MYSQLND_FIELD * new_fields;
	MYSQLND_FIELD * orig_fields = meta->fields;
	size_t len = meta->field_count * sizeof(struct mysqlnd_field_hash_key);

	DBG_ENTER("mysqlnd_res_meta::clone_metadata");
	DBG_INF_FMT("persistent=%u", persistent);

	new_meta = mnd_pecalloc(1, sizeof(MYSQLND_RES_METADATA), persistent);
	if (!new_meta) {
		goto oom;
	}
	new_meta->persistent = persistent;
	new_meta->m = meta->m;

	new_fields = mnd_pecalloc(meta->field_count + 1, sizeof(MYSQLND_FIELD), persistent);
	if (!new_fields) {
		goto oom;
	}

	new_meta->zend_hash_keys = mnd_pemalloc(len, persistent);
	if (!new_meta->zend_hash_keys) {
		goto oom;
	}
	memcpy(new_meta->zend_hash_keys, meta->zend_hash_keys, len);

	/*
	  This will copy also the strings and the root, which we will have
	  to adjust in the loop
	*/
	memcpy(new_fields, orig_fields, (meta->field_count) * sizeof(MYSQLND_FIELD));
	for (i = 0; i < meta->field_count; i++) {
		/* First copy the root, then field by field adjust the pointers */
		new_fields[i].root = mnd_pemalloc(orig_fields[i].root_len, persistent);

		if (!new_fields[i].root) {
			goto oom;
		}

		memcpy(new_fields[i].root, orig_fields[i].root, new_fields[i].root_len);

		if (orig_fields[i].sname) {
			new_fields[i].sname = zend_string_copy(orig_fields[i].sname);
			new_fields[i].name = ZSTR_VAL(new_fields[i].sname);
			new_fields[i].name_length = ZSTR_LEN(new_fields[i].sname);
		}

		if (orig_fields[i].org_name && orig_fields[i].org_name != mysqlnd_empty_string) {
			new_fields[i].org_name = new_fields[i].root +
				(orig_fields[i].org_name - orig_fields[i].root);
		}
		if (orig_fields[i].table && orig_fields[i].table != mysqlnd_empty_string) {
			new_fields[i].table	= new_fields[i].root +
				(orig_fields[i].table - orig_fields[i].root);
		}
		if (orig_fields[i].org_table && orig_fields[i].org_table != mysqlnd_empty_string) {
			new_fields[i].org_table	= new_fields[i].root +
				(orig_fields[i].org_table - orig_fields[i].root);
		}
		if (orig_fields[i].db && orig_fields[i].db != mysqlnd_empty_string) {
			new_fields[i].db = new_fields[i].root + (orig_fields[i].db - orig_fields[i].root);
		}
		if (orig_fields[i].catalog && orig_fields[i].catalog != mysqlnd_empty_string) {
			new_fields[i].catalog = new_fields[i].root + (orig_fields[i].catalog - orig_fields[i].root);
		}
		/* def is not on the root, if allocated at all */
		if (orig_fields[i].def) {
			new_fields[i].def = mnd_pemalloc(orig_fields[i].def_length + 1, persistent);
			if (!new_fields[i].def) {
				goto oom;
			}
			/* copy the trailing \0 too */
			memcpy(new_fields[i].def, orig_fields[i].def, orig_fields[i].def_length + 1);
		}
	}
	new_meta->current_field = 0;
	new_meta->field_count = meta->field_count;

	new_meta->fields = new_fields;

	DBG_RETURN(new_meta);
oom:
	if (new_meta) {
		new_meta->m->free_metadata(new_meta);
		new_meta = NULL;
	}
	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_res_meta::fetch_field */
static const MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res_meta, fetch_field)(MYSQLND_RES_METADATA * const meta)
{
	DBG_ENTER("mysqlnd_res_meta::fetch_field");
	if (meta->current_field >= meta->field_count) {
		DBG_INF("no more fields");
		DBG_RETURN(NULL);
	}
	DBG_INF_FMT("name=%s max_length=%u",
		meta->fields[meta->current_field].name? meta->fields[meta->current_field].name:"",
		meta->fields[meta->current_field].max_length);
	DBG_RETURN(&meta->fields[meta->current_field++]);
}
/* }}} */


/* {{{ mysqlnd_res_meta::fetch_field_direct */
static const MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res_meta, fetch_field_direct)(const MYSQLND_RES_METADATA * const meta, const MYSQLND_FIELD_OFFSET fieldnr)
{
	DBG_ENTER("mysqlnd_res_meta::fetch_field_direct");
	DBG_INF_FMT("fieldnr=%u", fieldnr);
	DBG_INF_FMT("name=%s max_length=%u",
		meta->fields[meta->current_field].name? meta->fields[meta->current_field].name:"",
		meta->fields[meta->current_field].max_length);
	DBG_RETURN(&meta->fields[fieldnr]);
}
/* }}} */


/* {{{ mysqlnd_res_meta::fetch_fields */
static const MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res_meta, fetch_fields)(MYSQLND_RES_METADATA * const meta)
{
	DBG_ENTER("mysqlnd_res_meta::fetch_fields");
	DBG_RETURN(meta->fields);
}
/* }}} */


/* {{{ mysqlnd_res_meta::field_tell */
static MYSQLND_FIELD_OFFSET
MYSQLND_METHOD(mysqlnd_res_meta, field_tell)(const MYSQLND_RES_METADATA * const meta)
{
	return meta->current_field;
}
/* }}} */


/* {{{ mysqlnd_res_meta::field_seek */
static MYSQLND_FIELD_OFFSET
MYSQLND_METHOD(mysqlnd_res_meta, field_seek)(MYSQLND_RES_METADATA * const meta, const MYSQLND_FIELD_OFFSET field_offset)
{
	MYSQLND_FIELD_OFFSET return_value = 0;
	DBG_ENTER("mysqlnd_res_meta::fetch_fields");
	return_value = meta->current_field;
	meta->current_field = field_offset;
	DBG_RETURN(return_value);
}
/* }}} */

static
MYSQLND_CLASS_METHODS_START(mysqlnd_res_meta)
	MYSQLND_METHOD(mysqlnd_res_meta, fetch_field),
	MYSQLND_METHOD(mysqlnd_res_meta, fetch_field_direct),
	MYSQLND_METHOD(mysqlnd_res_meta, fetch_fields),
	MYSQLND_METHOD(mysqlnd_res_meta, field_tell),
	MYSQLND_METHOD(mysqlnd_res_meta, field_seek),
	MYSQLND_METHOD(mysqlnd_res_meta, read_metadata),
	MYSQLND_METHOD(mysqlnd_res_meta, clone_metadata),
	MYSQLND_METHOD(mysqlnd_res_meta, free),
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_result_meta_init */
PHPAPI MYSQLND_RES_METADATA *
mysqlnd_result_meta_init(unsigned int field_count, zend_bool persistent)
{
	size_t alloc_size = sizeof(MYSQLND_RES_METADATA) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_RES_METADATA *ret = mnd_pecalloc(1, alloc_size, persistent);
	DBG_ENTER("mysqlnd_result_meta_init");
	DBG_INF_FMT("persistent=%u", persistent);

	do {
		if (!ret) {
			break;
		}
		ret->m = & mysqlnd_mysqlnd_res_meta_methods;

		ret->persistent = persistent;
		ret->field_count = field_count;
		/* +1 is to have empty marker at the end */
		ret->fields = mnd_pecalloc(field_count + 1, sizeof(MYSQLND_FIELD), ret->persistent);
		ret->zend_hash_keys = mnd_pecalloc(field_count, sizeof(struct mysqlnd_field_hash_key), ret->persistent);
		if (!ret->fields || !ret->zend_hash_keys) {
			break;
		}
		DBG_INF_FMT("meta=%p", ret);
		DBG_RETURN(ret);
	} while (0);
	if (ret) {
		ret->m->free_metadata(ret);
	}
	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_res_meta_get_methods */
PHPAPI struct st_mysqlnd_res_meta_methods *
mysqlnd_result_metadata_get_methods()
{
	return &mysqlnd_mysqlnd_res_meta_methods;
}
/* }}} */


/* {{{ _mysqlnd_plugin_get_plugin_result_metadata_data */
PHPAPI void **
_mysqlnd_plugin_get_plugin_result_metadata_data(const MYSQLND_RES_METADATA * meta, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_result_metadata_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!meta || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)meta + sizeof(MYSQLND_RES_METADATA) + plugin_id * sizeof(void *)));
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
