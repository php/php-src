/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Adam Dickmeiss <adam@indexdata.dk>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"

#if HAVE_YAZ

#include "ext/standard/info.h"
#include "php_yaz.h"

#include <yaz/yaz-version.h>

#ifndef YAZ_VERSIONL
#error YAZ version 1.9 or later must be used.
#endif

#if YAZ_VERSIONL < 0x010900
#error YAZ version 1.9 or later must be used.
#endif

#ifdef PHP_WIN32
#include <process.h>
#endif

#include <yaz/proto.h>
#include <yaz/marcdisp.h>
#include <yaz/yaz-util.h>
#include <yaz/yaz-ccl.h>
#include <yaz/zoom.h>

#define MAX_ASSOC 100

typedef struct Yaz_AssociationInfo *Yaz_Association;

struct Yaz_AssociationInfo {
	CCL_parser ccl_parser;
	ZOOM_connection zoom_conn;
	ZOOM_resultset zoom_set;
	ZOOM_scanset zoom_scan;
    ZOOM_package zoom_package;
	char *sort_criteria;
	int persistent;
	int in_use;
	int order;
};

static Yaz_Association yaz_association_mk ()
{
	Yaz_Association p = xmalloc (sizeof(*p));

	p->zoom_conn = ZOOM_connection_create (0);
	p->zoom_set = 0;
	p->zoom_scan = 0;
    p->zoom_package = 0;
	ZOOM_connection_option_set(p->zoom_conn, "implementationName", "PHP");
	ZOOM_connection_option_set(p->zoom_conn, "async", "1");
	p->sort_criteria = 0;
	p->in_use = 0;
	p->order = 0;
	p->persistent = 0;
	p->ccl_parser = ccl_parser_create();
	p->ccl_parser->bibset = 0;
	return p;
}

static void yaz_association_destroy (Yaz_Association p)
{
	if (!p)
		return ;
	ZOOM_resultset_destroy (p->zoom_set);
	ZOOM_scanset_destroy (p->zoom_scan);
    ZOOM_package_destroy (p->zoom_package);
	ZOOM_connection_destroy (p->zoom_conn);
	xfree (p->sort_criteria);
	ccl_qual_rm(&p->ccl_parser->bibset);
	ccl_parser_destroy(p->ccl_parser);
}

#ifdef ZTS
static MUTEX_T yaz_mutex;
#endif

ZEND_DECLARE_MODULE_GLOBALS(yaz);

static Yaz_Association *shared_associations;
static int order_associations;
static int le_link;

static unsigned char third_argument_force_ref[] = {
	3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

static unsigned char second_argument_force_ref[] = {
	2, BYREF_NONE, BYREF_FORCE };

function_entry yaz_functions [] = {
	PHP_FE(yaz_connect, NULL)
	PHP_FE(yaz_close, NULL)
	PHP_FE(yaz_search, NULL)
	PHP_FE(yaz_wait, second_argument_force_ref)
	PHP_FE(yaz_errno, NULL)
	PHP_FE(yaz_error, NULL)
	PHP_FE(yaz_addinfo, NULL)
	PHP_FE(yaz_hits, NULL)
	PHP_FE(yaz_record, NULL)
	PHP_FE(yaz_syntax, NULL)
	PHP_FE(yaz_element, NULL)
	PHP_FE(yaz_range, NULL)
	PHP_FE(yaz_itemorder, NULL)
	PHP_FE(yaz_es_result, NULL)
	PHP_FE(yaz_scan, NULL)
	PHP_FE(yaz_scan_result, second_argument_force_ref)
	PHP_FE(yaz_present, NULL)
	PHP_FE(yaz_ccl_conf, NULL)
	PHP_FE(yaz_ccl_parse, third_argument_force_ref)
	PHP_FE(yaz_database, NULL)
	PHP_FE(yaz_sort, NULL)
	PHP_FE(yaz_schema, NULL)
	{NULL, NULL, NULL}
};

static void get_assoc (INTERNAL_FUNCTION_PARAMETERS, pval **id,
                       Yaz_Association *assocp)
{
	Yaz_Association *as = 0;
	
	*assocp = 0;
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	ZEND_FETCH_RESOURCE(as, Yaz_Association *, id, -1,
						"YAZ link", le_link);
	if (as && *as && (*as)->order == YAZSG(assoc_seq) && (*as)->in_use)
	{
		*assocp = *as;
	}
	else
	{
#ifdef ZTS
		tsrm_mutex_unlock (yaz_mutex);
#endif
		php_error(E_WARNING, "Invalid YAZ handle");
	}
}

static void release_assoc (Yaz_Association assoc)
{
#ifdef ZTS
	if (assoc)
		tsrm_mutex_unlock(yaz_mutex);
#endif
}

static const char *array_lookup_string(HashTable *ht, const char *idx)
{
	pval **pvalue;

	if (ht && zend_hash_find(ht, (char*) idx, strlen(idx)+1,
							 (void**) &pvalue) == SUCCESS)
	{
		SEPARATE_ZVAL(pvalue);
		convert_to_string(*pvalue);
		return (*pvalue)->value.str.val;
	}
	return 0;
}

static long *array_lookup_long(HashTable *ht, const char *idx)
{
	pval **pvalue;

	if (ht && zend_hash_find(ht, (char*) idx, strlen(idx)+1,
							 (void**) &pvalue) == SUCCESS)
	{
		SEPARATE_ZVAL(pvalue);
		convert_to_long(*pvalue);
		return &(*pvalue)->value.lval;
	}
	return 0;
}

static long *array_lookup_bool(HashTable *ht, const char *idx)
{
	pval **pvalue;

	if (ht && zend_hash_find(ht, (char*) idx, strlen(idx)+1,
							 (void**) &pvalue) == SUCCESS)
	{
		SEPARATE_ZVAL(pvalue);
		convert_to_boolean(*pvalue);
		return &(*pvalue)->value.lval;
	}
	return 0;
}

static int send_present (Yaz_Association t);
static int send_sort_present (Yaz_Association t);
static int send_sort (Yaz_Association t);

const char *option_get (Yaz_Association as, const char *name)
{
	if (!as)
		return 0;
	return ZOOM_connection_option_get (as->zoom_conn, name);
}

int option_get_int (Yaz_Association as, const char *name, int def)
{
	const char *v;
	v = ZOOM_connection_option_get (as->zoom_conn, name);
	if (!v)
		return def;
	return atoi(v);
}

void option_set (Yaz_Association as, const char *name, const char *value)
{
	if (as && value)
		ZOOM_connection_option_set (as->zoom_conn, name, value);
}

void option_set_int (Yaz_Association as, const char *name, int v)
{
	if (as)
	{
		char s[30];

		sprintf (s, "%d", v);
		ZOOM_connection_option_set (as->zoom_conn, name, s);
	}
}

static int strcmp_null(const char *s1, const char *s2)
{
	if (s1 == 0 && s2 == 0)
		return 0;
	if (s1 == 0 || s2 == 0)
		return -1;
	return strcmp(s1, s2);
}

/* {{{ proto int yaz_connect(string zurl [ array options])
   Create target with given zurl. Returns positive id if successful. */
PHP_FUNCTION(yaz_connect)
{
	int i;
	char *cp;
	char *zurl_str;
	const char *user_str = 0, *group_str = 0, *pass_str = 0;
	const char *cookie_str = 0, *proxy_str = 0;
	const char *charset_str = 0;
    const char *client_IP = 0;
    const char *otherInfo[3];
	int persistent = 1;
	int piggyback = 1;
	pval **zurl, **user = 0;
	Yaz_Association as;

    otherInfo[0] = otherInfo[1] = otherInfo[2] = 0;
	if (ZEND_NUM_ARGS() == 1)
	{
		if (zend_get_parameters_ex (1, &zurl) == FAILURE)
			WRONG_PARAM_COUNT;
	}
	else if (ZEND_NUM_ARGS() == 2)
	{
		if (zend_get_parameters_ex (2, &zurl, &user) == FAILURE)
			WRONG_PARAM_COUNT;

		if (Z_TYPE_PP(user) == IS_ARRAY)
		{
			long *persistent_val;
			long *piggyback_val;
			HashTable *ht = Z_ARRVAL_PP(user);
			user_str = array_lookup_string(ht, "user");
			group_str = array_lookup_string(ht, "group");
			pass_str = array_lookup_string(ht, "password");
			cookie_str = array_lookup_string(ht, "cookie");
			proxy_str = array_lookup_string(ht, "proxy");
			charset_str = array_lookup_string(ht, "charset");
			persistent_val = array_lookup_bool(ht, "persistent");
			if (persistent_val)
				persistent = *persistent_val;
			piggyback_val = array_lookup_bool(ht, "piggyback");
			if (piggyback_val)
				piggyback = *piggyback_val;
            otherInfo[0] = array_lookup_string(ht, "otherInfo0");
            otherInfo[1] = array_lookup_string(ht, "otherInfo1");
            otherInfo[2] = array_lookup_string(ht, "otherInfo2");
		}
		else
		{
			convert_to_string_ex (user);
			user_str = (*user)->value.str.val;
		}
	}
	else
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex (zurl);
	zurl_str = (*zurl)->value.str.val;
	for (cp = zurl_str; *cp && strchr("\t\n ", *cp); cp++)
		;
	if (!*cp)
		RETURN_LONG(0);
		
	/* see if we have it already ... */
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	for (i = 0; i<YAZSG(max_links); i++)
	{
		as = shared_associations[i];
		if (persistent && as && !as->in_use &&
			!strcmp_null(option_get(as, "host"), zurl_str) &&
			!strcmp_null(option_get(as, "proxy"), proxy_str) &&
			!strcmp_null(option_get(as, "user"), user_str) &&
			!strcmp_null(option_get(as, "group"), group_str) &&
			!strcmp_null(option_get(as, "pass"), pass_str) &&
			!strcmp_null(option_get(as, "cookie"), cookie_str) &&
			!strcmp_null(option_get(as, "charset"), charset_str))
		{
            option_set (as, "clientIP", client_IP);
            option_set (as, "otherInfo0", otherInfo[0]);
            option_set (as, "otherInfo1", otherInfo[1]);
            option_set (as, "otherInfo2", otherInfo[2]);
            option_set (as, "piggyback", piggyback ? "1" : "0");
            ZOOM_connection_connect (as->zoom_conn, zurl_str, 0);
			break;
		}
	}
	if (i == YAZSG(max_links))
	{
		/* we didn't have it (or already in use) */
		int i0 = -1;
		int min_order = 2000000000;
		/* find completely free slot or the oldest one */
		for (i = 0; i<YAZSG(max_links) && shared_associations[i]; i++)
		{
			as = shared_associations[i];
			if (persistent && !as->in_use && as->order < min_order)
			{
				min_order = as->order;
				i0 = i;
			}
		}
		if (i == YAZSG(max_links))
		{
			i = i0;
			if (i == -1)
			{
#ifdef ZTS
				tsrm_mutex_unlock (yaz_mutex);
#endif
				RETURN_LONG(0);			 /* no free slot */
			}
			else						 /* "best" free slot */
				yaz_association_destroy(shared_associations[i]);
		}
		shared_associations[i] = as = yaz_association_mk ();

		option_set (as, "user", user_str);
		option_set (as, "group", group_str);
		option_set (as, "pass", pass_str);
		option_set (as, "cookie", cookie_str);
        option_set (as, "clientIP", client_IP);

        option_set (as, "otherInfo0", otherInfo[0]);
        option_set (as, "otherInfo1", otherInfo[1]);
        option_set (as, "otherInfo2", otherInfo[2]);
        option_set (as, "proxy", proxy_str);
        option_set (as, "piggyback", piggyback ? "1" : "0");
        option_set (as, "charset", charset_str);
        
		ZOOM_connection_connect (as->zoom_conn, zurl_str, 0);
	}
	as->in_use = 1;
	as->persistent = persistent;
	as->order = YAZSG(assoc_seq);

#ifdef ZTS
	tsrm_mutex_unlock (yaz_mutex);
#endif
	ZEND_REGISTER_RESOURCE(return_value, &shared_associations[i], le_link);
}
/* }}} */

/* {{{ proto int yaz_close(int id)
   Destory and close target */
PHP_FUNCTION(yaz_close)
{
	Yaz_Association p;
	pval **id;
	if (ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex (1, &id) == FAILURE)
		RETURN_FALSE;
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, id, &p);
	if (!p)
		RETURN_FALSE;
	release_assoc (p);
	zend_list_delete ((*id)->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int yaz_search(int id, string type, string query)
   Specify query of type for search - returns true if successful */
PHP_FUNCTION(yaz_search)
{
	char *query_str, *type_str;
	pval **id, **type, **query;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() == 3)
	{
		if (zend_get_parameters_ex(3, &id, &type, &query) == FAILURE)
		{
			WRONG_PARAM_COUNT;
		}
	}
	else
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, id, &p);
	if (!p)
	{
		RETURN_FALSE;
	}
	convert_to_string_ex (type);
	type_str = (*type)->value.str.val;
	convert_to_string_ex (query);
	query_str = (*query)->value.str.val;

	ZOOM_resultset_destroy (p->zoom_set);
	p->zoom_set = 0;
	if (!strcmp (type_str, "rpn"))
	{
		ZOOM_query q = ZOOM_query_create ();
		ZOOM_query_prefix (q, query_str);
		if (p->sort_criteria)
			ZOOM_query_sortby (q, p->sort_criteria);
		p->zoom_set = ZOOM_connection_search (p->zoom_conn, q);
		ZOOM_query_destroy (q);
		RETVAL_TRUE;
	}
	else
	{
		RETVAL_FALSE;
	}
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_present(int id)
   Retrieve records */
PHP_FUNCTION(yaz_present)
{
	pval **id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(1, &id) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, id, &p);
	if (!p)
	{
		RETURN_FALSE;
	}
	if (p->zoom_set)
	{
		size_t start = option_get_int (p, "start", 0);
		size_t count = option_get_int (p, "count", 0);
		if (count > 0)
			ZOOM_resultset_records (p->zoom_set, 0 /* recs */, start, count);
	}
	release_assoc (p);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int yaz_wait([array options])
   Process events. */
PHP_FUNCTION(yaz_wait)
{
	int no = 0;
	ZOOM_connection conn_ar[MAX_ASSOC];
	int i, timeout = 15;

	if (ZEND_NUM_ARGS() == 1)
	{
		long *val = 0;
		pval **pval_options = 0;
		HashTable *options_ht = 0;
		if (zend_get_parameters_ex(1, &pval_options) == FAILURE)
		{
			WRONG_PARAM_COUNT;
		}
		if (Z_TYPE_PP(pval_options) != IS_ARRAY)
		{
			php_error(E_WARNING, "yaz_wait: Expected array parameter");
			RETURN_FALSE;
		}
		options_ht = Z_ARRVAL_PP(pval_options);
		val = array_lookup_long(options_ht, "timeout");
		if (val)
			timeout = *val;
	}
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	for (i = 0; i<YAZSG(max_links); i++)
	{
		Yaz_Association p = shared_associations[i];
		if (p && p->order == YAZSG(assoc_seq))
        {
            char str[20];

            sprintf (str, "%d", timeout);
            ZOOM_connection_option_set (p->zoom_conn, "timeout", str);
			conn_ar[no++] = p->zoom_conn;
        }
	}
#ifdef ZTS
	tsrm_mutex_unlock (yaz_mutex);
#endif
	if (no)
		while (ZOOM_event (no, conn_ar))
			;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int yaz_errno(int id)
   Return last error number (>0 for bib-1 diagnostic, <0 for other error, 0 for no error */
PHP_FUNCTION(yaz_errno)
{
	pval **id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &id) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, id, &p);
	if (!p)
	{
		RETURN_LONG(0);
	}
	RETVAL_LONG(ZOOM_connection_errcode (p->zoom_conn));
	release_assoc (p);
}
/* }}} */

/* {{{ proto string yaz_error(int id)
   Return last error message */
PHP_FUNCTION(yaz_error)
{
	pval **id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &id) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, id, &p);
	if (p)
	{
		int code = ZOOM_connection_errcode (p->zoom_conn);
		const char *msg = ZOOM_connection_errmsg (p->zoom_conn);
		if (!code)
			msg = "";
		return_value->value.str.len = strlen(msg);
		return_value->value.str.val =
			estrndup(msg, return_value->value.str.len);
		return_value->type = IS_STRING;
	}
	release_assoc (p);
}
/* }}} */

/* {{{ proto string yaz_addinfo(int id)
   Return additional info for last error (empty string if none) */
PHP_FUNCTION(yaz_addinfo)
{
	pval **id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &id) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, id, &p);
	if (p)
	{
		const char *addinfo = ZOOM_connection_addinfo (p->zoom_conn);
		return_value->value.str.len = strlen(addinfo);
		return_value->value.str.val =
			estrndup(addinfo, return_value->value.str.len);
		return_value->type = IS_STRING;
	}		 
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_hits(int id)
   Return number of hits (result count) for last search */
PHP_FUNCTION(yaz_hits)
{
	pval **id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &id) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, id, &p);
	if (p && p->zoom_set)
	{
		RETVAL_LONG(ZOOM_resultset_size (p->zoom_set));
	}
	else
	{
		RETVAL_LONG(0);
	}
	release_assoc (p);
}
/* }}} */

static Z_GenericRecord *marc_to_grs1(const char *buf, ODR o)
{
	int entry_p;
	int record_length;
	int indicator_length;
	int identifier_length;
	int base_address;
	int length_data_entry;
	int length_starting;
	int length_implementation;
	int max_elements = 256;
	Z_GenericRecord *r = odr_malloc (o, sizeof(*r));
	r->elements = odr_malloc (o, sizeof(*r->elements) * max_elements);
	r->num_elements = 0;
	
	record_length = atoi_n (buf, 5);
	if (record_length < 25)
		return 0;
	indicator_length = atoi_n (buf+10, 1);
	identifier_length = atoi_n (buf+11, 1);
	base_address = atoi_n (buf+12, 5);
	
	length_data_entry = atoi_n (buf+20, 1);
	length_starting = atoi_n (buf+21, 1);
	length_implementation = atoi_n (buf+22, 1);
	
	for (entry_p = 24; buf[entry_p] != ISO2709_FS; )
	{
		entry_p += 3+length_data_entry+length_starting;
		if (entry_p >= record_length)
			return 0;
	}
	base_address = entry_p+1;
	for (entry_p = 24; buf[entry_p] != ISO2709_FS; )
	{
		Z_TaggedElement *tag;
		int data_length;
		int data_offset;
		int end_offset;
		int i;
		char tag_str[4];
		int identifier_flag = 1;
		
		memcpy (tag_str, buf+entry_p, 3);
		entry_p += 3;
		tag_str[3] = '\0';
		
		if ((r->num_elements + 1) >= max_elements)
		{
			Z_TaggedElement **tmp = r->elements;
			
			/* double array space, throw away old buffer (nibble memory) */
			r->elements = odr_malloc(o, sizeof(*r->elements) *
									 (max_elements *= 2));
			memcpy(r->elements, tmp, r->num_elements * sizeof(*tmp));
		}
		tag = r->elements[r->num_elements++] = odr_malloc (o, sizeof(*tag));
		tag->tagType = odr_malloc(o, sizeof(*tag->tagType));
		*tag->tagType = 3;
		tag->tagOccurrence = 0;
		tag->metaData = 0;
		tag->appliedVariant = 0;
		tag->tagValue = odr_malloc (o, sizeof(*tag->tagValue));
		tag->tagValue->which = Z_StringOrNumeric_string;
		tag->tagValue->u.string = odr_strdup(o, tag_str);
		
		tag->content = odr_malloc(o, sizeof(*tag->content));
		tag->content->which = Z_ElementData_subtree;
		
		tag->content->u.subtree =
			odr_malloc (o, sizeof(*tag->content->u.subtree));
		tag->content->u.subtree->elements =
			odr_malloc (o, sizeof(*r->elements));
		tag->content->u.subtree->num_elements = 1;
		
		tag = tag->content->u.subtree->elements[0] =
			odr_malloc (o, sizeof(**tag->content->u.subtree->elements));
		
		tag->tagType = odr_malloc(o, sizeof(*tag->tagType));
		*tag->tagType = 3;
		tag->tagOccurrence = 0;
		tag->metaData = 0;
		tag->appliedVariant = 0;
		tag->tagValue = odr_malloc (o, sizeof(*tag->tagValue));
		tag->tagValue->which = Z_StringOrNumeric_string;
		tag->content = odr_malloc(o, sizeof(*tag->content));
		
		data_length = atoi_n (buf+entry_p, length_data_entry);
		entry_p += length_data_entry;
		data_offset = atoi_n (buf+entry_p, length_starting);
		entry_p += length_starting;
		i = data_offset + base_address;
		end_offset = i+data_length-1;

		
		if (indicator_length > 0 && indicator_length < 5)
		{
			if (buf[i + indicator_length] != ISO2709_IDFS)
				identifier_flag = 0;
		}
		else if (!memcmp (tag_str, "00", 2))
			identifier_flag = 0;

		
		if (identifier_flag && indicator_length)
		{
			/* indicator */
			tag->tagValue->u.string = odr_malloc(o, indicator_length+1);
			memcpy (tag->tagValue->u.string, buf + i, indicator_length);
			tag->tagValue->u.string[indicator_length] = '\0';
			i += indicator_length;
			
			tag->content->which = Z_ElementData_subtree;

			tag->content->u.subtree =
				odr_malloc (o, sizeof(*tag->content->u.subtree));
			tag->content->u.subtree->elements =
				odr_malloc (o, 256 * sizeof(*r->elements));
			tag->content->u.subtree->num_elements = 0;
			
			while (buf[i] != ISO2709_RS && buf[i] != ISO2709_FS
				   && i < end_offset)
			{
				int i0;
				/* prepare tag */
				Z_TaggedElement *parent_tag = tag;
				Z_TaggedElement *tag = odr_malloc (o, sizeof(*tag));
				
				if (parent_tag->content->u.subtree->num_elements < 256)
					parent_tag->content->u.subtree->elements[
					parent_tag->content->u.subtree->num_elements++] = tag;
				
				tag->tagType = odr_malloc(o, sizeof(*tag->tagType));
				*tag->tagType = 3;
				tag->tagOccurrence = 0;
				tag->metaData = 0;
				tag->appliedVariant = 0;
				tag->tagValue = odr_malloc (o, sizeof(*tag->tagValue));
				tag->tagValue->which = Z_StringOrNumeric_string;
				
				/* sub field */
				tag->tagValue->u.string = odr_malloc (o, identifier_length);
				memcpy (tag->tagValue->u.string, buf+i+1, identifier_length-1);
				tag->tagValue->u.string[identifier_length-1] = '\0';
				i += identifier_length;
				
				/* data ... */
				tag->content = odr_malloc(o, sizeof(*tag->content));
				tag->content->which = Z_ElementData_string;
				
				i0 = i;
				while (buf[i] != ISO2709_RS && buf[i] != ISO2709_IDFS &&
					   buf[i] != ISO2709_FS && i < end_offset)
					i++;
				
				tag->content->u.string = odr_malloc (o, i - i0 + 1);
				memcpy (tag->content->u.string, buf + i0, i - i0);
				tag->content->u.string[i - i0] = '\0';
			}
		}
		else
		{
			int i0 = i;
			
			tag->tagValue->u.string = "@";
			tag->content->which = Z_ElementData_string;
			
			while (buf[i] != ISO2709_RS && buf[i] != ISO2709_FS &&
				   i < end_offset)
				i++;
			tag->content->u.string = odr_malloc (o, i - i0 +1);
			memcpy (tag->content->u.string, buf+i0, i - i0);
			tag->content->u.string[i-i0] = '\0';
		}
	}
	return r;
}

static void retval_grs1 (zval *return_value, Z_GenericRecord *p)
{
	Z_GenericRecord *grs[20];
	int eno[20];
	int level = 0;

	if (array_init(return_value) == FAILURE)
	{
		RETURN_FALSE;
	}
	eno[level] = 0;
	grs[level] = p;
	while (level >= 0)
	{
		zval *my_zval;
		Z_TaggedElement *e = 0;
		Z_GenericRecord *p = grs[level];
		int i;
		char tag[256];
		int taglen = 0;

		if (eno[level] >= p->num_elements)
		{
			--level;
			if (level >= 0)
				eno[level]++;
			continue;
		}
		/* eno[level]++; */

		*tag = '\0';
		for (i = 0; i<=level; i++)
		{
			int tag_type = 3;
			e = grs[i]->elements[eno[i]];

			if (e->tagType)
				tag_type = *e->tagType;

			taglen = strlen(tag);
			sprintf (tag+taglen, "(%d,", tag_type);
			taglen = strlen(tag);

			if (e->tagValue->which == Z_StringOrNumeric_string)
			{
				int len = strlen(e->tagValue->u.string);
				memcpy (tag + taglen, e->tagValue->u.string, len);
				tag[taglen+len] = '\0';
			}
			else if (e->tagValue->which == Z_StringOrNumeric_numeric)
			{
				sprintf (tag + taglen, "%d", *e->tagValue->u.numeric);
			}
			taglen = strlen(tag);
			strcpy (tag + taglen, ")");
		}
		ALLOC_ZVAL(my_zval);
		array_init(my_zval);
		INIT_PZVAL(my_zval);
		
		add_next_index_string(my_zval, tag, 1);

		switch (e->content->which)
		{
		case Z_ElementData_string:
			add_next_index_string (my_zval, e->content->u.string, 1);
			break;
		case Z_ElementData_numeric:
			add_next_index_long (my_zval, *e->content->u.numeric);
			break;
		case Z_ElementData_trueOrFalse:
			add_next_index_long (my_zval, *e->content->u.trueOrFalse);
			break;
		case Z_ElementData_subtree:
			level++;
			grs[level] = e->content->u.subtree;
			eno[level] = -1;
		}
		zend_hash_next_index_insert (return_value->value.ht,
									 (void *) &my_zval, sizeof(zval *), NULL);
		eno[level]++;
	}
}


/* {{{ proto string yaz_record(int id, int pos, string type)
   Return record information at given result set position */
PHP_FUNCTION(yaz_record)
{
	pval **pval_id, **pval_pos, **pval_type;
	Yaz_Association p;
	int pos;
	char *type;

	if (ZEND_NUM_ARGS() != 3)
		WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(3, &pval_id, &pval_pos, &pval_type) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);

	convert_to_long_ex(pval_pos);
	pos = (*pval_pos)->value.lval;

	convert_to_string_ex(pval_type);
	type = (*pval_type)->value.str.val;
	if (p && p->zoom_set)
	{
		ZOOM_record r = ZOOM_resultset_record (p->zoom_set, pos-1);
		if (!strcmp(type, "string"))
			type = "render";
		if (r)
		{
			if (!strcmp (type, "array"))
			{
				Z_External *ext = (Z_External *) ZOOM_record_get (r, "ext", 0);
				oident *ent = oid_getentbyoid(ext->direct_reference);

				if (ext->which == Z_External_grs1 && ent->value == VAL_GRS1)
				{
					retval_grs1 (return_value, ext->u.grs1);
				}
				else if (ext->which == Z_External_octet)
				{
					char *buf = (char *) (ext->u.octet_aligned->buf);
					ODR odr = odr_createmem (ODR_DECODE);
					Z_GenericRecord *rec = 0;

					switch (ent->value)
					{
					case VAL_SOIF:
					case VAL_HTML:
						break;
					case VAL_TEXT_XML:
					case VAL_APPLICATION_XML:
						/* text2grs1 (&buf, &len, t->odr_in, 0, 0); */
						break;
					default:
						rec = marc_to_grs1 (buf, odr);
					}
					if (rec)
						retval_grs1 (return_value, rec);
					odr_destroy (odr);
				}
			}
            else
			{
                int rlen;
				const char *info = ZOOM_record_get (r, type, &rlen);

                return_value->value.str.len = (rlen > 0) ? rlen : 0;
                return_value->value.str.val =
                    estrndup(info, return_value->value.str.len);
                return_value->type = IS_STRING;
			}
		}
	}
	release_assoc (p);
}
/* }}} */


/* {{{ proto int yaz_syntax(int id, string syntax)
   Set record syntax for retrieval */
PHP_FUNCTION(yaz_syntax)
{
	pval **pval_id, **pval_syntax;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &pval_id, &pval_syntax) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	convert_to_string_ex (pval_syntax);
	option_set (p, "preferredRecordSyntax", (*pval_syntax)->value.str.val);
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_element(int id, string elementsetname)
   Set Element-Set-Name for retrieval */
PHP_FUNCTION(yaz_element)
{
	pval **pval_id, **pval_element;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &pval_id, &pval_element) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	convert_to_string_ex (pval_element);
	option_set (p, "elementSetName", (*pval_element)->value.str.val);
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_schema(int id, string schema)
   Set Schema for retrieval */
PHP_FUNCTION(yaz_schema)
{
	pval **pval_id, **pval_element;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &pval_id, &pval_element) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	convert_to_string_ex (pval_element);
    option_set (p, "schema", (*pval_element)->value.str.val);
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_range(int id, int start, int number)
   Set result set start point and number of records to request */

PHP_FUNCTION(yaz_range)
{
	pval **pval_id, **pval_start, **pval_number;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3, &pval_id, &pval_start, &pval_number) ==
		FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	convert_to_long_ex (pval_start);
	convert_to_long_ex (pval_number);
	option_set_int (p, "start",  (*pval_start)->value.lval - 1);
	option_set_int (p, "count",  (*pval_number)->value.lval);
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_sort(int id, string sortspec)
   Set result set sorting criteria */

PHP_FUNCTION(yaz_sort)
{
	pval **pval_id, **pval_criteria;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &pval_id, &pval_criteria) ==
		FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	if (p)
	{
		convert_to_string_ex (pval_criteria);
		xfree (p->sort_criteria);
		p->sort_criteria = xstrdup ((*pval_criteria)->value.str.val);
	}
	release_assoc (p);
}
/* }}} */

const char *ill_array_lookup (void *handle, const char *name)
{
	return array_lookup_string((HashTable *) handle, name);
}


/* {{{ proto int yaz_itemorder(int id, array package)
   Sends Item Order request */

PHP_FUNCTION(yaz_itemorder)
{
	pval **pval_id, **pval_package;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &pval_id, &pval_package) ==
		FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	if (Z_TYPE_PP(pval_package) != IS_ARRAY)
	{
		php_error(E_WARNING, "yaz_itemorder: Expected array parameter");
		RETURN_FALSE;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	if (p)
	{
        ZOOM_options options = ZOOM_options_create();
        
        ZOOM_options_set_callback (options, ill_array_lookup,
                                   Z_ARRVAL_PP(pval_package));
        ZOOM_package_destroy (p->zoom_package);
        p->zoom_package = ZOOM_connection_package (p->zoom_conn, options);
        ZOOM_package_send(p->zoom_package, "itemorder");
        ZOOM_options_destroy (options);
	}
	release_assoc (p);
}
/* }}} */



/* {{{ proto int yaz_scan(int id, type, query [, flags])
   Sends Scan Request */
PHP_FUNCTION(yaz_scan)
{
	pval **pval_id, **pval_type, **pval_query, **pval_flags = 0;
	HashTable *flags_ht = 0;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() == 3)
	{	
		if (zend_get_parameters_ex(3, &pval_id, &pval_type, &pval_query) ==
			FAILURE)
		{
			WRONG_PARAM_COUNT;
		}
	}
	else if (ZEND_NUM_ARGS() == 4)
	{
		if (zend_get_parameters_ex(4, &pval_id, &pval_type, &pval_query,
								   &pval_flags) ==
			FAILURE)
		{
			WRONG_PARAM_COUNT;
		}
		if (Z_TYPE_PP(pval_flags) != IS_ARRAY)
		{
			php_error(E_WARNING, "yaz_scan: Bad flags parameter");
			RETURN_FALSE;
		}
		flags_ht = Z_ARRVAL_PP(pval_flags);
	}
	else
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex (pval_type);
	convert_to_string_ex (pval_query);

	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	ZOOM_scanset_destroy (p->zoom_scan);
	p->zoom_scan = 0;
	if (p)
	{
		option_set (p, "number", array_lookup_string (flags_ht, "number"));
		option_set (p, "position", array_lookup_string (flags_ht, "position"));
		option_set (p, "stepSize", array_lookup_string (flags_ht, "stepsize"));
		p->zoom_scan = ZOOM_connection_scan (p->zoom_conn,
											 Z_STRVAL_PP(pval_query));}
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_es_result(int id)
   Inspects Extended Services Result */
PHP_FUNCTION(yaz_es_result)
{
	pval **pval_id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() == 1)
	{
		if (zend_get_parameters_ex(1, &pval_id) == FAILURE)
		{
			WRONG_PARAM_COUNT;
		}
	}
	else
	{
		WRONG_PARAM_COUNT;
	}
	if (array_init(return_value) == FAILURE)
	{
		RETURN_FALSE;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
    if (p && p->zoom_package)
    {
        const char *str = ZOOM_package_option_get(p->zoom_package,
                                                  "targetReference");
        if (str)
            add_assoc_string (return_value, "targetReference", 
                              (char*)str, 1);
    }
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_scan_result(int id, array options)
   Inspects Scan Result */
PHP_FUNCTION(yaz_scan_result)
{
	pval **pval_id, **pval_opt = 0;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() == 2)
	{	
		if (zend_get_parameters_ex(2, &pval_id, &pval_opt) == FAILURE)
		{
			WRONG_PARAM_COUNT;
		}
	}
	else if (ZEND_NUM_ARGS() == 1)
	{
		if (zend_get_parameters_ex(1, &pval_id) == FAILURE)
		{
			WRONG_PARAM_COUNT;
		}
	}
	else
	{
		WRONG_PARAM_COUNT;
	}
	if (array_init(return_value) == FAILURE)
	{
		RETURN_FALSE;
	}
	if (pval_opt && array_init(*pval_opt) == FAILURE)
	{
		RETURN_FALSE;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	if (p && p->zoom_scan)
	{
		int pos = 0;
		int occ, len;
		int size = ZOOM_scanset_size (p->zoom_scan);
		
		for (pos = 0; pos < size; pos++)
		{
			const char *term =
				ZOOM_scanset_term(p->zoom_scan, pos, &occ, &len);
			zval *my_zval;
			ALLOC_ZVAL(my_zval);
			array_init(my_zval);
			INIT_PZVAL(my_zval);
			
			add_next_index_string(my_zval, "term", 1);

			if (term)
				add_next_index_stringl (my_zval, (char*) term, len, 1);
			else
				add_next_index_string (my_zval, "?", 1);
			add_next_index_long (my_zval, occ);
			
			zend_hash_next_index_insert (
				return_value->value.ht, (void *) &my_zval, sizeof(zval *),
				NULL);
		}
		if (pval_opt)
		{
			const char *v;
			add_assoc_long(*pval_opt, "number", size);
			
			v = ZOOM_scanset_option_get (p->zoom_scan, "stepSize");
			if (v)
				add_assoc_long(*pval_opt, "stepsize", atoi(v));
			
			v = ZOOM_scanset_option_get (p->zoom_scan, "position");
			if (v)
				add_assoc_long(*pval_opt, "position", atoi(v));

			v = ZOOM_scanset_option_get (p->zoom_scan, "scanStatus");
			if (v)
				add_assoc_long(*pval_opt, "status", atoi(v));
		}
	}
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_ccl_conf(int id, array package)
   Configure CCL package */

PHP_FUNCTION(yaz_ccl_conf)
{
	pval **pval_id, **pval_package;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &pval_id, &pval_package) ==
		FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	if (Z_TYPE_PP(pval_package) != IS_ARRAY)
	{
		php_error(E_WARNING, "yaz_ccl_conf: Expected array parameter");
		RETURN_FALSE;
	}
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	if (p)
	{
		HashTable *ht = Z_ARRVAL_PP(pval_package);
		HashPosition pos;
		zval **ent;
		char *key;

		ccl_qual_rm(&p->ccl_parser->bibset);
		p->ccl_parser->bibset = ccl_qual_mk();
		for(zend_hash_internal_pointer_reset_ex(ht, &pos);
			zend_hash_get_current_data_ex(ht, (void**) &ent, &pos) == SUCCESS;
			zend_hash_move_forward_ex(ht, &pos)) 
		{
			ulong idx;
#if PHP_API_VERSION > 20010101
			int type = zend_hash_get_current_key_ex(ht, &key, 0, 
													&idx, 0, &pos);
#else
			int type = zend_hash_get_current_key_ex(ht, &key, 0, 
													&idx, &pos);
#endif
			if (type != HASH_KEY_IS_STRING || Z_TYPE_PP(ent) != IS_STRING)
				continue;
			ccl_qual_fitem(p->ccl_parser->bibset, (*ent)->value.str.val, key);
		}
	}
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_ccl_parse(int id, string query, array res)
   Parse a CCL query */

PHP_FUNCTION(yaz_ccl_parse)
{
	pval **pval_id, **pval_query, **pval_res = 0;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3, &pval_id, &pval_query, &pval_res) ==
		FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	
	pval_destructor(*pval_res);
	if (array_init(*pval_res) == FAILURE)
	{
		php_error(E_WARNING, "cannot initialize array");
		RETURN_FALSE;
	}
	convert_to_string_ex (pval_query);
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	if (p)
	{
		const char *query_str = (*pval_query)->value.str.val;
		struct ccl_rpn_node *rpn;
		struct ccl_token *token_list =
			ccl_parser_tokenize(p->ccl_parser, query_str);
		rpn = ccl_parser_find(p->ccl_parser, token_list);
		ccl_token_del(token_list);

		add_assoc_long(*pval_res, "errorcode", p->ccl_parser->error_code);
		if (p->ccl_parser->error_code)
		{
			add_assoc_string(*pval_res, "errorstring",
							 (char*) ccl_err_msg(p->ccl_parser->error_code),
							 1);
			add_assoc_long(*pval_res, "errorpos",
						   p->ccl_parser->error_pos - query_str);
			RETVAL_FALSE;
		}
		else
		{
			WRBUF wrbuf_pqf = wrbuf_alloc();
			ccl_pquery(wrbuf_pqf, rpn);
			add_assoc_stringl(*pval_res, "rpn", wrbuf_buf(wrbuf_pqf),
							  wrbuf_len(wrbuf_pqf),1);
			wrbuf_free(wrbuf_pqf, 1);
			RETVAL_TRUE;
		}
		ccl_rpn_delete(rpn);
	}
	else
		RETVAL_FALSE;
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_database (int id, string databases)
   Specify the databases within a session */

PHP_FUNCTION(yaz_database)
{
	pval **pval_id, **pval_database;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &pval_id, &pval_database) ==
		FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex (pval_database);
	get_assoc (INTERNAL_FUNCTION_PARAM_PASSTHRU, pval_id, &p);
	option_set (p, "databaseName", (*pval_database)->value.str.val);
	RETVAL_TRUE;
	release_assoc (p);
}
/* }}} */


/* {{{ php_yaz_init_globals
 */
static void php_yaz_init_globals(zend_yaz_globals *yaz_globals)
{
	yaz_globals->assoc_seq = 0;
}
/* }}} */

void yaz_close_session(Yaz_Association *as TSRMLS_DC)
{
	if (*as && (*as)->order == YAZSG(assoc_seq))
	{
		if ((*as)->persistent)
			(*as)->in_use = 0;
		else
		{
			yaz_association_destroy(*as);
			*as = 0;
		}
	}
}

static void yaz_close_link (zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	Yaz_Association *as = (Yaz_Association *) rsrc->ptr;
	yaz_close_session (as TSRMLS_CC);
}

/* {{{ PHP_INI_BEGIN
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("yaz.max_links", "100", PHP_INI_ALL,
                      OnUpdateInt, max_links,
                      zend_yaz_globals, yaz_globals)
    STD_PHP_INI_ENTRY("yaz.log_file", "", PHP_INI_ALL,
                      OnUpdateString, log_file,
                      zend_yaz_globals, yaz_globals)
    PHP_INI_END()
/* }}} */
    
PHP_MINIT_FUNCTION(yaz)
{
	int i;
	nmem_init();
#ifdef ZTS
	yaz_mutex = tsrm_mutex_alloc();
#endif
    yaz_log_init_file ("/dev/null");
	ZEND_INIT_MODULE_GLOBALS(yaz, php_yaz_init_globals, NULL);

    REGISTER_INI_ENTRIES();

    if (YAZSG(log_file))
    {
        yaz_log_init_file(YAZSG(log_file));
        yaz_log_init_level (LOG_ALL);
    }
	le_link = zend_register_list_destructors_ex (yaz_close_link, 0,
												"YAZ link", module_number);
	order_associations = 1;
	shared_associations = xmalloc (sizeof(*shared_associations) * MAX_ASSOC);
	for (i = 0; i<MAX_ASSOC; i++)
		shared_associations[i] = 0;
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(yaz)
{
	int i;

	if (shared_associations)
	{
		for (i = 0; i<MAX_ASSOC; i++)
			yaz_association_destroy (shared_associations[i]);
		xfree (shared_associations);
		shared_associations = 0;
		nmem_exit();
	}
#ifdef ZTS
	tsrm_mutex_free (yaz_mutex);
#endif
	return SUCCESS;
}

PHP_MINFO_FUNCTION(yaz)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "YAZ Support", "enabled");
	php_info_print_table_row(2, "YAZ Version", YAZ_VERSION);
	php_info_print_table_row(2, "ZOOM", "enabled");
	php_info_print_table_end();
}

PHP_RSHUTDOWN_FUNCTION(yaz)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(yaz)
{
    char pidstr[20];
    sprintf (pidstr, "%ld", (long) getpid());
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	YAZSG(assoc_seq) = order_associations++;
#ifdef ZTS
	tsrm_mutex_unlock (yaz_mutex);
#endif
    yaz_log_init_prefix(pidstr);
	return SUCCESS;
}

zend_module_entry yaz_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"yaz",
	yaz_functions,
	PHP_MINIT(yaz),
	PHP_MSHUTDOWN(yaz),
	PHP_RINIT(yaz),
	PHP_RSHUTDOWN(yaz),
	PHP_MINFO(yaz),
#if ZEND_MODULE_API_NO >= 20010901
	NO_VERSION_YET,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_YAZ
ZEND_GET_MODULE(yaz)
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
