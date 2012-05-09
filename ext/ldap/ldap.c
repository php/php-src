/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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
   | PHP 4.0 updates:  Zeev Suraski <zeev@zend.com>                       |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */
#define IS_EXT_MODULE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Additional headers for NetWare */
#if defined(NETWARE) && (NEW_LIBC)
#include <sys/select.h>
#include <sys/timeval.h>
#endif

#include "php.h"
#include "php_ini.h"

#include <stddef.h>

#include "ext/standard/dl.h"
#include "php_ldap.h"

#ifdef PHP_WIN32
#include <string.h>
#include "config.w32.h"
#if HAVE_NSLDAP
#include <winsock2.h>
#endif
#define strdup _strdup
#undef WINDOWS
#undef strcasecmp
#undef strncasecmp
#define WINSOCK 1
#define __STDC__ 1
#endif

#include "ext/standard/php_string.h"
#include "ext/standard/info.h"

#ifdef HAVE_LDAP_SASL_H
#include <sasl.h>
#elif defined(HAVE_LDAP_SASL_SASL_H)
#include <sasl/sasl.h>
#endif

typedef struct {
	LDAP *link;
#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
	zval *rebindproc;
#endif
} ldap_linkdata;

typedef struct {
	LDAPMessage *data;
	BerElement *ber;
	int id;
} ldap_resultentry;

ZEND_DECLARE_MODULE_GLOBALS(ldap)
static PHP_GINIT_FUNCTION(ldap);

static int le_link, le_result, le_result_entry;

#ifdef COMPILE_DL_LDAP
ZEND_GET_MODULE(ldap)
#endif

static void _close_ldap_link(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	ldap_linkdata *ld = (ldap_linkdata *)rsrc->ptr;

	ldap_unbind_s(ld->link);
#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
	if (ld->rebindproc != NULL) {
		zval_dtor(ld->rebindproc);
		FREE_ZVAL(ld->rebindproc);
	}
#endif
	efree(ld);
	LDAPG(num_links)--;
}
/* }}} */

static void _free_ldap_result(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	LDAPMessage *result = (LDAPMessage *)rsrc->ptr;
	ldap_msgfree(result);
}
/* }}} */

static void _free_ldap_result_entry(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	ldap_resultentry *entry = (ldap_resultentry *)rsrc->ptr;

	if (entry->ber != NULL) {
		ber_free(entry->ber, 0);
		entry->ber = NULL;
	}
	zend_list_delete(entry->id);
	efree(entry);
} 
/* }}} */

/* {{{ _php_ber_to_array_recursive
   Convert 'struct berval' to an indexed array. This function does the work
   recursively as BER is a recursive data structure.
 */
static int _php_ber_to_array_recursive(BerElement * element, zval * array)
{
	int error_count = 0;
	zval *new = NULL, *value = NULL, *root = NULL;
	char *cookie = NULL;
	ber_tag_t tag = LBER_ERROR;
	ber_len_t len = 0, bitstring_len = 0, bitstring_len_bytes = 0;
	ber_int_t int_value = 0;
	struct berval * berval_value = NULL;
	char * char_value = NULL;
	div_t bits_to_bytes = {0, 0};

	if(element != NULL && array != NULL) {
		for(
				tag = ber_first_element(element, &len, &cookie);
				tag != LBER_ERROR;
				tag = ber_next_element(element, &len, cookie))
		{
			switch(tag) {
				case LBER_BOOLEAN:
					if( LBER_ERROR != ber_scanf(element, "b" ,&int_value)) {
						ALLOC_INIT_ZVAL(value);
						array_init(value);

						add_assoc_string(value, "type", "boolean", 1);
						add_assoc_bool(value, "value", int_value ? 1 : 0);

						add_next_index_zval(array, value);
						value = NULL;
					} else {
						error_count++;
					}
					break;
				case LBER_INTEGER:
					if( LBER_ERROR != ber_scanf(element, "i", &int_value)) {
						ALLOC_INIT_ZVAL(value);
						array_init(value);

						add_assoc_string(value, "type", "integer", 1);
						add_assoc_long(value, "value", int_value);

						add_next_index_zval(array, value);
						value = NULL;
					} else {
						add_next_index_null(array);
						error_count++;
					}
					break;
				case LBER_BITSTRING:
					if( LBER_ERROR != ber_scanf(
								element,
								"B",
								&char_value,
								&bitstring_len)) {
						ALLOC_INIT_ZVAL(value);
						array_init(value);

						/* bitstrings length is in bits. We calculate enough
						   space for the whole bitstring.
						 */
						bits_to_bytes = div(abs(bitstring_len), 8);
						bitstring_len_bytes = bits_to_bytes.quot;
						if(bits_to_bytes.rem > 0) bitstring_len_bytes++;

						add_assoc_string(value, "type", "bitstring", 1);
						add_assoc_stringl(
								value,
								"value",
								char_value,
								bitstring_len_bytes,
								1);
						add_assoc_long(value, "lenght", bitstring_len);

						add_next_index_zval(array, value);
						value = NULL;

						ber_memfree(char_value);
						char_value=NULL;
					} else {
						error_count++;
					}
					break;
				case LBER_OCTETSTRING:
					if( LBER_ERROR != ber_scanf(element, "O", &berval_value)) {
						ALLOC_INIT_ZVAL(value);
						array_init(value);

						add_assoc_string(value, "type", "octetstring", 1);
						add_assoc_stringl(
								value,
								"value",
								berval_value->bv_val,
								berval_value->bv_len,
								1);
						add_assoc_long(value, "length", berval_value->bv_len);
						
						add_next_index_zval(array, value);
						value = NULL;

						ber_bvfree(berval_value);
						char_value = NULL;
					} else {
						add_next_index_null(array);
						error_count++;
					}
					break;
				case LBER_NULL:
					if( LBER_ERROR != ber_scanf(element, "n")) {
						ALLOC_INIT_ZVAL(value);
						array_init(value);

						add_assoc_string(value, "type", "null", 1);
						add_assoc_null(value, "value");

						add_next_index_zval(array, value);
						value = NULL;
					} else {
						add_next_index_null(array);
						error_count++;
					}
					break;
				case LBER_ENUMERATED:
					if( LBER_ERROR != ber_scanf(element, "e", &int_value)) {
						ALLOC_INIT_ZVAL(value);
						array_init(value);

						add_assoc_string(value, "type", "enumerated", 1);
						add_assoc_long(value, "value", int_value);

						add_next_index_zval(array, value);
					} else {
						add_next_index_null(array);
						error_count++;
					}
					break;
				/* Recursive call for sequence and set */
				case LBER_SEQUENCE:
				case LBER_SET:
					if(LBER_SEQUENCE == ber_scanf(element, "}") ||
							LBER_SET == ber_scanf(element, "]")) {
						return error_count;
					}

					ALLOC_INIT_ZVAL(value);
					array_init(value);
					
					if(tag == LBER_SEQUENCE) {
						add_assoc_string(value, "type", "sequence", 1);
					} else {
						add_assoc_string(value, "type", "set", 1);
					}

					ALLOC_INIT_ZVAL(new);
					array_init(new);
					error_count+=_php_ber_to_array_recursive(element, new);
					
					add_assoc_zval(value, "value", new);

					add_next_index_zval(array, value);
					new = NULL;
					break;
				default: break;	
			}

		}
	}
	
	return error_count;
}
/* }}}
 */

/* {{{ _php_ber_to_array
   Convert 'struct berval' to an indexed array. Does some initialization then
   call _php_ber_to_array_recursive.
 */
static int _php_ber_to_array(struct berval * ber, zval ** array) 
{
	int error_count = -1;
	ber_tag_t tag = LBER_DEFAULT;
	ber_len_t len = 0;
	BerElement *element = NULL;
	zval *new = NULL, *root = NULL;

	if(array != NULL) {
		if(	ber == NULL) {
			*array=NULL;
		} else {
			element = ber_init(ber);
			if(element == NULL) {
				*array=NULL;
			} else {
				/* First ber element is either a set or a sequence but libler
				   skips this firts one. We add it now. If it's not a sequence
				   or a set, we add a sequence anyway.
				   LBER_DEFAULT is returned if something's wrong.
		 		 */
				ALLOC_INIT_ZVAL(root);
				array_init(root);

				tag = ber_peek_tag(element, &len);
				if(tag == LBER_SET) {
					add_assoc_string(root, "type", "set", 1);
				} else {
					add_assoc_string(root, "type", "sequence", 1);
				}
				
				ALLOC_INIT_ZVAL(new);
				array_init(new);
				error_count = _php_ber_to_array_recursive(element, new);
				
				add_assoc_zval(root, "value", new);

				*array=root;
				ber_memfree(element);
				element = NULL;
			}
		}
	}

	return error_count;
}
/* }}}
 */

/* {{{ _php_ber_from_array_recursive
 */
static int _php_ber_from_array_recursive(HashTable * array, BerElement ** pelement) 
{
	zval **data = NULL, **type = NULL, **value = NULL, **length=NULL;
	HashPosition p;
	HashTable * sub = NULL;
	int i = 0, error_count = 0;
	BerElement * element = NULL;
	ber_tag_t tag = LBER_DEFAULT;

	if(pelement != NULL) {
		element = *pelement;
	} else {
		return -1;
	}

	if(array != NULL && element != NULL) {
		if(zend_hash_find(array, "type", sizeof("type"), (void **) &type) != FAILURE) {
			if(zend_hash_find(array, "value", sizeof("value"), (void **) &value) != FAILURE) {
				convert_to_string_ex(type);
				if(strncmp(Z_STRVAL_PP(type), "null", Z_STRLEN_PP(type)) == 0) {
					if(LBER_ERROR == ber_printf(element, "n")) {
						error_count++;
					}
				} else if(strncmp(
							Z_STRVAL_PP(type),
							"boolean",
							Z_STRLEN_PP(type))==0) {
					convert_to_long_ex(value);
					if(LBER_ERROR == ber_printf(
								element,
								"b",
								(ber_int_t)Z_LVAL_PP(value) ? 1 : 0)) {
						error_count++;
					}
				} else if(strncmp(
							Z_STRVAL_PP(type),
							"integer",
							Z_STRLEN_PP(type))==0) {
					convert_to_long_ex(value);
					if(LBER_ERROR == ber_printf(
								element,
								"i",
								(ber_int_t)Z_LVAL_PP(value))) {
						error_count++;
					}
				} else if(strncmp(
							Z_STRVAL_PP(type),
							"bitstring",
							Z_STRLEN_PP(type))==0) {
					
					convert_to_string_ex(value);
					if(FAILURE != zend_hash_find(
								array,
								"length",
								sizeof("length"), 
								(void **) &length)) {
						convert_to_long_ex(length);
						
						if(LBER_ERROR == ber_printf(
									element,
									"B",
									Z_STRVAL_PP(value),
									(ber_len_t)Z_LVAL_PP(length))) {
							error_count++;
						}
					} else {
						if(LBER_ERROR == ber_printf(
									element,
									"B",
									Z_STRVAL_PP(value),
									/* bistrings require number of _bits_ */
									(ber_len_t)(Z_STRLEN_PP(value) * 8))) {
							error_count++;
						}
					}
				} else if(strncmp(
							Z_STRVAL_PP(type),
							"octetstring",
							Z_STRLEN_PP(type))==0) {
					convert_to_string_ex(value);
					
					if(FAILURE != zend_hash_find(
								array,
								"length",
								sizeof("length"), 
								(void **) &length)) {
						convert_to_long_ex(length);

						if(LBER_ERROR == ber_printf(
									element,
									"o",
									Z_STRVAL_PP(value),
									(ber_len_t)Z_LVAL_PP(length))) {
							error_count++;
						}
					} else {
						if(LBER_ERROR == ber_printf(
									element,
									"o",
									Z_STRVAL_PP(value),
									(ber_len_t)Z_STRLEN_PP(value))) {
							error_count++;
						}
					}
				} else if(strncmp(
							Z_STRVAL_PP(type),
							"enumerated",
							Z_STRLEN_PP(type)) == 0) {
					convert_to_long_ex(value);
					if(LBER_ERROR == ber_printf(
								element,
								"e",
								(ber_int_t)Z_LVAL_PP(value))) {
						error_count++;
					}
				} else if(
						(strncmp(Z_STRVAL_PP(type), "sequence", Z_STRLEN_PP(type)) == 0) ||
						(strncmp(Z_STRVAL_PP(type), "set", Z_STRLEN_PP(type)) == 0)) {
					if(Z_TYPE_PP(value) == IS_ARRAY) {
						if(strncmp(Z_STRVAL_PP(type), "set", Z_STRLEN_PP(type)) == 0) {
							tag = ber_printf(element, "[");
						} else {
							tag = ber_printf(element, "{");
						}
						if(LBER_ERROR != tag) {
							for(
									zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(value), &p);
									zend_hash_get_current_data_ex(Z_ARRVAL_PP(value), (void **) &data, &p) == SUCCESS;
									zend_hash_move_forward_ex(Z_ARRVAL_PP(value), &p)) {
								if(Z_TYPE_PP(data) == IS_ARRAY) {
									sub = Z_ARRVAL_PP(data);
									error_count+=_php_ber_from_array_recursive(
											sub,
											&element);
								}
							}
							
							if(strncmp(Z_STRVAL_PP(type), "set", Z_STRLEN_PP(type)) == 0) {
								tag = ber_printf(element, "]");
							} else {
								tag = ber_printf(element, "}");
							}
							if(LBER_ERROR == tag) {
								error_count++;
							}
						} else {
							error_count++;
						}
					} else {
						error_count++;
					}
				} else {
					error_count++;
				}
			} else {
				error_count++;
			}
		} else {
			error_count++;
		}
	}
	*pelement = element;

	return error_count;
}
/* }}}
 */

/* {{{ _php_ber_from_array
 */
static int _php_ber_from_array(HashTable * array, struct berval ** ber)
{
	BerElement * new = NULL;
	int error_count = -1;
	
	if(array != NULL && ber != NULL) {
		new = ber_alloc_t(LBER_USE_DER);
		
		error_count=_php_ber_from_array_recursive(array, &new);

		if(error_count == 0) {
			if(LBER_ERROR == ber_flatten(new, ber)) {
				*ber = NULL;
			}
		} else {
			*ber = NULL;
		}

		ber_memfree(new);
	}
	return error_count;
}
/* }}}
 */

/* _php_ldap_create_controls returns <= 0 if ctrls must not be freed by the
 * caller and > 0 if it must be freed. controls_succeed, if not NULL, returns
 * the number of controls created. controls_expected returns the number of
 * controls that should have been created. Everything is fine when return value
 * is > 1 and controls_succeed == controls_expected.
 */
/* {{{ _php_ldap_create_controls
 */
static int _php_ldap_create_controls(HashTable * array, LDAPControl *** ctrls, 
		int * controls_succeed, int * controls_expected)
{
	int ctrls_created = 0, ret = -1, iscritical = 0, to_create = 0, i = 0;
	zval ** php_ctrl = NULL, **php_val = NULL;
	LDAPControl *ctrl = NULL, **ctrlp = NULL;
	HashPosition p;
	char * oid = NULL;
	struct berval * value = NULL;

	if(ctrls == NULL) return ret;

	if((to_create = zend_hash_num_elements(array)) > 0) {

		*ctrls = safe_emalloc((to_create + 1),  sizeof(***ctrls), 0);

		if(*ctrls != NULL) {
			/* If ret > 0, ctrls must be freed by the caller */
			ret = 1;

			ctrlp = *ctrls;
			/* Set to NULL the whole array */
			for(i = 0;i < (to_create + 1); i++) *(ctrlp + i) = NULL;
			ctrlp = *ctrls;

			for(zend_hash_internal_pointer_reset_ex(array, &p);
					zend_hash_get_current_data_ex(array, (void **)&php_ctrl, &p) == SUCCESS;
					zend_hash_move_forward_ex(array, &p)) {
				oid = NULL;
				value = NULL;

				/* Get OID */
				if(Z_TYPE_PP(php_ctrl) == IS_ARRAY) {
					if(zend_hash_find(Z_ARRVAL_PP(php_ctrl), "oid", sizeof("oid"),
								(void **)&php_val) == SUCCESS) {
						if(Z_TYPE_PP(php_val) == IS_STRING) {
							convert_to_string_ex(php_val);
							oid = Z_STRVAL_PP(php_val);
						}
					}
				}

				if(oid != NULL && strlen(oid) > 0) {
					if(zend_hash_find(Z_ARRVAL_PP(php_ctrl), "iscritical",
								sizeof("iscritical"), (void **)&php_val) == SUCCESS) {
						convert_to_boolean_ex(php_val);
						iscritical = Z_BVAL_PP(php_val) ? 1 : 0;
					} else {
						iscritical = 0;
					}

					if(zend_hash_find(Z_ARRVAL_PP(php_ctrl), "value", sizeof("value"),
								(void **)&php_val) == SUCCESS) {
						/* If value is not an array, we assume it's a berval. */
						if(Z_TYPE_PP(php_val) != IS_ARRAY) {
							convert_to_string_ex(php_val);
							value = ber_memalloc(sizeof * value);
							if(value != NULL) {
								value->bv_val = Z_STRVAL_PP(php_val);
								value->bv_len = Z_STRLEN_PP(php_val);
							}
						} else {
							if(_php_ber_from_array(Z_ARRVAL_PP(php_val), &value) != 0) {
								if(value != NULL) {
									ber_memfree(value);
									value = NULL;
								}
							}
						}
					} else {
						value = NULL;
					}
				
					if(LDAP_SUCCESS == ldap_control_create(oid, iscritical,
								value, 1, &ctrl)) {
						*ctrlp = ctrl;
						++ctrlp;
						++ctrls_created;
					}
					/* Free value as ldap_control_create copies value */
					if(value != NULL) {
						ber_memfree(value);
						value = NULL;
					}
				}
			}
		}
	} else {
		/* No error, but no controls to create */
		ret = 0;
	}

	if(controls_succeed != NULL) {
		*controls_succeed = ctrls_created;
	}
	if(controls_expected != NULL) {
		*controls_expected = to_create;
	}

	return ret;
}
/* }}}
 */

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

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10
	/* LDAP options */
	REGISTER_LONG_CONSTANT("LDAP_OPT_DEREF", LDAP_OPT_DEREF, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_SIZELIMIT", LDAP_OPT_SIZELIMIT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_TIMELIMIT", LDAP_OPT_TIMELIMIT, CONST_PERSISTENT | CONST_CS);
#ifdef LDAP_OPT_NETWORK_TIMEOUT
	REGISTER_LONG_CONSTANT("LDAP_OPT_NETWORK_TIMEOUT", LDAP_OPT_NETWORK_TIMEOUT, CONST_PERSISTENT | CONST_CS);
#elif defined (LDAP_X_OPT_CONNECT_TIMEOUT)
	REGISTER_LONG_CONSTANT("LDAP_OPT_NETWORK_TIMEOUT", LDAP_X_OPT_CONNECT_TIMEOUT, CONST_PERSISTENT | CONST_CS);
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

#ifdef HAVE_LDAP_SASL
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_MECH", LDAP_OPT_X_SASL_MECH, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_REALM", LDAP_OPT_X_SASL_REALM, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_AUTHCID", LDAP_OPT_X_SASL_AUTHCID, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_X_SASL_AUTHZID", LDAP_OPT_X_SASL_AUTHZID, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef ORALDAP
	REGISTER_LONG_CONSTANT("GSLC_SSL_NO_AUTH", GSLC_SSL_NO_AUTH, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("GSLC_SSL_ONEWAY_AUTH", GSLC_SSL_ONEWAY_AUTH, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("GSLC_SSL_TWOWAY_AUTH", GSLC_SSL_TWOWAY_AUTH, CONST_PERSISTENT | CONST_CS);
#endif

	le_link = zend_register_list_destructors_ex(_close_ldap_link, NULL, "ldap link", module_number);
	le_result = zend_register_list_destructors_ex(_free_ldap_result, NULL, "ldap result", module_number);
	le_result_entry = zend_register_list_destructors_ex(_free_ldap_result_entry, NULL, "ldap result entry", module_number);

	Z_TYPE(ldap_module_entry) = type;

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
#if HAVE_NSLDAP
	LDAPVersion ver;
	double SDKVersion;
#endif

	php_info_print_table_start();
	php_info_print_table_row(2, "LDAP Support", "enabled");
	php_info_print_table_row(2, "RCS Version", "$Id$");

	if (LDAPG(max_links) == -1) {
		snprintf(tmp, 31, "%ld/unlimited", LDAPG(num_links));
	} else {
		snprintf(tmp, 31, "%ld/%ld", LDAPG(num_links), LDAPG(max_links));
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

#if HAVE_NSLDAP
	SDKVersion = ldap_version(&ver);
	snprintf(tmp, 31, "%F", SDKVersion/100.0);
	php_info_print_table_row(2, "SDK Version", tmp);

	snprintf(tmp, 31, "%F", ver.protocol_version/100.0);
	php_info_print_table_row(2, "Highest LDAP Protocol Supported", tmp);

	snprintf(tmp, 31, "%F", ver.SSL_version/100.0);
	php_info_print_table_row(2, "SSL Level Supported", tmp);

	if (ver.security_level != LDAP_SECURITY_NONE) {
		snprintf(tmp, 31, "%d", ver.security_level);
	} else {
		strcpy(tmp, "SSL not enabled");
	}
	php_info_print_table_row(2, "Level of Encryption", tmp);
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
	int hostlen;
	long port = 389; /* Default port */
#ifdef HAVE_ORALDAP
	char *wallet = NULL, *walletpasswd = NULL;
	int walletlen = 0, walletpasswdlen = 0;
	long authmode = GSLC_SSL_NO_AUTH;
	int ssl=0;
#endif
	ldap_linkdata *ld;
	LDAP *ldap;

#ifdef HAVE_ORALDAP
	if (ZEND_NUM_ARGS() == 3 || ZEND_NUM_ARGS() == 4) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|slssl", &host, &hostlen, &port, &wallet, &walletlen, &walletpasswd, &walletpasswdlen, &authmode) != SUCCESS) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 5) {
		ssl = 1;
	}
#else
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sl", &host, &hostlen, &port) != SUCCESS) {
		RETURN_FALSE;
	}
#endif

	if (LDAPG(max_links) != -1 && LDAPG(num_links) >= LDAPG(max_links)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too many open links (%ld)", LDAPG(num_links));
		RETURN_FALSE;
	}

	ld = ecalloc(1, sizeof(ldap_linkdata));

#ifdef LDAP_API_FEATURE_X_OPENLDAP
	if (host != NULL && strchr(host, '/')) {
		int rc;
		
		rc = ldap_initialize(&ldap, host);
		if (rc != LDAP_SUCCESS) {
			efree(ld);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create session handle: %s", ldap_err2string(rc));
			RETURN_FALSE;
		}
	} else {
		ldap = ldap_init(host, port);
	}
#else
	ldap = ldap_open(host, port);
#endif
	
	if (ldap == NULL) {
		efree(ld);
		RETURN_FALSE;
	} else {
#ifdef HAVE_ORALDAP
		if (ssl) {
			if (ldap_init_SSL(&ldap->ld_sb, wallet, walletpasswd, authmode)) {
				efree(ld);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSL init failed");
				RETURN_FALSE;
			}
		}			
#endif
		LDAPG(num_links)++;
		ld->link = ldap;
		ZEND_REGISTER_RESOURCE(return_value, ld, le_link);
	}

}
/* }}} */

/* {{{ _get_lderrno
 */
static int _get_lderrno(LDAP *ldap)
{
#if !HAVE_NSLDAP
#if LDAP_API_VERSION > 2000 || HAVE_ORALDAP_10
	int lderr;

	/* New versions of OpenLDAP do it this way */
	ldap_get_option(ldap, LDAP_OPT_ERROR_NUMBER, &lderr);
	return lderr;
#else
	return ldap->ld_errno;
#endif
#else
	return ldap_get_lderrno(ldap, NULL, NULL);
#endif
}
/* }}} */

/* {{{ proto bool ldap_bind(resource link [, string dn [, string password]])
   Bind to LDAP directory */
PHP_FUNCTION(ldap_bind)
{
	zval *link;
	char *ldap_bind_dn = NULL, *ldap_bind_pw = NULL;
	int ldap_bind_dnlen, ldap_bind_pwlen;
	ldap_linkdata *ld;
	int rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|ss", &link, &ldap_bind_dn, &ldap_bind_dnlen, &ldap_bind_pw, &ldap_bind_pwlen) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if ((rc = ldap_bind_s(ld->link, ldap_bind_dn, ldap_bind_pw, LDAP_AUTH_SIMPLE)) != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to bind to server: %s", ldap_err2string(rc));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
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
	int rc, dn_len, passwd_len, mech_len, realm_len, authc_id_len, authz_id_len, props_len;
	php_ldap_bictx *ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|sssssss", &link, &binddn, &dn_len, &passwd, &passwd_len, &sasl_mech, &mech_len, &sasl_realm, &realm_len, &sasl_authc_id, &authc_id_len, &sasl_authz_id, &authz_id_len, &props, &props_len) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	ctx = _php_sasl_setdefs(ld->link, sasl_mech, sasl_realm, sasl_authc_id, passwd, sasl_authz_id);

	if (props) {
		ldap_set_option(ld->link, LDAP_OPT_X_SASL_SECPROPS, props);
	}

	rc = ldap_sasl_interactive_bind_s(ld->link, binddn, ctx->mech, NULL, NULL, LDAP_SASL_QUIET, _php_sasl_interact, ctx);
	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to bind to server: %s", ldap_err2string(rc));
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	zend_list_delete(Z_LVAL_P(link));
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_set_opts
 */
#if LDAP_API_VERSION > 2000
static void php_set_opts(LDAP *ldap, int timelimit, int deref, int *old_timelimit, int *old_deref)
{
#else
static void php_set_opts(LDAP *ldap, int sizelimit, int timelimit, int deref, int *old_sizelimit, int *old_timelimit, int *old_deref)
{
	/* sizelimit */
	if (sizelimit > -1) {
#if (LDAP_API_VERSION >= 2004) || HAVE_NSLDAP || HAVE_ORALDAP_10
		ldap_get_option(ldap, LDAP_OPT_SIZELIMIT, old_sizelimit);
		ldap_set_option(ldap, LDAP_OPT_SIZELIMIT, &sizelimit);
#else
		*old_sizelimit = ldap->ld_sizelimit; 
		ldap->ld_sizelimit = sizelimit; 
#endif
	}
#endif
	/* timelimit */
	if (timelimit > -1) {
#if (LDAP_API_VERSION >= 2004) || HAVE_NSLDAP || HAVE_ORALDAP_10
		ldap_get_option(ldap, LDAP_OPT_SIZELIMIT, old_timelimit);
		ldap_set_option(ldap, LDAP_OPT_TIMELIMIT, &timelimit);
#else
		*old_timelimit = ldap->ld_timelimit; 
		ldap->ld_timelimit = timelimit; 
#endif
	}

	/* deref */
	if (deref > -1) {
#if (LDAP_API_VERSION >= 2004) || HAVE_NSLDAP || HAVE_ORALDAP_10
		ldap_get_option(ldap, LDAP_OPT_SIZELIMIT, old_deref);
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
	zval *link, *base_dn, **filter, *attrs = NULL, **attr;
	long attrsonly, sizelimit, timelimit, deref;
	char *ldap_base_dn = NULL, *ldap_filter = NULL, **ldap_attrs = NULL; 
	ldap_linkdata *ld = NULL;
	LDAPMessage *ldap_res;
#if LDAP_API_VERSION > 2000
	int ctrls_count = 0, created_ctrls = 0, expected_ctrls = 0;
	LDAPControl ** ctrls[2] = { NULL, NULL }, ** tmp_ctrls = NULL;
	zval * zctrls[2] = { NULL, NULL };
#endif
	int ldap_attrsonly = 0, ldap_sizelimit = -1, ldap_timelimit = -1, ldap_deref = -1;	 
	int old_ldap_sizelimit = -1, old_ldap_timelimit = -1, old_ldap_deref = -1;	 
	int num_attribs = 0, ret = 1, i, errno, argcount = ZEND_NUM_ARGS();

#if LDAP_API_VERSION > 2000
	if (zend_parse_parameters(argcount TSRMLS_CC, "zzZ|allllzz", &link, &base_dn, &filter, &attrs, &attrsonly,
		&sizelimit, &timelimit, &deref, &zctrls[0], &zctrls[1]) == FAILURE) {
#else
	if (zend_parse_parameters(argcount TSRMLS_CC, "zzZ|allll", &link, &base_dn, &filter, &attrs, &attrsonly,
		&sizelimit, &timelimit, &deref) == FAILURE) {
#endif
		return;
	}

	/* Reverse -> fall through */
	switch (argcount) {
#if LDAP_API_VERSION > 2000
		case 10:
			ctrls_count = 1;
		case 9:
			for(;ctrls_count >= 0; ctrls_count--) {
				if((Z_TYPE_P(zctrls[i]) == IS_ARRAY) &&
						(zend_hash_num_elements(Z_ARRVAL_P(zctrls[i])) > 0)) {
					if(_php_ldap_create_controls(Z_ARRVAL_P(zctrls[i]), &ctrls[i],
								&created_ctrls, &expected_ctrls) > 0) {
						if(created_ctrls != expected_ctrls) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING,
									"Not all controls have been converted to "
									"LDAPControl. Expected %d, created %d.",
									expected_ctrls, created_ctrls);
						}
					} else {
						zctrls[i] = NULL;
					}
				}
			}
#endif
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
				if (zend_hash_index_find(Z_ARRVAL_P(attrs), i, (void **) &attr) != SUCCESS) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array initialization wrong");
					ret = 0;
					goto cleanup;
				}

				SEPARATE_ZVAL(attr);
				convert_to_string_ex(attr);
				ldap_attrs[i] = Z_STRVAL_PP(attr);
			}
			ldap_attrs[num_attribs] = NULL;
		default:
			break;
	}

	/* parallel search? */
	if (Z_TYPE_P(link) == IS_ARRAY) {
		int i, nlinks, nbases, nfilters, *rcs;
		ldap_linkdata **lds;
		zval **entry, *resource;
		
		nlinks = zend_hash_num_elements(Z_ARRVAL_P(link));
		if (nlinks == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No links in link array");
			ret = 0;
			goto cleanup;
		}

		if (Z_TYPE_P(base_dn) == IS_ARRAY) {
			nbases = zend_hash_num_elements(Z_ARRVAL_P(base_dn));
			if (nbases != nlinks) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Base must either be a string, or an array with the same number of elements as the links array");
				ret = 0;
				goto cleanup;
			}
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(base_dn));
		} else {
			nbases = 0; /* this means string, not array */
			/* If anything else than string is passed, ldap_base_dn = NULL */
			if (Z_TYPE_P(base_dn) == IS_STRING) {
				ldap_base_dn = Z_STRVAL_P(base_dn);
			} else {
				ldap_base_dn = NULL;
			}
		}

		if (Z_TYPE_PP(filter) == IS_ARRAY) {
			nfilters = zend_hash_num_elements(Z_ARRVAL_PP(filter));
			if (nfilters != nlinks) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filter must either be a string, or an array with the same number of elements as the links array");
				ret = 0;
				goto cleanup;
			}
			zend_hash_internal_pointer_reset(Z_ARRVAL_PP(filter));
		} else {
			nfilters = 0; /* this means string, not array */
			convert_to_string_ex(filter);
			ldap_filter = Z_STRVAL_PP(filter);
		}

		lds = safe_emalloc(nlinks, sizeof(ldap_linkdata), 0);
		rcs = safe_emalloc(nlinks, sizeof(*rcs), 0);
		
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(link));
		for (i=0; i<nlinks; i++) {
			zend_hash_get_current_data(Z_ARRVAL_P(link), (void **)&entry);

			ld = (ldap_linkdata *) zend_fetch_resource(entry TSRMLS_CC, -1, "ldap link", NULL, 1, le_link);
			if (ld == NULL) {
				ret = 0;
				goto cleanup_parallel;
			}
			if (nbases != 0) { /* base_dn an array? */
				zend_hash_get_current_data(Z_ARRVAL_P(base_dn), (void **)&entry);
				zend_hash_move_forward(Z_ARRVAL_P(base_dn));

				/* If anything else than string is passed, ldap_base_dn = NULL */
				if (Z_TYPE_PP(entry) == IS_STRING) {
					ldap_base_dn = Z_STRVAL_PP(entry);
				} else {
					ldap_base_dn = NULL;
				}
			}
			if (nfilters != 0) { /* filter an array? */
				zend_hash_get_current_data(Z_ARRVAL_PP(filter), (void **)&entry);
				zend_hash_move_forward(Z_ARRVAL_PP(filter));
				convert_to_string_ex(entry);
				ldap_filter = Z_STRVAL_PP(entry);
			}

#if LDAP_API_VERSION > 2000
			php_set_opts(ld->link, ldap_timelimit, ldap_deref, &old_ldap_timelimit, &old_ldap_deref);
			
			/* Run the actual search */	
			if(LDAP_OPT_SUCCESS != ldap_search_ext(
					ld->link,
					ldap_base_dn,
					scope,
					ldap_filter,
					ldap_attrs,
					ldap_attrsonly,
					ctrls[0],
					ctrls[1],
					NULL, /* timeval not defined, see ldap.h */
					ldap_sizelimit,
					&(rcs[i]))) {
				rcs[i] = -1;
			}
#else
			php_set_opts(ld->link, ldap_sizelimit, ldap_timelimit, ldap_deref, &old_ldap_sizelimit, &old_ldap_timelimit, &old_ldap_deref);
			rcs[i] = ldap_search(ld->link, ldap_base_dn, scope, ldap_filter, ldap_attrs, ldap_attrsonly);
#endif
			lds[i] = ld;
			
			zend_hash_move_forward(Z_ARRVAL_P(link));
		}
		
		array_init(return_value);

		/* Collect results from the searches */
		for (i=0; i<nlinks; i++) {
			MAKE_STD_ZVAL(resource);
			if (rcs[i] != -1) {
				rcs[i] = ldap_result(lds[i]->link, LDAP_RES_ANY, 1 /* LDAP_MSG_ALL */, NULL, &ldap_res);
			}
			if (rcs[i] != -1) {
				ZEND_REGISTER_RESOURCE(resource, ldap_res, le_result);
				add_next_index_zval(return_value, resource);
			} else {
				add_next_index_bool(return_value, 0);
			}
		}

cleanup_parallel:
		efree(lds);
		efree(rcs);
	} else {
		convert_to_string_ex(filter);
		ldap_filter = Z_STRVAL_PP(filter);

		/* If anything else than string is passed, ldap_base_dn = NULL */
		if (Z_TYPE_P(base_dn) == IS_STRING) {
			ldap_base_dn = Z_STRVAL_P(base_dn);
		}

		ld = (ldap_linkdata *) zend_fetch_resource(&link TSRMLS_CC, -1, "ldap link", NULL, 1, le_link);
		if (ld == NULL) {
			ret = 0;
			goto cleanup;
		}

#if LDAP_API_VERSION > 2000
		php_set_opts(ld->link, ldap_timelimit, ldap_deref, &old_ldap_timelimit, &old_ldap_deref);

		/* Run the actual search */	
		errno = ldap_search_ext_s(
				ld->link,
				ldap_base_dn,
				scope,
				ldap_filter,
				ldap_attrs,
				ldap_attrsonly,
				ctrls[0],
				ctrls[1],
				NULL, /* timeval not defined, see ldap.h */
				ldap_sizelimit,
				&ldap_res);
#else
		php_set_opts(ld->link, ldap_sizelimit, ldap_timelimit, ldap_deref, &old_ldap_sizelimit, &old_ldap_timelimit, &old_ldap_deref);
		errno = ldap_search_s(ld->link, ldap_base_dn, scope, ldap_filter, ldap_attrs, ldap_attrsonly, &ldap_res);
#endif

		if (errno != LDAP_SUCCESS
			&& errno != LDAP_SIZELIMIT_EXCEEDED
#ifdef LDAP_ADMINLIMIT_EXCEEDED
			&& errno != LDAP_ADMINLIMIT_EXCEEDED
#endif
#ifdef LDAP_REFERRAL
			&& errno != LDAP_REFERRAL
#endif
		) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Search: %s", ldap_err2string(errno));
			ret = 0;
		} else {
			if (errno == LDAP_SIZELIMIT_EXCEEDED) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Partial search results returned: Sizelimit exceeded");
			}
#ifdef LDAP_ADMINLIMIT_EXCEEDED
			else if (errno == LDAP_ADMINLIMIT_EXCEEDED) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Partial search results returned: Adminlimit exceeded");
			}
#endif
			
			ZEND_REGISTER_RESOURCE(return_value, ldap_res, le_result);
		}
	}

cleanup:
#if LDAP_API_VERSION > 2000
	/* Free controls. Don't use ldap_controls_free as we allocate with
	 * safe_emalloc the list of LDAPControl.
	 */
	for(ctrls_count = 0; ctrls_count < 2; ctrls_count++) {
		if(ctrls[ctrls_count] != NULL) {
			for(tmp_ctrls = ctrls[ctrls_count];
					*tmp_ctrls!=NULL;
					tmp_ctrls++) {
				ldap_control_free(*tmp_ctrls);
			}
		
			efree(ctrls[ctrls_count]);
			ctrls[ctrls_count] = NULL;
		}
	}

	/* Restoring previous options */
	if (ld) {
		php_set_opts(ld->link, old_ldap_timelimit, old_ldap_deref, &ldap_timelimit, &ldap_deref);
	}
#else
	if (ld) {
		php_set_opts(ld->link, old_ldap_sizelimit, old_ldap_timelimit, old_ldap_deref, &ldap_sizelimit, &ldap_timelimit, &ldap_deref);
	}
#endif

	if (ldap_attrs != NULL) {
		efree(ldap_attrs);
	}
	if (!ret) {
		RETVAL_BOOL(ret);
	}
}
/* }}} */

/* {{{ proto resource ldap_read(resource|array link, string base_dn, string filter [, array attrs [, int attrsonly [, int sizelimit [, int timelimit [, int deref]]]]])
   Read an entry */
PHP_FUNCTION(ldap_read)
{
	php_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_BASE);
}
/* }}} */

/* {{{ proto resource ldap_list(resource|array link, string base_dn, string filter [, array attrs [, int attrsonly [, int sizelimit [, int timelimit [, int deref]]]]])
   Single-level search */
PHP_FUNCTION(ldap_list)
{
	php_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_ONELEVEL);
}
/* }}} */

/* {{{ proto resource ldap_search(resource|array link, string base_dn, string filter [, array attrs [, int attrsonly [, int sizelimit [, int timelimit [, int deref]]]]])
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	zend_list_delete(Z_LVAL_P(result));  /* Delete list entry */
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	if ((entry = ldap_first_entry(ld->link, ldap_result)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry, le_result_entry);
		resultentry->id = Z_LVAL_P(result);
		zend_list_addref(resultentry->id);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	if ((entry_next = ldap_next_entry(ld->link, resultentry->data)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry_next = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry_next, le_result_entry);
		resultentry_next->id = resultentry->id;
		zend_list_addref(resultentry->id);
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
	zval *tmp1, *tmp2;
	ldap_linkdata *ld;
	LDAP *ldap;
	int num_entries, num_attrib, num_values, i;
	BerElement *ber;
	char *attribute;
	size_t attr_len;
	struct berval **ldap_value;
	char *dn;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	ldap = ld->link;
	num_entries = ldap_count_entries(ldap, ldap_result);

	array_init(return_value);
	add_assoc_long(return_value, "count", num_entries);

	if (num_entries == 0) {
		return;
	}
	
	ldap_result_entry = ldap_first_entry(ldap, ldap_result);
	if (ldap_result_entry == NULL) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}

	num_entries = 0;
	while (ldap_result_entry != NULL) {
		MAKE_STD_ZVAL(tmp1);
		array_init(tmp1);

		num_attrib = 0;
		attribute = ldap_first_attribute(ldap, ldap_result_entry, &ber);

		while (attribute != NULL) {
			ldap_value = ldap_get_values_len(ldap, ldap_result_entry, attribute);
			num_values = ldap_count_values_len(ldap_value);

			MAKE_STD_ZVAL(tmp2);
			array_init(tmp2);
			add_assoc_long(tmp2, "count", num_values);
			for (i = 0; i < num_values; i++) {
				add_index_stringl(tmp2, i, ldap_value[i]->bv_val, ldap_value[i]->bv_len, 1);
			}	
			ldap_value_free_len(ldap_value);

			attr_len = strlen(attribute);
			zend_hash_update(Z_ARRVAL_P(tmp1), php_strtolower(attribute, attr_len), attr_len+1, (void *) &tmp2, sizeof(zval *), NULL);
			add_index_string(tmp1, num_attrib, attribute, 1);

			num_attrib++;
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
			ldap_memfree(attribute);
#endif
			attribute = ldap_next_attribute(ldap, ldap_result_entry, ber);
		}
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
		if (ber != NULL) {
			ber_free(ber, 0);
		}
#endif

		add_assoc_long(tmp1, "count", num_attrib);
		dn = ldap_get_dn(ldap, ldap_result_entry);
		add_assoc_string(tmp1, "dn", dn, 1);
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
		ldap_memfree(dn);
#else
		free(dn);
#endif

		zend_hash_index_update(Z_ARRVAL_P(return_value), num_entries, (void *) &tmp1, sizeof(zval *), NULL);
		
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
	long dummy_ber;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr|l", &link, &result_entry, &dummy_ber) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	if ((attribute = ldap_first_attribute(ld->link, resultentry->data, &resultentry->ber)) == NULL) {
		RETURN_FALSE;
	} else {
		RETVAL_STRING(attribute, 1);
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
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
	long dummy_ber;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr|l", &link, &result_entry, &dummy_ber) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	if (resultentry->ber == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "called before calling ldap_first_attribute() or no attributes found in result entry");
		RETURN_FALSE;
	}

	if ((attribute = ldap_next_attribute(ld->link, resultentry->data, resultentry->ber)) == NULL) {
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
		if (resultentry->ber != NULL) {
			ber_free(resultentry->ber, 0);
			resultentry->ber = NULL;
		}
#endif
		RETURN_FALSE;
	} else {
		RETVAL_STRING(attribute, 1);
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
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
	zval *tmp;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attribute;
	struct berval **ldap_value;
	int i, num_values, num_attrib;
	BerElement *ber;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	array_init(return_value);
	num_attrib = 0;
	
	attribute = ldap_first_attribute(ld->link, resultentry->data, &ber);
	while (attribute != NULL) {
		ldap_value = ldap_get_values_len(ld->link, resultentry->data, attribute);
		num_values = ldap_count_values_len(ldap_value);

		MAKE_STD_ZVAL(tmp);
		array_init(tmp);
		add_assoc_long(tmp, "count", num_values);
		for (i = 0; i < num_values; i++) {
			add_index_stringl(tmp, i, ldap_value[i]->bv_val, ldap_value[i]->bv_len, 1);
		}
		ldap_value_free_len(ldap_value);

		zend_hash_update(Z_ARRVAL_P(return_value), attribute, strlen(attribute)+1, (void *) &tmp, sizeof(zval *), NULL);
		add_index_string(return_value, num_attrib, attribute, 1);

		num_attrib++;
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
		ldap_memfree(attribute);
#endif
		attribute = ldap_next_attribute(ld->link, resultentry->data, ber);
	}
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
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
	int i, num_values, attr_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrs", &link, &result_entry, &attr, &attr_len) != SUCCESS) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	if ((ldap_value_len = ldap_get_values_len(ld->link, resultentry->data, attr)) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot get the value(s) of attribute %s", ldap_err2string(_get_lderrno(ld->link)));
		RETURN_FALSE;
	}
	
	num_values = ldap_count_values_len(ldap_value_len);
	array_init(return_value);
	
	for (i=0; i<num_values; i++) {
		add_next_index_stringl(return_value, ldap_value_len[i]->bv_val, ldap_value_len[i]->bv_len, 1);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result_entry) != SUCCESS) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	text = ldap_get_dn(ld->link, resultentry->data);
	if (text != NULL) {
		RETVAL_STRING(text, 1);
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
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
	long with_attrib;
	char *dn, **ldap_value;
	int i, count, dn_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &dn, &dn_len, &with_attrib) != SUCCESS) {
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
		add_index_string(return_value, i, ldap_value[i], 1);
	}

	ldap_value_free(ldap_value);
}
/* }}} */

/* {{{ proto string ldap_dn2ufn(string dn)
   Convert DN to User Friendly Naming format */
PHP_FUNCTION(ldap_dn2ufn)
{
	char *dn, *ufn;
	int dn_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dn, &dn_len) != SUCCESS) {
		return;
	}
	
	ufn = ldap_dn2ufn(dn);
	
	if (ufn != NULL) {
		RETVAL_STRING(ufn, 1);
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 || WINDOWS
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
static void php_ldap_do_modify(INTERNAL_FUNCTION_PARAMETERS, int oper)
{
	zval *link, *entry, **value, **ivalue;
	ldap_linkdata *ld;
	char *dn;
	LDAPMod **ldap_mods;
#if LDAP_API_VERSION > 2000
	LDAPControl **clientctrl = NULL, **serverctrl = NULL;
#endif
	int i, j, num_attribs, num_values, dn_len;
	int *num_berval;
	char *attribute;
	ulong index;
	int is_full_add=0; /* flag for full add operation so ldap_mod_add can be put back into oper, gerrit THomson */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsa", &link, &dn, &dn_len, &entry) != SUCCESS) {
		return;
	}	

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

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

		if (zend_hash_get_current_key(Z_ARRVAL_P(entry), &attribute, &index, 0) == HASH_KEY_IS_STRING) {
			ldap_mods[i]->mod_type = estrdup(attribute);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown attribute in the data");
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

		zend_hash_get_current_data(Z_ARRVAL_P(entry), (void **)&value);

		if (Z_TYPE_PP(value) != IS_ARRAY) {
			num_values = 1;
		} else {
			num_values = zend_hash_num_elements(Z_ARRVAL_PP(value));
		}
		
		num_berval[i] = num_values;
		ldap_mods[i]->mod_bvalues = safe_emalloc((num_values + 1), sizeof(struct berval *), 0);

/* allow for arrays with one element, no allowance for arrays with none but probably not required, gerrit thomson. */
		if ((num_values == 1) && (Z_TYPE_PP(value) != IS_ARRAY)) {
			convert_to_string_ex(value);
			ldap_mods[i]->mod_bvalues[0] = (struct berval *) emalloc (sizeof(struct berval));
			ldap_mods[i]->mod_bvalues[0]->bv_len = Z_STRLEN_PP(value);
			ldap_mods[i]->mod_bvalues[0]->bv_val = Z_STRVAL_PP(value);
		} else {	
			for (j = 0; j < num_values; j++) {
				if (zend_hash_index_find(Z_ARRVAL_PP(value), j, (void **) &ivalue) != SUCCESS) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Value array must have consecutive indices 0, 1, ...");
					num_berval[i] = j;
					num_attribs = i + 1;
					RETVAL_FALSE;
					goto errexit;
				}
				convert_to_string_ex(ivalue);
				ldap_mods[i]->mod_bvalues[j] = (struct berval *) emalloc (sizeof(struct berval));
				ldap_mods[i]->mod_bvalues[j]->bv_len = Z_STRLEN_PP(ivalue);
				ldap_mods[i]->mod_bvalues[j]->bv_val = Z_STRVAL_PP(ivalue);
			}
		}
		ldap_mods[i]->mod_bvalues[num_values] = NULL;
		zend_hash_move_forward(Z_ARRVAL_P(entry));
	}
	ldap_mods[num_attribs] = NULL;

#if LDAP_API_VERSION > 2000
	if(LDAP_OPT_SUCCESS != ldap_get_option(ld->link, LDAP_OPT_CLIENT_CONTROLS, &clientctrl)) {
		clientctrl = NULL;
	}
	if(LDAP_OPT_SUCCESS != ldap_get_option(ld->link, LDAP_OPT_SERVER_CONTROLS, &serverctrl)) {
		serverctrl = NULL;
	}
#endif

/* check flag to see if do_mod was called to perform full add , gerrit thomson */
	if (is_full_add == 1) {
#if LDAP_API_VERSION > 2000
		if ((i = ldap_add_ext_s(ld->link, dn, ldap_mods, serverctrl, clientctrl)) != LDAP_SUCCESS) {
#else
		if ((i = ldap_add_s(ld->link, dn, ldap_mods)) != LDAP_SUCCESS) {
#endif
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Add: %s", ldap_err2string(i));
			RETVAL_FALSE;
		} else RETVAL_TRUE;
	} else {
#if LDAP_API_VERSION > 2000
		if ((i = ldap_modify_ext_s(ld->link, dn, ldap_mods, serverctrl, clientctrl)) != LDAP_SUCCESS) {
#else
		if ((i = ldap_modify_s(ld->link, dn, ldap_mods)) != LDAP_SUCCESS) {
#endif
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Modify: %s", ldap_err2string(i));
			RETVAL_FALSE;
		} else RETVAL_TRUE;	
	}

errexit:

#if LDAP_API_VERSION > 2000
	if(serverctrl != NULL) {
		ldap_controls_free(serverctrl);
		serverctrl = NULL;
	}
	if(clientctrl != NULL) {
		ldap_controls_free(clientctrl);
		clientctrl = NULL;
	}
#endif

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

	return;
}
/* }}} */

/* {{{ proto bool ldap_add(resource link, string dn, array entry)
   Add entries to LDAP directory */
PHP_FUNCTION(ldap_add)
{
	/* use a newly define parameter into the do_modify so ldap_mod_add can be used the way it is supposed to be used , Gerrit THomson */
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_LD_FULL_ADD);
}
/* }}} */

/* three functions for attribute base modifications, gerrit Thomson */

/* {{{ proto bool ldap_mod_replace(resource link, string dn, array entry)
   Replace attribute values with new ones */
PHP_FUNCTION(ldap_mod_replace)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_MOD_REPLACE);
}
/* }}} */

/* {{{ proto bool ldap_mod_add(resource link, string dn, array entry)
   Add attribute values to current */
PHP_FUNCTION(ldap_mod_add)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_MOD_ADD);
}
/* }}} */

/* {{{ proto bool ldap_mod_del(resource link, string dn, array entry)
   Delete attribute values */
PHP_FUNCTION(ldap_mod_del)
{
	php_ldap_do_modify(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_MOD_DELETE);
}
/* }}} */

/* {{{ proto bool ldap_delete(resource link, string dn)
   Delete an entry from a directory */
PHP_FUNCTION(ldap_delete)
{
	zval *link;
	ldap_linkdata *ld;
	char *dn;
	int rc, dn_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &link, &dn, &dn_len) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if ((rc = ldap_delete_s(ld->link, dn)) != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Delete: %s", ldap_err2string(rc));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ldap_errno(resource link)
   Get the current ldap error number */
PHP_FUNCTION(ldap_errno)
{
	zval *link;
	ldap_linkdata *ld;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	RETURN_LONG(_get_lderrno(ld->link));
}
/* }}} */

/* {{{ proto string ldap_err2str(int errno)
   Convert error number to error string */
PHP_FUNCTION(ldap_err2str)
{
	long perrno;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &perrno) != SUCCESS) {
		return;
	}

	RETURN_STRING(ldap_err2string(perrno), 1);
}
/* }}} */

/* {{{ proto string ldap_error(resource link)
   Get the current ldap error string */
PHP_FUNCTION(ldap_error) 
{
	zval *link;
	ldap_linkdata *ld;
	int ld_errno;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	ld_errno = _get_lderrno(ld->link);

	RETURN_STRING(ldap_err2string(ld_errno), 1);
}
/* }}} */

/* {{{ proto bool ldap_compare(resource link, string dn, string attr, string value)
   Determine if an entry has a specific value for one of its attributes */
PHP_FUNCTION(ldap_compare) 
{
	zval *link;
	char *dn, *attr, *value;
	int dn_len, attr_len, value_len;
	ldap_linkdata *ld;
	int errno;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsss", &link, &dn, &dn_len, &attr, &attr_len, &value, &value_len) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	errno = ldap_compare_s(ld->link, dn, attr, value);

	switch (errno) {
		case LDAP_COMPARE_TRUE:
			RETURN_TRUE;
			break;

		case LDAP_COMPARE_FALSE:
			RETURN_FALSE;
			break;
	}
	
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Compare: %s", ldap_err2string(errno));
	RETURN_LONG(-1);
}
/* }}} */

/* {{{ proto bool ldap_sort(resource link, resource result, string sortfilter)
   Sort LDAP result entries */
PHP_FUNCTION(ldap_sort)
{
	zval *link, *result;
	ldap_linkdata *ld;
	char *sortfilter;
	int sflen;
	zend_rsrc_list_entry *le;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrs", &link, &result, &sortfilter, &sflen) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (zend_hash_index_find(&EG(regular_list), Z_LVAL_P(result), (void **) &le) != SUCCESS || le->type != le_result) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Supplied resource is not a valid ldap result resource");
		RETURN_FALSE;
	}

	if (ldap_sort_entries(ld->link, (LDAPMessage **) &le->ptr, sflen ? sortfilter : NULL, strcmp) != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ldap_err2string(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10
/* {{{ proto bool ldap_get_option(resource link, int option, mixed retval)
   Get the current value of various session-wide parameters */
PHP_FUNCTION(ldap_get_option) 
{
	zval *link, *retval;
	ldap_linkdata *ld;
	long option;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlz", &link, &option, &retval) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

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
		{
			int val;

			if (ldap_get_option(ld->link, option, &val)) {
				RETURN_FALSE;
			}
			zval_dtor(retval);
			ZVAL_LONG(retval, val);
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
			zval_dtor(retval);
			ZVAL_LONG(retval, timeout->tv_sec);
			ldap_memfree(timeout);
		} break;
#elif defined(LDAP_X_OPT_CONNECT_TIMEOUT)
	case LDAP_X_OPT_CONNECT_TIMEOUT:
		{
			int timeout;

			if (ldap_get_option(ld->link, LDAP_X_OPT_CONNECT_TIMEOUT, &timeout)) {
				RETURN_FALSE;
			}			
			zval_dtor(retval);
			ZVAL_LONG(retval, (timeout / 1000));
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
			zval_dtor(retval);
			ZVAL_STRING(retval, val, 1);
			ldap_memfree(val);
		} break;
/* options not implemented
	case LDAP_OPT_SERVER_CONTROLS:
	case LDAP_OPT_CLIENT_CONTROLS:
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
	zval *link, **newval;
	ldap_linkdata *ld;
	LDAP *ldap;
	long option;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zlZ", &link, &option, &newval) != SUCCESS) {
		return;
	}

	if (Z_TYPE_P(link) == IS_NULL) {
		ldap = NULL;
	} else {
		ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
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
		{
			int val;

			convert_to_long_ex(newval);
			val = Z_LVAL_PP(newval);
			if (ldap_set_option(ldap, option, &val)) {
				RETURN_FALSE;
			}
		} break;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
	case LDAP_OPT_NETWORK_TIMEOUT:
		{
			struct timeval timeout;

			convert_to_long_ex(newval);
			timeout.tv_sec = Z_LVAL_PP(newval);
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
			timeout = 1000 * Z_LVAL_PP(newval); /* Convert to milliseconds */
			if (ldap_set_option(ldap, LDAP_X_OPT_CONNECT_TIMEOUT, &timeout)) {
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
#ifdef LDAP_OPT_MATCHED_DN
	case LDAP_OPT_MATCHED_DN:
#endif
		{
			char *val;
			convert_to_string_ex(newval);
			val = Z_STRVAL_PP(newval);
			if (ldap_set_option(ldap, option, val)) {
				RETURN_FALSE;
			}
		} break;
		/* options with boolean value */
	case LDAP_OPT_REFERRALS:
#ifdef LDAP_OPT_RESTART
	case LDAP_OPT_RESTART:
#endif
		{
			void *val;
			convert_to_boolean_ex(newval);
			val = Z_LVAL_PP(newval)
				? LDAP_OPT_ON : LDAP_OPT_OFF;
			if (ldap_set_option(ldap, option, val)) {
				RETURN_FALSE;
			}
		} break;
		/* options with control list value */
	case LDAP_OPT_SERVER_CONTROLS:
	case LDAP_OPT_CLIENT_CONTROLS:
		{
			LDAPControl **ctrls = NULL, **tmp_ctrls = NULL;
			int created_controls = 0, expected_controls = 0,
				set_error = LDAP_OPT_ERROR;

			if ((Z_TYPE_PP(newval) != IS_ARRAY) || (zend_hash_num_elements(Z_ARRVAL_PP(newval)) <= 0)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected non-empty array value for this option");
				RETURN_FALSE;
			}

			if(_php_ldap_create_controls(Z_ARRVAL_PP(newval), &ctrls,
					&created_controls, &expected_controls) > 0) {
				if(created_controls != expected_controls) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING,
							"Not all controls have been converted to "
							"LDAPControl. Expected %d, created %d.",
							expected_controls, created_controls);
				}
				
				if(created_controls > 0 && ctrls != NULL) {
					set_error = ldap_set_option(ldap, option, ctrls);
				}
				
				/* Free ctrls. If return value is >0, ctrls must not be NULL.
				 * Better safe than sorry
				 */
				if(ctrls != NULL) {
					for(tmp_ctrls = ctrls; *tmp_ctrls != NULL; tmp_ctrls++) ldap_control_free(*tmp_ctrls);
					efree(ctrls);
				}

				if(set_error != LDAP_OPT_SUCCESS) RETURN_FALSE;
			} else {
				/* No cleaning needed if return value is <=0 */
				RETURN_FALSE;
			}

		} break;
	default:
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_LDAP_PARSE_RESULT
/* {{{ proto bool ldap_parse_result(resource link, resource result, int errcode, string matcheddn, string errmsg, array referrals, array serverctrls)
   Extract information from result */
PHP_FUNCTION(ldap_parse_result) 
{
	zval *link, *result, *errcode, *matcheddn, *errmsg, *referrals, *serverctrls, *ctrl, *php_ber;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;
	LDAPControl **lserverctrls = NULL, **ctrlp = NULL;
	char **lreferrals, **refp;
	char *lmatcheddn, *lerrmsg;
	int rc, lerrcode, myargcount = ZEND_NUM_ARGS(), ber_decode_error_count = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrz|zzzz", &link, &result, &errcode, &matcheddn, &errmsg, &referrals, &serverctrls) != SUCCESS) {
		return; 
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	rc = ldap_parse_result(ld->link, ldap_result, &lerrcode,
				myargcount > 3 ? &lmatcheddn : NULL,
				myargcount > 4 ? &lerrmsg : NULL,
				myargcount > 5 ? &lreferrals : NULL,
				myargcount > 6 ? &lserverctrls : NULL,
				0);
	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to parse result: %s", ldap_err2string(rc));
		RETURN_FALSE;
	}

	zval_dtor(errcode);
	ZVAL_LONG(errcode, lerrcode);

	/* Reverse -> fall through */
	switch (myargcount) {
		case 7:
			zval_dtor(serverctrls);
			array_init(serverctrls);
			if(lserverctrls != NULL) {
				ctrlp = lserverctrls;
				while (*ctrlp) {
					ALLOC_INIT_ZVAL(ctrl);
					array_init(ctrl);

					add_assoc_bool(
							ctrl,
							"critical",
							(*ctrlp)->ldctl_iscritical ? 1 : 0);

					php_ber = NULL;
					ber_decode_error_count = _php_ber_to_array(
							&((*ctrlp)->ldctl_value), 
							&php_ber);
					if(php_ber != NULL) {
						add_assoc_zval(ctrl, "value", php_ber);
					} else {
						add_assoc_null(ctrl, "value");
					}
					add_assoc_long(
							ctrl,
							"ber_error_count",
							ber_decode_error_count);
					add_assoc_string(ctrl, "oid", (*ctrlp)->ldctl_oid, 1);	

					add_next_index_zval(serverctrls, ctrl);
					ctrlp++;
				}
				ldap_controls_free(lserverctrls);
			}
		case 6:
			zval_dtor(referrals);
			array_init(referrals);
			if (lreferrals != NULL) {
				refp = lreferrals;
				while (*refp) {
					add_next_index_string(referrals, *refp, 1);
					refp++;
				}
				ldap_value_free(lreferrals);
			}
		case 5:
			zval_dtor(errmsg);
			if (lerrmsg == NULL) {
				ZVAL_EMPTY_STRING(errmsg);
			} else {
				ZVAL_STRING(errmsg, lerrmsg, 1);
				ldap_memfree(lerrmsg);
			}
		case 4: 
			zval_dtor(matcheddn);
			if (lmatcheddn == NULL) {
				ZVAL_EMPTY_STRING(matcheddn);
			} else {
				ZVAL_STRING(matcheddn, lmatcheddn, 1);
				ldap_memfree(lmatcheddn);
			}
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto resource ldap_first_reference(resource link, resource result)
   Return first reference */
PHP_FUNCTION(ldap_first_reference)
{
	zval *link, *result;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	LDAPMessage *ldap_result, *entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	if ((entry = ldap_first_reference(ld->link, ldap_result)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry, le_result_entry);
		resultentry->id = Z_LVAL_P(result);
		zend_list_addref(resultentry->id);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	if ((entry_next = ldap_next_reference(ld->link, resultentry->data)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry_next = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry_next, le_result_entry);
		resultentry_next->id = resultentry->id;
		zend_list_addref(resultentry->id);
		resultentry_next->data = entry_next;
		resultentry_next->ber = NULL;
	}
}
/* }}} */

#ifdef HAVE_LDAP_PARSE_REFERENCE
/* {{{ proto bool ldap_parse_reference(resource link, resource reference_entry, array referrals)
   Extract information from reference entry */
PHP_FUNCTION(ldap_parse_reference)
{
	zval *link, *result_entry, *referrals;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char **lreferrals, **refp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrz", &link, &result_entry, &referrals) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	if (ldap_parse_reference(ld->link, resultentry->data, &lreferrals, NULL /* &serverctrls */, 0) != LDAP_SUCCESS) {
		RETURN_FALSE;
	}

	zval_dtor(referrals);
	array_init(referrals);
	if (lreferrals != NULL) {
		refp = lreferrals;
		while (*refp) {
			add_next_index_string(referrals, *refp, 1);
			refp++;
		}
		ldap_value_free(lreferrals);
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto bool ldap_rename(resource link, string dn, string newrdn, string newparent, bool deleteoldrdn);
   Modify the name of an entry */
PHP_FUNCTION(ldap_rename)
{
	zval *link;
	ldap_linkdata *ld;
	int rc;
	char *dn, *newrdn, *newparent;
	int dn_len, newrdn_len, newparent_len;
	zend_bool deleteoldrdn;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsssb", &link, &dn, &dn_len, &newrdn, &newrdn_len, &newparent, &newparent_len, &deleteoldrdn) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (newparent_len == 0) {
		newparent = NULL;
	}

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10
	rc = ldap_rename_s(ld->link, dn, newrdn, newparent, deleteoldrdn, NULL, NULL);
#else
	if (newparent_len != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "You are using old LDAP API, newparent must be the empty string, can only modify RDN");
		RETURN_FALSE;
	}
/* could support old APIs but need check for ldap_modrdn2()/ldap_modrdn() */
	rc = ldap_modrdn2_s(ld->link, dn, newrdn, deleteoldrdn);
#endif

	if (rc == LDAP_SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (((rc = ldap_set_option(ld->link, LDAP_OPT_PROTOCOL_VERSION, &protocol)) != LDAP_SUCCESS) ||
		((rc = ldap_start_tls_s(ld->link, NULL, NULL)) != LDAP_SUCCESS)
	) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Unable to start TLS: %s", ldap_err2string(rc));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */
#endif
#endif /* (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10 */

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
/* {{{ _ldap_rebind_proc()
*/
int _ldap_rebind_proc(LDAP *ldap, const char *url, ber_tag_t req, ber_int_t msgid, void *params)
{
	ldap_linkdata *ld;
	int retval;
	zval *cb_url;
	zval **cb_args[2];
	zval *cb_retval;
	zval *cb_link = (zval *) params;
	TSRMLS_FETCH();

	ld = (ldap_linkdata *) zend_fetch_resource(&cb_link TSRMLS_CC, -1, "ldap link", NULL, 1, le_link);

	/* link exists and callback set? */
	if (ld == NULL || ld->rebindproc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Link not found or no callback set");
		return LDAP_OTHER;
	}

	/* callback */
	MAKE_STD_ZVAL(cb_url);
	ZVAL_STRING(cb_url, estrdup(url), 0);
	cb_args[0] = &cb_link;
	cb_args[1] = &cb_url;
	if (call_user_function_ex(EG(function_table), NULL, ld->rebindproc, &cb_retval, 2, cb_args, 0, NULL TSRMLS_CC) == SUCCESS && cb_retval) {
		convert_to_long_ex(&cb_retval);
		retval = Z_LVAL_P(cb_retval);
		zval_ptr_dtor(&cb_retval);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "rebind_proc PHP callback failed");
		retval = LDAP_OTHER;
	}
	zval_dtor(cb_url);
	FREE_ZVAL(cb_url);
	return retval;
}
/* }}} */

/* {{{ proto bool ldap_set_rebind_proc(resource link, string callback)
   Set a callback function to do re-binds on referral chasing. */
PHP_FUNCTION(ldap_set_rebind_proc)
{
	zval *link, *callback;
	ldap_linkdata *ld;
	char *callback_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &link, &callback) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (Z_TYPE_P(callback) == IS_STRING && Z_STRLEN_P(callback) == 0) {
		/* unregister rebind procedure */
		if (ld->rebindproc != NULL) {
			zval_dtor(ld->rebindproc);
			ld->rebindproc = NULL;
			ldap_set_rebind_proc(ld->link, NULL, NULL);
		}
		RETURN_TRUE;
	}

	/* callable? */
	if (!zend_is_callable(callback, 0, &callback_name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Two arguments expected for '%s' to be a valid callback", callback_name);
		efree(callback_name);
		RETURN_FALSE;
	}
	efree(callback_name);

	/* register rebind procedure */
	if (ld->rebindproc == NULL) {
		ldap_set_rebind_proc(ld->link, _ldap_rebind_proc, (void *) link);
	} else {
		zval_dtor(ld->rebindproc);
	}

	ALLOC_ZVAL(ld->rebindproc);
	*ld->rebindproc = *callback;
	zval_copy_ctor(ld->rebindproc);
	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef STR_TRANSLATION
/* {{{ php_ldap_do_translate
 */
static void php_ldap_do_translate(INTERNAL_FUNCTION_PARAMETERS, int way) 
{
	char *value;
	int result, ldap_len;
		
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) != SUCCESS) {
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
		RETVAL_STRINGL(value, value_len, 1);
		free(value);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Conversion from iso-8859-1 to t61 failed: %s", ldap_err2string(result));
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
	long pagesize;
	zend_bool iscritical;
	zval *link;
	char *cookie = NULL;
	int cookie_len = 0;
	struct berval lcookie = { 0, NULL };
	ldap_linkdata *ld;
	LDAP *ldap;
	BerElement *ber = NULL;
	LDAPControl	ctrl, *ctrlsp[2];
	int rc, myargcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|bs", &link, &pagesize, &iscritical, &cookie, &cookie_len) != SUCCESS) {
		return;
	}

	if (Z_TYPE_P(link) == IS_NULL) {
		ldap = NULL;
	} else {
		ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
		ldap = ld->link;
	}

	ber = ber_alloc_t(LBER_USE_DER);
	if (ber == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to alloc BER encoding resources for paged results control");
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to BER printf paged results control");
		RETVAL_FALSE;
		goto lcpr_error_out;
	}
	rc = ber_flatten2(ber, &ctrl.ldctl_value, 0);
	if (rc == LBER_ERROR) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to BER encode paged results control");
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set paged results control: %s (%d)", ldap_err2string(rc), rc);
			RETVAL_FALSE;
			goto lcpr_error_out;
		}
		RETVAL_TRUE;
	} else {
		/* return a PHP control object */
		array_init(return_value);

		add_assoc_string(return_value, "oid", ctrl.ldctl_oid, 1);
		if (ctrl.ldctl_value.bv_len) {
			add_assoc_stringl(return_value, "value", ctrl.ldctl_value.bv_val, ctrl.ldctl_value.bv_len, 1);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr|zz", &link, &result, &cookie, &estimated) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	rc = ldap_parse_result(ld->link,
				ldap_result,
				&lerrcode,
				NULL,		/* matcheddn */
				NULL,		/* errmsg */
				NULL,		/* referrals */
				&lserverctrls,
				0);

	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to parse result: %s (%d)", ldap_err2string(rc), rc);
		RETURN_FALSE;
	}

	if (lerrcode != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Result is: %s (%d)", ldap_err2string(lerrcode), lerrcode);
		RETURN_FALSE;
	}

	if (lserverctrls == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No server controls in result");
		RETURN_FALSE;
	}

	lctrl = ldap_find_control(LDAP_CONTROL_PAGEDRESULTS, lserverctrls);
	if (lctrl == NULL) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No paged results control response in result");
		RETURN_FALSE;
	}

	ber = ber_init(&lctrl->ldctl_value);
	if (ber == NULL) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to alloc BER decoding resources for paged results control response");
		RETURN_FALSE;
	}

	tag = ber_scanf(ber, "{io}", &lestimated, &lcookie);
	(void)ber_free(ber, 1);

	if (tag == LBER_ERROR) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to decode paged results control response");
		RETURN_FALSE;
	}

	if (lestimated < 0) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid paged results control response value");
		RETURN_FALSE;
	}

	ldap_controls_free(lserverctrls);
	if (myargcount == 4) {
		zval_dtor(estimated);
		ZVAL_LONG(estimated, lestimated);
	}

	zval_dtor(cookie);
 	if (lcookie.bv_len == 0) {
		ZVAL_EMPTY_STRING(cookie);
 	} else {
		ZVAL_STRINGL(cookie, lcookie.bv_val, lcookie.bv_len, 1);
 	}
 	ldap_memfree(lcookie.bv_val);

	RETURN_TRUE;
}
/* }}} */
#endif

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
	ZEND_ARG_INFO(0, serverctrls)
	ZEND_ARG_INFO(0, clientctrls)
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
	ZEND_ARG_INFO(0, serverctrls)
	ZEND_ARG_INFO(0, clientctrls)
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
	ZEND_ARG_INFO(0, serverctrls)
	ZEND_ARG_INFO(0, clientctrls)
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
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_delete, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_modify, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_add, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_replace, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_del, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_err2str, 0, 0, 1)
	ZEND_ARG_INFO(0, errno)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_compare, 0, 0, 4)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, attribute)
	ZEND_ARG_INFO(0, value)
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

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_rename, 0, 0, 5)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, dn)
	ZEND_ARG_INFO(0, newrdn)
	ZEND_ARG_INFO(0, newparent)
	ZEND_ARG_INFO(0, deleteoldrdn)
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

#ifdef STR_TRANSLATION
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_t61_to_8859, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_8859_to_t61, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
#endif
/* }}} */
	
/*
	This is just a small subset of the functionality provided by the LDAP library. All the 
	operations are synchronous. Referrals are not handled automatically.
*/
/* {{{ ldap_functions[]
 */
const zend_function_entry ldap_functions[] = {
	PHP_FE(ldap_connect,								arginfo_ldap_connect)
	PHP_FALIAS(ldap_close,		ldap_unbind,			arginfo_ldap_resource)
	PHP_FE(ldap_bind,									arginfo_ldap_bind)
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
	PHP_FE(ldap_delete,									arginfo_ldap_delete)
	PHP_FALIAS(ldap_modify,		ldap_mod_replace,		arginfo_ldap_modify)

/* additional functions for attribute based modifications, Gerrit Thomson */
	PHP_FE(ldap_mod_add,								arginfo_ldap_mod_add)
	PHP_FE(ldap_mod_replace,							arginfo_ldap_mod_replace)
	PHP_FE(ldap_mod_del,								arginfo_ldap_mod_del)
/* end gjt mod */

	PHP_FE(ldap_errno,									arginfo_ldap_resource)
	PHP_FE(ldap_err2str,								arginfo_ldap_err2str)
	PHP_FE(ldap_error,									arginfo_ldap_resource)
	PHP_FE(ldap_compare,								arginfo_ldap_compare)
	PHP_FE(ldap_sort,									arginfo_ldap_sort)

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP_10
	PHP_FE(ldap_rename,									arginfo_ldap_rename)
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
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
	PHP_FE(ldap_set_rebind_proc,						arginfo_ldap_set_rebind_proc)
#endif

#ifdef STR_TRANSLATION
	PHP_FE(ldap_t61_to_8859,							arginfo_ldap_t61_to_8859)
	PHP_FE(ldap_8859_to_t61,							arginfo_ldap_8859_to_t61)
#endif

#ifdef LDAP_CONTROL_PAGEDRESULTS
	PHP_FE(ldap_control_paged_result,							arginfo_ldap_control_paged_result)
	PHP_FE(ldap_control_paged_result_response,		arginfo_ldap_control_paged_result_response)
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
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(ldap),
	PHP_GINIT(ldap),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
