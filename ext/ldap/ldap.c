/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Authors: Amitay Isaacs  <amitay@w-o-i.com>                           |
   |          Eric Warnke    <ericw@albany.edu>                           |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Gerrit Thomson <334647@swin.edu.au>                         |
   |          Jani Taskinen  <sniper@iki.fi>                              |
   |          Stig Venaas    <venaas@uninett.no>                          |
   |          Doug Goldstein <cardoe@cardoe.com>                          |
   |          Côme Chilliet  <mcmic@php.net>                              |
   | PHP 4.0 updates:  Zeev Suraski <zeev@php.net>                        |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"

#include <stddef.h>

#include "ext/standard/dl.h"
#include "php_ldap.h"

#ifdef PHP_WIN32
#include <string.h>
#include "config.w32.h"
#define strdup _strdup
#undef WINDOWS
#undef strcasecmp
#undef strncasecmp
#define WINSOCK 1
#define __STDC__ 1
#endif

#include "ext/standard/php_string.h"
#include "ext/standard/info.h"

#ifdef HAVE_LDAP_SASL
#include <sasl/sasl.h>
#endif

#define PHP_LDAP_ESCAPE_FILTER 0x01
#define PHP_LDAP_ESCAPE_DN     0x02

#if defined(LDAP_CONTROL_PAGEDRESULTS) && !defined(HAVE_LDAP_CONTROL_FIND)
LDAPControl *ldap_control_find( const char *oid, LDAPControl **ctrls, LDAPControl ***nextctrlp)
{
	assert(nextctrlp == NULL);
	return ldap_find_control(oid, ctrls);
}
#endif

#if !defined(LDAP_API_FEATURE_X_OPENLDAP)
void ldap_memvfree(void **v)
{
	ldap_value_free((char **)v);
}
#endif

typedef struct {
	LDAP *link;
#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
	zval rebindproc;
#endif
} ldap_linkdata;

typedef struct {
	LDAPMessage *data;
	BerElement  *ber;
	zval         res;
} ldap_resultentry;

ZEND_DECLARE_MODULE_GLOBALS(ldap)
static PHP_GINIT_FUNCTION(ldap);

static int le_link, le_result, le_result_entry;

#ifdef COMPILE_DL_LDAP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(ldap)
#endif

static void _close_ldap_link(zend_resource *rsrc) /* {{{ */
{
	ldap_linkdata *ld = (ldap_linkdata *)rsrc->ptr;

	/* We use ldap_destroy rather than ldap_unbind here, because ldap_unbind
	 * will skip the destructor entirely if a critical client control is set. */
	ldap_destroy(ld->link);

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
	zval_ptr_dtor(&ld->rebindproc);
#endif

	efree(ld);
	LDAPG(num_links)--;
}
/* }}} */

static void _free_ldap_result(zend_resource *rsrc) /* {{{ */
{
	LDAPMessage *result = (LDAPMessage *)rsrc->ptr;
	ldap_msgfree(result);
}
/* }}} */

static void _free_ldap_result_entry(zend_resource *rsrc) /* {{{ */
{
	ldap_resultentry *entry = (ldap_resultentry *)rsrc->ptr;

	if (entry->ber != NULL) {
		ber_free(entry->ber, 0);
		entry->ber = NULL;
	}
	zval_ptr_dtor(&entry->res);
	efree(entry);
}
/* }}} */

/* {{{ Parse controls from and to arrays */
static void _php_ldap_control_to_array(LDAP *ld, LDAPControl* ctrl, zval* array, int request)
{
	array_init(array);

	add_assoc_string(array, "oid", ctrl->ldctl_oid);
	if (request) {
		/* iscritical field only makes sense in request controls (which may be obtained by ldap_get_option) */
		add_assoc_bool(array, "iscritical", (ctrl->ldctl_iscritical != 0));
	}

	/* If it is a known oid, parse to values */
	if (strcmp(ctrl->ldctl_oid, LDAP_CONTROL_PASSWORDPOLICYRESPONSE) == 0) {
		int expire = 0, grace = 0, rc;
		LDAPPasswordPolicyError pperr;
		zval value;

		rc = ldap_parse_passwordpolicy_control(ld, ctrl, &expire, &grace, &pperr);
		if ( rc == LDAP_SUCCESS ) {
			array_init(&value);
			add_assoc_long(&value, "expire", expire);
			add_assoc_long(&value, "grace", grace);

			if ( pperr != PP_noError ) {
				add_assoc_long(&value, "error", pperr);
			}
			add_assoc_zval(array, "value", &value);
		} else {
			add_assoc_null(array, "value");
		}
	} else if (strcmp(ctrl->ldctl_oid, LDAP_CONTROL_PAGEDRESULTS) == 0) {
		int lestimated, rc;
		struct berval lcookie = { 0L, NULL };
		zval value;

		if (ctrl->ldctl_value.bv_len) {
			/* ldap_parse_pageresponse_control() allocates lcookie.bv_val */
			rc = ldap_parse_pageresponse_control(ld, ctrl, &lestimated, &lcookie);
		} else {
			/* ldap_parse_pageresponse_control will crash if value is empty */
			rc = -1;
		}

		if ( rc == LDAP_SUCCESS ) {
			array_init(&value);
			add_assoc_long(&value, "size", lestimated);
			add_assoc_stringl(&value, "cookie", lcookie.bv_val, lcookie.bv_len);
			add_assoc_zval(array, "value", &value);
		} else {
			add_assoc_null(array, "value");
		}

		if (lcookie.bv_val) {
			ldap_memfree(lcookie.bv_val);
		}
	} else if ((strcmp(ctrl->ldctl_oid, LDAP_CONTROL_PRE_READ) == 0) || (strcmp(ctrl->ldctl_oid, LDAP_CONTROL_POST_READ) == 0)) {
		BerElement *ber;
		struct berval bv;

		ber = ber_init(&ctrl->ldctl_value);
		if (ber == NULL) {
			add_assoc_null(array, "value");
		} else if (ber_scanf(ber, "{m{" /*}}*/, &bv) == LBER_ERROR) {
			add_assoc_null(array, "value");
		} else {
			zval value;

			array_init(&value);
			add_assoc_stringl(&value, "dn", bv.bv_val, bv.bv_len);

			while (ber_scanf(ber, "{m" /*}*/, &bv) != LBER_ERROR) {
				int	 i;
				BerVarray vals = NULL;
				zval tmp;

				if (ber_scanf(ber, "[W]", &vals) == LBER_ERROR || vals == NULL)
				{
					break;
				}

				array_init(&tmp);
				for (i = 0; vals[i].bv_val != NULL; i++) {
					add_next_index_stringl(&tmp, vals[i].bv_val, vals[i].bv_len);
				}
				add_assoc_zval(&value, bv.bv_val, &tmp);

				ber_bvarray_free(vals);
			}
			add_assoc_zval(array, "value", &value);
		}

		if (ber != NULL) {
			ber_free(ber, 1);
		}
	} else if (strcmp(ctrl->ldctl_oid, LDAP_CONTROL_SORTRESPONSE) == 0) {
		zval value;
		int errcode, rc;
		char* attribute;

		if (ctrl->ldctl_value.bv_len) {
			rc = ldap_parse_sortresponse_control(ld, ctrl, &errcode, &attribute);
		} else {
			rc = -1;
		}
		if ( rc == LDAP_SUCCESS ) {
			array_init(&value);
			add_assoc_long(&value, "errcode", errcode);
			if (attribute) {
				add_assoc_string(&value, "attribute", attribute);
				ldap_memfree(attribute);
			}
			add_assoc_zval(array, "value", &value);
		} else {
			add_assoc_null(array, "value");
		}
	} else if (strcmp(ctrl->ldctl_oid, LDAP_CONTROL_VLVRESPONSE) == 0) {
		int target, count, errcode, rc;
		struct berval *context;
		zval value;

		if (ctrl->ldctl_value.bv_len) {
			rc = ldap_parse_vlvresponse_control(ld, ctrl, &target, &count, &context, &errcode);
		} else {
			rc = -1;
		}
		if ( rc == LDAP_SUCCESS ) {
			array_init(&value);
			add_assoc_long(&value, "target", target);
			add_assoc_long(&value, "count", count);
			add_assoc_long(&value, "errcode", errcode);
			if ( context && (context->bv_len >= 0) ) {
				add_assoc_stringl(&value, "context", context->bv_val, context->bv_len);
			}
			add_assoc_zval(array, "value", &value);
			ber_bvfree(context);
		} else {
			add_assoc_null(array, "value");
		}
	} else {
		if (ctrl->ldctl_value.bv_len) {
			add_assoc_stringl(array, "value", ctrl->ldctl_value.bv_val, ctrl->ldctl_value.bv_len);
		} else {
			add_assoc_null(array, "value");
		}
	}
}

static int _php_ldap_control_from_array(LDAP *ld, LDAPControl** ctrl, zval* array)
{
	zval* val;
	zend_string *control_oid;
	int control_iscritical = 0, rc = LDAP_SUCCESS;
	char** ldap_attrs = NULL;
	LDAPSortKey** sort_keys = NULL;
	zend_string *tmpstring = NULL, **tmpstrings1 = NULL, **tmpstrings2 = NULL;
	size_t num_tmpstrings1 = 0, num_tmpstrings2 = 0;

	if ((val = zend_hash_str_find(Z_ARRVAL_P(array), "oid", sizeof("oid") - 1)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Control must have an oid key");
		return -1;
	}

	control_oid = zval_get_string(val);
	if (EG(exception)) {
		return -1;
	}

	if ((val = zend_hash_str_find(Z_ARRVAL_P(array), "iscritical", sizeof("iscritical") - 1)) != NULL) {
		control_iscritical = zend_is_true(val);
	} else {
		control_iscritical = 0;
	}

	BerElement *ber = NULL;
	struct berval control_value = { 0L, NULL };
	int control_value_alloc = 0;

	if ((val = zend_hash_str_find(Z_ARRVAL_P(array), "value", sizeof("value") - 1)) != NULL) {
		if (Z_TYPE_P(val) != IS_ARRAY) {
			tmpstring = zval_get_string(val);
			if (EG(exception)) {
				rc = -1;
				goto failure;
			}
			control_value.bv_val = ZSTR_VAL(tmpstring);
			control_value.bv_len = ZSTR_LEN(tmpstring);
		} else if (strcmp(ZSTR_VAL(control_oid), LDAP_CONTROL_PAGEDRESULTS) == 0) {
			zval* tmp;
			int pagesize = 1;
			struct berval cookie = { 0L, NULL };
			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "size", sizeof("size") - 1)) != NULL) {
				pagesize = zval_get_long(tmp);
			}
			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "cookie", sizeof("cookie") - 1)) != NULL) {
				tmpstring = zval_get_string(tmp);
				if (EG(exception)) {
					rc = -1;
					goto failure;
				}
				cookie.bv_val = ZSTR_VAL(tmpstring);
				cookie.bv_len = ZSTR_LEN(tmpstring);
			}
			/* ldap_create_page_control_value() allocates memory for control_value.bv_val */
			control_value_alloc = 1;
			rc = ldap_create_page_control_value(ld, pagesize, &cookie, &control_value);
			if (rc != LDAP_SUCCESS) {
				php_error_docref(NULL, E_WARNING, "Failed to create paged result control value: %s (%d)", ldap_err2string(rc), rc);
			}
		} else if (strcmp(ZSTR_VAL(control_oid), LDAP_CONTROL_ASSERT) == 0) {
			zval* tmp;
			zend_string* assert;
			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "filter", sizeof("filter") - 1)) == NULL) {
				rc = -1;
				php_error_docref(NULL, E_WARNING, "Filter missing from assert control value array");
			} else {
				assert = zval_get_string(tmp);
				if (EG(exception)) {
					rc = -1;
					goto failure;
				}
				/* ldap_create_assertion_control_value does not reset ld_errno, we need to do it ourselves
					 See http://www.openldap.org/its/index.cgi/Incoming?id=8674 */
				int success = LDAP_SUCCESS;
				ldap_set_option(ld, LDAP_OPT_RESULT_CODE, &success);
				/* ldap_create_assertion_control_value() allocates memory for control_value.bv_val */
				control_value_alloc = 1;
				rc = ldap_create_assertion_control_value(ld, ZSTR_VAL(assert), &control_value);
				if (rc != LDAP_SUCCESS) {
					php_error_docref(NULL, E_WARNING, "Failed to create assert control value: %s (%d)", ldap_err2string(rc), rc);
				}
				zend_string_release(assert);
			}
		} else if (strcmp(ZSTR_VAL(control_oid), LDAP_CONTROL_VALUESRETURNFILTER) == 0) {
			zval* tmp;
			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "filter", sizeof("filter") - 1)) == NULL) {
				rc = -1;
				php_error_docref(NULL, E_WARNING, "Filter missing from control value array");
			} else {
				ber = ber_alloc_t(LBER_USE_DER);
				if (ber == NULL) {
					rc = -1;
					php_error_docref(NULL, E_WARNING, "Failed to allocate control value");
				} else {
					tmpstring = zval_get_string(tmp);
					if (EG(exception)) {
						rc = -1;
						goto failure;
					}
					if (ldap_put_vrFilter(ber, ZSTR_VAL(tmpstring)) == -1) {
						rc = -1;
						php_error_docref(NULL, E_WARNING, "Failed to create control value: Bad ValuesReturnFilter: %s", ZSTR_VAL(tmpstring));
					} else if (ber_flatten2(ber, &control_value, control_value_alloc) == -1) {
						rc = -1;
					}
				}
			}
		} else if ((strcmp(ZSTR_VAL(control_oid), LDAP_CONTROL_PRE_READ) == 0) || (strcmp(ZSTR_VAL(control_oid), LDAP_CONTROL_POST_READ) == 0)) {
			zval* tmp;
			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "attrs", sizeof("attrs") - 1)) == NULL) {
				rc = -1;
				php_error_docref(NULL, E_WARNING, "Attributes list missing from control value array");
			} else {
				ber = ber_alloc_t(LBER_USE_DER);

				if (ber == NULL) {
					rc = -1;
					php_error_docref(NULL, E_WARNING, "Failed to allocate control value");
				} else {
					int num_attribs, i;
					zval* attr;

					num_attribs = zend_hash_num_elements(Z_ARRVAL_P(tmp));
					ldap_attrs = safe_emalloc((num_attribs+1), sizeof(char *), 0);
					tmpstrings1 = safe_emalloc(num_attribs, sizeof(zend_string*), 0);
					num_tmpstrings1 = 0;

					for (i = 0; i<num_attribs; i++) {
						if ((attr = zend_hash_index_find(Z_ARRVAL_P(tmp), i)) == NULL) {
							rc = -1;
							php_error_docref(NULL, E_WARNING, "Failed to encode attribute list");
							goto failure;
						}

						tmpstrings1[num_tmpstrings1] = zval_get_string(attr);
						if (EG(exception)) {
							rc = -1;
							goto failure;
						}
						ldap_attrs[i] = ZSTR_VAL(tmpstrings1[num_tmpstrings1]);
						++num_tmpstrings1;
					}
					ldap_attrs[num_attribs] = NULL;

					ber_init2( ber, NULL, LBER_USE_DER );

					if (ber_printf(ber, "{v}", ldap_attrs) == -1) {
						rc = -1;
						php_error_docref(NULL, E_WARNING, "Failed to encode attribute list");
					} else {
						int err;
						err = ber_flatten2(ber, &control_value, control_value_alloc);
						if (err < 0) {
							rc = -1;
							php_error_docref(NULL, E_WARNING, "Failed to encode control value (%d)", err);
						}
					}
				}
			}
		} else if (strcmp(ZSTR_VAL(control_oid), LDAP_CONTROL_SORTREQUEST) == 0) {
			int num_keys, i;
			zval *sortkey, *tmp;

			num_keys = zend_hash_num_elements(Z_ARRVAL_P(val));
			sort_keys = safe_emalloc((num_keys+1), sizeof(LDAPSortKey*), 0);
			tmpstrings1 = safe_emalloc(num_keys, sizeof(zend_string*), 0);
			tmpstrings2 = safe_emalloc(num_keys, sizeof(zend_string*), 0);
			num_tmpstrings1 = 0;
			num_tmpstrings2 = 0;

			for (i = 0; i<num_keys; i++) {
				if ((sortkey = zend_hash_index_find(Z_ARRVAL_P(val), i)) == NULL) {
					rc = -1;
					php_error_docref(NULL, E_WARNING, "Failed to encode sort keys list");
					goto failure;
				}

				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(sortkey), "attr", sizeof("attr") - 1)) == NULL) {
					rc = -1;
					php_error_docref(NULL, E_WARNING, "Sort key list missing field");
					goto failure;
				}
				sort_keys[i] = emalloc(sizeof(LDAPSortKey));
				tmpstrings1[num_tmpstrings1] = zval_get_string(tmp);
				if (EG(exception)) {
					rc = -1;
					goto failure;
				}
				sort_keys[i]->attributeType = ZSTR_VAL(tmpstrings1[num_tmpstrings1]);
				++num_tmpstrings1;

				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(sortkey), "oid", sizeof("oid") - 1)) != NULL) {
					tmpstrings2[num_tmpstrings2] = zval_get_string(tmp);
					if (EG(exception)) {
						rc = -1;
						goto failure;
					}
					sort_keys[i]->orderingRule = ZSTR_VAL(tmpstrings2[num_tmpstrings2]);
					++num_tmpstrings2;
				} else {
					sort_keys[i]->orderingRule = NULL;
				}

				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(sortkey), "reverse", sizeof("reverse") - 1)) != NULL) {
					sort_keys[i]->reverseOrder = zend_is_true(tmp);
				} else {
					sort_keys[i]->reverseOrder = 0;
				}
			}
			sort_keys[num_keys] = NULL;
			/* ldap_create_sort_control_value() allocates memory for control_value.bv_val */
			control_value_alloc = 1;
			rc = ldap_create_sort_control_value(ld, sort_keys, &control_value);
			if (rc != LDAP_SUCCESS) {
				php_error_docref(NULL, E_WARNING, "Failed to create sort control value: %s (%d)", ldap_err2string(rc), rc);
			}
		} else if (strcmp(ZSTR_VAL(control_oid), LDAP_CONTROL_VLVREQUEST) == 0) {
			zval* tmp;
			LDAPVLVInfo vlvInfo;
			struct berval attrValue;
			struct berval context;

			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "before", sizeof("before") - 1)) != NULL) {
				vlvInfo.ldvlv_before_count = zval_get_long(tmp);
			} else {
				rc = -1;
				php_error_docref(NULL, E_WARNING, "Before key missing from array value for VLV control");
				goto failure;
			}

			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "after", sizeof("after") - 1)) != NULL) {
				vlvInfo.ldvlv_after_count = zval_get_long(tmp);
			} else {
				rc = -1;
				php_error_docref(NULL, E_WARNING, "After key missing from array value for VLV control");
				goto failure;
			}

			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "attrvalue", sizeof("attrvalue") - 1)) != NULL) {
				tmpstring = zval_get_string(tmp);
				if (EG(exception)) {
					rc = -1;
					goto failure;
				}
				attrValue.bv_val = ZSTR_VAL(tmpstring);
				attrValue.bv_len = ZSTR_LEN(tmpstring);
				vlvInfo.ldvlv_attrvalue = &attrValue;
			} else if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "offset", sizeof("offset") - 1)) != NULL) {
				vlvInfo.ldvlv_attrvalue = NULL;
				vlvInfo.ldvlv_offset = zval_get_long(tmp);
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "count", sizeof("count") - 1)) != NULL) {
					vlvInfo.ldvlv_count = zval_get_long(tmp);
				} else {
					rc = -1;
					php_error_docref(NULL, E_WARNING, "Count key missing from array value for VLV control");
					goto failure;
				}
			} else {
				rc = -1;
				php_error_docref(NULL, E_WARNING, "Missing either attrvalue or offset key from array value for VLV control");
				goto failure;
			}

			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(val), "context", sizeof("context") - 1)) != NULL) {
				tmpstring = zval_get_string(tmp);
				if (EG(exception)) {
					rc = -1;
					goto failure;
				}
				context.bv_val = ZSTR_VAL(tmpstring);
				context.bv_len = ZSTR_LEN(tmpstring);
				vlvInfo.ldvlv_context = &context;
			} else {
				vlvInfo.ldvlv_context = NULL;
			}

			/* ldap_create_vlv_control_value() allocates memory for control_value.bv_val */
			control_value_alloc = 1;
			rc = ldap_create_vlv_control_value(ld, &vlvInfo, &control_value);
			if (rc != LDAP_SUCCESS) {
				php_error_docref(NULL, E_WARNING, "Failed to create VLV control value: %s (%d)", ldap_err2string(rc), rc);
			}
		} else {
			php_error_docref(NULL, E_WARNING, "Control OID %s does not expect an array as value", ZSTR_VAL(control_oid));
			rc = -1;
		}
	}

	if (rc == LDAP_SUCCESS) {
		rc = ldap_control_create(ZSTR_VAL(control_oid), control_iscritical, &control_value, 1, ctrl);
	}

failure:
	zend_string_release(control_oid);
	if (tmpstring != NULL) {
		zend_string_release(tmpstring);
	}
	if (tmpstrings1 != NULL) {
		int i;
		for (i = 0; i < num_tmpstrings1; ++i) {
			zend_string_release(tmpstrings1[i]);
		}
		efree(tmpstrings1);
	}
	if (tmpstrings2 != NULL) {
		int i;
		for (i = 0; i < num_tmpstrings2; ++i) {
			zend_string_release(tmpstrings2[i]);
		}
		efree(tmpstrings2);
	}
	if (control_value.bv_val != NULL && control_value_alloc != 0) {
		ber_memfree(control_value.bv_val);
	}
	if (ber != NULL) {
		ber_free(ber, 1);
	}
	if (ldap_attrs != NULL) {
		efree(ldap_attrs);
	}
	if (sort_keys != NULL) {
		LDAPSortKey** sortp = sort_keys;
		while (*sortp) {
			efree(*sortp);
			sortp++;
		}
		efree(sort_keys);
		sort_keys = NULL;
	}

	if (rc == LDAP_SUCCESS) {
		return LDAP_SUCCESS;
	}

	/* Failed */
	*ctrl = NULL;
	return -1;
}

static void _php_ldap_controls_to_array(LDAP *ld, LDAPControl** ctrls, zval* array, int request)
{
	zval tmp1;
	LDAPControl **ctrlp;

	array = zend_try_array_init(array);
	if (!array) {
		return;
	}

	if (ctrls == NULL) {
		return;
	}
	ctrlp = ctrls;
	while (*ctrlp != NULL) {
		_php_ldap_control_to_array(ld, *ctrlp, &tmp1, request);
		add_assoc_zval(array, (*ctrlp)->ldctl_oid, &tmp1);
		ctrlp++;
	}
	ldap_controls_free(ctrls);
}

static LDAPControl** _php_ldap_controls_from_array(LDAP *ld, zval* array)
{
	int ncontrols;
	LDAPControl** ctrlp, **ctrls = NULL;
	zval* ctrlarray;
	int error = 0;

	ncontrols = zend_hash_num_elements(Z_ARRVAL_P(array));
	ctrls = safe_emalloc((1 + ncontrols), sizeof(*ctrls), 0);
	*ctrls = NULL;
	ctrlp = ctrls;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(array), ctrlarray) {
		if (Z_TYPE_P(ctrlarray) != IS_ARRAY) {
			php_error_docref(NULL, E_WARNING, "The array value must contain only arrays, where each array is a control");
			error = 1;
			break;
		}

		if (_php_ldap_control_from_array(ld, ctrlp, ctrlarray) == LDAP_SUCCESS) {
			++ctrlp;
		} else {
			error = 1;
			break;
		}

		*ctrlp = NULL;
	} ZEND_HASH_FOREACH_END();

	if (error) {
		ctrlp = ctrls;
		while (*ctrlp) {
			ldap_control_free(*ctrlp);
			ctrlp++;
		}
		efree(ctrls);
		ctrls = NULL;
	}

	return ctrls;
}

static void _php_ldap_controls_free (LDAPControl*** ctrls)
{
	LDAPControl **ctrlp;

	if (*ctrls) {
		ctrlp = *ctrls;
		while (*ctrlp) {
			ldap_control_free(*ctrlp);
			ctrlp++;
		}
		efree(*ctrls);
		*ctrls = NULL;
	}
}
/* }}} */

/* {{{ PHP_INI_BEGIN
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY_EX("ldap.max_links", "-1", PHP_INI_SYSTEM, OnUpdateLong, max_links, zend_ldap_globals, ldap_globals, display_link_numbers)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(ldap)
{
#if defined(COMPILE_DL_LDAP) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	ldap_globals->num_links = 0;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ldap)
{
	REGISTER_INI_ENTRIES();

	/* Constants to be used with deref-parameter in php_ldap_do_search() */
	REGISTER_LONG_CONSTANT("LDAP_DEREF_NEVER", LDAP_DEREF_NEVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_DEREF_SEARCHING", LDAP_DEREF_SEARCHING, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_DEREF_FINDING", LDAP_DEREF_FINDING, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_DEREF_ALWAYS", LDAP_DEREF_ALWAYS, CONST_PERSISTENT | CONST_CS);

	/* Constants to be used with ldap_modify_batch() */
	REGISTER_LONG_CONSTANT("LDAP_MODIFY_BATCH_ADD", LDAP_MODIFY_BATCH_ADD, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_MODIFY_BATCH_REMOVE", LDAP_MODIFY_BATCH_REMOVE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_MODIFY_BATCH_REMOVE_ALL", LDAP_MODIFY_BATCH_REMOVE_ALL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_MODIFY_BATCH_REPLACE", LDAP_MODIFY_BATCH_REPLACE, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_MODIFY_BATCH_ATTRIB", LDAP_MODIFY_BATCH_ATTRIB, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_MODIFY_BATCH_MODTYPE", LDAP_MODIFY_BATCH_MODTYPE, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_MODIFY_BATCH_VALUES", LDAP_MODIFY_BATCH_VALUES, CONST_PERSISTENT | CONST_CS);

#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP
	/* LDAP options */
	REGISTER_LONG_CONSTANT("LDAP_OPT_DEREF", LDAP_OPT_DEREF, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_SIZELIMIT", LDAP_OPT_SIZELIMIT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_TIMELIMIT", LDAP_OPT_TIMELIMIT, CONST_PERSISTENT | CONST_CS);
#ifdef LDAP_OPT_NETWORK_TIMEOUT
	REGISTER_LONG_CONSTANT("LDAP_OPT_NETWORK_TIMEOUT", LDAP_OPT_NETWORK_TIMEOUT, CONST_PERSISTENT | CONST_CS);
#elif defined (LDAP_X_OPT_CONNECT_TIMEOUT)
	REGISTER_LONG_CONSTANT("LDAP_OPT_NETWORK_TIMEOUT", LDAP_X_OPT_CONNECT_TIMEOUT, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_OPT_TIMEOUT
	REGISTER_LONG_CONSTANT("LDAP_OPT_TIMEOUT", LDAP_OPT_TIMEOUT, CONST_PERSISTENT | CONST_CS);
#endif
	REGISTER_LONG_CONSTANT("LDAP_OPT_PROTOCOL_VERSION", LDAP_OPT_PROTOCOL_VERSION, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_ERROR_NUMBER", LDAP_OPT_ERROR_NUMBER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_REFERRALS", LDAP_OPT_REFERRALS, CONST_PERSISTENT | CONST_CS);
#ifdef LDAP_OPT_RESTART
	REGISTER_LONG_CONSTANT("LDAP_OPT_RESTART", LDAP_OPT_RESTART, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_OPT_HOST_NAME
	REGISTER_LONG_CONSTANT("LDAP_OPT_HOST_NAME", LDAP_OPT_HOST_NAME, CONST_PERSISTENT | CONST_CS);
#endif
	REGISTER_LONG_CONSTANT("LDAP_OPT_ERROR_STRING", LDAP_OPT_ERROR_STRING, CONST_PERSISTENT | CONST_CS);
#ifdef LDAP_OPT_MATCHED_DN
	REGISTER_LONG_CONSTANT("LDAP_OPT_MATCHED_DN", LDAP_OPT_MATCHED_DN, CONST_PERSISTENT | CONST_CS);
#endif
	REGISTER_LONG_CONSTANT("LDAP_OPT_SERVER_CONTROLS", LDAP_OPT_SERVER_CONTROLS, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_CLIENT_CONTROLS", LDAP_OPT_CLIENT_CONTROLS, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_OPT_DEBUG_LEVEL
	REGISTER_LONG_CONSTANT("LDAP_OPT_DEBUG_LEVEL", LDAP_OPT_DEBUG_LEVEL, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LDAP_OPT_DIAGNOSTIC_MESSAGE
	REGISTER_LONG_CONSTANT("LDAP_OPT_DIAGNOSTIC_MESSAGE", LDAP_OPT_DIAGNOSTIC_MESSAGE, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef HAVE_LDAP_SASL
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_MECH", LDAP_OPT_X_SASL_MECH, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_REALM", LDAP_OPT_X_SASL_REALM, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_AUTHCID", LDAP_OPT_X_SASL_AUTHCID, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_AUTHZID", LDAP_OPT_X_SASL_AUTHZID, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_OPT_X_SASL_NOCANON
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_NOCANON", LDAP_OPT_X_SASL_NOCANON, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_OPT_X_SASL_USERNAME
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_USERNAME", LDAP_OPT_X_SASL_USERNAME, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef ORALDAP
	REGISTER_LONG_CONSTANT("GSLC_SSL_NO_AUTH", GSLC_SSL_NO_AUTH, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("GSLC_SSL_ONEWAY_AUTH", GSLC_SSL_ONEWAY_AUTH, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("GSLC_SSL_TWOWAY_AUTH", GSLC_SSL_TWOWAY_AUTH, CONST_PERSISTENT | CONST_CS);
#endif

#if (LDAP_API_VERSION > 2000)
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_REQUIRE_CERT", LDAP_OPT_X_TLS_REQUIRE_CERT, CONST_PERSISTENT | CONST_CS);

	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_NEVER", LDAP_OPT_X_TLS_NEVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_HARD", LDAP_OPT_X_TLS_HARD, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_DEMAND", LDAP_OPT_X_TLS_DEMAND, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_ALLOW", LDAP_OPT_X_TLS_ALLOW, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_TRY", LDAP_OPT_X_TLS_TRY, CONST_PERSISTENT | CONST_CS);

	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_CACERTDIR", LDAP_OPT_X_TLS_CACERTDIR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_CACERTFILE", LDAP_OPT_X_TLS_CACERTFILE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_CERTFILE", LDAP_OPT_X_TLS_CERTFILE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_CIPHER_SUITE", LDAP_OPT_X_TLS_CIPHER_SUITE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_KEYFILE", LDAP_OPT_X_TLS_KEYFILE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_RANDOM_FILE", LDAP_OPT_X_TLS_RANDOM_FILE, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LDAP_OPT_X_TLS_CRLCHECK
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_CRLCHECK", LDAP_OPT_X_TLS_CRLCHECK, CONST_PERSISTENT | CONST_CS);

	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_CRL_NONE", LDAP_OPT_X_TLS_CRL_NONE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_CRL_PEER", LDAP_OPT_X_TLS_CRL_PEER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_CRL_ALL", LDAP_OPT_X_TLS_CRL_ALL, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LDAP_OPT_X_TLS_DHFILE
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_DHFILE", LDAP_OPT_X_TLS_DHFILE, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LDAP_OPT_X_TLS_CRLFILE
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_CRLFILE", LDAP_OPT_X_TLS_CRLFILE, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LDAP_OPT_X_TLS_PROTOCOL_MIN
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_PROTOCOL_MIN", LDAP_OPT_X_TLS_PROTOCOL_MIN, CONST_PERSISTENT | CONST_CS);

	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_PROTOCOL_SSL2", LDAP_OPT_X_TLS_PROTOCOL_SSL2, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_PROTOCOL_SSL3", LDAP_OPT_X_TLS_PROTOCOL_SSL3, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_PROTOCOL_TLS1_0", LDAP_OPT_X_TLS_PROTOCOL_TLS1_0, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_PROTOCOL_TLS1_1", LDAP_OPT_X_TLS_PROTOCOL_TLS1_1, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_PROTOCOL_TLS1_2", LDAP_OPT_X_TLS_PROTOCOL_TLS1_2, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LDAP_OPT_X_TLS_PACKAGE
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_TLS_PACKAGE", LDAP_OPT_X_TLS_PACKAGE, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LDAP_OPT_X_KEEPALIVE_IDLE
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_KEEPALIVE_IDLE", LDAP_OPT_X_KEEPALIVE_IDLE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_KEEPALIVE_PROBES", LDAP_OPT_X_KEEPALIVE_PROBES, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_KEEPALIVE_INTERVAL", LDAP_OPT_X_KEEPALIVE_INTERVAL, CONST_PERSISTENT | CONST_CS);
#endif

	REGISTER_LONG_CONSTANT("LDAP_ESCAPE_FILTER", PHP_LDAP_ESCAPE_FILTER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_ESCAPE_DN", PHP_LDAP_ESCAPE_DN, CONST_PERSISTENT | CONST_CS);

#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
	REGISTER_STRING_CONSTANT("LDAP_EXOP_START_TLS", LDAP_EXOP_START_TLS, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_EXOP_MODIFY_PASSWD", LDAP_EXOP_MODIFY_PASSWD, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_EXOP_REFRESH", LDAP_EXOP_REFRESH, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_EXOP_WHO_AM_I", LDAP_EXOP_WHO_AM_I, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_EXOP_TURN", LDAP_EXOP_TURN, CONST_PERSISTENT | CONST_CS);
#endif

/* LDAP Controls */
/*	standard track controls */
#ifdef LDAP_CONTROL_MANAGEDSAIT
	/* RFC 3296 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_MANAGEDSAIT", LDAP_CONTROL_MANAGEDSAIT, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_PROXY_AUTHZ
	/* RFC 4370 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_PROXY_AUTHZ", LDAP_CONTROL_PROXY_AUTHZ, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_SUBENTRIES
	/* RFC 3672 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_SUBENTRIES", LDAP_CONTROL_SUBENTRIES, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_VALUESRETURNFILTER
	/* RFC 3876 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_VALUESRETURNFILTER", LDAP_CONTROL_VALUESRETURNFILTER, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_ASSERT
	/* RFC 4528 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_ASSERT", LDAP_CONTROL_ASSERT, CONST_PERSISTENT | CONST_CS);
	/* RFC 4527 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_PRE_READ", LDAP_CONTROL_PRE_READ, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_POST_READ", LDAP_CONTROL_POST_READ, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_SORTREQUEST
	/* RFC 2891 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_SORTREQUEST", LDAP_CONTROL_SORTREQUEST, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_SORTRESPONSE", LDAP_CONTROL_SORTRESPONSE, CONST_PERSISTENT | CONST_CS);
#endif
/*	non-standard track controls */
#ifdef LDAP_CONTROL_PAGEDRESULTS
	/* RFC 2696 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_PAGEDRESULTS", LDAP_CONTROL_PAGEDRESULTS, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_AUTHZID_REQUEST
	/* RFC 3829 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_AUTHZID_REQUEST", LDAP_CONTROL_AUTHZID_REQUEST, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_AUTHZID_RESPONSE", LDAP_CONTROL_AUTHZID_RESPONSE, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_SYNC
	/* LDAP Content Synchronization Operation -- RFC 4533 */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_SYNC", LDAP_CONTROL_SYNC, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_SYNC_STATE", LDAP_CONTROL_SYNC_STATE, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_SYNC_DONE", LDAP_CONTROL_SYNC_DONE, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_DONTUSECOPY
	/* LDAP Don't Use Copy Control (RFC 6171) */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_DONTUSECOPY", LDAP_CONTROL_DONTUSECOPY, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_PASSWORDPOLICYREQUEST
	/* Password policy Controls */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_PASSWORDPOLICYREQUEST", LDAP_CONTROL_PASSWORDPOLICYREQUEST, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_PASSWORDPOLICYRESPONSE", LDAP_CONTROL_PASSWORDPOLICYRESPONSE, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_X_INCREMENTAL_VALUES
	/* MS Active Directory controls */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_X_INCREMENTAL_VALUES", LDAP_CONTROL_X_INCREMENTAL_VALUES, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_X_DOMAIN_SCOPE", LDAP_CONTROL_X_DOMAIN_SCOPE, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_X_PERMISSIVE_MODIFY", LDAP_CONTROL_X_PERMISSIVE_MODIFY, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_X_SEARCH_OPTIONS", LDAP_CONTROL_X_SEARCH_OPTIONS, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_X_TREE_DELETE", LDAP_CONTROL_X_TREE_DELETE, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_X_EXTENDED_DN", LDAP_CONTROL_X_EXTENDED_DN, CONST_PERSISTENT | CONST_CS);
#endif
#ifdef LDAP_CONTROL_VLVREQUEST
	/* LDAP VLV */
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_VLVREQUEST", LDAP_CONTROL_VLVREQUEST, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("LDAP_CONTROL_VLVRESPONSE", LDAP_CONTROL_VLVRESPONSE, CONST_PERSISTENT | CONST_CS);
#endif

	le_link = zend_register_list_destructors_ex(_close_ldap_link, NULL, "ldap link", module_number);
	le_result = zend_register_list_destructors_ex(_free_ldap_result, NULL, "ldap result", module_number);
	le_result_entry = zend_register_list_destructors_ex(_free_ldap_result_entry, NULL, "ldap result entry", module_number);

	ldap_module_entry.type = type;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ldap)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ldap)
{
	char tmp[32];

	php_info_print_table_start();
	php_info_print_table_row(2, "LDAP Support", "enabled");

	if (LDAPG(max_links) == -1) {
		snprintf(tmp, 31, ZEND_LONG_FMT "/unlimited", LDAPG(num_links));
	} else {
		snprintf(tmp, 31, ZEND_LONG_FMT "/" ZEND_LONG_FMT, LDAPG(num_links), LDAPG(max_links));
	}
	php_info_print_table_row(2, "Total Links", tmp);

#ifdef LDAP_API_VERSION
	snprintf(tmp, 31, "%d", LDAP_API_VERSION);
	php_info_print_table_row(2, "API Version", tmp);
#endif

#ifdef LDAP_VENDOR_NAME
	php_info_print_table_row(2, "Vendor Name", LDAP_VENDOR_NAME);
#endif

#ifdef LDAP_VENDOR_VERSION
	snprintf(tmp, 31, "%d", LDAP_VENDOR_VERSION);
	php_info_print_table_row(2, "Vendor Version", tmp);
#endif

#ifdef HAVE_LDAP_SASL
	php_info_print_table_row(2, "SASL Support", "Enabled");
#endif

	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto resource ldap_connect([string host [, int port [, string wallet [, string wallet_passwd [, int authmode]]]]])
   Connect to an LDAP server */
PHP_FUNCTION(ldap_connect)
{
	char *host = NULL;
	size_t hostlen = 0;
	zend_long port = LDAP_PORT;
#ifdef HAVE_ORALDAP
	char *wallet = NULL, *walletpasswd = NULL;
	size_t walletlen = 0, walletpasswdlen = 0;
	zend_long authmode = GSLC_SSL_NO_AUTH;
	int ssl=0;
#endif
	ldap_linkdata *ld;
	LDAP *ldap = NULL;

#ifdef HAVE_ORALDAP
	if (ZEND_NUM_ARGS() == 3 || ZEND_NUM_ARGS() == 4) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|slssl", &host, &hostlen, &port, &wallet, &walletlen, &walletpasswd, &walletpasswdlen, &authmode) != SUCCESS) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 5) {
		ssl = 1;
	}
#else
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sl", &host, &hostlen, &port) != SUCCESS) {
		RETURN_FALSE;
	}
#endif

	if (LDAPG(max_links) != -1 && LDAPG(num_links) >= LDAPG(max_links)) {
		php_error_docref(NULL, E_WARNING, "Too many open links (" ZEND_LONG_FMT ")", LDAPG(num_links));
		RETURN_FALSE;
	}

	ld = ecalloc(1, sizeof(ldap_linkdata));

	{
		int rc = LDAP_SUCCESS;
		char	*url = host;
		if (url && !ldap_is_ldap_url(url)) {
			size_t urllen = hostlen + sizeof( "ldap://:65535" );

			if (port <= 0 || port > 65535) {
				efree(ld);
				php_error_docref(NULL, E_WARNING, "invalid port number: " ZEND_LONG_FMT, port);
				RETURN_FALSE;
			}

			url = emalloc(urllen);
			snprintf( url, urllen, "ldap://%s:" ZEND_LONG_FMT, host, port );
		}

#ifdef LDAP_API_FEATURE_X_OPENLDAP
		/* ldap_init() is deprecated, use ldap_initialize() instead.
		 */
		rc = ldap_initialize(&ldap, url);
#else /* ! LDAP_API_FEATURE_X_OPENLDAP */
		/* ldap_init does not support URLs.
		 * We must try the original host and port information.
		 */
		ldap = ldap_init(host, port);
		if (ldap == NULL) {
			efree(ld);
			php_error_docref(NULL, E_WARNING, "Could not create session handle");
			RETURN_FALSE;
		}
#endif /* ! LDAP_API_FEATURE_X_OPENLDAP */
		if (url != host) {
			efree(url);
		}
		if (rc != LDAP_SUCCESS) {
			efree(ld);
			php_error_docref(NULL, E_WARNING, "Could not create session handle: %s", ldap_err2string(rc));
			RETURN_FALSE;
		}
	}

	if (ldap == NULL) {
		efree(ld);
		RETURN_FALSE;
	} else {
#ifdef HAVE_ORALDAP
		if (ssl) {
			if (ldap_init_SSL(&ldap->ld_sb, wallet, walletpasswd, authmode)) {
				efree(ld);
				php_error_docref(NULL, E_WARNING, "SSL init failed");
				RETURN_FALSE;
			}
		}
#endif
		LDAPG(num_links)++;
		ld->link = ldap;
		RETURN_RES(zend_register_resource(ld, le_link));
	}

}
/* }}} */

/* {{{ _get_lderrno
 */
static int _get_lderrno(LDAP *ldap)
{
#if LDAP_API_VERSION > 2000 || HAVE_ORALDAP
	int lderr;

	/* New versions of OpenLDAP do it this way */
	ldap_get_option(ldap, LDAP_OPT_ERROR_NUMBER, &lderr);
	return lderr;
#else
	return ldap->ld_errno;
#endif
}
/* }}} */

/* {{{ _set_lderrno
 */
static void _set_lderrno(LDAP *ldap, int lderr)
{
#if LDAP_API_VERSION > 2000 || HAVE_ORALDAP
	/* New versions of OpenLDAP do it this way */
	ldap_set_option(ldap, LDAP_OPT_ERROR_NUMBER, &lderr);
#else
	ldap->ld_errno = lderr;
#endif
}
/* }}} */

/* {{{ proto bool ldap_bind(resource link [, string dn [, string password]])
   Bind to LDAP directory */
PHP_FUNCTION(ldap_bind)
{
	zval *link;
	char *ldap_bind_dn = NULL, *ldap_bind_pw = NULL;
	size_t ldap_bind_dnlen, ldap_bind_pwlen;
	ldap_linkdata *ld;
	int rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|ss", &link, &ldap_bind_dn, &ldap_bind_dnlen, &ldap_bind_pw, &ldap_bind_pwlen) != SUCCESS) {
		RETURN_FALSE;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if (ldap_bind_dn != NULL && memchr(ldap_bind_dn, '\0', ldap_bind_dnlen) != NULL) {
		_set_lderrno(ld->link, LDAP_INVALID_CREDENTIALS);
		php_error_docref(NULL, E_WARNING, "DN contains a null byte");
		RETURN_FALSE;
	}

	if (ldap_bind_pw != NULL && memchr(ldap_bind_pw, '\0', ldap_bind_pwlen) != NULL) {
		_set_lderrno(ld->link, LDAP_INVALID_CREDENTIALS);
		php_error_docref(NULL, E_WARNING, "Password contains a null byte");
		RETURN_FALSE;
	}

	{
#ifdef LDAP_API_FEATURE_X_OPENLDAP
		/* ldap_simple_bind_s() is deprecated, use ldap_sasl_bind_s() instead.
		 */
		struct berval   cred;

		cred.bv_val = ldap_bind_pw;
		cred.bv_len = ldap_bind_pw ? ldap_bind_pwlen : 0;
		rc = ldap_sasl_bind_s(ld->link, ldap_bind_dn, LDAP_SASL_SIMPLE, &cred,
				NULL, NULL,     /* no controls right now */
				NULL);	  /* we don't care about the server's credentials */
#else /* ! LDAP_API_FEATURE_X_OPENLDAP */
		rc = ldap_simple_bind_s(ld->link, ldap_bind_dn, ldap_bind_pw);
#endif /* ! LDAP_API_FEATURE_X_OPENLDAP */
	}
	if ( rc != LDAP_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Unable to bind to server: %s", ldap_err2string(rc));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto resource ldap_bind_ext(resource link [, string dn [, string password [, serverctrls]]])
   Bind to LDAP directory */
PHP_FUNCTION(ldap_bind_ext)
{
	zval *serverctrls = NULL;
	zval *link;
	char *ldap_bind_dn = NULL, *ldap_bind_pw = NULL;
	size_t ldap_bind_dnlen, ldap_bind_pwlen;
	ldap_linkdata *ld;
	LDAPControl **lserverctrls = NULL;
	LDAPMessage *ldap_res;
	int rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|ssa", &link, &ldap_bind_dn, &ldap_bind_dnlen, &ldap_bind_pw, &ldap_bind_pwlen, &serverctrls) != SUCCESS) {
		RETURN_FALSE;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if (ldap_bind_dn != NULL && memchr(ldap_bind_dn, '\0', ldap_bind_dnlen) != NULL) {
		_set_lderrno(ld->link, LDAP_INVALID_CREDENTIALS);
		php_error_docref(NULL, E_WARNING, "DN contains a null byte");
		RETURN_FALSE;
	}

	if (ldap_bind_pw != NULL && memchr(ldap_bind_pw, '\0', ldap_bind_pwlen) != NULL) {
		_set_lderrno(ld->link, LDAP_INVALID_CREDENTIALS);
		php_error_docref(NULL, E_WARNING, "Password contains a null byte");
		RETURN_FALSE;
	}

	if (serverctrls) {
		lserverctrls = _php_ldap_controls_from_array(ld->link, serverctrls);
		if (lserverctrls == NULL) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}

	{
		/* ldap_simple_bind() is deprecated, use ldap_sasl_bind() instead */
		struct berval   cred;
		int msgid;

		cred.bv_val = ldap_bind_pw;
		cred.bv_len = ldap_bind_pw ? ldap_bind_pwlen : 0;
		/* asynchronous call */
		rc = ldap_sasl_bind(ld->link, ldap_bind_dn, LDAP_SASL_SIMPLE, &cred,
				lserverctrls, NULL, &msgid);
		if (rc != LDAP_SUCCESS ) {
			php_error_docref(NULL, E_WARNING, "Unable to bind to server: %s (%d)", ldap_err2string(rc), rc);
			RETVAL_FALSE;
			goto cleanup;
		}

		rc = ldap_result(ld->link, msgid, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
		if (rc == -1) {
			php_error_docref(NULL, E_WARNING, "Bind operation failed");
			RETVAL_FALSE;
			goto cleanup;
		}

		/* return a PHP control object */
		RETVAL_RES(zend_register_resource(ldap_res, le_result));
	}

cleanup:
	if (lserverctrls) {
		_php_ldap_controls_free(&lserverctrls);
	}

	return;
}
/* }}} */

#ifdef HAVE_LDAP_SASL
typedef struct {
	char *mech;
	char *realm;
	char *authcid;
	char *passwd;
	char *authzid;
} php_ldap_bictx;

/* {{{ _php_sasl_setdefs
 */
static php_ldap_bictx *_php_sasl_setdefs(LDAP *ld, char *sasl_mech, char *sasl_realm, char *sasl_authc_id, char *passwd, char *sasl_authz_id)
{
	php_ldap_bictx *ctx;

	ctx = ber_memalloc(sizeof(php_ldap_bictx));
	ctx->mech    = (sasl_mech) ? ber_strdup(sasl_mech) : NULL;
	ctx->realm   = (sasl_realm) ? ber_strdup(sasl_realm) : NULL;
	ctx->authcid = (sasl_authc_id) ? ber_strdup(sasl_authc_id) : NULL;
	ctx->passwd  = (passwd) ? ber_strdup(passwd) : NULL;
	ctx->authzid = (sasl_authz_id) ? ber_strdup(sasl_authz_id) : NULL;

	if (ctx->mech == NULL) {
		ldap_get_option(ld, LDAP_OPT_X_SASL_MECH, &ctx->mech);
	}
	if (ctx->realm == NULL) {
		ldap_get_option(ld, LDAP_OPT_X_SASL_REALM, &ctx->realm);
	}
	if (ctx->authcid == NULL) {
		ldap_get_option(ld, LDAP_OPT_X_SASL_AUTHCID, &ctx->authcid);
	}
	if (ctx->authzid == NULL) {
		ldap_get_option(ld, LDAP_OPT_X_SASL_AUTHZID, &ctx->authzid);
	}

	return ctx;
}
/* }}} */

/* {{{ _php_sasl_freedefs
 */
static void _php_sasl_freedefs(php_ldap_bictx *ctx)
{
	if (ctx->mech) ber_memfree(ctx->mech);
	if (ctx->realm) ber_memfree(ctx->realm);
	if (ctx->authcid) ber_memfree(ctx->authcid);
	if (ctx->passwd) ber_memfree(ctx->passwd);
	if (ctx->authzid) ber_memfree(ctx->authzid);
	ber_memfree(ctx);
}
/* }}} */

/* {{{ _php_sasl_interact
   Internal interact function for SASL */
static int _php_sasl_interact(LDAP *ld, unsigned flags, void *defaults, void *in)
{
	sasl_interact_t *interact = in;
	const char *p;
	php_ldap_bictx *ctx = defaults;

	for (;interact->id != SASL_CB_LIST_END;interact++) {
		p = NULL;
		switch(interact->id) {
			case SASL_CB_GETREALM:
				p = ctx->realm;
				break;
			case SASL_CB_AUTHNAME:
				p = ctx->authcid;
				break;
			case SASL_CB_USER:
				p = ctx->authzid;
				break;
			case SASL_CB_PASS:
				p = ctx->passwd;
				break;
		}
		if (p) {
			interact->result = p;
			interact->len = strlen(interact->result);
		}
	}
	return LDAP_SUCCESS;
}
/* }}} */

/* {{{ proto bool ldap_sasl_bind(resource link [, string binddn [, string password [, string sasl_mech [, string sasl_realm [, string sasl_authc_id [, string sasl_authz_id [, string props]]]]]]])
   Bind to LDAP directory using SASL */
PHP_FUNCTION(ldap_sasl_bind)
{
	zval *link;
	ldap_linkdata *ld;
	char *binddn = NULL;
	char *passwd = NULL;
	char *sasl_mech = NULL;
	char *sasl_realm = NULL;
	char *sasl_authz_id = NULL;
	char *sasl_authc_id = NULL;
	char *props = NULL;
	size_t rc, dn_len, passwd_len, mech_len, realm_len, authc_id_len, authz_id_len, props_len;
	php_ldap_bictx *ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|sssssss", &link, &binddn, &dn_len, &passwd, &passwd_len, &sasl_mech, &mech_len, &sasl_realm, &realm_len, &sasl_authc_id, &authc_id_len, &sasl_authz_id, &authz_id_len, &props, &props_len) != SUCCESS) {
		RETURN_FALSE;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	ctx = _php_sasl_setdefs(ld->link, sasl_mech, sasl_realm, sasl_authc_id, passwd, sasl_authz_id);

	if (props) {
		ldap_set_option(ld->link, LDAP_OPT_X_SASL_SECPROPS, props);
	}

	rc = ldap_sasl_interactive_bind_s(ld->link, binddn, ctx->mech, NULL, NULL, LDAP_SASL_QUIET, _php_sasl_interact, ctx);
	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Unable to bind to server: %s", ldap_err2string(rc));
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}
	_php_sasl_freedefs(ctx);
}
/* }}} */
#endif /* HAVE_LDAP_SASL */

/* {{{ proto bool ldap_unbind(resource link)
   Unbind from LDAP directory */
PHP_FUNCTION(ldap_unbind)
{
	zval *link;
	ldap_linkdata *ld;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &link) != SUCCESS) {
		RETURN_FALSE;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	zend_list_close(Z_RES_P(link));
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_set_opts
 */
static void php_set_opts(LDAP *ldap, int sizelimit, int timelimit, int deref, int *old_sizelimit, int *old_timelimit, int *old_deref)
{
	/* sizelimit */
	if (sizelimit > -1) {
#if (LDAP_API_VERSION >= 2004) || HAVE_ORALDAP
		ldap_get_option(ldap, LDAP_OPT_SIZELIMIT, old_sizelimit);
		ldap_set_option(ldap, LDAP_OPT_SIZELIMIT, &sizelimit);
#else
		*old_sizelimit = ldap->ld_sizelimit;
		ldap->ld_sizelimit = sizelimit;
#endif
	}

	/* timelimit */
	if (timelimit > -1) {
#if (LDAP_API_VERSION >= 2004) || HAVE_ORALDAP
		ldap_get_option(ldap, LDAP_OPT_TIMELIMIT, old_timelimit);
		ldap_set_option(ldap, LDAP_OPT_TIMELIMIT, &timelimit);
#else
		*old_timelimit = ldap->ld_timelimit;
		ldap->ld_timelimit = timelimit;
#endif
	}

	/* deref */
	if (deref > -1) {
#if (LDAP_API_VERSION >= 2004) || HAVE_ORALDAP
		ldap_get_option(ldap, LDAP_OPT_DEREF, old_deref);
		ldap_set_option(ldap, LDAP_OPT_DEREF, &deref);
#else
		*old_deref = ldap->ld_deref;
		ldap->ld_deref = deref;
#endif
	}
}
/* }}} */

/* {{{ php_ldap_do_search
 */
static void php_ldap_do_search(INTERNAL_FUNCTION_PARAMETERS, int scope)
{
	zval *link, *base_dn, *filter, *attrs = NULL, *attr, *serverctrls = NULL;
	zend_long attrsonly, sizelimit, timelimit, deref;
	zend_string *ldap_filter = NULL, *ldap_base_dn = NULL;
	char **ldap_attrs = NULL;
	ldap_linkdata *ld = NULL;
	LDAPMessage *ldap_res = NULL;
	LDAPControl **lserverctrls = NULL;
	int ldap_attrsonly = 0, ldap_sizelimit = -1, ldap_timelimit = -1, ldap_deref = -1;
	int old_ldap_sizelimit = -1, old_ldap_timelimit = -1, old_ldap_deref = -1;
	int num_attribs = 0, ret = 1, i, errno, argcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argcount, "zzz|a/lllla/", &link, &base_dn, &filter, &attrs, &attrsonly,
		&sizelimit, &timelimit, &deref, &serverctrls) == FAILURE) {
		return;
	}

	/* Reverse -> fall through */
	switch (argcount) {
		case 9:
		case 8:
			ldap_deref = deref;
		case 7:
			ldap_timelimit = timelimit;
		case 6:
			ldap_sizelimit = sizelimit;
		case 5:
			ldap_attrsonly = attrsonly;
		case 4:
			num_attribs = zend_hash_num_elements(Z_ARRVAL_P(attrs));
			ldap_attrs = safe_emalloc((num_attribs+1), sizeof(char *), 0);

			for (i = 0; i<num_attribs; i++) {
				if ((attr = zend_hash_index_find(Z_ARRVAL_P(attrs), i)) == NULL) {
					php_error_docref(NULL, E_WARNING, "Array initialization wrong");
					ret = 0;
					goto cleanup;
				}

				convert_to_string(attr);
				if (EG(exception)) {
					ret = 0;
					goto cleanup;
				}
				ldap_attrs[i] = Z_STRVAL_P(attr);
			}
			ldap_attrs[num_attribs] = NULL;
		default:
			break;
	}

	/* parallel search? */
	if (Z_TYPE_P(link) == IS_ARRAY) {
		int i, nlinks, nbases, nfilters, *rcs;
		ldap_linkdata **lds;
		zval *entry, resource;

		nlinks = zend_hash_num_elements(Z_ARRVAL_P(link));
		if (nlinks == 0) {
			php_error_docref(NULL, E_WARNING, "No links in link array");
			ret = 0;
			goto cleanup;
		}

		if (Z_TYPE_P(base_dn) == IS_ARRAY) {
			nbases = zend_hash_num_elements(Z_ARRVAL_P(base_dn));
			if (nbases != nlinks) {
				php_error_docref(NULL, E_WARNING, "Base must either be a string, or an array with the same number of elements as the links array");
				ret = 0;
				goto cleanup;
			}
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(base_dn));
		} else {
			nbases = 0; /* this means string, not array */
			ldap_base_dn = zval_get_string(base_dn);
			if (EG(exception)) {
				ret = 0;
				goto cleanup;
			}
		}

		if (Z_TYPE_P(filter) == IS_ARRAY) {
			nfilters = zend_hash_num_elements(Z_ARRVAL_P(filter));
			if (nfilters != nlinks) {
				php_error_docref(NULL, E_WARNING, "Filter must either be a string, or an array with the same number of elements as the links array");
				ret = 0;
				goto cleanup;
			}
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(filter));
		} else {
			nfilters = 0; /* this means string, not array */
			ldap_filter = zval_get_string(filter);
			if (EG(exception)) {
				ret = 0;
				goto cleanup;
			}
		}

		lds = safe_emalloc(nlinks, sizeof(ldap_linkdata), 0);
		rcs = safe_emalloc(nlinks, sizeof(*rcs), 0);

		zend_hash_internal_pointer_reset(Z_ARRVAL_P(link));
		for (i=0; i<nlinks; i++) {
			entry = zend_hash_get_current_data(Z_ARRVAL_P(link));

			ld = (ldap_linkdata *) zend_fetch_resource_ex(entry, "ldap link", le_link);
			if (ld == NULL) {
				ret = 0;
				goto cleanup_parallel;
			}
			if (nbases != 0) { /* base_dn an array? */
				entry = zend_hash_get_current_data(Z_ARRVAL_P(base_dn));
				zend_hash_move_forward(Z_ARRVAL_P(base_dn));
				ldap_base_dn = zval_get_string(entry);
				if (EG(exception)) {
					ret = 0;
					goto cleanup_parallel;
				}
			}
			if (nfilters != 0) { /* filter an array? */
				entry = zend_hash_get_current_data(Z_ARRVAL_P(filter));
				zend_hash_move_forward(Z_ARRVAL_P(filter));
				ldap_filter = zval_get_string(entry);
				if (EG(exception)) {
					ret = 0;
					goto cleanup_parallel;
				}
			}

			if (argcount > 8) {
				/* We have to parse controls again for each link as they use it */
				_php_ldap_controls_free(&lserverctrls);
				lserverctrls = _php_ldap_controls_from_array(ld->link, serverctrls);
				if (lserverctrls == NULL) {
					rcs[i] = -1;
					continue;
				}
			}

			php_set_opts(ld->link, ldap_sizelimit, ldap_timelimit, ldap_deref, &old_ldap_sizelimit, &old_ldap_timelimit, &old_ldap_deref);

			/* Run the actual search */
			ldap_search_ext(ld->link, ZSTR_VAL(ldap_base_dn), scope, ZSTR_VAL(ldap_filter), ldap_attrs, ldap_attrsonly, lserverctrls, NULL, NULL, ldap_sizelimit, &rcs[i]);
			lds[i] = ld;
			zend_hash_move_forward(Z_ARRVAL_P(link));
		}

		array_init(return_value);

		/* Collect results from the searches */
		for (i=0; i<nlinks; i++) {
			if (rcs[i] != -1) {
				rcs[i] = ldap_result(lds[i]->link, LDAP_RES_ANY, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
			}
			if (rcs[i] != -1) {
				ZVAL_RES(&resource, zend_register_resource(ldap_res, le_result));
				add_next_index_zval(return_value, &resource);
			} else {
				add_next_index_bool(return_value, 0);
			}
		}

cleanup_parallel:
		efree(lds);
		efree(rcs);
	} else {
		ldap_filter = zval_get_string(filter);
		if (EG(exception)) {
			ret = 0;
			goto cleanup;
		}

		ldap_base_dn = zval_get_string(base_dn);
		if (EG(exception)) {
			ret = 0;
			goto cleanup;
		}

		ld = (ldap_linkdata *) zend_fetch_resource_ex(link, "ldap link", le_link);
		if (ld == NULL) {
			ret = 0;
			goto cleanup;
		}

		if (argcount > 8) {
			lserverctrls = _php_ldap_controls_from_array(ld->link, serverctrls);
			if (lserverctrls == NULL) {
				ret = 0;
				goto cleanup;
			}
		}

		php_set_opts(ld->link, ldap_sizelimit, ldap_timelimit, ldap_deref, &old_ldap_sizelimit, &old_ldap_timelimit, &old_ldap_deref);

		/* Run the actual search */
		errno = ldap_search_ext_s(ld->link, ZSTR_VAL(ldap_base_dn), scope, ZSTR_VAL(ldap_filter), ldap_attrs, ldap_attrsonly, lserverctrls, NULL, NULL, ldap_sizelimit, &ldap_res);

		if (errno != LDAP_SUCCESS
			&& errno != LDAP_SIZELIMIT_EXCEEDED
#ifdef LDAP_ADMINLIMIT_EXCEEDED
			&& errno != LDAP_ADMINLIMIT_EXCEEDED
#endif
#ifdef LDAP_REFERRAL
			&& errno != LDAP_REFERRAL
#endif
		) {
			/* ldap_res should be freed regardless of return value of ldap_search_ext_s()
			 * see: https://linux.die.net/man/3/ldap_search_ext_s */
			if (ldap_res != NULL) {
				ldap_msgfree(ldap_res);
			}
			php_error_docref(NULL, E_WARNING, "Search: %s", ldap_err2string(errno));
			ret = 0;
		} else {
			if (errno == LDAP_SIZELIMIT_EXCEEDED) {
				php_error_docref(NULL, E_WARNING, "Partial search results returned: Sizelimit exceeded");
			}
#ifdef LDAP_ADMINLIMIT_EXCEEDED
			else if (errno == LDAP_ADMINLIMIT_EXCEEDED) {
				php_error_docref(NULL, E_WARNING, "Partial search results returned: Adminlimit exceeded");
			}
#endif

			RETVAL_RES(zend_register_resource(ldap_res, le_result));
		}
	}

cleanup:
	if (ld) {
		/* Restoring previous options */
		php_set_opts(ld->link, old_ldap_sizelimit, old_ldap_timelimit, old_ldap_deref, &ldap_sizelimit, &ldap_timelimit, &ldap_deref);
	}
	if (ldap_filter) {
		zend_string_release(ldap_filter);
	}
	if (ldap_base_dn) {
		zend_string_release(ldap_base_dn);
	}
	if (ldap_attrs != NULL) {
		efree(ldap_attrs);
	}
	if (!ret) {
		RETVAL_BOOL(ret);
	}
	if (lserverctrls) {
		_php_ldap_controls_free(&lserverctrls);
	}
}
/* }}} */

/* {{{ proto resource ldap_read(resource|array link, string base_dn, string filter [, array attrs [, int attrsonly [, int sizelimit [, int timelimit [, int deref [, array servercontrols]]]]]])
   Read an entry */
PHP_FUNCTION(ldap_read)
{
	php_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_BASE);
}
/* }}} */

/* {{{ proto resource ldap_list(resource|array link, string base_dn, string filter [, array attrs [, int attrsonly [, int sizelimit [, int timelimit [, int deref [, array servercontrols]]]]]])
   Single-level search */
PHP_FUNCTION(ldap_list)
{
	php_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_ONELEVEL);
}
/* }}} */

/* {{{ proto resource ldap_search(resource|array link, string base_dn, string filter [, array attrs [, int attrsonly [, int sizelimit [, int timelimit [, int deref [, array servercontrols]]]]]])
   Search LDAP tree under base_dn */
PHP_FUNCTION(ldap_search)
{
	php_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_SUBTREE);
}
/* }}} */

/* {{{ proto bool ldap_free_result(resource result)
   Free result memory */
PHP_FUNCTION(ldap_free_result)
{
	zval *result;
	LDAPMessage *ldap_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &result) != SUCCESS) {
		return;
	}

	if ((ldap_result = (LDAPMessage *)zend_fetch_resource(Z_RES_P(result), "ldap result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	zend_list_close(Z_RES_P(result));  /* Delete list entry */
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int ldap_count_entries(resource link, resource result)
   Count the number of entries in a search result */
PHP_FUNCTION(ldap_count_entries)
{
	zval *link, *result;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &link, &result) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((ldap_result = (LDAPMessage *)zend_fetch_resource(Z_RES_P(result), "ldap result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(ldap_count_entries(ld->link, ldap_result));
}
/* }}} */

/* {{{ proto resource ldap_first_entry(resource link, resource result)
   Return first result id */
PHP_FUNCTION(ldap_first_entry)
{
	zval *link, *result;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	LDAPMessage *ldap_result, *entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &link, &result) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((ldap_result = (LDAPMessage *)zend_fetch_resource(Z_RES_P(result), "ldap result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	if ((entry = ldap_first_entry(ld->link, ldap_result)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry = emalloc(sizeof(ldap_resultentry));
		RETVAL_RES(zend_register_resource(resultentry, le_result_entry));
		ZVAL_COPY(&resultentry->res, result);
		resultentry->data = entry;
		resultentry->ber = NULL;
	}
}
/* }}} */

/* {{{ proto resource ldap_next_entry(resource link, resource result_entry)
   Get next result entry */
PHP_FUNCTION(ldap_next_entry)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry, *resultentry_next;
	LDAPMessage *entry_next;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}
	if ((resultentry = (ldap_resultentry *)zend_fetch_resource(Z_RES_P(result_entry), "ldap result entry", le_result_entry)) == NULL) {
		RETURN_FALSE;
	}

	if ((entry_next = ldap_next_entry(ld->link, resultentry->data)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry_next = emalloc(sizeof(ldap_resultentry));
		RETVAL_RES(zend_register_resource(resultentry_next, le_result_entry));
		ZVAL_COPY(&resultentry_next->res, &resultentry->res);
		resultentry_next->data = entry_next;
		resultentry_next->ber = NULL;
	}
}
/* }}} */

/* {{{ proto array ldap_get_entries(resource link, resource result)
   Get all result entries */
PHP_FUNCTION(ldap_get_entries)
{
	zval *link, *result;
	LDAPMessage *ldap_result, *ldap_result_entry;
	zval tmp1, tmp2;
	ldap_linkdata *ld;
	LDAP *ldap;
	int num_entries, num_attrib, num_values, i;
	BerElement *ber;
	char *attribute;
	size_t attr_len;
	struct berval **ldap_value;
	char *dn;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &link, &result) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}
	if ((ldap_result = (LDAPMessage *)zend_fetch_resource(Z_RES_P(result), "ldap result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	ldap = ld->link;
	num_entries = ldap_count_entries(ldap, ldap_result);

	array_init(return_value);
	add_assoc_long(return_value, "count", num_entries);

	if (num_entries == 0) {
		return;
	}

	ldap_result_entry = ldap_first_entry(ldap, ldap_result);
	if (ldap_result_entry == NULL) {
		zend_array_destroy(Z_ARR_P(return_value));
		RETURN_FALSE;
	}

	num_entries = 0;
	while (ldap_result_entry != NULL) {
		array_init(&tmp1);

		num_attrib = 0;
		attribute = ldap_first_attribute(ldap, ldap_result_entry, &ber);

		while (attribute != NULL) {
			ldap_value = ldap_get_values_len(ldap, ldap_result_entry, attribute);
			num_values = ldap_count_values_len(ldap_value);

			array_init(&tmp2);
			add_assoc_long(&tmp2, "count", num_values);
			for (i = 0; i < num_values; i++) {
				add_index_stringl(&tmp2, i, ldap_value[i]->bv_val, ldap_value[i]->bv_len);
			}
			ldap_value_free_len(ldap_value);

			attr_len = strlen(attribute);
			zend_hash_str_update(Z_ARRVAL(tmp1), php_strtolower(attribute, attr_len), attr_len, &tmp2);
			add_index_string(&tmp1, num_attrib, attribute);

			num_attrib++;
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
			ldap_memfree(attribute);
#endif
			attribute = ldap_next_attribute(ldap, ldap_result_entry, ber);
		}
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
		if (ber != NULL) {
			ber_free(ber, 0);
		}
#endif

		add_assoc_long(&tmp1, "count", num_attrib);
		dn = ldap_get_dn(ldap, ldap_result_entry);
		if (dn) {
			add_assoc_string(&tmp1, "dn", dn);
		} else {
			add_assoc_null(&tmp1, "dn");
		}
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
		ldap_memfree(dn);
#else
		free(dn);
#endif

		zend_hash_index_update(Z_ARRVAL_P(return_value), num_entries, &tmp1);

		num_entries++;
		ldap_result_entry = ldap_next_entry(ldap, ldap_result_entry);
	}

	add_assoc_long(return_value, "count", num_entries);

}
/* }}} */

/* {{{ proto string ldap_first_attribute(resource link, resource result_entry)
   Return first attribute */
PHP_FUNCTION(ldap_first_attribute)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attribute;
	zend_long dummy_ber;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr|l", &link, &result_entry, &dummy_ber) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((resultentry = (ldap_resultentry *)zend_fetch_resource(Z_RES_P(result_entry), "ldap result entry", le_result_entry)) == NULL) {
		RETURN_FALSE;
	}

	if ((attribute = ldap_first_attribute(ld->link, resultentry->data, &resultentry->ber)) == NULL) {
		RETURN_FALSE;
	} else {
		RETVAL_STRING(attribute);
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
		ldap_memfree(attribute);
#endif
	}
}
/* }}} */

/* {{{ proto string ldap_next_attribute(resource link, resource result_entry)
   Get the next attribute in result */
PHP_FUNCTION(ldap_next_attribute)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attribute;
	zend_long dummy_ber;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr|l", &link, &result_entry, &dummy_ber) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((resultentry = (ldap_resultentry *)zend_fetch_resource(Z_RES_P(result_entry), "ldap result entry", le_result_entry)) == NULL) {
		RETURN_FALSE;
	}

	if (resultentry->ber == NULL) {
		php_error_docref(NULL, E_WARNING, "called before calling ldap_first_attribute() or no attributes found in result entry");
		RETURN_FALSE;
	}

	if ((attribute = ldap_next_attribute(ld->link, resultentry->data, resultentry->ber)) == NULL) {
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
		if (resultentry->ber != NULL) {
			ber_free(resultentry->ber, 0);
			resultentry->ber = NULL;
		}
#endif
		RETURN_FALSE;
	} else {
		RETVAL_STRING(attribute);
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
		ldap_memfree(attribute);
#endif
	}
}
/* }}} */

/* {{{ proto array ldap_get_attributes(resource link, resource result_entry)
   Get attributes from a search result entry */
PHP_FUNCTION(ldap_get_attributes)
{
	zval *link, *result_entry;
	zval tmp;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attribute;
	struct berval **ldap_value;
	int i, num_values, num_attrib;
	BerElement *ber;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((resultentry = (ldap_resultentry *)zend_fetch_resource(Z_RES_P(result_entry), "ldap result entry", le_result_entry)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	num_attrib = 0;

	attribute = ldap_first_attribute(ld->link, resultentry->data, &ber);
	while (attribute != NULL) {
		ldap_value = ldap_get_values_len(ld->link, resultentry->data, attribute);
		num_values = ldap_count_values_len(ldap_value);

		array_init(&tmp);
		add_assoc_long(&tmp, "count", num_values);
		for (i = 0; i < num_values; i++) {
			add_index_stringl(&tmp, i, ldap_value[i]->bv_val, ldap_value[i]->bv_len);
		}
		ldap_value_free_len(ldap_value);

		zend_hash_str_update(Z_ARRVAL_P(return_value), attribute, strlen(attribute), &tmp);
		add_index_string(return_value, num_attrib, attribute);

		num_attrib++;
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
		ldap_memfree(attribute);
#endif
		attribute = ldap_next_attribute(ld->link, resultentry->data, ber);
	}
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
	if (ber != NULL) {
		ber_free(ber, 0);
	}
#endif

	add_assoc_long(return_value, "count", num_attrib);
}
/* }}} */

/* {{{ proto array ldap_get_values_len(resource link, resource result_entry, string attribute)
   Get all values with lengths from a result entry */
PHP_FUNCTION(ldap_get_values_len)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attr;
	struct berval **ldap_value_len;
	int i, num_values;
	size_t attr_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rrs", &link, &result_entry, &attr, &attr_len) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((resultentry = (ldap_resultentry *)zend_fetch_resource(Z_RES_P(result_entry), "ldap result entry", le_result_entry)) == NULL) {
		RETURN_FALSE;
	}

	if ((ldap_value_len = ldap_get_values_len(ld->link, resultentry->data, attr)) == NULL) {
		php_error_docref(NULL, E_WARNING, "Cannot get the value(s) of attribute %s", ldap_err2string(_get_lderrno(ld->link)));
		RETURN_FALSE;
	}

	num_values = ldap_count_values_len(ldap_value_len);
	array_init(return_value);

	for (i=0; i<num_values; i++) {
		add_next_index_stringl(return_value, ldap_value_len[i]->bv_val, ldap_value_len[i]->bv_len);
	}

	add_assoc_long(return_value, "count", num_values);
	ldap_value_free_len(ldap_value_len);

}
/* }}} */

/* {{{ proto string ldap_get_dn(resource link, resource result_entry)
   Get the DN of a result entry */
PHP_FUNCTION(ldap_get_dn)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *text;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((resultentry = (ldap_resultentry *)zend_fetch_resource(Z_RES_P(result_entry), "ldap result entry", le_result_entry)) == NULL) {
		RETURN_FALSE;
	}

	text = ldap_get_dn(ld->link, resultentry->data);
	if (text != NULL) {
		RETVAL_STRING(text);
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
		ldap_memfree(text);
#else
		free(text);
#endif
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array ldap_explode_dn(string dn, int with_attrib)
   Splits DN into its component parts */
PHP_FUNCTION(ldap_explode_dn)
{
	zend_long with_attrib;
	char *dn, **ldap_value;
	int i, count;
	size_t dn_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl", &dn, &dn_len, &with_attrib) != SUCCESS) {
		return;
	}

	if (!(ldap_value = ldap_explode_dn(dn, with_attrib))) {
		/* Invalid parameters were passed to ldap_explode_dn */
		RETURN_FALSE;
	}

	i=0;
	while (ldap_value[i] != NULL) i++;
	count = i;

	array_init(return_value);

	add_assoc_long(return_value, "count", count);
	for (i = 0; i<count; i++) {
		add_index_string(return_value, i, ldap_value[i]);
	}

	ldap_memvfree((void **)ldap_value);
}
/* }}} */

/* {{{ proto string ldap_dn2ufn(string dn)
   Convert DN to User Friendly Naming format */
PHP_FUNCTION(ldap_dn2ufn)
{
	char *dn, *ufn;
	size_t dn_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &dn, &dn_len) != SUCCESS) {
		return;
	}

	ufn = ldap_dn2ufn(dn);

	if (ufn != NULL) {
		RETVAL_STRING(ufn);
#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP || WINDOWS
		ldap_memfree(ufn);
#endif
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* added to fix use of ldap_modify_add for doing an ldap_add, gerrit thomson. */
#define PHP_LD_FULL_ADD 0xff
/* {{{ php_ldap_do_modify
 */
static void php_ldap_do_modify(INTERNAL_FUNCTION_PARAMETERS, int oper, int ext)
{
	zval *serverctrls = NULL;
	zval *link, *entry, *value, *ivalue;
	ldap_linkdata *ld;
	char *dn;
	LDAPMod **ldap_mods;
	LDAPControl **lserverctrls = NULL;
	LDAPMessage *ldap_res;
	int i, j, num_attribs, num_values, msgid;
	size_t dn_len;
	int *num_berval;
	zend_string *attribute;
	zend_ulong index;
	int is_full_add=0; /* flag for full add operation so ldap_mod_add can be put back into oper, gerrit THomson */

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsa/|a", &link, &dn, &dn_len, &entry, &serverctrls) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	num_attribs = zend_hash_num_elements(Z_ARRVAL_P(entry));
	ldap_mods = safe_emalloc((num_attribs+1), sizeof(LDAPMod *), 0);
	num_berval = safe_emalloc(num_attribs, sizeof(int), 0);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(entry));

	/* added by gerrit thomson to fix ldap_add using ldap_mod_add */
	if (oper == PHP_LD_FULL_ADD) {
		oper = LDAP_MOD_ADD;
		is_full_add = 1;
	}
	/* end additional , gerrit thomson */

	for (i = 0; i < num_attribs; i++) {
		ldap_mods[i] = emalloc(sizeof(LDAPMod));
		ldap_mods[i]->mod_op = oper | LDAP_MOD_BVALUES;
		ldap_mods[i]->mod_type = NULL;

		if (zend_hash_get_current_key(Z_ARRVAL_P(entry), &attribute, &index) == HASH_KEY_IS_STRING) {
			ldap_mods[i]->mod_type = estrndup(ZSTR_VAL(attribute), ZSTR_LEN(attribute));
		} else {
			php_error_docref(NULL, E_WARNING, "Unknown attribute in the data");
			/* Free allocated memory */
			while (i >= 0) {
				if (ldap_mods[i]->mod_type) {
					efree(ldap_mods[i]->mod_type);
				}
				efree(ldap_mods[i]);
				i--;
			}
			efree(num_berval);
			efree(ldap_mods);
			RETURN_FALSE;
		}

		value = zend_hash_get_current_data(Z_ARRVAL_P(entry));

		ZVAL_DEREF(value);
		if (Z_TYPE_P(value) != IS_ARRAY) {
			num_values = 1;
		} else {
			SEPARATE_ARRAY(value);
			num_values = zend_hash_num_elements(Z_ARRVAL_P(value));
		}

		num_berval[i] = num_values;
		ldap_mods[i]->mod_bvalues = safe_emalloc((num_values + 1), sizeof(struct berval *), 0);

/* allow for arrays with one element, no allowance for arrays with none but probably not required, gerrit thomson. */
		if ((num_values == 1) && (Z_TYPE_P(value) != IS_ARRAY)) {
			convert_to_string(value);
			if (EG(exception)) {
				RETVAL_FALSE;
				goto cleanup;
			}
			ldap_mods[i]->mod_bvalues[0] = (struct berval *) emalloc (sizeof(struct berval));
			ldap_mods[i]->mod_bvalues[0]->bv_val = Z_STRVAL_P(value);
			ldap_mods[i]->mod_bvalues[0]->bv_len = Z_STRLEN_P(value);
		} else {
			for (j = 0; j < num_values; j++) {
				if ((ivalue = zend_hash_index_find(Z_ARRVAL_P(value), j)) == NULL) {
					php_error_docref(NULL, E_WARNING, "Value array must have consecutive indices 0, 1, ...");
					num_berval[i] = j;
					num_attribs = i + 1;
					RETVAL_FALSE;
					goto cleanup;
				}
				convert_to_string(ivalue);
				if (EG(exception)) {
					RETVAL_FALSE;
					goto cleanup;
				}
				ldap_mods[i]->mod_bvalues[j] = (struct berval *) emalloc (sizeof(struct berval));
				ldap_mods[i]->mod_bvalues[j]->bv_val = Z_STRVAL_P(ivalue);
				ldap_mods[i]->mod_bvalues[j]->bv_len = Z_STRLEN_P(ivalue);
			}
		}
		ldap_mods[i]->mod_bvalues[num_values] = NULL;
		zend_hash_move_forward(Z_ARRVAL_P(entry));
	}
	ldap_mods[num_attribs] = NULL;

	if (serverctrls) {
		lserverctrls = _php_ldap_controls_from_array(ld->link, serverctrls);
		if (lserverctrls == NULL) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}

/* check flag to see if do_mod was called to perform full add , gerrit thomson */
	if (is_full_add == 1) {
		if (ext) {
			i = ldap_add_ext(ld->link, dn, ldap_mods, lserverctrls, NULL, &msgid);
		} else {
			i = ldap_add_ext_s(ld->link, dn, ldap_mods, lserverctrls, NULL);
		}
		if (i != LDAP_SUCCESS) {
			php_error_docref(NULL, E_WARNING, "Add: %s", ldap_err2string(i));
			RETVAL_FALSE;
		} else if (ext) {
			i = ldap_result(ld->link, msgid, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
			if (i == -1) {
				php_error_docref(NULL, E_WARNING, "Add operation failed");
				RETVAL_FALSE;
				goto cleanup;
			}

			/* return a PHP control object */
			RETVAL_RES(zend_register_resource(ldap_res, le_result));
		} else RETVAL_TRUE;
	} else {
		if (ext) {
			i = ldap_modify_ext(ld->link, dn, ldap_mods, lserverctrls, NULL, &msgid);
		} else {
			i = ldap_modify_ext_s(ld->link, dn, ldap_mods, lserverctrls, NULL);
		}
		if (i != LDAP_SUCCESS) {
			php_error_docref(NULL, E_WARNING, "Modify: %s", ldap_err2string(i));
			RETVAL_FALSE;
		} else if (ext) {
			i = ldap_result(ld->link, msgid, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
			if (i == -1) {
				php_error_docref(NULL, E_WARNING, "Modify operation failed");
				RETVAL_FALSE;
				goto cleanup;
			}

			/* return a PHP control object */
			RETVAL_RES(zend_register_resource(ldap_res, le_result));
		} else RETVAL_TRUE;
	}

cleanup:
	for (i = 0; i < num_attribs; i++) {
		efree(ldap_mods[i]->mod_type);
		for (j = 0; j < num_berval[i]; j++) {
			efree(ldap_mods[i]->mod_bvalues[j]);
		}
		efree(ldap_mods[i]->mod_bvalues);
		efree(ldap_mods[i]);
	}
	efree(num_berval);
	efree(ldap_mods);

	if (lserverctrls) {
		_php_ldap_controls_free(&lserverctrls);
	}

	return;
}
/* }}} */

/* {{{ proto bool ldap_add(resource link, string dn, array entry [, array servercontrols])
   Add entries to LDAP directory */
PHP_FUNCTION(ldap_add)
{
	/* use a newly define parameter into the do_modify so ldap_mod_add can be used the way it is supposed to be used , Gerrit THomson */
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_LD_FULL_ADD, 0);
}
/* }}} */

/* {{{ proto resource ldap_add_ext(resource link, string dn, array entry [, array servercontrols])
   Add entries to LDAP directory */
PHP_FUNCTION(ldap_add_ext)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_LD_FULL_ADD, 1);
}
/* }}} */

/* three functions for attribute base modifications, gerrit Thomson */

/* {{{ proto bool ldap_mod_replace(resource link, string dn, array entry [, array servercontrols])
   Replace attribute values with new ones */
PHP_FUNCTION(ldap_mod_replace)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_MOD_REPLACE, 0);
}
/* }}} */

/* {{{ proto resource ldap_mod_replace_ext(resource link, string dn, array entry [, array servercontrols])
   Replace attribute values with new ones */
PHP_FUNCTION(ldap_mod_replace_ext)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_MOD_REPLACE, 1);
}
/* }}} */

/* {{{ proto bool ldap_mod_add(resource link, string dn, array entry [, array servercontrols])
   Add attribute values to current */
PHP_FUNCTION(ldap_mod_add)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_MOD_ADD, 0);
}
/* }}} */

/* {{{ proto resource ldap_mod_add(resource link, string dn, array entry [, array servercontrols])
   Add attribute values to current */
PHP_FUNCTION(ldap_mod_add_ext)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_MOD_ADD, 1);
}
/* }}} */

/* {{{ proto bool ldap_mod_del(resource link, string dn, array entry [, array servercontrols])
   Delete attribute values */
PHP_FUNCTION(ldap_mod_del)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_MOD_DELETE, 0);
}
/* }}} */

/* {{{ proto resource ldap_mod_del_ext(resource link, string dn, array entry [, array servercontrols])
   Delete attribute values */
PHP_FUNCTION(ldap_mod_del_ext)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_MOD_DELETE, 1);
}
/* }}} */

/* {{{ php_ldap_do_delete
 */
static void php_ldap_do_delete(INTERNAL_FUNCTION_PARAMETERS, int ext)
{
	zval *serverctrls = NULL;
	zval *link;
	ldap_linkdata *ld;
	LDAPControl **lserverctrls = NULL;
	LDAPMessage *ldap_res;
	char *dn;
	int rc, msgid;
	size_t dn_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|a", &link, &dn, &dn_len, &serverctrls) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if (serverctrls) {
		lserverctrls = _php_ldap_controls_from_array(ld->link, serverctrls);
		if (lserverctrls == NULL) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}

	if (ext) {
		rc = ldap_delete_ext(ld->link, dn, lserverctrls, NULL, &msgid);
	} else {
		rc = ldap_delete_ext_s(ld->link, dn, lserverctrls, NULL);
	}
	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Delete: %s", ldap_err2string(rc));
		RETVAL_FALSE;
		goto cleanup;
	} else if (ext) {
		rc = ldap_result(ld->link, msgid, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
		if (rc == -1) {
			php_error_docref(NULL, E_WARNING, "Delete operation failed");
			RETVAL_FALSE;
			goto cleanup;
		}

		/* return a PHP control object */
		RETVAL_RES(zend_register_resource(ldap_res, le_result));
	} else {
		RETVAL_TRUE;
	}

cleanup:
	if (lserverctrls) {
		_php_ldap_controls_free(&lserverctrls);
	}

	return;
}
/* }}} */

/* {{{ proto bool ldap_delete(resource link, string dn [, array servercontrols])
   Delete an entry from a directory */
PHP_FUNCTION(ldap_delete)
{
	php_ldap_do_delete(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto resource ldap_delete_ext(resource link, string dn [, array servercontrols])
   Delete an entry from a directory */
PHP_FUNCTION(ldap_delete_ext)
{
	php_ldap_do_delete(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ _ldap_str_equal_to_const
 */
static size_t _ldap_str_equal_to_const(const char *str, size_t str_len, const char *cstr)
{
	size_t i;

	if (strlen(cstr) != str_len)
		return 0;

	for (i = 0; i < str_len; ++i) {
		if (str[i] != cstr[i]) {
			return 0;
		}
	}

	return 1;
}
/* }}} */

/* {{{ _ldap_strlen_max
 */
static size_t _ldap_strlen_max(const char *str, size_t max_len)
{
	size_t i;

	for (i = 0; i < max_len; ++i) {
		if (str[i] == '\0') {
			return i;
		}
	}

	return max_len;
}
/* }}} */

/* {{{ _ldap_hash_fetch
 */
static void _ldap_hash_fetch(zval *hashTbl, const char *key, zval **out)
{
	*out = zend_hash_str_find(Z_ARRVAL_P(hashTbl), key, strlen(key));
}
/* }}} */

/* {{{ proto bool ldap_modify_batch(resource link, string dn, array modifs [, array servercontrols])
   Perform multiple modifications as part of one operation */
PHP_FUNCTION(ldap_modify_batch)
{
	zval *serverctrls = NULL;
	ldap_linkdata *ld;
	zval *link, *mods, *mod, *modinfo;
	zend_string *modval;
	zval *attrib, *modtype, *vals;
	zval *fetched;
	char *dn;
	size_t dn_len;
	int i, j, k;
	int num_mods, num_modprops, num_modvals;
	LDAPMod **ldap_mods;
	LDAPControl **lserverctrls = NULL;
	uint32_t oper;

	/*
	$mods = array(
		array(
			"attrib" => "unicodePwd",
			"modtype" => LDAP_MODIFY_BATCH_REMOVE,
			"values" => array($oldpw)
		),
		array(
			"attrib" => "unicodePwd",
			"modtype" => LDAP_MODIFY_BATCH_ADD,
			"values" => array($newpw)
		),
		array(
			"attrib" => "userPrincipalName",
			"modtype" => LDAP_MODIFY_BATCH_REPLACE,
			"values" => array("janitor@corp.contoso.com")
		),
		array(
			"attrib" => "userCert",
			"modtype" => LDAP_MODIFY_BATCH_REMOVE_ALL
		)
	);
	*/

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsa/|a", &link, &dn, &dn_len, &mods, &serverctrls) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	/* perform validation */
	{
		zend_string *modkey;
		zend_long modtype;

		/* to store the wrongly-typed keys */
		zend_ulong tmpUlong;

		/* make sure the DN contains no NUL bytes */
		if (_ldap_strlen_max(dn, dn_len) != dn_len) {
			php_error_docref(NULL, E_WARNING, "DN must not contain NUL bytes");
			RETURN_FALSE;
		}

		/* make sure the top level is a normal array */
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(mods));
		if (zend_hash_get_current_key_type(Z_ARRVAL_P(mods)) != HASH_KEY_IS_LONG) {
			php_error_docref(NULL, E_WARNING, "Modifications array must not be string-indexed");
			RETURN_FALSE;
		}

		num_mods = zend_hash_num_elements(Z_ARRVAL_P(mods));

		for (i = 0; i < num_mods; i++) {
			/* is the numbering consecutive? */
			if ((fetched = zend_hash_index_find(Z_ARRVAL_P(mods), i)) == NULL) {
				php_error_docref(NULL, E_WARNING, "Modifications array must have consecutive indices 0, 1, ...");
				RETURN_FALSE;
			}
			mod = fetched;

			/* is it an array? */
			if (Z_TYPE_P(mod) != IS_ARRAY) {
				php_error_docref(NULL, E_WARNING, "Each entry of modifications array must be an array itself");
				RETURN_FALSE;
			}

			SEPARATE_ARRAY(mod);
			/* for the modification hashtable... */
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(mod));
			num_modprops = zend_hash_num_elements(Z_ARRVAL_P(mod));

			for (j = 0; j < num_modprops; j++) {
				/* are the keys strings? */
				if (zend_hash_get_current_key(Z_ARRVAL_P(mod), &modkey, &tmpUlong) != HASH_KEY_IS_STRING) {
					php_error_docref(NULL, E_WARNING, "Each entry of modifications array must be string-indexed");
					RETURN_FALSE;
				}

				/* is this a valid entry? */
				if (
					!_ldap_str_equal_to_const(ZSTR_VAL(modkey), ZSTR_LEN(modkey), LDAP_MODIFY_BATCH_ATTRIB) &&
					!_ldap_str_equal_to_const(ZSTR_VAL(modkey), ZSTR_LEN(modkey), LDAP_MODIFY_BATCH_MODTYPE) &&
					!_ldap_str_equal_to_const(ZSTR_VAL(modkey), ZSTR_LEN(modkey), LDAP_MODIFY_BATCH_VALUES)
				) {
					php_error_docref(NULL, E_WARNING, "The only allowed keys in entries of the modifications array are '" LDAP_MODIFY_BATCH_ATTRIB "', '" LDAP_MODIFY_BATCH_MODTYPE "' and '" LDAP_MODIFY_BATCH_VALUES "'");
					RETURN_FALSE;
				}

				fetched = zend_hash_get_current_data(Z_ARRVAL_P(mod));
				modinfo = fetched;

				/* does the value type match the key? */
				if (_ldap_str_equal_to_const(ZSTR_VAL(modkey), ZSTR_LEN(modkey), LDAP_MODIFY_BATCH_ATTRIB)) {
					if (Z_TYPE_P(modinfo) != IS_STRING) {
						php_error_docref(NULL, E_WARNING, "A '" LDAP_MODIFY_BATCH_ATTRIB "' value must be a string");
						RETURN_FALSE;
					}

					if (Z_STRLEN_P(modinfo) != _ldap_strlen_max(Z_STRVAL_P(modinfo), Z_STRLEN_P(modinfo))) {
						php_error_docref(NULL, E_WARNING, "A '" LDAP_MODIFY_BATCH_ATTRIB "' value must not contain NUL bytes");
						RETURN_FALSE;
					}
				}
				else if (_ldap_str_equal_to_const(ZSTR_VAL(modkey), ZSTR_LEN(modkey), LDAP_MODIFY_BATCH_MODTYPE)) {
					if (Z_TYPE_P(modinfo) != IS_LONG) {
						php_error_docref(NULL, E_WARNING, "A '" LDAP_MODIFY_BATCH_MODTYPE "' value must be a long");
						RETURN_FALSE;
					}

					/* is the value in range? */
					modtype = Z_LVAL_P(modinfo);
					if (
						modtype != LDAP_MODIFY_BATCH_ADD &&
						modtype != LDAP_MODIFY_BATCH_REMOVE &&
						modtype != LDAP_MODIFY_BATCH_REPLACE &&
						modtype != LDAP_MODIFY_BATCH_REMOVE_ALL
					) {
						php_error_docref(NULL, E_WARNING, "The '" LDAP_MODIFY_BATCH_MODTYPE "' value must match one of the LDAP_MODIFY_BATCH_* constants");
						RETURN_FALSE;
					}

					/* if it's REMOVE_ALL, there must not be a values array; otherwise, there must */
					if (modtype == LDAP_MODIFY_BATCH_REMOVE_ALL) {
						if (zend_hash_str_exists(Z_ARRVAL_P(mod), LDAP_MODIFY_BATCH_VALUES, strlen(LDAP_MODIFY_BATCH_VALUES))) {
							php_error_docref(NULL, E_WARNING, "If '" LDAP_MODIFY_BATCH_MODTYPE "' is LDAP_MODIFY_BATCH_REMOVE_ALL, a '" LDAP_MODIFY_BATCH_VALUES "' array must not be provided");
							RETURN_FALSE;
						}
					}
					else {
						if (!zend_hash_str_exists(Z_ARRVAL_P(mod), LDAP_MODIFY_BATCH_VALUES, strlen(LDAP_MODIFY_BATCH_VALUES))) {
							php_error_docref(NULL, E_WARNING, "If '" LDAP_MODIFY_BATCH_MODTYPE "' is not LDAP_MODIFY_BATCH_REMOVE_ALL, a '" LDAP_MODIFY_BATCH_VALUES "' array must be provided");
							RETURN_FALSE;
						}
					}
				}
				else if (_ldap_str_equal_to_const(ZSTR_VAL(modkey), ZSTR_LEN(modkey), LDAP_MODIFY_BATCH_VALUES)) {
					if (Z_TYPE_P(modinfo) != IS_ARRAY) {
						php_error_docref(NULL, E_WARNING, "A '" LDAP_MODIFY_BATCH_VALUES "' value must be an array");
						RETURN_FALSE;
					}

					SEPARATE_ARRAY(modinfo);
					/* is the array not empty? */
					zend_hash_internal_pointer_reset(Z_ARRVAL_P(modinfo));
					num_modvals = zend_hash_num_elements(Z_ARRVAL_P(modinfo));
					if (num_modvals == 0) {
						php_error_docref(NULL, E_WARNING, "A '" LDAP_MODIFY_BATCH_VALUES "' array must have at least one element");
						RETURN_FALSE;
					}

					/* are its keys integers? */
					if (zend_hash_get_current_key_type(Z_ARRVAL_P(modinfo)) != HASH_KEY_IS_LONG) {
						php_error_docref(NULL, E_WARNING, "A '" LDAP_MODIFY_BATCH_VALUES "' array must not be string-indexed");
						RETURN_FALSE;
					}

					/* are the keys consecutive? */
					for (k = 0; k < num_modvals; k++) {
						if ((fetched = zend_hash_index_find(Z_ARRVAL_P(modinfo), k)) == NULL) {
							php_error_docref(NULL, E_WARNING, "A '" LDAP_MODIFY_BATCH_VALUES "' array must have consecutive indices 0, 1, ...");
							RETURN_FALSE;
						}
					}
				}

				zend_hash_move_forward(Z_ARRVAL_P(mod));
			}
		}
	}
	/* validation was successful */

	/* allocate array of modifications */
	ldap_mods = safe_emalloc((num_mods+1), sizeof(LDAPMod *), 0);

	/* for each modification */
	for (i = 0; i < num_mods; i++) {
		/* allocate the modification struct */
		ldap_mods[i] = safe_emalloc(1, sizeof(LDAPMod), 0);

		/* fetch the relevant data */
		fetched = zend_hash_index_find(Z_ARRVAL_P(mods), i);
		mod = fetched;

		_ldap_hash_fetch(mod, LDAP_MODIFY_BATCH_ATTRIB, &attrib);
		_ldap_hash_fetch(mod, LDAP_MODIFY_BATCH_MODTYPE, &modtype);
		_ldap_hash_fetch(mod, LDAP_MODIFY_BATCH_VALUES, &vals);

		/* map the modification type */
		switch (Z_LVAL_P(modtype)) {
			case LDAP_MODIFY_BATCH_ADD:
				oper = LDAP_MOD_ADD;
				break;
			case LDAP_MODIFY_BATCH_REMOVE:
			case LDAP_MODIFY_BATCH_REMOVE_ALL:
				oper = LDAP_MOD_DELETE;
				break;
			case LDAP_MODIFY_BATCH_REPLACE:
				oper = LDAP_MOD_REPLACE;
				break;
			default:
				zend_throw_error(NULL, "Unknown and uncaught modification type.");
				RETVAL_FALSE;
				efree(ldap_mods[i]);
				num_mods = i;
				goto cleanup;
		}

		/* fill in the basic info */
		ldap_mods[i]->mod_op = oper | LDAP_MOD_BVALUES;
		ldap_mods[i]->mod_type = estrndup(Z_STRVAL_P(attrib), Z_STRLEN_P(attrib));

		if (Z_LVAL_P(modtype) == LDAP_MODIFY_BATCH_REMOVE_ALL) {
			/* no values */
			ldap_mods[i]->mod_bvalues = NULL;
		}
		else {
			/* allocate space for the values as part of this modification */
			num_modvals = zend_hash_num_elements(Z_ARRVAL_P(vals));
			ldap_mods[i]->mod_bvalues = safe_emalloc((num_modvals+1), sizeof(struct berval *), 0);

			/* for each value */
			for (j = 0; j < num_modvals; j++) {
				/* fetch it */
				fetched = zend_hash_index_find(Z_ARRVAL_P(vals), j);
				modval = zval_get_string(fetched);
				if (EG(exception)) {
					RETVAL_FALSE;
					ldap_mods[i]->mod_bvalues[j] = NULL;
					num_mods = i + 1;
					goto cleanup;
				}

				/* allocate the data struct */
				ldap_mods[i]->mod_bvalues[j] = safe_emalloc(1, sizeof(struct berval), 0);

				/* fill it */
				ldap_mods[i]->mod_bvalues[j]->bv_len = ZSTR_LEN(modval);
				ldap_mods[i]->mod_bvalues[j]->bv_val = estrndup(ZSTR_VAL(modval), ZSTR_LEN(modval));
				zend_string_release(modval);
			}

			/* NULL-terminate values */
			ldap_mods[i]->mod_bvalues[num_modvals] = NULL;
		}
	}

	/* NULL-terminate modifications */
	ldap_mods[num_mods] = NULL;

	if (serverctrls) {
		lserverctrls = _php_ldap_controls_from_array(ld->link, serverctrls);
		if (lserverctrls == NULL) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}

	/* perform (finally) */
	if ((i = ldap_modify_ext_s(ld->link, dn, ldap_mods, lserverctrls, NULL)) != LDAP_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Batch Modify: %s", ldap_err2string(i));
		RETVAL_FALSE;
	} else RETVAL_TRUE;

	/* clean up */
	cleanup: {
		for (i = 0; i < num_mods; i++) {
			/* attribute */
			efree(ldap_mods[i]->mod_type);

			if (ldap_mods[i]->mod_bvalues != NULL) {
				/* each BER value */
				for (j = 0; ldap_mods[i]->mod_bvalues[j] != NULL; j++) {
					/* free the data bytes */
					efree(ldap_mods[i]->mod_bvalues[j]->bv_val);

					/* free the bvalue struct */
					efree(ldap_mods[i]->mod_bvalues[j]);
				}

				/* the BER value array */
				efree(ldap_mods[i]->mod_bvalues);
			}

			/* the modification */
			efree(ldap_mods[i]);
		}

		/* the modifications array */
		efree(ldap_mods);

		if (lserverctrls) {
			_php_ldap_controls_free(&lserverctrls);
		}
	}
}
/* }}} */

/* {{{ proto int ldap_errno(resource link)
   Get the current ldap error number */
PHP_FUNCTION(ldap_errno)
{
	zval *link;
	ldap_linkdata *ld;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &link) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(_get_lderrno(ld->link));
}
/* }}} */

/* {{{ proto string ldap_err2str(int errno)
   Convert error number to error string */
PHP_FUNCTION(ldap_err2str)
{
	zend_long perrno;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &perrno) != SUCCESS) {
		return;
	}

	RETURN_STRING(ldap_err2string(perrno));
}
/* }}} */

/* {{{ proto string ldap_error(resource link)
   Get the current ldap error string */
PHP_FUNCTION(ldap_error)
{
	zval *link;
	ldap_linkdata *ld;
	int ld_errno;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &link) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	ld_errno = _get_lderrno(ld->link);

	RETURN_STRING(ldap_err2string(ld_errno));
}
/* }}} */

/* {{{ proto bool ldap_compare(resource link, string dn, string attr, string value)
   Determine if an entry has a specific value for one of its attributes */
PHP_FUNCTION(ldap_compare)
{
	zval *serverctrls = NULL;
	zval *link;
	char *dn, *attr, *value;
	size_t dn_len, attr_len, value_len;
	ldap_linkdata *ld;
	LDAPControl **lserverctrls = NULL;
	int errno;
	struct berval lvalue;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsss|a", &link, &dn, &dn_len, &attr, &attr_len, &value, &value_len, &serverctrls) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if (serverctrls) {
		lserverctrls = _php_ldap_controls_from_array(ld->link, serverctrls);
		if (lserverctrls == NULL) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}

	lvalue.bv_val = value;
	lvalue.bv_len = value_len;

	errno = ldap_compare_ext_s(ld->link, dn, attr, &lvalue, lserverctrls, NULL);

	switch (errno) {
		case LDAP_COMPARE_TRUE:
			RETVAL_TRUE;
			break;

		case LDAP_COMPARE_FALSE:
			RETVAL_FALSE;
			break;

		default:
			php_error_docref(NULL, E_WARNING, "Compare: %s", ldap_err2string(errno));
			RETVAL_LONG(-1);
	}

cleanup:
	if (lserverctrls) {
		_php_ldap_controls_free(&lserverctrls);
	}

	return;
}
/* }}} */

/* {{{ proto bool ldap_sort(resource link, resource result, string sortfilter)
   Sort LDAP result entries */
PHP_FUNCTION(ldap_sort)
{
	zval *link, *result;
	ldap_linkdata *ld;
	char *sortfilter;
	size_t sflen;
	zend_resource *le;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rrs", &link, &result, &sortfilter, &sflen) != SUCCESS) {
		RETURN_FALSE;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	le = Z_RES_P(result);
	if (le->type != le_result) {
		php_error_docref(NULL, E_WARNING, "Supplied resource is not a valid ldap result resource");
		RETURN_FALSE;
	}

	if (ldap_sort_entries(ld->link, (LDAPMessage **) &le->ptr, sflen ? sortfilter : NULL, strcmp) != LDAP_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s", ldap_err2string(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP
/* {{{ proto bool ldap_get_option(resource link, int option, mixed retval)
   Get the current value of various session-wide parameters */
PHP_FUNCTION(ldap_get_option)
{
	zval *link, *retval;
	ldap_linkdata *ld;
	zend_long option;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlz", &link, &option, &retval) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	switch (option) {
	/* options with int value */
	case LDAP_OPT_DEREF:
	case LDAP_OPT_SIZELIMIT:
	case LDAP_OPT_TIMELIMIT:
	case LDAP_OPT_PROTOCOL_VERSION:
	case LDAP_OPT_ERROR_NUMBER:
	case LDAP_OPT_REFERRALS:
#ifdef LDAP_OPT_RESTART
	case LDAP_OPT_RESTART:
#endif
#ifdef LDAP_OPT_X_SASL_NOCANON
	case LDAP_OPT_X_SASL_NOCANON:
#endif
#ifdef LDAP_OPT_X_TLS_REQUIRE_CERT
	case LDAP_OPT_X_TLS_REQUIRE_CERT:
#endif
#ifdef LDAP_OPT_X_TLS_CRLCHECK
	case LDAP_OPT_X_TLS_CRLCHECK:
#endif
#ifdef LDAP_OPT_X_TLS_PROTOCOL_MIN
	case LDAP_OPT_X_TLS_PROTOCOL_MIN:
#endif
#ifdef LDAP_OPT_X_KEEPALIVE_IDLE
	case LDAP_OPT_X_KEEPALIVE_IDLE:
	case LDAP_OPT_X_KEEPALIVE_PROBES:
	case LDAP_OPT_X_KEEPALIVE_INTERVAL:
#endif
		{
			int val;

			if (ldap_get_option(ld->link, option, &val)) {
				RETURN_FALSE;
			}
			ZEND_TRY_ASSIGN_REF_LONG(retval, val);
		} break;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
	case LDAP_OPT_NETWORK_TIMEOUT:
		{
			struct timeval *timeout = NULL;

			if (ldap_get_option(ld->link, LDAP_OPT_NETWORK_TIMEOUT, (void *) &timeout)) {
				if (timeout) {
					ldap_memfree(timeout);
				}
				RETURN_FALSE;
			}
			if (!timeout) {
				RETURN_FALSE;
			}
			ZEND_TRY_ASSIGN_REF_LONG(retval, timeout->tv_sec);
			ldap_memfree(timeout);
		} break;
#elif defined(LDAP_X_OPT_CONNECT_TIMEOUT)
	case LDAP_X_OPT_CONNECT_TIMEOUT:
		{
			int timeout;

			if (ldap_get_option(ld->link, LDAP_X_OPT_CONNECT_TIMEOUT, &timeout)) {
				RETURN_FALSE;
			}
			ZEND_TRY_ASSIGN_REF_LONG(retval, (timeout / 1000));
		} break;
#endif
#ifdef LDAP_OPT_TIMEOUT
	case LDAP_OPT_TIMEOUT:
		{
			struct timeval *timeout = NULL;

			if (ldap_get_option(ld->link, LDAP_OPT_TIMEOUT, (void *) &timeout)) {
				if (timeout) {
					ldap_memfree(timeout);
				}
				RETURN_FALSE;
			}
			if (!timeout) {
				RETURN_FALSE;
			}
			ZEND_TRY_ASSIGN_REF_LONG(retval, timeout->tv_sec);
			ldap_memfree(timeout);
		} break;
#endif
	/* options with string value */
	case LDAP_OPT_ERROR_STRING:
#ifdef LDAP_OPT_HOST_NAME
	case LDAP_OPT_HOST_NAME:
#endif
#ifdef HAVE_LDAP_SASL
	case LDAP_OPT_X_SASL_MECH:
	case LDAP_OPT_X_SASL_REALM:
	case LDAP_OPT_X_SASL_AUTHCID:
	case LDAP_OPT_X_SASL_AUTHZID:
#endif
#ifdef LDAP_OPT_X_SASL_USERNAME
	case LDAP_OPT_X_SASL_USERNAME:
#endif
#if (LDAP_API_VERSION > 2000)
	case LDAP_OPT_X_TLS_CACERTDIR:
	case LDAP_OPT_X_TLS_CACERTFILE:
	case LDAP_OPT_X_TLS_CERTFILE:
	case LDAP_OPT_X_TLS_CIPHER_SUITE:
	case LDAP_OPT_X_TLS_KEYFILE:
	case LDAP_OPT_X_TLS_RANDOM_FILE:
#endif
#ifdef LDAP_OPT_X_TLS_PACKAGE
	case LDAP_OPT_X_TLS_PACKAGE:
#endif
#ifdef LDAP_OPT_X_TLS_CRLFILE
	case LDAP_OPT_X_TLS_CRLFILE:
#endif
#ifdef LDAP_OPT_X_TLS_DHFILE
	case LDAP_OPT_X_TLS_DHFILE:
#endif
#ifdef LDAP_OPT_MATCHED_DN
	case LDAP_OPT_MATCHED_DN:
#endif
		{
			char *val = NULL;

			if (ldap_get_option(ld->link, option, &val) || val == NULL || *val == '\0') {
				if (val) {
					ldap_memfree(val);
				}
				RETURN_FALSE;
			}
			ZEND_TRY_ASSIGN_REF_STRING(retval, val);
			ldap_memfree(val);
		} break;
	case LDAP_OPT_SERVER_CONTROLS:
	case LDAP_OPT_CLIENT_CONTROLS:
		{
			LDAPControl **ctrls = NULL;

			if (ldap_get_option(ld->link, option, &ctrls) || ctrls == NULL) {
				if (ctrls) {
					ldap_memfree(ctrls);
				}
				RETURN_FALSE;
			}
			_php_ldap_controls_to_array(ld->link, ctrls, retval, 1);
		} break;
/* options not implemented
	case LDAP_OPT_API_INFO:
	case LDAP_OPT_API_FEATURE_INFO:
*/
	default:
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ldap_set_option(resource link, int option, mixed newval)
   Set the value of various session-wide parameters */
PHP_FUNCTION(ldap_set_option)
{
	zval *link, *newval;
	ldap_linkdata *ld;
	LDAP *ldap;
	zend_long option;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zlz", &link, &option, &newval) != SUCCESS) {
		return;
	}

	if (Z_TYPE_P(link) == IS_NULL) {
		ldap = NULL;
	} else {
		if ((ld = (ldap_linkdata *)zend_fetch_resource_ex(link, "ldap link", le_link)) == NULL) {
			RETURN_FALSE;
		}
		ldap = ld->link;
	}

	switch (option) {
	/* options with int value */
	case LDAP_OPT_DEREF:
	case LDAP_OPT_SIZELIMIT:
	case LDAP_OPT_TIMELIMIT:
	case LDAP_OPT_PROTOCOL_VERSION:
	case LDAP_OPT_ERROR_NUMBER:
#ifdef LDAP_OPT_DEBUG_LEVEL
	case LDAP_OPT_DEBUG_LEVEL:
#endif
#ifdef LDAP_OPT_X_TLS_REQUIRE_CERT
	case LDAP_OPT_X_TLS_REQUIRE_CERT:
#endif
#ifdef LDAP_OPT_X_TLS_CRLCHECK
	case LDAP_OPT_X_TLS_CRLCHECK:
#endif
#ifdef LDAP_OPT_X_TLS_PROTOCOL_MIN
	case LDAP_OPT_X_TLS_PROTOCOL_MIN:
#endif
#ifdef LDAP_OPT_X_KEEPALIVE_IDLE
	case LDAP_OPT_X_KEEPALIVE_IDLE:
	case LDAP_OPT_X_KEEPALIVE_PROBES:
	case LDAP_OPT_X_KEEPALIVE_INTERVAL:
#endif
		{
			int val;

			convert_to_long_ex(newval);
			if (ZEND_LONG_EXCEEDS_INT(Z_LVAL_P(newval))) {
				php_error_docref(NULL, E_WARNING, "Option value is too big");
				RETURN_FALSE;
			}
			val = (int)Z_LVAL_P(newval);
			if (ldap_set_option(ldap, option, &val)) {
				RETURN_FALSE;
			}
		} break;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
	case LDAP_OPT_NETWORK_TIMEOUT:
		{
			struct timeval timeout;

			convert_to_long_ex(newval);
			timeout.tv_sec = Z_LVAL_P(newval);
			timeout.tv_usec = 0;
			if (ldap_set_option(ldap, LDAP_OPT_NETWORK_TIMEOUT, (void *) &timeout)) {
				RETURN_FALSE;
			}
		} break;
#elif defined(LDAP_X_OPT_CONNECT_TIMEOUT)
	case LDAP_X_OPT_CONNECT_TIMEOUT:
		{
			int timeout;

			convert_to_long_ex(newval);
			timeout = 1000 * Z_LVAL_P(newval); /* Convert to milliseconds */
			if (ldap_set_option(ldap, LDAP_X_OPT_CONNECT_TIMEOUT, &timeout)) {
				RETURN_FALSE;
			}
		} break;
#endif
#ifdef LDAP_OPT_TIMEOUT
	case LDAP_OPT_TIMEOUT:
		{
			struct timeval timeout;

			convert_to_long_ex(newval);
			timeout.tv_sec = Z_LVAL_P(newval);
			timeout.tv_usec = 0;
			if (ldap_set_option(ldap, LDAP_OPT_TIMEOUT, (void *) &timeout)) {
				RETURN_FALSE;
			}
		} break;
#endif
		/* options with string value */
	case LDAP_OPT_ERROR_STRING:
#ifdef LDAP_OPT_HOST_NAME
	case LDAP_OPT_HOST_NAME:
#endif
#ifdef HAVE_LDAP_SASL
	case LDAP_OPT_X_SASL_MECH:
	case LDAP_OPT_X_SASL_REALM:
	case LDAP_OPT_X_SASL_AUTHCID:
	case LDAP_OPT_X_SASL_AUTHZID:
#endif
#if (LDAP_API_VERSION > 2000)
	case LDAP_OPT_X_TLS_CACERTDIR:
	case LDAP_OPT_X_TLS_CACERTFILE:
	case LDAP_OPT_X_TLS_CERTFILE:
	case LDAP_OPT_X_TLS_CIPHER_SUITE:
	case LDAP_OPT_X_TLS_KEYFILE:
	case LDAP_OPT_X_TLS_RANDOM_FILE:
#endif
#ifdef LDAP_OPT_X_TLS_CRLFILE
	case LDAP_OPT_X_TLS_CRLFILE:
#endif
#ifdef LDAP_OPT_X_TLS_DHFILE
	case LDAP_OPT_X_TLS_DHFILE:
#endif
#ifdef LDAP_OPT_MATCHED_DN
	case LDAP_OPT_MATCHED_DN:
#endif
		{
			zend_string *val;
			val = zval_get_string(newval);
			if (EG(exception)) {
				RETURN_FALSE;
			}
			if (ldap_set_option(ldap, option, ZSTR_VAL(val))) {
				zend_string_release(val);
				RETURN_FALSE;
			}
			zend_string_release(val);
		} break;
		/* options with boolean value */
	case LDAP_OPT_REFERRALS:
#ifdef LDAP_OPT_RESTART
	case LDAP_OPT_RESTART:
#endif
#ifdef LDAP_OPT_X_SASL_NOCANON
	case LDAP_OPT_X_SASL_NOCANON:
#endif
		{
			void *val;
			val = zend_is_true(newval) ? LDAP_OPT_ON : LDAP_OPT_OFF;
			if (ldap_set_option(ldap, option, val)) {
				RETURN_FALSE;
			}
		} break;
		/* options with control list value */
	case LDAP_OPT_SERVER_CONTROLS:
	case LDAP_OPT_CLIENT_CONTROLS:
		{
			LDAPControl **ctrls;
			int rc;

			if (Z_TYPE_P(newval) != IS_ARRAY) {
				php_error_docref(NULL, E_WARNING, "Expected array value for this option");
				RETURN_FALSE;
			}

			ctrls = _php_ldap_controls_from_array(ldap, newval);

			if (ctrls == NULL) {
				RETURN_FALSE;
			} else {
				rc = ldap_set_option(ldap, option, ctrls);
				_php_ldap_controls_free(&ctrls);
				if (rc != LDAP_SUCCESS) {
					RETURN_FALSE;
				}
			}
		} break;
	default:
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_LDAP_PARSE_RESULT
/* {{{ proto bool ldap_parse_result(resource link, resource result, int &errcode [, string &matcheddn [, string &errmsg [, array &referrals [, array &controls]]]])
   Extract information from result */
PHP_FUNCTION(ldap_parse_result)
{
	zval *link, *result, *errcode, *matcheddn, *errmsg, *referrals, *serverctrls;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;
	LDAPControl **lserverctrls = NULL;
	char **lreferrals, **refp;
	char *lmatcheddn, *lerrmsg;
	int rc, lerrcode, myargcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(myargcount, "rrz|zzzz", &link, &result, &errcode, &matcheddn, &errmsg, &referrals, &serverctrls) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((ldap_result = (LDAPMessage *)zend_fetch_resource(Z_RES_P(result), "ldap result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	rc = ldap_parse_result(ld->link, ldap_result, &lerrcode,
				myargcount > 3 ? &lmatcheddn : NULL,
				myargcount > 4 ? &lerrmsg : NULL,
				myargcount > 5 ? &lreferrals : NULL,
				myargcount > 6 ? &lserverctrls : NULL,
				0);
	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Unable to parse result: %s", ldap_err2string(rc));
		RETURN_FALSE;
	}

	ZEND_TRY_ASSIGN_REF_LONG(errcode, lerrcode);

	/* Reverse -> fall through */
	switch (myargcount) {
		case 7:
			_php_ldap_controls_to_array(ld->link, lserverctrls, serverctrls, 0);
		case 6:
			referrals = zend_try_array_init(referrals);
			if (!referrals) {
				return;
			}
			if (lreferrals != NULL) {
				refp = lreferrals;
				while (*refp) {
					add_next_index_string(referrals, *refp);
					refp++;
				}
				ldap_memvfree((void**)lreferrals);
			}
		case 5:
			if (lerrmsg == NULL) {
				ZEND_TRY_ASSIGN_REF_EMPTY_STRING(errmsg);
			} else {
				ZEND_TRY_ASSIGN_REF_STRING(errmsg, lerrmsg);
				ldap_memfree(lerrmsg);
			}
		case 4:
			if (lmatcheddn == NULL) {
				ZEND_TRY_ASSIGN_REF_EMPTY_STRING(matcheddn);
			} else {
				ZEND_TRY_ASSIGN_REF_STRING(matcheddn, lmatcheddn);
				ldap_memfree(lmatcheddn);
			}
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ Extended operation response parsing, Pierangelo Masarati */
#ifdef HAVE_LDAP_PARSE_EXTENDED_RESULT
/* {{{ proto bool ldap_parse_exop(resource link, resource result [, string &retdata [, string &retoid]])
   Extract information from extended operation result */
PHP_FUNCTION(ldap_parse_exop)
{
	zval *link, *result, *retdata, *retoid;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;
	char *lretoid;
	struct berval *lretdata;
	int rc, myargcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(myargcount, "rr|zz", &link, &result, &retdata, &retoid) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((ldap_result = (LDAPMessage *)zend_fetch_resource(Z_RES_P(result), "ldap result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	rc = ldap_parse_extended_result(ld->link, ldap_result,
				myargcount > 3 ? &lretoid: NULL,
				myargcount > 2 ? &lretdata: NULL,
				0);
	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Unable to parse extended operation result: %s", ldap_err2string(rc));
		RETURN_FALSE;
	}

	/* Reverse -> fall through */
	switch (myargcount) {
		case 4:
			if (lretoid == NULL) {
				ZEND_TRY_ASSIGN_REF_EMPTY_STRING(retoid);
			} else {
				ZEND_TRY_ASSIGN_REF_STRING(retoid, lretoid);
				ldap_memfree(lretoid);
			}
		case 3:
			/* use arg #3 as the data returned by the server */
			if (lretdata == NULL) {
				ZEND_TRY_ASSIGN_REF_EMPTY_STRING(retdata);
			} else {
				ZEND_TRY_ASSIGN_REF_STRINGL(retdata, lretdata->bv_val, lretdata->bv_len);
				ldap_memfree(lretdata->bv_val);
				ldap_memfree(lretdata);
			}
	}
	RETURN_TRUE;
}
/* }}} */
#endif
/* }}} */

/* {{{ proto resource ldap_first_reference(resource link, resource result)
   Return first reference */
PHP_FUNCTION(ldap_first_reference)
{
	zval *link, *result;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	LDAPMessage *ldap_result, *entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &link, &result) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((ldap_result = (LDAPMessage *)zend_fetch_resource(Z_RES_P(result), "ldap result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	if ((entry = ldap_first_reference(ld->link, ldap_result)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry = emalloc(sizeof(ldap_resultentry));
		RETVAL_RES(zend_register_resource(resultentry, le_result_entry));
		ZVAL_COPY(&resultentry->res, result);
		resultentry->data = entry;
		resultentry->ber = NULL;
	}
}
/* }}} */

/* {{{ proto resource ldap_next_reference(resource link, resource reference_entry)
   Get next reference */
PHP_FUNCTION(ldap_next_reference)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry, *resultentry_next;
	LDAPMessage *entry_next;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((resultentry = (ldap_resultentry *)zend_fetch_resource(Z_RES_P(result_entry), "ldap result entry", le_result_entry)) == NULL) {
		RETURN_FALSE;
	}

	if ((entry_next = ldap_next_reference(ld->link, resultentry->data)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry_next = emalloc(sizeof(ldap_resultentry));
		RETVAL_RES(zend_register_resource(resultentry_next, le_result_entry));
		ZVAL_COPY(&resultentry_next->res, &resultentry->res);
		resultentry_next->data = entry_next;
		resultentry_next->ber = NULL;
	}
}
/* }}} */

#ifdef HAVE_LDAP_PARSE_REFERENCE
/* {{{ proto bool ldap_parse_reference(resource link, resource reference_entry, array &referrals)
   Extract information from reference entry */
PHP_FUNCTION(ldap_parse_reference)
{
	zval *link, *result_entry, *referrals;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char **lreferrals, **refp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rrz", &link, &result_entry, &referrals) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((resultentry = (ldap_resultentry *)zend_fetch_resource(Z_RES_P(result_entry), "ldap result entry", le_result_entry)) == NULL) {
		RETURN_FALSE;
	}

	if (ldap_parse_reference(ld->link, resultentry->data, &lreferrals, NULL /* &serverctrls */, 0) != LDAP_SUCCESS) {
		RETURN_FALSE;
	}

	referrals = zend_try_array_init(referrals);
	if (!referrals) {
		return;
	}

	if (lreferrals != NULL) {
		refp = lreferrals;
		while (*refp) {
			add_next_index_string(referrals, *refp);
			refp++;
		}
		ldap_memvfree((void**)lreferrals);
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ php_ldap_do_rename
 */
static void php_ldap_do_rename(INTERNAL_FUNCTION_PARAMETERS, int ext)
{
	zval *serverctrls = NULL;
	zval *link;
	ldap_linkdata *ld;
	LDAPControl **lserverctrls = NULL;
	LDAPMessage *ldap_res;
	int rc, msgid;
	char *dn, *newrdn, *newparent;
	size_t dn_len, newrdn_len, newparent_len;
	zend_bool deleteoldrdn;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsssb|a", &link, &dn, &dn_len, &newrdn, &newrdn_len, &newparent, &newparent_len, &deleteoldrdn, &serverctrls) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if (newparent_len == 0) {
		newparent = NULL;
	}

#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP
	if (serverctrls) {
		lserverctrls = _php_ldap_controls_from_array(ld->link, serverctrls);
		if (lserverctrls == NULL) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}

	if (ext) {
		rc = ldap_rename(ld->link, dn, newrdn, newparent, deleteoldrdn, lserverctrls, NULL, &msgid);
	} else {
		rc = ldap_rename_s(ld->link, dn, newrdn, newparent, deleteoldrdn, lserverctrls, NULL);
	}
#else
	if (newparent_len != 0) {
		php_error_docref(NULL, E_WARNING, "You are using old LDAP API, newparent must be the empty string, can only modify RDN");
		RETURN_FALSE;
	}
	if (serverctrls) {
		php_error_docref(NULL, E_WARNING, "You are using old LDAP API, controls are not supported");
		RETURN_FALSE;
	}
	if (ext) {
		php_error_docref(NULL, E_WARNING, "You are using old LDAP API, ldap_rename_ext is not supported");
		RETURN_FALSE;
	}
/* could support old APIs but need check for ldap_modrdn2()/ldap_modrdn() */
	rc = ldap_modrdn2_s(ld->link, dn, newrdn, deleteoldrdn);
#endif

	if (rc != LDAP_SUCCESS) {
		RETVAL_FALSE;
	} else if (ext) {
		rc = ldap_result(ld->link, msgid, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
		if (rc == -1) {
			php_error_docref(NULL, E_WARNING, "Rename operation failed");
			RETVAL_FALSE;
			goto cleanup;
		}

		/* return a PHP control object */
		RETVAL_RES(zend_register_resource(ldap_res, le_result));
	} else {
		RETVAL_TRUE;
	}

cleanup:
	if (lserverctrls) {
		_php_ldap_controls_free(&lserverctrls);
	}

	return;
}
/* }}} */

/* {{{ proto bool ldap_rename(resource link, string dn, string newrdn, string newparent, bool deleteoldrdn [, array servercontrols])
   Modify the name of an entry */
PHP_FUNCTION(ldap_rename)
{
	php_ldap_do_rename(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto resource ldap_rename_ext(resource link, string dn, string newrdn, string newparent, bool deleteoldrdn [, array servercontrols])
   Modify the name of an entry */
PHP_FUNCTION(ldap_rename_ext)
{
	php_ldap_do_rename(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

#ifdef HAVE_LDAP_START_TLS_S
/* {{{ proto bool ldap_start_tls(resource link)
   Start TLS */
PHP_FUNCTION(ldap_start_tls)
{
	zval *link;
	ldap_linkdata *ld;
	int rc, protocol = LDAP_VERSION3;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &link) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if (((rc = ldap_set_option(ld->link, LDAP_OPT_PROTOCOL_VERSION, &protocol)) != LDAP_SUCCESS) ||
		((rc = ldap_start_tls_s(ld->link, NULL, NULL)) != LDAP_SUCCESS)
	) {
		php_error_docref(NULL, E_WARNING,"Unable to start TLS: %s", ldap_err2string(rc));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */
#endif
#endif /* (LDAP_API_VERSION > 2000) || HAVE_ORALDAP */

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
/* {{{ _ldap_rebind_proc()
*/
int _ldap_rebind_proc(LDAP *ldap, const char *url, ber_tag_t req, ber_int_t msgid, void *params)
{
	ldap_linkdata *ld;
	int retval;
	zval cb_args[2];
	zval cb_retval;
	zval *cb_link = (zval *) params;

	ld = (ldap_linkdata *) zend_fetch_resource_ex(cb_link, "ldap link", le_link);

	/* link exists and callback set? */
	if (ld == NULL || Z_ISUNDEF(ld->rebindproc)) {
		php_error_docref(NULL, E_WARNING, "Link not found or no callback set");
		return LDAP_OTHER;
	}

	/* callback */
	ZVAL_COPY_VALUE(&cb_args[0], cb_link);
	ZVAL_STRING(&cb_args[1], url);
	if (call_user_function_ex(EG(function_table), NULL, &ld->rebindproc, &cb_retval, 2, cb_args, 0, NULL) == SUCCESS && !Z_ISUNDEF(cb_retval)) {
		retval = zval_get_long(&cb_retval);
		zval_ptr_dtor(&cb_retval);
	} else {
		php_error_docref(NULL, E_WARNING, "rebind_proc PHP callback failed");
		retval = LDAP_OTHER;
	}
	zval_ptr_dtor(&cb_args[1]);
	return retval;
}
/* }}} */

/* {{{ proto bool ldap_set_rebind_proc(resource link, string callback)
   Set a callback function to do re-binds on referral chasing. */
PHP_FUNCTION(ldap_set_rebind_proc)
{
	zval *link, *callback;
	ldap_linkdata *ld;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &link, &callback) != SUCCESS) {
		RETURN_FALSE;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(callback) == IS_STRING && Z_STRLEN_P(callback) == 0) {
		/* unregister rebind procedure */
		if (!Z_ISUNDEF(ld->rebindproc)) {
			zval_ptr_dtor(&ld->rebindproc);
			ZVAL_UNDEF(&ld->rebindproc);
			ldap_set_rebind_proc(ld->link, NULL, NULL);
		}
		RETURN_TRUE;
	}

	/* callable? */
	if (!zend_is_callable(callback, 0, NULL)) {
		zend_string *callback_name = zend_get_callable_name(callback);
		php_error_docref(NULL, E_WARNING, "Two arguments expected for '%s' to be a valid callback", ZSTR_VAL(callback_name));
		zend_string_release_ex(callback_name, 0);
		RETURN_FALSE;
	}

	/* register rebind procedure */
	if (Z_ISUNDEF(ld->rebindproc)) {
		ldap_set_rebind_proc(ld->link, _ldap_rebind_proc, (void *) link);
	} else {
		zval_ptr_dtor(&ld->rebindproc);
	}

	ZVAL_COPY(&ld->rebindproc, callback);
	RETURN_TRUE;
}
/* }}} */
#endif

static zend_string* php_ldap_do_escape(const zend_bool *map, const char *value, size_t valuelen, zend_long flags)
{
	char hex[] = "0123456789abcdef";
	size_t i, p = 0;
	size_t len = 0;
	zend_string *ret;

	for (i = 0; i < valuelen; i++) {
		len += (map[(unsigned char) value[i]]) ? 3 : 1;
	}
	/* Per RFC 4514, a leading and trailing space must be escaped */
	if ((flags & PHP_LDAP_ESCAPE_DN) && (value[0] == ' ')) {
		len += 2;
	}
	if ((flags & PHP_LDAP_ESCAPE_DN) && ((valuelen > 1) && (value[valuelen - 1] == ' '))) {
		len += 2;
	}

	ret =  zend_string_alloc(len, 0);

	for (i = 0; i < valuelen; i++) {
		unsigned char v = (unsigned char) value[i];

		if (map[v] || ((flags & PHP_LDAP_ESCAPE_DN) && ((i == 0) || (i + 1 == valuelen)) && (v == ' '))) {
			ZSTR_VAL(ret)[p++] = '\\';
			ZSTR_VAL(ret)[p++] = hex[v >> 4];
			ZSTR_VAL(ret)[p++] = hex[v & 0x0f];
		} else {
			ZSTR_VAL(ret)[p++] = v;
		}
	}

	ZSTR_VAL(ret)[p] = '\0';
	ZSTR_LEN(ret) = p;
	return ret;
}

static void php_ldap_escape_map_set_chars(zend_bool *map, const char *chars, const size_t charslen, char escape)
{
	size_t i = 0;
	while (i < charslen) {
		map[(unsigned char) chars[i++]] = escape;
	}
}

PHP_FUNCTION(ldap_escape)
{
	char *value, *ignores;
	size_t valuelen = 0, ignoreslen = 0;
	int i;
	zend_long flags = 0;
	zend_bool map[256] = {0}, havecharlist = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|sl", &value, &valuelen, &ignores, &ignoreslen, &flags) != SUCCESS) {
		return;
	}

	if (!valuelen) {
		RETURN_EMPTY_STRING();
	}

	if (flags & PHP_LDAP_ESCAPE_FILTER) {
		havecharlist = 1;
		php_ldap_escape_map_set_chars(map, "\\*()\0", sizeof("\\*()\0") - 1, 1);
	}

	if (flags & PHP_LDAP_ESCAPE_DN) {
		havecharlist = 1;
		php_ldap_escape_map_set_chars(map, "\\,=+<>;\"#\r", sizeof("\\,=+<>;\"#\r") - 1, 1);
	}

	if (!havecharlist) {
		for (i = 0; i < 256; i++) {
			map[i] = 1;
		}
	}

	if (ignoreslen) {
		php_ldap_escape_map_set_chars(map, ignores, ignoreslen, 0);
	}

	RETURN_NEW_STR(php_ldap_do_escape(map, value, valuelen, flags));
}

#ifdef STR_TRANSLATION
/* {{{ php_ldap_do_translate
 */
static void php_ldap_do_translate(INTERNAL_FUNCTION_PARAMETERS, int way)
{
	char *value;
	size_t value_len;
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &value, &value_len) != SUCCESS) {
		return;
	}

	if (value_len == 0) {
		RETURN_FALSE;
	}

	if (way == 1) {
		result = ldap_8859_to_t61(&value, &value_len, 0);
	} else {
		result = ldap_t61_to_8859(&value, &value_len, 0);
	}

	if (result == LDAP_SUCCESS) {
		RETVAL_STRINGL(value, value_len);
		free(value);
	} else {
		php_error_docref(NULL, E_WARNING, "Conversion from iso-8859-1 to t61 failed: %s", ldap_err2string(result));
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string ldap_t61_to_8859(string value)
   Translate t61 characters to 8859 characters */
PHP_FUNCTION(ldap_t61_to_8859)
{
	php_ldap_do_translate(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string ldap_8859_to_t61(string value)
   Translate 8859 characters to t61 characters */
PHP_FUNCTION(ldap_8859_to_t61)
{
	php_ldap_do_translate(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */
#endif

#ifdef LDAP_CONTROL_PAGEDRESULTS
/* {{{ proto mixed ldap_control_paged_result(resource link, int pagesize [, bool iscritical [, string cookie]])
   Inject paged results control*/
PHP_FUNCTION(ldap_control_paged_result)
{
	zend_long pagesize;
	zend_bool iscritical;
	zval *link;
	char *cookie = NULL;
	size_t cookie_len = 0;
	struct berval lcookie = { 0L, NULL };
	ldap_linkdata *ld;
	LDAP *ldap;
	BerElement *ber = NULL;
	LDAPControl	ctrl, *ctrlsp[2];
	int rc, myargcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(myargcount, "rl|bs", &link, &pagesize, &iscritical, &cookie, &cookie_len) != SUCCESS) {
		return;
	}

	if (Z_TYPE_P(link) == IS_NULL) {
		ldap = NULL;
	} else {
		if ((ld = (ldap_linkdata *)zend_fetch_resource_ex(link, "ldap link", le_link)) == NULL) {
			RETURN_FALSE;
		}
		ldap = ld->link;
	}

	ber = ber_alloc_t(LBER_USE_DER);
	if (ber == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to alloc BER encoding resources for paged results control");
		RETURN_FALSE;
	}

	ctrl.ldctl_iscritical = 0;

	switch (myargcount) {
		case 4:
			lcookie.bv_val = cookie;
			lcookie.bv_len = cookie_len;
			/* fallthru */
		case 3:
			ctrl.ldctl_iscritical = (int)iscritical;
			/* fallthru */
	}

	if (ber_printf(ber, "{iO}", (int)pagesize, &lcookie) == LBER_ERROR) {
		php_error_docref(NULL, E_WARNING, "Unable to BER printf paged results control");
		RETVAL_FALSE;
		goto lcpr_error_out;
	}
	rc = ber_flatten2(ber, &ctrl.ldctl_value, 0);
	if (rc == LBER_ERROR) {
		php_error_docref(NULL, E_WARNING, "Unable to BER encode paged results control");
		RETVAL_FALSE;
		goto lcpr_error_out;
	}

	ctrl.ldctl_oid = LDAP_CONTROL_PAGEDRESULTS;

	if (ldap) {
		/* directly set the option */
		ctrlsp[0] = &ctrl;
		ctrlsp[1] = NULL;

		rc = ldap_set_option(ldap, LDAP_OPT_SERVER_CONTROLS, ctrlsp);
		if (rc != LDAP_SUCCESS) {
			php_error_docref(NULL, E_WARNING, "Unable to set paged results control: %s (%d)", ldap_err2string(rc), rc);
			RETVAL_FALSE;
			goto lcpr_error_out;
		}
		RETVAL_TRUE;
	} else {
		/* return a PHP control object */
		array_init(return_value);

		add_assoc_string(return_value, "oid", ctrl.ldctl_oid);
		if (ctrl.ldctl_value.bv_len) {
			add_assoc_stringl(return_value, "value", ctrl.ldctl_value.bv_val, ctrl.ldctl_value.bv_len);
		}
		if (ctrl.ldctl_iscritical) {
			add_assoc_bool(return_value, "iscritical", ctrl.ldctl_iscritical);
		}
	}

lcpr_error_out:
	if (ber != NULL) {
		ber_free(ber, 1);
	}
	return;
}
/* }}} */

/* {{{ proto bool ldap_control_paged_result_response(resource link, resource result [, string &cookie [, int &estimated]])
   Extract paged results control response */
PHP_FUNCTION(ldap_control_paged_result_response)
{
	zval *link, *result, *cookie, *estimated;
	struct berval lcookie;
	int lestimated;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;
	LDAPControl **lserverctrls, *lctrl;
	BerElement *ber;
	ber_tag_t tag;
	int rc, lerrcode, myargcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(myargcount, "rr|zz", &link, &result, &cookie, &estimated) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if ((ldap_result = (LDAPMessage *)zend_fetch_resource(Z_RES_P(result), "ldap result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	rc = ldap_parse_result(ld->link,
				ldap_result,
				&lerrcode,
				NULL,		/* matcheddn */
				NULL,		/* errmsg */
				NULL,		/* referrals */
				&lserverctrls,
				0);

	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Unable to parse result: %s (%d)", ldap_err2string(rc), rc);
		RETURN_FALSE;
	}

	if (lerrcode != LDAP_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Result is: %s (%d)", ldap_err2string(lerrcode), lerrcode);
		RETURN_FALSE;
	}

	if (lserverctrls == NULL) {
		php_error_docref(NULL, E_WARNING, "No server controls in result");
		RETURN_FALSE;
	}

	lctrl = ldap_control_find(LDAP_CONTROL_PAGEDRESULTS, lserverctrls, NULL);
	if (lctrl == NULL) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL, E_WARNING, "No paged results control response in result");
		RETURN_FALSE;
	}

	ber = ber_init(&lctrl->ldctl_value);
	if (ber == NULL) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL, E_WARNING, "Unable to alloc BER decoding resources for paged results control response");
		RETURN_FALSE;
	}

	tag = ber_scanf(ber, "{io}", &lestimated, &lcookie);
	(void)ber_free(ber, 1);

	if (tag == LBER_ERROR) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL, E_WARNING, "Unable to decode paged results control response");
		RETURN_FALSE;
	}

	if (lestimated < 0) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL, E_WARNING, "Invalid paged results control response value");
		RETURN_FALSE;
	}

	ldap_controls_free(lserverctrls);
	if (myargcount == 4) {
		ZEND_TRY_ASSIGN_REF_LONG(estimated, lestimated);
	}

 	if (lcookie.bv_len == 0) {
		ZEND_TRY_ASSIGN_REF_EMPTY_STRING(cookie);
 	} else {
		ZEND_TRY_ASSIGN_REF_STRINGL(cookie, lcookie.bv_val, lcookie.bv_len);
 	}
 	ldap_memfree(lcookie.bv_val);

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ Extended operations, Pierangelo Masarati */
#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
/* {{{ proto resource ldap_exop(resource link, string reqoid [, string reqdata [, array servercontrols [, string &retdata [, string &retoid]]]])
   Extended operation */
PHP_FUNCTION(ldap_exop)
{
	zval *serverctrls = NULL;
	zval *link, *retdata = NULL, *retoid = NULL;
	char *lretoid = NULL;
	zend_string *reqoid, *reqdata = NULL;
	struct berval lreqdata, *lretdata = NULL;
	ldap_linkdata *ld;
	LDAPMessage *ldap_res;
	LDAPControl **lserverctrls = NULL;
	int rc, msgid;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS|S!a!zz", &link, &reqoid, &reqdata, &serverctrls, &retdata, &retoid) != SUCCESS) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	if (reqdata) {
		lreqdata.bv_val = ZSTR_VAL(reqdata);
		lreqdata.bv_len = ZSTR_LEN(reqdata);
	} else {
		lreqdata.bv_len = 0;
	}

	if (serverctrls) {
		lserverctrls = _php_ldap_controls_from_array(ld->link, serverctrls);
		if (lserverctrls == NULL) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}

	if (retdata) {
		/* synchronous call */
		rc = ldap_extended_operation_s(ld->link, ZSTR_VAL(reqoid),
			lreqdata.bv_len > 0 ? &lreqdata: NULL,
			lserverctrls,
			NULL,
			retoid ? &lretoid : NULL,
			&lretdata );
		if (rc != LDAP_SUCCESS ) {
			php_error_docref(NULL, E_WARNING, "Extended operation %s failed: %s (%d)", ZSTR_VAL(reqoid), ldap_err2string(rc), rc);
			RETVAL_FALSE;
			goto cleanup;
		}

		if (retoid) {
			if (lretoid) {
				ZEND_TRY_ASSIGN_REF_STRING(retoid, lretoid);
				ldap_memfree(lretoid);
			} else {
				ZEND_TRY_ASSIGN_REF_EMPTY_STRING(retoid);
			}
		}

		if (lretdata) {
			ZEND_TRY_ASSIGN_REF_STRINGL(retdata, lretdata->bv_val, lretdata->bv_len);
			ldap_memfree(lretdata->bv_val);
			ldap_memfree(lretdata);
		} else {
			ZEND_TRY_ASSIGN_REF_EMPTY_STRING(retdata);
		}

		RETVAL_TRUE;
		goto cleanup;
	}

	/* asynchronous call */
	rc = ldap_extended_operation(ld->link, ZSTR_VAL(reqoid),
		lreqdata.bv_len > 0 ? &lreqdata: NULL,
		lserverctrls,
		NULL,
		&msgid);
	if (rc != LDAP_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Extended operation %s failed: %s (%d)", ZSTR_VAL(reqoid), ldap_err2string(rc), rc);
		RETVAL_FALSE;
		goto cleanup;
	}

	rc = ldap_result(ld->link, msgid, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
	if (rc == -1) {
		php_error_docref(NULL, E_WARNING, "Extended operation %s failed", ZSTR_VAL(reqoid));
		RETVAL_FALSE;
		goto cleanup;
	}

	/* return a PHP control object */
	RETVAL_RES(zend_register_resource(ldap_res, le_result));

	cleanup:
	if (lserverctrls) {
		_php_ldap_controls_free(&lserverctrls);
	}
}
/* }}} */
#endif

#ifdef HAVE_LDAP_PASSWD
/* {{{ proto bool|string ldap_exop_passwd(resource link [, string user [, string oldpw [, string newpw [, array ctrls]]]])
   Passwd modify extended operation */
PHP_FUNCTION(ldap_exop_passwd)
{
	zval *link, *serverctrls;
	struct berval luser = { 0L, NULL };
	struct berval loldpw = { 0L, NULL };
	struct berval lnewpw = { 0L, NULL };
	struct berval lgenpasswd = { 0L, NULL };
	LDAPControl *ctrl, **lserverctrls = NULL, *requestctrls[2] = { NULL, NULL };
	LDAPMessage* ldap_res = NULL;
	ldap_linkdata *ld;
	int rc, myargcount = ZEND_NUM_ARGS(), msgid, err;
	char* errmsg = NULL;

	if (zend_parse_parameters(myargcount, "r|sssz/", &link, &luser.bv_val, &luser.bv_len, &loldpw.bv_val, &loldpw.bv_len, &lnewpw.bv_val, &lnewpw.bv_len, &serverctrls) == FAILURE) {
		return;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETVAL_FALSE;
		goto cleanup;
	}

	switch (myargcount) {
		case 5:
			/* ldap_create_passwordpolicy_control() allocates ctrl */
			if (ldap_create_passwordpolicy_control(ld->link, &ctrl) == LDAP_SUCCESS) {
				requestctrls[0] = ctrl;
			}
	}

	/* asynchronous call to get result and controls */
	rc = ldap_passwd(ld->link, &luser,
		loldpw.bv_len > 0 ? &loldpw : NULL,
		lnewpw.bv_len > 0 ? &lnewpw : NULL,
		requestctrls,
		NULL, &msgid);

	if (requestctrls[0] != NULL) {
		ldap_control_free(requestctrls[0]);
	}

	if (rc != LDAP_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Passwd modify extended operation failed: %s (%d)", ldap_err2string(rc), rc);
		RETVAL_FALSE;
		goto cleanup;
	}

	rc = ldap_result(ld->link, msgid, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
	if ((rc < 0) || !ldap_res) {
		rc = _get_lderrno(ld->link);
		php_error_docref(NULL, E_WARNING, "Passwd modify extended operation failed: %s (%d)", ldap_err2string(rc), rc);
		RETVAL_FALSE;
		goto cleanup;
	}

	rc = ldap_parse_passwd(ld->link, ldap_res, &lgenpasswd);
	if( rc != LDAP_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Passwd modify extended operation failed: %s (%d)", ldap_err2string(rc), rc);
		RETVAL_FALSE;
		goto cleanup;
	}

	rc = ldap_parse_result(ld->link, ldap_res, &err, NULL, &errmsg, NULL, (myargcount > 4 ? &lserverctrls : NULL), 0);
	if( rc != LDAP_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Passwd modify extended operation failed: %s (%d)", ldap_err2string(rc), rc);
		RETVAL_FALSE;
		goto cleanup;
	}

	if (myargcount > 4) {
		_php_ldap_controls_to_array(ld->link, lserverctrls, serverctrls, 0);
	}

	/* return */
	if (lnewpw.bv_len == 0) {
		if (lgenpasswd.bv_len == 0) {
			RETVAL_EMPTY_STRING();
		} else {
			RETVAL_STRINGL(lgenpasswd.bv_val, lgenpasswd.bv_len);
		}
	} else if (err == LDAP_SUCCESS) {
		RETVAL_TRUE;
	} else {
		php_error_docref(NULL, E_WARNING, "Passwd modify extended operation failed: %s (%d)", (errmsg ? errmsg : ldap_err2string(err)), err);
		RETVAL_FALSE;
	}

cleanup:
	if (lgenpasswd.bv_val != NULL) {
		ldap_memfree(lgenpasswd.bv_val);
	}
	if (ldap_res != NULL) {
		ldap_msgfree(ldap_res);
	}
	if (errmsg != NULL) {
		ldap_memfree(errmsg);
	}
}
/* }}} */
#endif

#ifdef HAVE_LDAP_WHOAMI_S
/* {{{ proto bool|string ldap_exop_whoami(resource link)
   Whoami extended operation */
PHP_FUNCTION(ldap_exop_whoami)
{
	zval *link;
	struct berval *lauthzid;
	ldap_linkdata *ld;
	int rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	/* synchronous call */
	rc = ldap_whoami_s(ld->link, &lauthzid, NULL, NULL);
	if (rc != LDAP_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Whoami extended operation failed: %s (%d)", ldap_err2string(rc), rc);
		RETURN_FALSE;
	}

	if (lauthzid == NULL) {
		RETVAL_EMPTY_STRING();
	} else {
		RETVAL_STRINGL(lauthzid->bv_val, lauthzid->bv_len);
		ldap_memfree(lauthzid->bv_val);
		ldap_memfree(lauthzid);
	}
}
/* }}} */
#endif

#ifdef HAVE_LDAP_REFRESH_S
/* {{{ proto bool|int ldap_exop_refresh(resource link , string dn , int ttl)
   DDS refresh extended operation */
PHP_FUNCTION(ldap_exop_refresh)
{
	zval *link, *ttl;
	struct berval ldn;
	ber_int_t lttl;
	ber_int_t newttl;
	ldap_linkdata *ld;
	int rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsz", &link, &ldn.bv_val, &ldn.bv_len, &ttl) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}

	if ((ld = (ldap_linkdata *)zend_fetch_resource(Z_RES_P(link), "ldap link", le_link)) == NULL) {
		RETURN_FALSE;
	}

	lttl = (ber_int_t)zval_get_long(ttl);

	rc = ldap_refresh_s(ld->link, &ldn, lttl, &newttl, NULL, NULL);
	if (rc != LDAP_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Refresh extended operation failed: %s (%d)", ldap_err2string(rc), rc);
		RETURN_FALSE;
	}

	RETURN_LONG(newttl);
}
/* }}} */
#endif

/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_connect, 0, 0, 0)
	ZEND_ARG_INFO(0, hostname)
	ZEND_ARG_INFO(0, port)
#ifdef HAVE_ORALDAP
	ZEND_ARG_INFO(0, wallet)
	ZEND_ARG_INFO(0, wallet_passwd)
	ZEND_ARG_INFO(0, authmode)
#endif
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_resource, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_bind, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, bind_rdn)
	ZEND_ARG_INFO(0, bind_password)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_bind_ext, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, bind_rdn)
	ZEND_ARG_INFO(0, bind_password)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

#ifdef HAVE_LDAP_SASL
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_sasl_bind, 0, 0, 1)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, binddn)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, sasl_mech)
	ZEND_ARG_INFO(0, sasl_realm)
	ZEND_ARG_INFO(0, sasl_authz_id)
	ZEND_ARG_INFO(0, props)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_read, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, base_dn)
	ZEND_ARG_INFO(0, filter)
	ZEND_ARG_INFO(0, attributes)
	ZEND_ARG_INFO(0, attrsonly)
	ZEND_ARG_INFO(0, sizelimit)
	ZEND_ARG_INFO(0, timelimit)
	ZEND_ARG_INFO(0, deref)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_list, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, base_dn)
	ZEND_ARG_INFO(0, filter)
	ZEND_ARG_INFO(0, attributes)
	ZEND_ARG_INFO(0, attrsonly)
	ZEND_ARG_INFO(0, sizelimit)
	ZEND_ARG_INFO(0, timelimit)
	ZEND_ARG_INFO(0, deref)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_search, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, base_dn)
	ZEND_ARG_INFO(0, filter)
	ZEND_ARG_INFO(0, attributes)
	ZEND_ARG_INFO(0, attrsonly)
	ZEND_ARG_INFO(0, sizelimit)
	ZEND_ARG_INFO(0, timelimit)
	ZEND_ARG_INFO(0, deref)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_count_entries, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_first_entry, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_next_entry, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_entries, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_first_attribute, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_next_attribute, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_attributes, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_values, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
	ZEND_ARG_INFO(0, attribute)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_values_len, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
	ZEND_ARG_INFO(0, attribute)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_dn, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_explode_dn, 0, 0, 2)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, with_attrib)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_dn2ufn, 0, 0, 1)
	ZEND_ARG_INFO(0, dn)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_add, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_add_ext, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_delete, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_delete_ext, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_modify, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_modify_batch, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_ARRAY_INFO(0, modifications_info, 0)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_add, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_add_ext, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_replace, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_replace_ext, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_del, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_del_ext, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_err2str, 0, 0, 1)
	ZEND_ARG_INFO(0, errno)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_compare, 0, 0, 4)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, attribute)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_sort, 0, 0, 3)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, sortfilter)
ZEND_END_ARG_INFO()

#ifdef LDAP_CONTROL_PAGEDRESULTS
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_control_paged_result, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, pagesize)
	ZEND_ARG_INFO(0, iscritical)
	ZEND_ARG_INFO(0, cookie)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_control_paged_result_response, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(1, cookie)
	ZEND_ARG_INFO(1, estimated)
ZEND_END_ARG_INFO();
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_rename, 0, 0, 5)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, newrdn)
	ZEND_ARG_INFO(0, newparent)
	ZEND_ARG_INFO(0, deleteoldrdn)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_rename_ext, 0, 0, 5)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, newrdn)
	ZEND_ARG_INFO(0, newparent)
	ZEND_ARG_INFO(0, deleteoldrdn)
	ZEND_ARG_INFO(0, servercontrols)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_option, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(1, retval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_set_option, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_first_reference, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_next_reference, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()

#ifdef HAVE_LDAP_PARSE_REFERENCE
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_parse_reference, 0, 0, 3)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(1, referrals)
ZEND_END_ARG_INFO()
#endif


#ifdef HAVE_LDAP_PARSE_RESULT
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_parse_result, 0, 0, 3)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(1, errcode)
	ZEND_ARG_INFO(1, matcheddn)
	ZEND_ARG_INFO(1, errmsg)
	ZEND_ARG_INFO(1, referrals)
	ZEND_ARG_INFO(1, serverctrls)
ZEND_END_ARG_INFO()
#endif
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_set_rebind_proc, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_escape, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, ignore)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

#ifdef STR_TRANSLATION
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_t61_to_8859, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_8859_to_t61, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_exop, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, reqoid)
	ZEND_ARG_INFO(0, reqdata)
	ZEND_ARG_INFO(0, servercontrols)
	ZEND_ARG_INFO(1, retdata)
	ZEND_ARG_INFO(1, retoid)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_LDAP_PASSWD
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_exop_passwd, 0, 0, 1)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, oldpw)
	ZEND_ARG_INFO(0, newpw)
	ZEND_ARG_INFO(1, serverctrls)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_LDAP_WHOAMI_S
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_exop_whoami, 0, 0, 1)
	ZEND_ARG_INFO(0, link)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_LDAP_REFRESH_S
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_exop_refresh, 0, 0, 3)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, ttl)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_LDAP_PARSE_EXTENDED_RESULT
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_parse_exop, 0, 0, 4)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(1, retdata)
	ZEND_ARG_INFO(1, retoid)
ZEND_END_ARG_INFO()
#endif
/* }}} */

/*
	This is just a small subset of the functionality provided by the LDAP library. All the
	operations are synchronous. Referrals are not handled automatically.
*/
/* {{{ ldap_functions[]
 */
static const zend_function_entry ldap_functions[] = {
	PHP_FE(ldap_connect,								arginfo_ldap_connect)
	PHP_FALIAS(ldap_close,		ldap_unbind,			arginfo_ldap_resource)
	PHP_FE(ldap_bind,									arginfo_ldap_bind)
	PHP_FE(ldap_bind_ext,								arginfo_ldap_bind_ext)
#ifdef HAVE_LDAP_SASL
	PHP_FE(ldap_sasl_bind,								arginfo_ldap_sasl_bind)
#endif
	PHP_FE(ldap_unbind,									arginfo_ldap_resource)
	PHP_FE(ldap_read,									arginfo_ldap_read)
	PHP_FE(ldap_list,									arginfo_ldap_list)
	PHP_FE(ldap_search,									arginfo_ldap_search)
	PHP_FE(ldap_free_result,							arginfo_ldap_resource)
	PHP_FE(ldap_count_entries,							arginfo_ldap_count_entries)
	PHP_FE(ldap_first_entry,							arginfo_ldap_first_entry)
	PHP_FE(ldap_next_entry,								arginfo_ldap_next_entry)
	PHP_FE(ldap_get_entries,							arginfo_ldap_get_entries)
	PHP_FE(ldap_first_attribute,						arginfo_ldap_first_attribute)
	PHP_FE(ldap_next_attribute,							arginfo_ldap_next_attribute)
	PHP_FE(ldap_get_attributes,							arginfo_ldap_get_attributes)
	PHP_FALIAS(ldap_get_values,	ldap_get_values_len,	arginfo_ldap_get_values)
	PHP_FE(ldap_get_values_len,							arginfo_ldap_get_values_len)
	PHP_FE(ldap_get_dn,									arginfo_ldap_get_dn)
	PHP_FE(ldap_explode_dn,								arginfo_ldap_explode_dn)
	PHP_FE(ldap_dn2ufn,									arginfo_ldap_dn2ufn)
	PHP_FE(ldap_add,									arginfo_ldap_add)
	PHP_FE(ldap_add_ext,								arginfo_ldap_add_ext)
	PHP_FE(ldap_delete,									arginfo_ldap_delete)
	PHP_FE(ldap_delete_ext,								arginfo_ldap_delete_ext)
	PHP_FE(ldap_modify_batch,							arginfo_ldap_modify_batch)
	PHP_FALIAS(ldap_modify,		ldap_mod_replace,		arginfo_ldap_modify)

/* additional functions for attribute based modifications, Gerrit Thomson */
	PHP_FE(ldap_mod_add,								arginfo_ldap_mod_add)
	PHP_FE(ldap_mod_add_ext,							arginfo_ldap_mod_add_ext)
	PHP_FE(ldap_mod_replace,							arginfo_ldap_mod_replace)
	PHP_FE(ldap_mod_replace_ext,						arginfo_ldap_mod_replace_ext)
	PHP_FE(ldap_mod_del,								arginfo_ldap_mod_del)
	PHP_FE(ldap_mod_del_ext,							arginfo_ldap_mod_del_ext)
/* end gjt mod */

	PHP_FE(ldap_errno,									arginfo_ldap_resource)
	PHP_FE(ldap_err2str,								arginfo_ldap_err2str)
	PHP_FE(ldap_error,									arginfo_ldap_resource)
	PHP_FE(ldap_compare,								arginfo_ldap_compare)
	PHP_DEP_FE(ldap_sort,									arginfo_ldap_sort)

#if (LDAP_API_VERSION > 2000) || HAVE_ORALDAP
	PHP_FE(ldap_rename,									arginfo_ldap_rename)
	PHP_FE(ldap_rename_ext,								arginfo_ldap_rename_ext)
	PHP_FE(ldap_get_option,								arginfo_ldap_get_option)
	PHP_FE(ldap_set_option,								arginfo_ldap_set_option)
	PHP_FE(ldap_first_reference,						arginfo_ldap_first_reference)
	PHP_FE(ldap_next_reference,							arginfo_ldap_next_reference)
#ifdef HAVE_LDAP_PARSE_REFERENCE
	PHP_FE(ldap_parse_reference,						arginfo_ldap_parse_reference)
#endif
#ifdef HAVE_LDAP_PARSE_RESULT
	PHP_FE(ldap_parse_result,							arginfo_ldap_parse_result)
#endif
#ifdef HAVE_LDAP_START_TLS_S
	PHP_FE(ldap_start_tls,								arginfo_ldap_resource)
#endif
#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
	PHP_FE(ldap_exop,									arginfo_ldap_exop)
#endif
#ifdef HAVE_LDAP_PASSWD
	PHP_FE(ldap_exop_passwd,							arginfo_ldap_exop_passwd)
#endif
#ifdef HAVE_LDAP_WHOAMI_S
	PHP_FE(ldap_exop_whoami,							arginfo_ldap_exop_whoami)
#endif
#ifdef HAVE_LDAP_REFRESH_S
	PHP_FE(ldap_exop_refresh,							arginfo_ldap_exop_refresh)
#endif
#ifdef HAVE_LDAP_PARSE_EXTENDED_RESULT
	PHP_FE(ldap_parse_exop,								arginfo_ldap_parse_exop)
#endif
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
	PHP_FE(ldap_set_rebind_proc,						arginfo_ldap_set_rebind_proc)
#endif

	PHP_FE(ldap_escape,									arginfo_ldap_escape)

#ifdef STR_TRANSLATION
	PHP_FE(ldap_t61_to_8859,							arginfo_ldap_t61_to_8859)
	PHP_FE(ldap_8859_to_t61,							arginfo_ldap_8859_to_t61)
#endif

#ifdef LDAP_CONTROL_PAGEDRESULTS
	PHP_DEP_FE(ldap_control_paged_result,				arginfo_ldap_control_paged_result)
	PHP_DEP_FE(ldap_control_paged_result_response,		arginfo_ldap_control_paged_result_response)
#endif
	PHP_FE_END
};
/* }}} */

zend_module_entry ldap_module_entry = { /* {{{ */
	STANDARD_MODULE_HEADER,
	"ldap",
	ldap_functions,
	PHP_MINIT(ldap),
	PHP_MSHUTDOWN(ldap),
	NULL,
	NULL,
	PHP_MINFO(ldap),
	PHP_LDAP_VERSION,
	PHP_MODULE_GLOBALS(ldap),
	PHP_GINIT(ldap),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */
