/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Ard Biesheuvel <abies@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE

#include "php.h"
#include "zend_exceptions.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_firebird.h"
#include "php_pdo_firebird_int.h"

static int firebird_alloc_prepare_stmt(pdo_dbh_t*, const char*, size_t, XSQLDA*, isc_stmt_handle*,
	HashTable*);

const char CHR_LETTER = 1;
const char CHR_DIGIT = 2;
const char CHR_IDENT = 4;
const char CHR_QUOTE = 8;
const char CHR_WHITE = 16;
const char CHR_HEX = 32;
const char CHR_INTRODUCER = 64;

static const char classes_array[] = {
	/* 000     */ 0,
	/* 001     */ 0,
	/* 002     */ 0,
	/* 003     */ 0,
	/* 004     */ 0,
	/* 005     */ 0,
	/* 006     */ 0,
	/* 007     */ 0,
	/* 008     */ 0,
	/* 009     */ 16, /* CHR_WHITE */
	/* 010     */ 16, /* CHR_WHITE */
	/* 011     */ 0,
	/* 012     */ 0,
	/* 013     */ 16, /* CHR_WHITE */
	/* 014     */ 0,
	/* 015     */ 0,
	/* 016     */ 0,
	/* 017     */ 0,
	/* 018     */ 0,
	/* 019     */ 0,
	/* 020     */ 0,
	/* 021     */ 0,
	/* 022     */ 0,
	/* 023     */ 0,
	/* 024     */ 0,
	/* 025     */ 0,
	/* 026     */ 0,
	/* 027     */ 0,
	/* 028     */ 0,
	/* 029     */ 0,
	/* 030     */ 0,
	/* 031     */ 0,
	/* 032     */ 16, /* CHR_WHITE */
	/* 033  !  */ 0,
	/* 034  "  */ 8, /* CHR_QUOTE */
	/* 035  #  */ 0,
	/* 036  $  */ 4, /* CHR_IDENT */
	/* 037  %  */ 0,
	/* 038  &  */ 0,
	/* 039  '  */ 8, /* CHR_QUOTE */
	/* 040  (  */ 0,
	/* 041  )  */ 0,
	/* 042  *  */ 0,
	/* 043  +  */ 0,
	/* 044  ,  */ 0,
	/* 045  -  */ 0,
	/* 046  .  */ 0,
	/* 047  /  */ 0,
	/* 048  0  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 049  1  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 050  2  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 051  3  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 052  4  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 053  5  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 054  6  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 055  7  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 056  8  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 057  9  */ 38, /* CHR_DIGIT | CHR_IDENT | CHR_HEX */
	/* 058  :  */ 0,
	/* 059  ;  */ 0,
	/* 060  <  */ 0,
	/* 061  =  */ 0,
	/* 062  >  */ 0,
	/* 063  ?  */ 0,
	/* 064  @  */ 0,
	/* 065  A  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 066  B  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 067  C  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 068  D  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 069  E  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 070  F  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 071  G  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 072  H  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 073  I  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 074  J  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 075  K  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 076  L  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 077  M  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 078  N  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 079  O  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 080  P  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 081  Q  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 082  R  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 083  S  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 084  T  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 085  U  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 086  V  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 087  W  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 088  X  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 089  Y  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 090  Z  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 091  [  */ 0,
	/* 092  \  */ 0,
	/* 093  ]  */ 0,
	/* 094  ^  */ 0,
	/* 095  _  */ 68, /* CHR_IDENT | CHR_INTRODUCER */
	/* 096  `  */ 0,
	/* 097  a  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 098  b  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 099  c  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 100  d  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 101  e  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 102  f  */ 37, /* CHR_LETTER | CHR_IDENT | CHR_HEX */
	/* 103  g  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 104  h  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 105  i  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 106  j  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 107  k  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 108  l  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 109  m  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 110  n  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 111  o  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 112  p  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 113  q  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 114  r  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 115  s  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 116  t  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 117  u  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 118  v  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 119  w  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 120  x  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 121  y  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 122  z  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 123  {  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 124  |  */ 0,
	/* 125  }  */ 5, /* CHR_LETTER | CHR_IDENT */
	/* 126  ~  */ 0,
	/* 127     */ 0
};

inline char classes(char idx)
{
	if (idx > 127) return 0;
	return classes_array[idx];
}

typedef enum {
	ttNone,
	ttWhite,
	ttComment,
	ttBrokenComment,
	ttString,
	ttParamMark,
	ttIdent,
	ttOther
} FbTokenType;

static FbTokenType getToken(const char** begin, const char* end)
{
	FbTokenType ret = ttNone;
	const char* p = *begin;

	char c = *p++;
	switch (c)
	{
	case ':':
	case '?':
		ret = ttParamMark;
		break;

	case '\'':
	case '"':
		while (p < end)
		{
			if (*p++ == c)
			{
				ret = ttString;
				break;
			}
		}
		break;

	case '/':
		if (p < end && *p == '*')
		{
			ret = ttBrokenComment;
			p++;
			while (p < end)
			{
				if (*p++ == '*' && p < end && *p == '/')
				{
					p++;
					ret = ttComment;
					break;
				}
			}
		}
		else {
			ret = ttOther;
		}
		break;

	case '-':
		if (p < end && *p == '-')
		{
			while (++p < end)
			{
				if (*p == '\r')
				{
					p++;
					if (p < end && *p == '\n')
						p++;
					break;
				}
				else if (*p == '\n')
					break;
			}

			ret = ttComment;
		}
		else
			ret = ttOther;
		break;

	default:
		if (classes(c) & CHR_DIGIT)
		{
			while (p < end && (classes(*p) & CHR_DIGIT))
				p++;
			ret = ttOther;
		}
		else if (classes(c) & CHR_IDENT)
		{
			while (p < end && (classes(*p) & CHR_IDENT))
				p++;
			ret = ttIdent;
		}
		else if (classes(c) & CHR_WHITE)
		{
			while (p < end && (classes(*p) & CHR_WHITE))
				p++;
			ret = ttWhite;
		}
		else
		{
			while (p < end && !(classes(*p) & (CHR_DIGIT | CHR_IDENT | CHR_WHITE)) &&
				(*p != '/') && (*p != '-') && (*p != ':') && (*p != '?') &&
				(*p != '\'') && (*p != '"'))
			{
				p++;
			}
			ret = ttOther;
		}
	}

	*begin = p;
	return ret;
}

int preprocess(const char* sql, int sql_len, char* sql_out, HashTable* named_params)
{
	zend_bool passAsIs = 1, execBlock = 0;
	zend_long pindex = -1;
	char pname[254], ident[253], ident2[253];
	unsigned int l;
	const char* p = sql, * end = sql + sql_len;
	const char* start = p;
	FbTokenType tok = getToken(&p, end);

	const char* i = start;
	while (p < end && (tok == ttComment || tok == ttWhite))
	{
		i = p;
		tok = getToken(&p, end);
	}

	if (p >= end || tok != ttIdent)
	{
		/* Execute statement preprocess SQL error */
		/* Statement expected */
		return 0;
	}
	/* skip leading comments ?? */
	start = i;
	l = p - i;
	/* check the length of the identifier */
	/* in Firebird 4.0 it is 63 characters, in previous versions 31 bytes */
	if (l > 252) {
		return 0;
	}
	strncpy(ident, i, l);
	ident[l] = '\0';
	if (!strcasecmp(ident, "EXECUTE"))
	{
		/* For EXECUTE PROCEDURE and EXECUTE BLOCK statements, named parameters must be processed. */
		/* However, in EXECUTE BLOCK this is done in a special way. */
		const char* i2 = p;
		tok = getToken(&p, end);
		while (p < end && (tok == ttComment || tok == ttWhite))
		{
			i2 = p;
			tok = getToken(&p, end);
		}
		if (p >= end || tok != ttIdent)
		{
			/* Execute statement preprocess SQL error */
			/* Statement expected */
			return 0;
		}
		l = p - i2;
		/* check the length of the identifier */
		/* in Firebird 4.0 it is 63 characters, in previous versions 31 bytes */
		if (l > 252) {
			return 0;
		}
		strncpy(ident2, i2, l);
		ident2[l] = '\0';
		execBlock = !strcasecmp(ident2, "BLOCK");
		passAsIs = 0;
	}
	else
	{
		/* Named parameters must be processed in the INSERT, UPDATE, DELETE, MERGE statements. */
		/* If CTEs are present in the query, they begin with the WITH keyword. */
		passAsIs = strcasecmp(ident, "INSERT") && strcasecmp(ident, "UPDATE") &&
			strcasecmp(ident, "DELETE") && strcasecmp(ident, "MERGE") &&
			strcasecmp(ident, "SELECT") && strcasecmp(ident, "WITH");
	}

	if (passAsIs)
	{
		strcpy(sql_out, sql);
		return 1;
	}

	strncat(sql_out, start, p - start);

	while (p < end)
	{
		start = p;
		tok = getToken(&p, end);
		switch (tok)
		{
		case ttParamMark:
			tok = getToken(&p, end);
			if (tok == ttIdent /*|| tok == ttString*/)
			{
				++pindex;
				l = p - start;
				/* check the length of the identifier */
				/* in Firebird 4.0 it is 63 characters, in previous versions 31 bytes */
				/* + symbol ":" */
				if (l > 253) {
					return 0;
				}
				strncpy(pname, start, l);
				pname[l] = '\0';
				
				if (named_params) {
					zval tmp;
					ZVAL_LONG(&tmp, pindex);
					zend_hash_str_update(named_params, pname, l, &tmp);
				}

				strcat(sql_out, "?");
			}
			else
			{
				if (strncmp(start, "?", 1)) {
					/* Execute statement preprocess SQL error */
					/* Parameter name expected */
					return 0;
				}
				++pindex;
				strncat(sql_out, start, p - start);
			}
			break;

		case ttIdent:
			if (execBlock)
			{
				/* In the EXECUTE BLOCK statement, processing must be */
				/* carried out up to the keyword AS. */
				l = p - start;
				/* check the length of the identifier */
				/* in Firebird 4.0 it is 63 characters, in previous versions 31 bytes */
				if (l > 252) {
					return 0;
				}
				strncpy(ident, start, l);
				ident[l] = '\0';
				if (!strcasecmp(ident, "AS"))
				{
					strncat(sql_out, start, end - start);
					return 1;
				}
			}

		case ttWhite:
		case ttComment:
		case ttString:
		case ttOther:
			strncat(sql_out, start, p - start);
			break;

		case ttBrokenComment:
		{
			/* Execute statement preprocess SQL error */
			/* Unclosed comment found near ''@1'' */
			return 0;
		}
		break;


		case ttNone:
			/* Execute statement preprocess SQL error */
			return 0;
			break;
		}
	}
	return 1;
}

/* map driver specific error message to PDO error */
void _firebird_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, char const *file, zend_long line) /* {{{ */
{
	pdo_error_type *const error_code = stmt ? &stmt->error_code : &dbh->error_code;

	strcpy(*error_code, "HY000");
}
/* }}} */

#define RECORD_ERROR(dbh) _firebird_error(dbh, NULL, __FILE__, __LINE__)

/* called by PDO to close a db handle */
static int firebird_handle_closer(pdo_dbh_t *dbh) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;

	if (dbh->in_txn) {
		if (dbh->auto_commit) {
			if (isc_commit_transaction(H->isc_status, &H->tr)) {
				RECORD_ERROR(dbh);
			}
		} else {
			if (isc_rollback_transaction(H->isc_status, &H->tr)) {
				RECORD_ERROR(dbh);
			}
		}
	}

	if (isc_detach_database(H->isc_status, &H->db)) {
		RECORD_ERROR(dbh);
	}

	if (H->date_format) {
		efree(H->date_format);
	}
	if (H->time_format) {
		efree(H->time_format);
	}
	if (H->timestamp_format) {
		efree(H->timestamp_format);
	}

	pefree(H, dbh->is_persistent);

	return 0;
}
/* }}} */

/* called by PDO to prepare an SQL query */
static int firebird_handle_preparer(pdo_dbh_t *dbh, const char *sql, size_t sql_len, /* {{{ */
	pdo_stmt_t *stmt, zval *driver_options)
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;
	pdo_firebird_stmt *S = NULL;
	HashTable *np;

	do {
		isc_stmt_handle s = PDO_FIREBIRD_HANDLE_INITIALIZER;
		XSQLDA num_sqlda;
		static char const info[] = { isc_info_sql_stmt_type };
		char result[8];

		num_sqlda.version = PDO_FB_SQLDA_VERSION;
		num_sqlda.sqln = 1;

		ALLOC_HASHTABLE(np);
		zend_hash_init(np, 8, NULL, NULL, 0);

		/* allocate and prepare statement */
		if (!firebird_alloc_prepare_stmt(dbh, sql, sql_len, &num_sqlda, &s, np)) {
			break;
		}

		/* allocate a statement handle struct of the right size (struct out_sqlda is inlined) */
		S = ecalloc(1, sizeof(*S)-sizeof(XSQLDA) + XSQLDA_LENGTH(num_sqlda.sqld));
		S->H = H;
		S->stmt = s;
		S->fetch_buf = ecalloc(1,sizeof(char*) * num_sqlda.sqld);
		S->out_sqlda.version = PDO_FB_SQLDA_VERSION;
		S->out_sqlda.sqln = stmt->column_count = num_sqlda.sqld;
		S->named_params = np;

		/* determine the statement type */
		if (isc_dsql_sql_info(H->isc_status, &s, sizeof(info), const_cast(info), sizeof(result),
				result)) {
			break;
		}
		S->statement_type = result[3];

		/* fill the output sqlda with information about the prepared query */
		if (isc_dsql_describe(H->isc_status, &s, PDO_FB_SQLDA_VERSION, &S->out_sqlda)) {
			RECORD_ERROR(dbh);
			break;
		}

		/* allocate the input descriptors */
		if (isc_dsql_describe_bind(H->isc_status, &s, PDO_FB_SQLDA_VERSION, &num_sqlda)) {
			break;
		}

		if (num_sqlda.sqld) {
			S->in_sqlda = ecalloc(1,XSQLDA_LENGTH(num_sqlda.sqld));
			S->in_sqlda->version = PDO_FB_SQLDA_VERSION;
			S->in_sqlda->sqln = num_sqlda.sqld;

			if (isc_dsql_describe_bind(H->isc_status, &s, PDO_FB_SQLDA_VERSION, S->in_sqlda)) {
				break;
			}
		}

		stmt->driver_data = S;
		stmt->methods = &firebird_stmt_methods;
		stmt->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL;

		return 1;

	} while (0);

	RECORD_ERROR(dbh);

	zend_hash_destroy(np);
	FREE_HASHTABLE(np);

	if (S) {
		if (S->in_sqlda) {
			efree(S->in_sqlda);
		}
		efree(S);
	}

	return 0;
}
/* }}} */

/* called by PDO to execute a statement that doesn't produce a result set */
static zend_long firebird_handle_doer(pdo_dbh_t *dbh, const char *sql, size_t sql_len) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;
	isc_stmt_handle stmt = PDO_FIREBIRD_HANDLE_INITIALIZER;
	static char const info_count[] = { isc_info_sql_records };
	char result[64];
	int ret = 0;
	XSQLDA in_sqlda, out_sqlda;

	/* TODO no placeholders in exec() for now */
	in_sqlda.version = out_sqlda.version = PDO_FB_SQLDA_VERSION;
	in_sqlda.sqld = out_sqlda.sqld = 0;
	out_sqlda.sqln = 1;

	/* allocate and prepare statement */
	if (!firebird_alloc_prepare_stmt(dbh, sql, sql_len, &out_sqlda, &stmt, 0)) {
		return -1;
	}

	/* execute the statement */
	if (isc_dsql_execute2(H->isc_status, &H->tr, &stmt, PDO_FB_SQLDA_VERSION, &in_sqlda, &out_sqlda)) {
		RECORD_ERROR(dbh);
		ret = -1;
		goto free_statement;
	}

	/* find out how many rows were affected */
	if (isc_dsql_sql_info(H->isc_status, &stmt, sizeof(info_count), const_cast(info_count),
			sizeof(result),	result)) {
		RECORD_ERROR(dbh);
		ret = -1;
		goto free_statement;
	}

	if (result[0] == isc_info_sql_records) {
		unsigned i = 3, result_size = isc_vax_integer(&result[1],2);

		if (result_size > sizeof(result)) {
			ret = -1;
			goto free_statement;
		}
		while (result[i] != isc_info_end && i < result_size) {
			short len = (short)isc_vax_integer(&result[i+1],2);
			/* bail out on bad len */
			if (len != 1 && len != 2 && len != 4) {
				ret = -1;
				goto free_statement;
			}
			if (result[i] != isc_info_req_select_count) {
				ret += isc_vax_integer(&result[i+3],len);
			}
			i += len+3;
		}
	}

	/* commit if we're in auto_commit mode */
	if (dbh->auto_commit && isc_commit_retaining(H->isc_status, &H->tr)) {
		RECORD_ERROR(dbh);
	}

free_statement:

	if (isc_dsql_free_statement(H->isc_status, &stmt, DSQL_drop)) {
		RECORD_ERROR(dbh);
	}

	return ret;
}
/* }}} */

/* called by the PDO SQL parser to add quotes to values that are copied into SQL */
static int firebird_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, size_t unquotedlen, /* {{{ */
	char **quoted, size_t *quotedlen, enum pdo_param_type paramtype)
{
	int qcount = 0;
	char const *co, *l, *r;
	char *c;

	if (!unquotedlen) {
		*quotedlen = 2;
		*quoted = emalloc(*quotedlen+1);
		strcpy(*quoted, "''");
		return 1;
	}

	/* Firebird only requires single quotes to be doubled if string lengths are used */
	/* count the number of ' characters */
	for (co = unquoted; (co = strchr(co,'\'')); qcount++, co++);

	*quotedlen = unquotedlen + qcount + 2;
	*quoted = c = emalloc(*quotedlen+1);
	*c++ = '\'';

	/* foreach (chunk that ends in a quote) */
	for (l = unquoted; (r = strchr(l,'\'')); l = r+1) {
		strncpy(c, l, r-l+1);
		c += (r-l+1);
		/* add the second quote */
		*c++ = '\'';
	}

	/* copy the remainder */
	strncpy(c, l, *quotedlen-(c-*quoted)-1);
	(*quoted)[*quotedlen-1] = '\'';
	(*quoted)[*quotedlen]   = '\0';

	return 1;
}
/* }}} */

/* called by PDO to start a transaction */
static int firebird_handle_begin(pdo_dbh_t *dbh) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;
	char tpb[8] = { isc_tpb_version3 }, *ptpb = tpb+1;
#ifdef abies_0
	if (dbh->transaction_flags & PDO_TRANS_ISOLATION_LEVEL) {
		if (dbh->transaction_flags & PDO_TRANS_READ_UNCOMMITTED) {
			/* this is a poor fit, but it's all we have */
			*ptpb++ = isc_tpb_read_committed;
			*ptpb++ = isc_tpb_rec_version;
			dbh->transaction_flags &= ~(PDO_TRANS_ISOLATION_LEVEL^PDO_TRANS_READ_UNCOMMITTED);
		} else if (dbh->transaction_flags & PDO_TRANS_READ_COMMITTED) {
			*ptpb++ = isc_tpb_read_committed;
			*ptpb++ = isc_tpb_no_rec_version;
			dbh->transaction_flags &= ~(PDO_TRANS_ISOLATION_LEVEL^PDO_TRANS_READ_COMMITTED);
		} else if (dbh->transaction_flags & PDO_TRANS_REPEATABLE_READ) {
			*ptpb++ = isc_tpb_concurrency;
			dbh->transaction_flags &= ~(PDO_TRANS_ISOLATION_LEVEL^PDO_TRANS_REPEATABLE_READ);
		} else {
			*ptpb++ = isc_tpb_consistency;
			dbh->transaction_flags &= ~(PDO_TRANS_ISOLATION_LEVEL^PDO_TRANS_SERIALIZABLE);
		}
	}

	if (dbh->transaction_flags & PDO_TRANS_ACCESS_MODE) {
		if (dbh->transaction_flags & PDO_TRANS_READONLY) {
			*ptpb++ = isc_tpb_read;
			dbh->transaction_flags &= ~(PDO_TRANS_ACCESS_MODE^PDO_TRANS_READONLY);
		} else {
			*ptpb++ = isc_tpb_write;
			dbh->transaction_flags &= ~(PDO_TRANS_ACCESS_MODE^PDO_TRANS_READWRITE);
		}
	}

	if (dbh->transaction_flags & PDO_TRANS_CONFLICT_RESOLUTION) {
		if (dbh->transaction_flags & PDO_TRANS_RETRY) {
			*ptpb++ = isc_tpb_wait;
			dbh->transaction_flags &= ~(PDO_TRANS_CONFLICT_RESOLUTION^PDO_TRANS_RETRY);
		} else {
			*ptpb++ = isc_tpb_nowait;
			dbh->transaction_flags &= ~(PDO_TRANS_CONFLICT_RESOLUTION^PDO_TRANS_ABORT);
		}
	}
#endif
	if (isc_start_transaction(H->isc_status, &H->tr, 1, &H->db, (unsigned short)(ptpb-tpb), tpb)) {
		RECORD_ERROR(dbh);
		return 0;
	}
	return 1;
}
/* }}} */

/* called by PDO to commit a transaction */
static int firebird_handle_commit(pdo_dbh_t *dbh) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;

	if (isc_commit_transaction(H->isc_status, &H->tr)) {
		RECORD_ERROR(dbh);
		return 0;
	}
	return 1;
}
/* }}} */

/* called by PDO to rollback a transaction */
static int firebird_handle_rollback(pdo_dbh_t *dbh) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;

	if (isc_rollback_transaction(H->isc_status, &H->tr)) {
		RECORD_ERROR(dbh);
		return 0;
	}
	return 1;
}
/* }}} */

/* used by prepare and exec to allocate a statement handle and prepare the SQL */
static int firebird_alloc_prepare_stmt(pdo_dbh_t *dbh, const char *sql, size_t sql_len, /* {{{ */
	XSQLDA *out_sqlda, isc_stmt_handle *s, HashTable *named_params)
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;
	char *new_sql;

	/* Firebird allows SQL statements up to 64k, so bail if it doesn't fit */
	if (sql_len > 65536) {
		strcpy(dbh->error_code, "01004");
		return 0;
	}

	/* start a new transaction implicitly if auto_commit is enabled and no transaction is open */
	if (dbh->auto_commit && !dbh->in_txn) {
		/* dbh->transaction_flags = PDO_TRANS_READ_UNCOMMITTED; */

		if (!firebird_handle_begin(dbh)) {
			return 0;
		}
		dbh->in_txn = 1;
	}

	/* allocate the statement */
	if (isc_dsql_allocate_statement(H->isc_status, &H->db, s)) {
		RECORD_ERROR(dbh);
		return 0;
	}

	/* in order to support named params, which Firebird itself doesn't,
	   we need to replace :foo by ?, and store the name we just replaced */
	new_sql = emalloc(sql_len+1);
	new_sql[0] = '\0';
	if (!preprocess(sql, sql_len, new_sql, named_params)) {
		strcpy(dbh->error_code, "07000");
		efree(new_sql);			
		return 0;
	}

	/* prepare the statement */
	if (isc_dsql_prepare(H->isc_status, &H->tr, s, 0, new_sql, H->sql_dialect, out_sqlda)) {
		RECORD_ERROR(dbh);
		efree(new_sql);
		return 0;
	}

	efree(new_sql);
	return 1;
}
/* }}} */

/* called by PDO to set a driver-specific dbh attribute */
static int firebird_handle_set_attribute(pdo_dbh_t *dbh, zend_long attr, zval *val) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_AUTOCOMMIT:
			{
				zend_bool bval = zval_get_long(val)? 1 : 0;

				/* ignore if the new value equals the old one */
				if (dbh->auto_commit ^ bval) {
					if (dbh->in_txn) {
						if (bval) {
							/* turning on auto_commit with an open transaction is illegal, because
							   we won't know what to do with it */
							H->last_app_error = "Cannot enable auto-commit while a transaction is already open";
							return 0;
						} else {
							/* close the transaction */
							if (!firebird_handle_commit(dbh)) {
								break;
							}
							dbh->in_txn = 0;
						}
					}
					dbh->auto_commit = bval;
				}
			}
			return 1;

		case PDO_ATTR_FETCH_TABLE_NAMES:
			H->fetch_table_names = zval_get_long(val)? 1 : 0;
			return 1;

		case PDO_FB_ATTR_DATE_FORMAT:
			{
				zend_string *str = zval_try_get_string(val);
				if (UNEXPECTED(!str)) {
					return 0;
				}
				if (H->date_format) {
					efree(H->date_format);
				}
				spprintf(&H->date_format, 0, "%s", ZSTR_VAL(str));
				zend_string_release_ex(str, 0);
			}
			return 1;

		case PDO_FB_ATTR_TIME_FORMAT:
			{
				zend_string *str = zval_try_get_string(val);
				if (UNEXPECTED(!str)) {
					return 0;
				}
				if (H->time_format) {
					efree(H->time_format);
				}
				spprintf(&H->time_format, 0, "%s", ZSTR_VAL(str));
				zend_string_release_ex(str, 0);
			}
			return 1;

		case PDO_FB_ATTR_TIMESTAMP_FORMAT:
			{
				zend_string *str = zval_try_get_string(val);
				if (UNEXPECTED(!str)) {
					return 0;
				}
				if (H->timestamp_format) {
					efree(H->timestamp_format);
				}
				spprintf(&H->timestamp_format, 0, "%s", ZSTR_VAL(str));
				zend_string_release_ex(str, 0);
			}
			return 1;
	}
	return 0;
}
/* }}} */

#define INFO_BUF_LEN 512

/* callback to used to report database server info */
static void firebird_info_cb(void *arg, char const *s) /* {{{ */
{
	if (arg) {
		if (*(char*)arg) { /* second call */
			strlcat(arg, " ", INFO_BUF_LEN);
		}
		strlcat(arg, s, INFO_BUF_LEN);
	}
}
/* }}} */

/* called by PDO to get a driver-specific dbh attribute */
static int firebird_handle_get_attribute(pdo_dbh_t *dbh, zend_long attr, zval *val) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;

	switch (attr) {
		char tmp[INFO_BUF_LEN];

		case PDO_ATTR_AUTOCOMMIT:
			ZVAL_LONG(val,dbh->auto_commit);
			return 1;

		case PDO_ATTR_CONNECTION_STATUS:
			ZVAL_BOOL(val, !isc_version(&H->db, firebird_info_cb, NULL));
			return 1;

		case PDO_ATTR_CLIENT_VERSION: {
#if defined(__GNUC__) || defined(PHP_WIN32)
			info_func_t info_func = NULL;
#ifdef __GNUC__
			info_func = (info_func_t)dlsym(RTLD_DEFAULT, "isc_get_client_version");
#else
			HMODULE l = GetModuleHandle("fbclient");

			if (!l) {
				break;
			}
			info_func = (info_func_t)GetProcAddress(l, "isc_get_client_version");
#endif
			if (info_func) {
				info_func(tmp);
				ZVAL_STRING(val, tmp);
			}
#else
			ZVAL_NULL(val);
#endif
			}
			return 1;

		case PDO_ATTR_SERVER_VERSION:
		case PDO_ATTR_SERVER_INFO:
			*tmp = 0;

			if (!isc_version(&H->db, firebird_info_cb, (void*)tmp)) {
				ZVAL_STRING(val, tmp);
				return 1;
			}

		case PDO_ATTR_FETCH_TABLE_NAMES:
			ZVAL_BOOL(val, H->fetch_table_names);
			return 1;
	}
	return 0;
}
/* }}} */

/* called by PDO to retrieve driver-specific information about an error that has occurred */
static int pdo_firebird_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;
	const ISC_STATUS *s = H->isc_status;
	char buf[400];
	zend_long i = 0, l, sqlcode = isc_sqlcode(s);

	if (sqlcode) {
		add_next_index_long(info, sqlcode);

		while ((sizeof(buf)>(i+2))&&(l = fb_interpret(&buf[i],(sizeof(buf)-i-2),&s))) {
			i += l;
			strcpy(&buf[i++], " ");
		}
		add_next_index_string(info, buf);
	} else if (H->last_app_error) {
		add_next_index_long(info, -999);
		add_next_index_string(info, const_cast(H->last_app_error));
	}
	return 1;
}
/* }}} */

static const struct pdo_dbh_methods firebird_methods = { /* {{{ */
	firebird_handle_closer,
	firebird_handle_preparer,
	firebird_handle_doer,
	firebird_handle_quoter,
	firebird_handle_begin,
	firebird_handle_commit,
	firebird_handle_rollback,
	firebird_handle_set_attribute,
	NULL, /* last_id not supported */
	pdo_firebird_fetch_error_func,
	firebird_handle_get_attribute,
	NULL /* check_liveness */
};
/* }}} */

/* the driver-specific PDO handle constructor */
static int pdo_firebird_handle_factory(pdo_dbh_t *dbh, zval *driver_options) /* {{{ */
{
	struct pdo_data_src_parser vars[] = {
		{ "dbname", NULL, 0 },
		{ "charset",  NULL,	0 },
		{ "role", NULL,	0 },
		{ "dialect", "3", 0 },
		{ "user", NULL, 0 },
		{ "password", NULL, 0 }
	};
	int i, ret = 0;
	short buf_len = 256, dpb_len;

	pdo_firebird_db_handle *H = dbh->driver_data = pecalloc(1,sizeof(*H),dbh->is_persistent);

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 6);

	if (!dbh->username && vars[4].optval) {
		dbh->username = pestrdup(vars[4].optval, dbh->is_persistent);
	}

	if (!dbh->password && vars[5].optval) {
		dbh->password = pestrdup(vars[5].optval, dbh->is_persistent);
	}

	do {
		static char const dpb_flags[] = {
			isc_dpb_user_name, isc_dpb_password, isc_dpb_lc_ctype, isc_dpb_sql_role_name };
		char const *dpb_values[] = { dbh->username, dbh->password, vars[1].optval, vars[2].optval };
		char dpb_buffer[256] = { isc_dpb_version1 }, *dpb;

		dpb = dpb_buffer + 1;

		/* loop through all the provided arguments and set dpb fields accordingly */
		for (i = 0; i < sizeof(dpb_flags); ++i) {
			if (dpb_values[i] && buf_len > 0) {
				dpb_len = slprintf(dpb, buf_len, "%c%c%s", dpb_flags[i], (unsigned char)strlen(dpb_values[i]),
					dpb_values[i]);
				dpb += dpb_len;
				buf_len -= dpb_len;
			}
		}

		H->sql_dialect = PDO_FB_DIALECT;
		if (vars[3].optval) {
			H->sql_dialect = atoi(vars[3].optval);
		}

		/* fire it up baby! */
		if (isc_attach_database(H->isc_status, 0, vars[0].optval, &H->db,(short)(dpb-dpb_buffer), dpb_buffer)) {
			break;
		}

		dbh->methods = &firebird_methods;
		dbh->native_case = PDO_CASE_UPPER;
		dbh->alloc_own_columns = 1;

		ret = 1;

	} while (0);

	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); ++i) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

	if (!dbh->methods) {
		char errmsg[512];
		const ISC_STATUS *s = H->isc_status;
		fb_interpret(errmsg, sizeof(errmsg),&s);
		zend_throw_exception_ex(php_pdo_get_exception(), H->isc_status[1], "SQLSTATE[%s] [%d] %s",
				"HY000", H->isc_status[1], errmsg);
	}

	if (!ret) {
		firebird_handle_closer(dbh);
	}

	return ret;
}
/* }}} */


const pdo_driver_t pdo_firebird_driver = { /* {{{ */
	PDO_DRIVER_HEADER(firebird),
	pdo_firebird_handle_factory
};
/* }}} */
