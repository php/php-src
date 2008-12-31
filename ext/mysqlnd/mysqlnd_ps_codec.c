/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2009 The PHP Group                                |
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
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"

#define MYSQLND_SILENT


enum mysqlnd_timestamp_type
{
  MYSQLND_TIMESTAMP_NONE= -2,
  MYSQLND_TIMESTAMP_ERROR= -1,
  MYSQLND_TIMESTAMP_DATE= 0,
  MYSQLND_TIMESTAMP_DATETIME= 1,
  MYSQLND_TIMESTAMP_TIME= 2
};


struct st_mysqlnd_time
{
  unsigned int  year, month, day, hour, minute, second;
  unsigned long second_part;
  zend_bool     neg;
  enum mysqlnd_timestamp_type time_type;
};


struct st_mysqlnd_perm_bind mysqlnd_ps_fetch_functions[MYSQL_TYPE_LAST + 1];

#define MYSQLND_PS_SKIP_RESULT_W_LEN	-1
#define MYSQLND_PS_SKIP_RESULT_STR		-2

/* {{{ ps_fetch_from_1_to_8_bytes */
void ps_fetch_from_1_to_8_bytes(zval *zv, const MYSQLND_FIELD * const field,
								unsigned int pack_len, zend_uchar **row, zend_bool as_unicode,
								unsigned int byte_count TSRMLS_DC)
{
	char tmp[22];
	size_t tmp_len = 0;
	zend_bool is_bit = field->type == MYSQL_TYPE_BIT;
	DBG_ENTER("ps_fetch_from_1_to_8_bytes");
	DBG_INF_FMT("zv=%p byte_count=%d", zv, byte_count);
	if (field->flags & UNSIGNED_FLAG) {
		uint64_t uval = 0;

		switch (byte_count) {
			case 8:uval = is_bit? (uint64_t) bit_uint8korr(*row):(uint64_t) uint8korr(*row);break;
			case 7:uval = bit_uint7korr(*row);break;
			case 6:uval = bit_uint6korr(*row);break;
			case 5:uval = bit_uint5korr(*row);break;
			case 4:uval = is_bit? (uint64_t) bit_uint4korr(*row):(uint64_t) uint4korr(*row);break;
			case 3:uval = is_bit? (uint64_t) bit_uint3korr(*row):(uint64_t) uint3korr(*row);break;
			case 2:uval = is_bit? (uint64_t) bit_uint2korr(*row):(uint64_t) uint2korr(*row);break;
			case 1:uval = (uint64_t) uint1korr(*row);break;
		}

#if SIZEOF_LONG==4
		if (uval > INT_MAX) {
			DBG_INF("stringify");
			tmp_len = sprintf((char *)&tmp, MYSQLND_LLU_SPEC, uval);
		} else 
#endif /* #if SIZEOF_LONG==4 */
		{
			if (byte_count < 8 || uval <= L64(9223372036854775807)) {
				ZVAL_LONG(zv, uval);
			} else {
				DBG_INF("stringify");
				tmp_len = sprintf((char *)&tmp, MYSQLND_LLU_SPEC, uval);
			}
		}
	} else {
		/* SIGNED */
		int64_t lval = 0;
		switch (byte_count) {
			case 8:lval = (int64_t) sint8korr(*row);break;
			/*
			  7, 6 and 5 are not possible.
			  BIT is only unsigned, thus only uint5|6|7 macroses exist
			*/
			case 4:lval = (int64_t) sint4korr(*row);break;
			case 3:lval = (int64_t) sint3korr(*row);break;
			case 2:lval = (int64_t) sint2korr(*row);break;
			case 1:lval = (int64_t) *(int8_t*)*row;break;
		}

#if SIZEOF_LONG==4
	    if ((L64(2147483647) < (int64_t) lval) || (L64(-2147483648) > (int64_t) lval)) {
			DBG_INF("stringify");
			tmp_len = sprintf((char *)&tmp, MYSQLND_LL_SPEC, lval);
		} else 
#endif /* SIZEOF */
		{
			ZVAL_LONG(zv, lval);
		}
	}

	if (tmp_len) {
#if PHP_MAJOR_VERSION >= 6
		if (as_unicode) {
			DBG_INF("stringify");
			ZVAL_UTF8_STRINGL(zv, tmp, tmp_len, ZSTR_DUPLICATE);
		} else
#endif
		{
			DBG_INF("stringify");
			ZVAL_STRINGL(zv, tmp, tmp_len, 1);
		}			
	}
	(*row)+= byte_count;
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ ps_fetch_null */
static
void ps_fetch_null(zval *zv, const MYSQLND_FIELD * const field,
				   unsigned int pack_len, zend_uchar **row,
				   zend_bool as_unicode TSRMLS_DC)
{
	ZVAL_NULL(zv);
}
/* }}} */


/* {{{ ps_fetch_int8 */
static
void ps_fetch_int8(zval *zv, const MYSQLND_FIELD * const field,
				   unsigned int pack_len, zend_uchar **row,
				   zend_bool as_unicode TSRMLS_DC)
{
	ps_fetch_from_1_to_8_bytes(zv, field, pack_len, row, as_unicode, 1 TSRMLS_CC);
}
/* }}} */


/* {{{ ps_fetch_int16 */
static
void ps_fetch_int16(zval *zv, const MYSQLND_FIELD * const field,
					unsigned int pack_len, zend_uchar **row,
					zend_bool as_unicode TSRMLS_DC)
{
	ps_fetch_from_1_to_8_bytes(zv, field, pack_len, row, as_unicode, 2 TSRMLS_CC);
}
/* }}} */


/* {{{ ps_fetch_int32 */
static
void ps_fetch_int32(zval *zv, const MYSQLND_FIELD * const field,
					unsigned int pack_len, zend_uchar **row,
					zend_bool as_unicode TSRMLS_DC)
{
	ps_fetch_from_1_to_8_bytes(zv, field, pack_len, row, as_unicode, 4 TSRMLS_CC);
}
/* }}} */


/* {{{ ps_fetch_int64 */
static
void ps_fetch_int64(zval *zv, const MYSQLND_FIELD * const field,
					unsigned int pack_len, zend_uchar **row,
					zend_bool as_unicode TSRMLS_DC)
{
	ps_fetch_from_1_to_8_bytes(zv, field, pack_len, row, as_unicode, 8 TSRMLS_CC);
}
/* }}} */


/* {{{ ps_fetch_float */
static
void ps_fetch_float(zval *zv, const MYSQLND_FIELD * const field,
					unsigned int pack_len, zend_uchar **row,
					zend_bool as_unicode TSRMLS_DC)
{
	float value;
	DBG_ENTER("ps_fetch_float");
	float4get(value, *row);
	ZVAL_DOUBLE(zv, value);
  	(*row)+= 4;
	DBG_INF_FMT("value=%f", value);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ ps_fetch_double */
static
void ps_fetch_double(zval *zv, const MYSQLND_FIELD * const field,
					unsigned int pack_len, zend_uchar **row,
					zend_bool as_unicode TSRMLS_DC)
{
	double value;
	DBG_ENTER("ps_fetch_double");
	float8get(value, *row);
	ZVAL_DOUBLE(zv, value);
	(*row)+= 8;
	DBG_INF_FMT("value=%f", value);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ ps_fetch_time */
static
void ps_fetch_time(zval *zv, const MYSQLND_FIELD * const field,
				   unsigned int pack_len, zend_uchar **row,
				   zend_bool as_unicode TSRMLS_DC)
{
	struct st_mysqlnd_time t;
	unsigned int length; /* First byte encodes the length*/
	char *to;
	DBG_ENTER("ps_fetch_time");

	if ((length = php_mysqlnd_net_field_length(row))) {
		zend_uchar *to= *row;

		t.time_type = MYSQLND_TIMESTAMP_TIME;
		t.neg			= (zend_bool) to[0];

		t.day			= (unsigned long) sint4korr(to+1);
		t.hour			= (unsigned int) to[5];
		t.minute		= (unsigned int) to[6];
		t.second		= (unsigned int) to[7];
		t.second_part	= (length > 8) ? (unsigned long) sint4korr(to+8) : 0;
		t.year			= t.month= 0;
		if (t.day) {
			/* Convert days to hours at once */
			t.hour += t.day*24;
			t.day	= 0;
		}

		(*row) += length;
	} else {
		memset(&t, 0, sizeof(t));
		t.time_type = MYSQLND_TIMESTAMP_TIME;
 	}

	/*
	  QQ : How to make this unicode without copying two times the buffer -
	  Unicode equivalent of spprintf?
	*/
	length = spprintf(&to, 0, "%s%02u:%02u:%02u",
					 (t.neg ? "-" : ""), t.hour, t.minute, t.second);

	DBG_INF_FMT("%s", to);
#if PHP_MAJOR_VERSION >= 6
	if (!as_unicode) {
#endif
		ZVAL_STRINGL(zv, to, length, 1);
		mnd_efree(to);
#if PHP_MAJOR_VERSION >= 6
	} else {
		ZVAL_UTF8_STRINGL(zv, to, length, ZSTR_AUTOFREE);	
	}
#endif
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ ps_fetch_date */
static
void ps_fetch_date(zval *zv, const MYSQLND_FIELD * const field,
				   unsigned int pack_len, zend_uchar **row,
				   zend_bool as_unicode TSRMLS_DC)
{
	struct st_mysqlnd_time t = {0};
	unsigned int length; /* First byte encodes the length*/
	char *to;
	DBG_ENTER("ps_fetch_date");

	if ((length = php_mysqlnd_net_field_length(row))) {
		zend_uchar *to= *row;

		t.time_type= MYSQLND_TIMESTAMP_DATE;
		t.neg= 0;

		t.second_part = t.hour = t.minute = t.second = 0;

		t.year	= (unsigned int) sint2korr(to);
		t.month = (unsigned int) to[2];
		t.day	= (unsigned int) to[3];

		(*row)+= length;
	} else {
		memset(&t, 0, sizeof(t));
		t.time_type = MYSQLND_TIMESTAMP_DATE;
	}

	/*
	  QQ : How to make this unicode without copying two times the buffer -
	  Unicode equivalent of spprintf?
	*/
	length = spprintf(&to, 0, "%04u-%02u-%02u", t.year, t.month, t.day);

	DBG_INF_FMT("%s", to);
#if PHP_MAJOR_VERSION >= 6
	if (!as_unicode) {
#endif
		ZVAL_STRINGL(zv, to, length, 1);
		mnd_efree(to);
#if PHP_MAJOR_VERSION >= 6
	} else {
		ZVAL_UTF8_STRINGL(zv, to, length, ZSTR_AUTOFREE);	
	}
#endif
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ ps_fetch_datetime */
static
void ps_fetch_datetime(zval *zv, const MYSQLND_FIELD * const field,
					   unsigned int pack_len, zend_uchar **row,
					   zend_bool as_unicode TSRMLS_DC)
{
	struct st_mysqlnd_time t;
	unsigned int length; /* First byte encodes the length*/
	char *to;
	DBG_ENTER("ps_fetch_datetime");

	if ((length = php_mysqlnd_net_field_length(row))) {
		zend_uchar *to= *row;

		t.time_type = MYSQLND_TIMESTAMP_DATETIME;
		t.neg	 = 0;

		t.year	 = (unsigned int) sint2korr(to);
		t.month = (unsigned int) to[2];
		t.day	 = (unsigned int) to[3];

		if (length > 4) {
			t.hour	 = (unsigned int) to[4];
			t.minute = (unsigned int) to[5];
			t.second = (unsigned int) to[6];
		} else {
      		t.hour = t.minute = t.second= 0;
		}
		t.second_part = (length > 7) ? (unsigned long) sint4korr(to+7) : 0;

		(*row)+= length;
	} else {
		memset(&t, 0, sizeof(t));
		t.time_type = MYSQLND_TIMESTAMP_DATETIME;
	}

	/*
	  QQ : How to make this unicode without copying two times the buffer -
	  Unicode equivalent of spprintf?
	*/
	length = spprintf(&to, 0, "%04u-%02u-%02u %02u:%02u:%02u",
					  t.year, t.month, t.day, t.hour, t.minute, t.second);

	DBG_INF_FMT("%s", to);
#if PHP_MAJOR_VERSION >= 6
	if (!as_unicode) {
#endif
		ZVAL_STRINGL(zv, to, length, 1);
		mnd_efree(to);
#if PHP_MAJOR_VERSION >= 6
	} else {
		ZVAL_UTF8_STRINGL(zv, to, length, ZSTR_AUTOFREE);	
	}
#endif
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ ps_fetch_string */
static
void ps_fetch_string(zval *zv, const MYSQLND_FIELD * const field,
					 unsigned int pack_len, zend_uchar **row,
					 zend_bool as_unicode TSRMLS_DC)
{
	/*
	  For now just copy, before we make it possible
	  to write \0 to the row buffer
	*/
	unsigned long length = php_mysqlnd_net_field_length(row);
	DBG_ENTER("ps_fetch_string");
	DBG_INF_FMT("len = %lu", length);
#if PHP_MAJOR_VERSION < 6
	ZVAL_STRINGL(zv, (char *)*row, length, 1);	
#else
	if (field->charsetnr == MYSQLND_BINARY_CHARSET_NR) {
		DBG_INF("Binary charset");
		ZVAL_STRINGL(zv, (char *)*row, length, 1);
	} else {
		ZVAL_UTF8_STRINGL(zv, (char*)*row, length, ZSTR_DUPLICATE);
	}
#endif

	(*row) += length;
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ ps_fetch_bit */
static
void ps_fetch_bit(zval *zv, const MYSQLND_FIELD * const field,
				  unsigned int pack_len, zend_uchar **row,
				  zend_bool as_unicode TSRMLS_DC)
{
	unsigned long length= php_mysqlnd_net_field_length(row);
	ps_fetch_from_1_to_8_bytes(zv, field, pack_len, row, as_unicode, length TSRMLS_CC);
}
/* }}} */


/* {{{ _mysqlnd_init_ps_subsystem */
void _mysqlnd_init_ps_subsystem()
{
	memset(mysqlnd_ps_fetch_functions, 0, sizeof(mysqlnd_ps_fetch_functions));
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NULL].func		= ps_fetch_null;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NULL].pack_len 	= 0;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NULL].php_type 	= IS_NULL;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NULL].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TINY].func		= ps_fetch_int8;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TINY].pack_len	= 1;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TINY].php_type	= IS_LONG;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TINY].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_SHORT].func		= ps_fetch_int16;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_SHORT].pack_len	= 2;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_SHORT].php_type	= IS_LONG;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_SHORT].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_YEAR].func		= ps_fetch_int16;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_YEAR].pack_len	= 2;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_YEAR].php_type	= IS_LONG;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_YEAR].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_INT24].func		= ps_fetch_int32;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_INT24].pack_len	= 4;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_INT24].php_type	= IS_LONG;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_INT24].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONG].func		= ps_fetch_int32;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONG].pack_len	= 4;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONG].php_type 	= IS_LONG;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONG].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONGLONG].func	= ps_fetch_int64;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONGLONG].pack_len= 8;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONGLONG].php_type = IS_LONG;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONGLONG].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_FLOAT].func		= ps_fetch_float;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_FLOAT].pack_len	= 4;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_FLOAT].php_type	= IS_DOUBLE;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_FLOAT].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DOUBLE].func		= ps_fetch_double;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DOUBLE].pack_len	= 8;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DOUBLE].php_type = IS_DOUBLE;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DOUBLE].can_ret_as_str_in_uni 	= TRUE;
	
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TIME].func		= ps_fetch_time;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TIME].pack_len	= MYSQLND_PS_SKIP_RESULT_W_LEN;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TIME].php_type	= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TIME].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DATE].func		= ps_fetch_date;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DATE].pack_len	= MYSQLND_PS_SKIP_RESULT_W_LEN;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DATE].php_type	= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DATE].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NEWDATE].func		= ps_fetch_date;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NEWDATE].pack_len	= MYSQLND_PS_SKIP_RESULT_W_LEN;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NEWDATE].php_type	= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NEWDATE].can_ret_as_str_in_uni 	= TRUE;
	
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DATETIME].func	= ps_fetch_datetime;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DATETIME].pack_len= MYSQLND_PS_SKIP_RESULT_W_LEN;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DATETIME].php_type= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DATETIME].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TIMESTAMP].func	= ps_fetch_datetime;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TIMESTAMP].pack_len= MYSQLND_PS_SKIP_RESULT_W_LEN;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TIMESTAMP].php_type= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TIMESTAMP].can_ret_as_str_in_uni = TRUE;
	
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TINY_BLOB].func	= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TINY_BLOB].pack_len= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TINY_BLOB].php_type = IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TINY_BLOB].is_possibly_blob = TRUE;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_TINY_BLOB].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_BLOB].func		= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_BLOB].pack_len	= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_BLOB].php_type	= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_BLOB].is_possibly_blob = TRUE;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_BLOB].can_ret_as_str_in_uni 	= TRUE;
	
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_MEDIUM_BLOB].func	= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_MEDIUM_BLOB].pack_len= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_MEDIUM_BLOB].php_type= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_MEDIUM_BLOB].is_possibly_blob = TRUE;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_MEDIUM_BLOB].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONG_BLOB].func		= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONG_BLOB].pack_len	= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONG_BLOB].php_type	= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONG_BLOB].is_possibly_blob = TRUE;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_LONG_BLOB].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_BIT].func		= ps_fetch_bit;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_BIT].pack_len	= 8;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_BIT].php_type	= IS_LONG;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_BIT].can_ret_as_str_in_uni = TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_VAR_STRING].func		= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_VAR_STRING].pack_len	= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_VAR_STRING].php_type = IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_VAR_STRING].is_possibly_blob = TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_VARCHAR].func		= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_VARCHAR].pack_len	= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_VARCHAR].php_type = IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_VARCHAR].is_possibly_blob = TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_STRING].func			= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_STRING].pack_len		= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_STRING].php_type 	= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_STRING].is_possibly_blob = TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DECIMAL].func		= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DECIMAL].pack_len	= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DECIMAL].php_type	= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_DECIMAL].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NEWDECIMAL].func		= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NEWDECIMAL].pack_len	= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NEWDECIMAL].php_type	= IS_STRING;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_NEWDECIMAL].can_ret_as_str_in_uni 	= TRUE;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_ENUM].func		= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_ENUM].pack_len	= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_ENUM].php_type	= IS_STRING;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_SET].func			= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_SET].pack_len		= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_SET].php_type		= IS_STRING;

	mysqlnd_ps_fetch_functions[MYSQL_TYPE_GEOMETRY].func	= ps_fetch_string;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_GEOMETRY].pack_len= MYSQLND_PS_SKIP_RESULT_STR;
	mysqlnd_ps_fetch_functions[MYSQL_TYPE_GEOMETRY].php_type= IS_STRING;
}
/* }}} */


/* {{{ mysqlnd_stmt_copy_it */
static void
mysqlnd_stmt_copy_it(zval *** copies, zval *original, unsigned int param_count, unsigned int current)
{
	if (!*copies) {
		*copies = ecalloc(param_count, sizeof(zval *));					
	}
	MAKE_STD_ZVAL((*copies)[current]);
	*(*copies)[current] = *original;
	Z_SET_REFCOUNT_P((*copies)[current], 1);
	zval_copy_ctor((*copies)[current]);
}
/* }}} */


/* {{{ mysqlnd_stmt_execute_store_params */
static void
mysqlnd_stmt_execute_store_params(MYSQLND_STMT *stmt, zend_uchar **buf, zend_uchar **p,
								  size_t *buf_len, unsigned int null_byte_offset TSRMLS_DC)
{
	unsigned int i = 0;
	size_t left = (*buf_len - (*p - *buf));
	size_t data_size = 0;
	zval **copies = NULL;/* if there are different types */

/* 1. Store type information */
	if (stmt->send_types_to_server) {

		/* 2 bytes per type, and leave 20 bytes for future use */
		if (left < ((stmt->param_count * 2) + 20)) {
			unsigned int offset = *p - *buf;
			zend_uchar *tmp_buf;
			*buf_len = offset + stmt->param_count * 2 + 20;
			tmp_buf = mnd_emalloc(*buf_len);
			memcpy(tmp_buf, *buf, offset);
			*buf = tmp_buf;
			
			/* Update our pos pointer */
			*p = *buf + offset;
		}
		for (i = 0; i < stmt->param_count; i++) {
			/* our types are not unsigned */
#if SIZEOF_LONG==8  
			if (stmt->param_bind[i].type == MYSQL_TYPE_LONG) {
				stmt->param_bind[i].type = MYSQL_TYPE_LONGLONG;
			}
#endif
			int2store(*p, stmt->param_bind[i].type);
			*p+= 2;
		}
	}

/* 2. Store data */
	/* 2.1 Calculate how much space we need */
	for (i = 0; i < stmt->param_count; i++) {
		unsigned int j;
		zval *the_var = stmt->param_bind[i].zv;

		if (!the_var || (stmt->param_bind[i].type != MYSQL_TYPE_LONG_BLOB &&
						 Z_TYPE_P(the_var) == IS_NULL)) {
			continue;
		}
		for (j = i + 1; j < stmt->param_count; j++) {
			if (stmt->param_bind[j].zv == the_var) {
				/* Double binding of the same zval, make a copy */
				mysqlnd_stmt_copy_it(&copies, the_var, stmt->param_count, i);
				break; 
			}
		}

		switch (stmt->param_bind[i].type) {
			case MYSQL_TYPE_DOUBLE:
				data_size += 8;
				if (Z_TYPE_P(the_var) != IS_DOUBLE) {
					if (!copies || !copies[i]) {
						mysqlnd_stmt_copy_it(&copies, the_var, stmt->param_count, i);
					}
				}
				break;
#if SIZEOF_LONG==8  
			case MYSQL_TYPE_LONGLONG:
				data_size += 8;
#elif SIZEOF_LONG==4
			case MYSQL_TYPE_LONG:
				data_size += 4;
#else
#error "Should not happen"
#endif
				if (Z_TYPE_P(the_var) != IS_LONG) {
					if (!copies || !copies[i]) {
						mysqlnd_stmt_copy_it(&copies, the_var, stmt->param_count, i);
					}
				}
				break;
			case MYSQL_TYPE_LONG_BLOB:
				if (!(stmt->param_bind[i].flags & MYSQLND_PARAM_BIND_BLOB_USED)) {
					/*
					  User hasn't sent anything, we will send empty string.
					  Empty string has length of 0, encoded in 1 byte. No real
					  data will follows after it.
					*/
					data_size++;
				}
				break;
			case MYSQL_TYPE_VAR_STRING:
				data_size += 8; /* max 8 bytes for size */
#if PHP_MAJOR_VERSION < 6
				if (Z_TYPE_P(the_var) != IS_STRING)
#elif PHP_MAJOR_VERSION >= 6
				if (Z_TYPE_P(the_var) != IS_STRING || (UG(unicode) && Z_TYPE_P(the_var) == IS_UNICODE))
#endif
				{
					if (!copies || !copies[i]) {
						mysqlnd_stmt_copy_it(&copies, the_var, stmt->param_count, i);
					}
					the_var = copies[i];
#if PHP_MAJOR_VERSION >= 6
					if (UG(unicode) && Z_TYPE_P(the_var) == IS_UNICODE) {
						zval_unicode_to_string_ex(the_var, UG(utf8_conv) TSRMLS_CC);
					}
#endif
				}
				convert_to_string_ex(&the_var);
				data_size += Z_STRLEN_P(the_var);
				break;
		}

	}

	/* 2.2 Enlarge the buffer, if needed */
	left = (*buf_len - (*p - *buf));
	if (left < data_size) {
		unsigned int offset = *p - *buf;
		zend_uchar *tmp_buf;
		*buf_len = offset + data_size + 10; /* Allocate + 10 for safety */
		tmp_buf = mnd_emalloc(*buf_len);
		memcpy(tmp_buf, *buf, offset);
		*buf = tmp_buf;
		/* Update our pos pointer */
		*p = *buf + offset;	
	}

	/* 2.3 Store the actual data */
	for (i = 0; i < stmt->param_count; i++) {
		zval *data = copies && copies[i]? copies[i]: stmt->param_bind[i].zv;
		/* Handle long data */
		if (stmt->param_bind[i].zv && Z_TYPE_P(data) == IS_NULL) {
			(*buf + null_byte_offset)[i/8] |= (zend_uchar) (1 << (i & 7));
		} else {
			switch (stmt->param_bind[i].type) {
				case MYSQL_TYPE_DOUBLE:
					convert_to_double_ex(&data);
					float8store(*p, Z_DVAL_P(data));
					(*p) += 8;
					break;
#if SIZEOF_LONG==8  
				case MYSQL_TYPE_LONGLONG:
					convert_to_long_ex(&data);
					int8store(*p, Z_LVAL_P(data));
					(*p) += 8;
					break;
#elif SIZEOF_LONG==4
				case MYSQL_TYPE_LONG:
					convert_to_long_ex(&data);
					int4store(*p, Z_LVAL_P(data));
					(*p) += 4;
					break;
#else
#error "Should not happen"
#endif
				case MYSQL_TYPE_LONG_BLOB:
					if (stmt->param_bind[i].flags & MYSQLND_PARAM_BIND_BLOB_USED) {
						stmt->param_bind[i].flags &= ~MYSQLND_PARAM_BIND_BLOB_USED;
					} else {
						/* send_long_data() not called, send empty string */
						*p = php_mysqlnd_net_store_length(*p, 0);
					}
					break;
				case MYSQL_TYPE_VAR_STRING:{
						unsigned int len = Z_STRLEN_P(data);
						/* to is after p. The latter hasn't been moved */
						*p = php_mysqlnd_net_store_length(*p, len);
						memcpy(*p, Z_STRVAL_P(data), len);
						(*p) += len;
					}
					break;
				default:
					/* Won't happen, but set to NULL */
					(*buf + null_byte_offset)[i/8] |= (zend_uchar) (1 << (i & 7));
					break;
			}
		}
	}
	if (copies) {
		for (i = 0; i < stmt->param_count; i++) {
			if (copies[i]) {
				zval_ptr_dtor(&copies[i]);
			}
		}
		efree(copies);	
	}
}
/* }}} */


/* {{{ mysqlnd_stmt_execute_generate_request */
zend_uchar* mysqlnd_stmt_execute_generate_request(MYSQLND_STMT *stmt, size_t *request_len,
												  zend_bool *free_buffer TSRMLS_DC)
{
	zend_uchar	*p = stmt->execute_cmd_buffer.buffer,
				*cmd_buffer = stmt->execute_cmd_buffer.buffer;
	size_t cmd_buffer_length = stmt->execute_cmd_buffer.length;
	unsigned int	null_byte_offset,
					null_count= (stmt->param_count + 7) / 8;

	int4store(p, stmt->stmt_id);
	p += 4;

	/* flags is 4 bytes, we store just 1 */
	int1store(p, (zend_uchar) stmt->flags);
	p++;

	/* Make it all zero */
	int4store(p, 0); 

	int1store(p, 1); /* and send 1 for iteration count */
	p+= 4;


	null_byte_offset = p - cmd_buffer;
	memset(p, 0, null_count);
	p += null_count;


	int1store(p, stmt->send_types_to_server); 
	p++;

	mysqlnd_stmt_execute_store_params(stmt, &cmd_buffer, &p, &cmd_buffer_length, null_byte_offset TSRMLS_CC);

	*free_buffer = (cmd_buffer != stmt->execute_cmd_buffer.buffer);
	*request_len = (p - cmd_buffer);
	return cmd_buffer;
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
