/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2008 The PHP Group                                |
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

/* $Id$ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_debug.h"
#include "ext/standard/basic_functions.h"


/* {{{ php_mysqlnd_free_field_metadata */
static
void php_mysqlnd_free_field_metadata(MYSQLND_FIELD *meta, zend_bool persistent TSRMLS_DC)
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
	}
}
/* }}} */


/* {{{ mysqlnd_handle_numeric */
/*
  The following code is stolen from ZE - HANDLE_NUMERIC() macro from zend_hash.c
  and modified for the needs of mysqlnd.
*/
static
zend_bool mysqlnd_is_key_numeric(char *key, size_t length, long *idx)
{
	register char *tmp=key;

	if (*tmp=='-') {
		tmp++;
	}
	if ((*tmp>='0' && *tmp<='9')) {
		do { /* possibly a numeric index */
			char *end=key+length-1;

			if (*tmp++=='0' && length>2) { /* don't accept numbers with leading zeros */
				break;
			}
			while (tmp<end) {
				if (!(*tmp>='0' && *tmp<='9')) {
					break;
				}
				tmp++;
			}
			if (tmp==end && *tmp=='\0') { /* a numeric index */
				if (*key=='-') {
					*idx = strtol(key, NULL, 10);
					if (*idx!=LONG_MIN) {
						return TRUE;
					}
				} else {
					*idx = strtol(key, NULL, 10);
					if (*idx!=LONG_MAX) {
						return TRUE;
					}
				}
			}
		} while (0);
	}
	return FALSE;
}
/* }}} */


#if PHP_MAJOR_VERSION >= 6
/* {{{ mysqlnd_unicode_is_key_numeric */
static
zend_bool mysqlnd_unicode_is_key_numeric(UChar *key, size_t length, long *idx)
{
	register UChar *tmp=key;

	if (*tmp==0x2D /*'-'*/) {
		tmp++;
	}
	if ((*tmp>=0x30 /*'0'*/ && *tmp<=0x39 /*'9'*/)) { /* possibly a numeric index */
		do {
			UChar *end=key+length-1;

			if (*tmp++==0x30 && length>2) { /* don't accept numbers with leading zeros */
				break;
			}
			while (tmp<end) {
				if (!(*tmp>=0x30 /*'0'*/ && *tmp<=0x39 /*'9'*/)) {
					break;
				}
				tmp++;
			}
			if (tmp==end && *tmp==0) { /* a numeric index */
				if (*key==0x2D /*'-'*/) {
					*idx = zend_u_strtol(key, NULL, 10);
					if (*idx!=LONG_MIN) {
						return TRUE;
					}
				} else {
					*idx = zend_u_strtol(key, NULL, 10);
					if (*idx!=LONG_MAX) {
						return TRUE;
					}
				}
			}
		} while (0);
	}
	return FALSE;
}
/* }}} */
#endif


/* {{{ mysqlnd_res_meta::read_metadata */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res_meta, read_metadata)(MYSQLND_RES_METADATA * const meta,
												MYSQLND *conn TSRMLS_DC)
{
	unsigned int i = 0;
	php_mysql_packet_res_field field_packet;

	DBG_ENTER("mysqlnd_res_meta::read_metadata");

	PACKET_INIT_ALLOCA(field_packet, PROT_RSET_FLD_PACKET);
	for (;i < meta->field_count; i++) {
		long idx;

		if (meta->fields[i].root) {
			/* We re-read metadata for PS */
			mnd_efree(meta->fields[i].root);
			meta->fields[i].root = NULL;
		}

		field_packet.metadata = &(meta->fields[i]);
		if (FAIL == PACKET_READ_ALLOCA(field_packet, conn)) {
			PACKET_FREE_ALLOCA(field_packet);
			DBG_RETURN(FAIL);
		}
		if (field_packet.stupid_list_fields_eof == TRUE) {
			break;
		}

		if (mysqlnd_ps_fetch_functions[meta->fields[i].type].func == NULL) {
			DBG_ERR_FMT("Unknown type %d sent by the server.  Please send a report to the developers",
						meta->fields[i].type);
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
							 "Unknown type %d sent by the server. "
							 "Please send a report to the developers",
							 meta->fields[i].type);
			PACKET_FREE_ALLOCA(field_packet);
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

#if PHP_MAJOR_VERSION >= 6
		if (UG(unicode)) {
			UChar *ustr;
			int ulen;
			zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen,
								   meta->fields[i].name,
								   meta->fields[i].name_length TSRMLS_CC);
			if ((meta->zend_hash_keys[i].is_numeric =
				 			mysqlnd_unicode_is_key_numeric(ustr, ulen + 1, &idx)))
			{
				meta->zend_hash_keys[i].key = idx;
				mnd_efree(ustr);
			} else {
				meta->zend_hash_keys[i].ustr.u = ustr;
				meta->zend_hash_keys[i].ulen = ulen;
				meta->zend_hash_keys[i].key = zend_u_get_hash_value(IS_UNICODE, ZSTR(ustr), ulen + 1);
			}

		} else 
#endif
		{
			/* For BC we have to check whether the key is numeric and use it like this */
			if ((meta->zend_hash_keys[i].is_numeric =
						mysqlnd_is_key_numeric(field_packet.metadata->name,
											   field_packet.metadata->name_length + 1,
											   &idx)))
			{
				meta->zend_hash_keys[i].key = idx;
			} else {
				meta->zend_hash_keys[i].key =
						zend_get_hash_value(field_packet.metadata->name,
											field_packet.metadata->name_length + 1);
			}
		}
	}
	PACKET_FREE_ALLOCA(field_packet);

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res_meta::free */
static void
MYSQLND_METHOD(mysqlnd_res_meta, free)(MYSQLND_RES_METADATA *meta, zend_bool persistent TSRMLS_DC)
{
	int i;
	MYSQLND_FIELD *fields;

	DBG_ENTER("mysqlnd_res_meta::free");
	DBG_INF_FMT("persistent=%d", persistent);

	if ((fields = meta->fields)) {
		DBG_INF("Freeing fields metadata");
		i = meta->field_count;
		while (i--) {
			php_mysqlnd_free_field_metadata(fields++, persistent TSRMLS_CC);
		}
		mnd_pefree(meta->fields, persistent);
		meta->fields = NULL;
	}

	if (meta->zend_hash_keys) {
		DBG_INF("Freeing zend_hash_keys");
#if PHP_MAJOR_VERSION >= 6
		if (UG(unicode)) {
			for (i = 0; i < meta->field_count; i++) {
				if (meta->zend_hash_keys[i].ustr.v) {
					mnd_pefree(meta->zend_hash_keys[i].ustr.v, persistent);
				}
			}
		}
#endif
		mnd_pefree(meta->zend_hash_keys, persistent);
		meta->zend_hash_keys = NULL;
	}
	DBG_INF("Freeing metadata structure");
	mnd_pefree(meta, persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::clone_metadata */
static MYSQLND_RES_METADATA *
MYSQLND_METHOD(mysqlnd_res_meta, clone_metadata)(const MYSQLND_RES_METADATA * const meta,
												 zend_bool persistent TSRMLS_DC)
{
	unsigned int i;
	/* +1 is to have empty marker at the end */
	MYSQLND_RES_METADATA *new_meta = mnd_pemalloc(sizeof(MYSQLND_RES_METADATA), persistent);
	MYSQLND_FIELD *new_fields = mnd_pecalloc(meta->field_count + 1, sizeof(MYSQLND_FIELD), persistent);
	MYSQLND_FIELD *orig_fields = meta->fields;
	size_t len = meta->field_count * sizeof(struct mysqlnd_field_hash_key);

	DBG_ENTER("mysqlnd_res_meta::clone_metadata");
	DBG_INF_FMT("persistent=%d", persistent);

	new_meta->zend_hash_keys = mnd_pemalloc(len, persistent);
	memcpy(new_meta->zend_hash_keys, meta->zend_hash_keys, len);
	new_meta->m = meta->m;

	/*
	  This will copy also the strings and the root, which we will have
	  to adjust in the loop
	*/
	memcpy(new_fields, orig_fields, (meta->field_count) * sizeof(MYSQLND_FIELD));
	for (i = 0; i < meta->field_count; i++) {
		/* First copy the root, then field by field adjust the pointers */
		new_fields[i].root = mnd_pemalloc(orig_fields[i].root_len, persistent);
		memcpy(new_fields[i].root, orig_fields[i].root, new_fields[i].root_len);

		if (orig_fields[i].name && orig_fields[i].name != mysqlnd_empty_string) {
			new_fields[i].name = new_fields[i].root +
								 (orig_fields[i].name - orig_fields[i].root);
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
			/* copy the trailing \0 too */
			memcpy(new_fields[i].def, orig_fields[i].def, orig_fields[i].def_length + 1);
		}
#if PHP_MAJOR_VERSION >= 6
		if (new_meta->zend_hash_keys[i].ustr.u) {
			new_meta->zend_hash_keys[i].ustr.u =
					eustrndup(new_meta->zend_hash_keys[i].ustr.u, new_meta->zend_hash_keys[i].ulen);
		}
#endif
	}
	new_meta->current_field = 0;
	new_meta->field_count = meta->field_count;

	new_meta->fields = new_fields;

	DBG_RETURN(new_meta);
}
/* }}} */

/* {{{ mysqlnd_res_meta::fetch_field */
static MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res_meta, fetch_field)(MYSQLND_RES_METADATA * const meta TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res_meta::fetch_field");
	if (meta->current_field >= meta->field_count) {
		DBG_RETURN(NULL);
	}
	DBG_RETURN(&meta->fields[meta->current_field++]);
}
/* }}} */


/* {{{ mysqlnd_res_meta::fetch_field_direct */
static MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res_meta, fetch_field_direct)(const MYSQLND_RES_METADATA * const meta,
													 MYSQLND_FIELD_OFFSET fieldnr TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res_meta::fetch_field_direct");
	DBG_INF_FMT("fieldnr=%d", fieldnr);
	DBG_RETURN(&meta->fields[fieldnr]);
}
/* }}} */


/* {{{ mysqlnd_res_meta::field_tell */
static MYSQLND_FIELD_OFFSET
MYSQLND_METHOD(mysqlnd_res_meta, field_tell)(const MYSQLND_RES_METADATA * const meta)
{
	return meta->current_field;
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_res_meta)
	MYSQLND_METHOD(mysqlnd_res_meta, fetch_field),
	MYSQLND_METHOD(mysqlnd_res_meta, fetch_field_direct),
	MYSQLND_METHOD(mysqlnd_res_meta, field_tell),
	MYSQLND_METHOD(mysqlnd_res_meta, read_metadata),
	MYSQLND_METHOD(mysqlnd_res_meta, clone_metadata),
	MYSQLND_METHOD(mysqlnd_res_meta, free),
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_result_meta_init */
MYSQLND_RES_METADATA *mysqlnd_result_meta_init(unsigned int field_count TSRMLS_DC)
{
	MYSQLND_RES_METADATA *ret;
	DBG_ENTER("mysqlnd_result_meta_init");

	/* +1 is to have empty marker at the end */
	ret = mnd_ecalloc(1, sizeof(MYSQLND_RES_METADATA));
	ret->field_count = field_count;
	ret->fields = mnd_ecalloc(field_count + 1, sizeof(MYSQLND_FIELD));
	ret->zend_hash_keys = mnd_ecalloc(field_count, sizeof(struct mysqlnd_field_hash_key));

	ret->m = & mysqlnd_mysqlnd_res_meta_methods;
	DBG_INF_FMT("meta=%p", ret);
	DBG_RETURN(ret);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
