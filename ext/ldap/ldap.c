/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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

typedef struct {
	LDAP *link;
#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
	zval *rebindproc;
#endif
} ldap_linkdata;

typedef struct {
	LDAPMessage *data;
	int id;
} ldap_resultentry;

ZEND_DECLARE_MODULE_GLOBALS(ldap)

static unsigned char third_argument_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
static unsigned char arg3to6of6_force_ref[] = { 6, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE };

static int le_link, le_result, le_result_entry, le_ber_entry;

/*
	This is just a small subset of the functionality provided by the LDAP library. All the 
	operations are synchronous. Referrals are not handled automatically.
*/
/* {{{ ldap_functions[]
 */
function_entry ldap_functions[] = {
	PHP_FE(ldap_connect,								NULL)
	PHP_FALIAS(ldap_close,		ldap_unbind,			NULL)
	PHP_FE(ldap_bind,									NULL)
	PHP_FE(ldap_unbind,									NULL)
	PHP_FE(ldap_read,									NULL)
	PHP_FE(ldap_list,									NULL)
	PHP_FE(ldap_search,									NULL)
	PHP_FE(ldap_free_result,							NULL)
	PHP_FE(ldap_count_entries,							NULL)
	PHP_FE(ldap_first_entry,							NULL)
	PHP_FE(ldap_next_entry,								NULL)
	PHP_FE(ldap_get_entries,							NULL)
	PHP_FE(ldap_first_attribute,	third_argument_force_ref)
	PHP_FE(ldap_next_attribute,		third_argument_force_ref)
	PHP_FE(ldap_get_attributes,							NULL)
	PHP_FE(ldap_get_values,								NULL)
	PHP_FE(ldap_get_values_len,							NULL)
	PHP_FE(ldap_get_dn,									NULL)
	PHP_FE(ldap_explode_dn,								NULL)
	PHP_FE(ldap_dn2ufn,									NULL)
	PHP_FE(ldap_add,									NULL)
	PHP_FE(ldap_delete,									NULL)
	PHP_FALIAS(ldap_modify,		ldap_mod_replace,		NULL)

/* additional functions for attribute based modifications, Gerrit Thomson */
	PHP_FE(ldap_mod_add,								NULL)
	PHP_FE(ldap_mod_replace,							NULL)
	PHP_FE(ldap_mod_del,								NULL)
/* end gjt mod */

	PHP_FE(ldap_errno,									NULL)
	PHP_FE(ldap_err2str,								NULL)
	PHP_FE(ldap_error,									NULL)
	PHP_FE(ldap_compare,								NULL)
	PHP_FE(ldap_sort,									NULL)

#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP
	PHP_FE(ldap_get_option,			third_argument_force_ref)
	PHP_FE(ldap_set_option,								NULL)
	PHP_FE(ldap_parse_result,			arg3to6of6_force_ref)
	PHP_FE(ldap_first_reference,						NULL)
	PHP_FE(ldap_next_reference,							NULL)
#ifdef HAVE_LDAP_PARSE_REFERENCE
	PHP_FE(ldap_parse_reference,	third_argument_force_ref)
#endif
	PHP_FE(ldap_rename,									NULL)
#ifdef HAVE_LDAP_START_TLS_S
	PHP_FE(ldap_start_tls,								NULL)
#endif
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
	PHP_FE(ldap_set_rebind_proc,						NULL)
#endif

#ifdef STR_TRANSLATION
	PHP_FE(ldap_t61_to_8859,							NULL)
	PHP_FE(ldap_8859_to_t61,							NULL)
#endif

	{NULL, NULL, NULL}
};
/* }}} */

zend_module_entry ldap_module_entry = {
	STANDARD_MODULE_HEADER,
	"ldap", 
	ldap_functions, 
	PHP_MINIT(ldap), 
	PHP_MSHUTDOWN(ldap), 
	NULL, 
	NULL,
	PHP_MINFO(ldap), 
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_LDAP
ZEND_GET_MODULE(ldap)
#endif


static void _close_ldap_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
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


static void _free_ldap_result(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	LDAPMessage *result = (LDAPMessage *)rsrc->ptr;
	ldap_msgfree(result);
}

static void _free_ldap_result_entry(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ldap_resultentry *entry = (ldap_resultentry *)rsrc->ptr;
	zend_list_delete(entry->id);
	efree(entry);
} 

/* {{{ PHP_INI_BEGIN
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY_EX("ldap.max_links",		"-1",	PHP_INI_SYSTEM,			OnUpdateInt,		max_links,			zend_ldap_globals,		ldap_globals,	display_link_numbers)
PHP_INI_END()
/* }}} */

/* {{{ php_ldap_init_globals
 */
static void php_ldap_init_globals(zend_ldap_globals *ldap_globals)
{
	ldap_globals->num_links = 0;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ldap)
{
	ZEND_INIT_MODULE_GLOBALS(ldap, php_ldap_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	/* Constants to be used with deref-parameter in php_ldap_do_search() */
	REGISTER_LONG_CONSTANT("LDAP_DEREF_NEVER", LDAP_DEREF_NEVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_DEREF_SEARCHING", LDAP_DEREF_SEARCHING, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_DEREF_FINDING", LDAP_DEREF_FINDING, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_DEREF_ALWAYS", LDAP_DEREF_ALWAYS, CONST_PERSISTENT | CONST_CS);

#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP
	/* LDAP options */
	REGISTER_LONG_CONSTANT("LDAP_OPT_DEREF", LDAP_OPT_DEREF, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_SIZELIMIT", LDAP_OPT_SIZELIMIT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_TIMELIMIT", LDAP_OPT_TIMELIMIT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_PROTOCOL_VERSION", LDAP_OPT_PROTOCOL_VERSION, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_ERROR_NUMBER", LDAP_OPT_ERROR_NUMBER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LDAP_OPT_REFERRALS", LDAP_OPT_REFERRALS, CONST_PERSISTENT | CONST_CS);
#ifdef LDAP_OPT_RESTART
	REGISTER_LONG_CONSTANT("LDAP_OPT_RESTART", LDAP_OPT_RESTART, CONST_PERSISTENT | CONST_CS);
#endif
	REGISTER_LONG_CONSTANT("LDAP_OPT_HOST_NAME", LDAP_OPT_HOST_NAME, CONST_PERSISTENT | CONST_CS);
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

#ifdef ORALDAP
	REGISTER_LONG_CONSTANT("GSLC_SSL_NO_AUTH", GSLC_SSL_NO_AUTH, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("GSLC_SSL_ONEWAY_AUTH", GSLC_SSL_ONEWAY_AUTH, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("GSLC_SSL_TWOWAY_AUTH", GSLC_SSL_TWOWAY_AUTH, CONST_PERSISTENT | CONST_CS);
#endif

	le_result = zend_register_list_destructors_ex(_free_ldap_result, NULL, "ldap result", module_number);
	le_link = zend_register_list_destructors_ex(_close_ldap_link, NULL, "ldap link", module_number);
	le_result_entry = zend_register_list_destructors_ex(_free_ldap_result_entry, NULL, "ldap result entry", module_number);
	le_ber_entry = zend_register_list_destructors_ex(NULL, NULL, "ldap ber entry", module_number);

	Z_TYPE(ldap_module_entry) = type;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ldap)
{
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
	php_info_print_table_row(2, "LDAP Support", "enabled" );
	php_info_print_table_row(2, "RCS Version", "$Id$" );

	if (LDAPG(max_links) == -1) {
		snprintf(tmp, 31, "%ld/unlimited", LDAPG(num_links));
	} else {
		snprintf(tmp, 31, "%ld/%ld", LDAPG(num_links), LDAPG(max_links));
	}
	tmp[31] = '\0';
	php_info_print_table_row(2, "Total Links", tmp);

#ifdef LDAP_API_VERSION
	snprintf(tmp, 31, "%d", LDAP_API_VERSION);
	tmp[31] = '\0';
	php_info_print_table_row(2, "API Version", tmp);
#endif

#ifdef LDAP_VENDOR_NAME
	php_info_print_table_row(2, "Vendor Name", LDAP_VENDOR_NAME);
#endif

#ifdef LDAP_VENDOR_VERSION
	snprintf(tmp, 31, "%d", LDAP_VENDOR_VERSION);
	tmp[31] = '\0';
	php_info_print_table_row(2, "Vendor Version", tmp);
#endif

#if HAVE_NSLDAP
	SDKVersion = ldap_version( &ver );
	snprintf(tmp, 31, "%f", SDKVersion/100.0 );
	tmp[31] = '\0';
	php_info_print_table_row(2, "SDK Version", tmp );

	snprintf(tmp, 31, "%f", ver.protocol_version/100.0 );
	tmp[31] = '\0';
	php_info_print_table_row(2, "Highest LDAP Protocol Supported", tmp );

	snprintf(tmp, 31, "%f", ver.SSL_version/100.0 );
	tmp[31] = '\0';
	php_info_print_table_row(2, "SSL Level Supported", tmp );

	if ( ver.security_level != LDAP_SECURITY_NONE ) {
		snprintf(tmp, 31, "%d", ver.security_level );
		tmp[31] = '\0';
	} else {
		strcpy(tmp, "SSL not enabled" );
	}
	php_info_print_table_row(2, "Level of Encryption", tmp );
#endif

	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource ldap_connect([string host [, int port]])
   Connect to an LDAP server */
PHP_FUNCTION(ldap_connect)
{
	char *host = NULL;
	int hostlen;
	int port = 389; /* Default port */
#ifdef HAVE_ORALDAP
	char *wallet, *walletpasswd;
	int walletlen, walletpasswdlen;
	int authmode;
	int ssl=0;
#endif
	ldap_linkdata *ld;
	LDAP *ldap;

#ifdef HAVE_ORALDAP
	if (ZEND_NUM_ARGS() == 3 || ZEND_NUM_ARGS() == 4) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|slssl", &host, &hostlen, &port, &wallet, &walletlen, &walletpasswd, &walletpasswdlen, &authmode) == FAILURE) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 5 ) {
		ssl = 1;
	}
#else
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sl", &host, &hostlen, &port) == FAILURE) {
		RETURN_FALSE;
	}
#endif

	if (LDAPG(max_links) != -1 && LDAPG(num_links) >= LDAPG(max_links)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too many open links (%d)", LDAPG(num_links));
		RETURN_FALSE;
	}

	ld = ecalloc(1, sizeof(ldap_linkdata));
	if (ld == NULL) {
		RETURN_FALSE;
	}

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
	
	if ( ldap == NULL ) {
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
#if LDAP_API_VERSION > 2000
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

/* {{{ proto bool ldap_bind(resource link [, string dn, string password])
   Bind to LDAP directory */
PHP_FUNCTION(ldap_bind)
{
	zval *link;
	char *ldap_bind_dn = NULL, *ldap_bind_pw = NULL;
	int ldap_bind_dnlen, ldap_bind_pwlen;
	ldap_linkdata *ld;
	int rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|ss", &link, &ldap_bind_dn, &ldap_bind_dnlen, &ldap_bind_pw, &ldap_bind_pwlen) == FAILURE) {
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

/* {{{ proto bool ldap_unbind(resource link)
   Unbind from LDAP directory */
PHP_FUNCTION(ldap_unbind)
{
	zval *link;
	ldap_linkdata *ld;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	zend_list_delete(Z_LVAL_P(link));
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_set_opts
 */
static void php_set_opts(LDAP *ldap, int sizelimit, int timelimit, int deref)
{
	/* sizelimit */
	if (sizelimit > -1) {
#if ( LDAP_API_VERSION >= 2004 ) || HAVE_NSLDAP
		ldap_set_option(ldap, LDAP_OPT_SIZELIMIT, &sizelimit);
#else
		ldap->ld_sizelimit = sizelimit; 
#endif
	}

	/* timelimit */
	if (timelimit > -1) {
#if ( LDAP_API_VERSION >= 2004 ) || HAVE_NSLDAP
		ldap_set_option(ldap, LDAP_OPT_TIMELIMIT, &timelimit);
#else
		ldap->ld_timelimit = timelimit; 
#endif
	}

	/* deref */
	if (deref > -1) {
#if ( LDAP_API_VERSION >= 2004 ) || HAVE_NSLDAP
		ldap_set_option(ldap, LDAP_OPT_DEREF, &deref);
#else
		ldap->ld_deref = deref; 
#endif
	}
}
/* }}} */

/* {{{ php_ldap_do_search
 */
static void php_ldap_do_search(INTERNAL_FUNCTION_PARAMETERS, int scope)
{
	pval **link, **base_dn, **filter, **attrs, **attr, **attrsonly, **sizelimit, **timelimit, **deref;
	char *ldap_base_dn = NULL;
	char *ldap_filter = NULL;
	char **ldap_attrs = NULL; 
	ldap_linkdata *ld;
	LDAPMessage *ldap_res;
	int ldap_attrsonly = 0;  
	int ldap_sizelimit = -1; 
	int ldap_timelimit = -1; 
	int ldap_deref = -1;	 
	int num_attribs = 0;
	int i, errno;
	int myargcount = ZEND_NUM_ARGS();
  
	if (myargcount < 3 || myargcount > 8 || zend_get_parameters_ex(myargcount, &link, &base_dn, &filter, &attrs, &attrsonly, &sizelimit, &timelimit, &deref) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* Reverse -> fall through */
	switch(myargcount) {
		case 8 :
			convert_to_long_ex(deref);
			ldap_deref = Z_LVAL_PP(deref);

		case 7 :
			convert_to_long_ex(timelimit);
			ldap_timelimit = Z_LVAL_PP(timelimit);

		case 6 :
			convert_to_long_ex(sizelimit);
			ldap_sizelimit = Z_LVAL_PP(sizelimit);

		case 5 :
			convert_to_long_ex(attrsonly);
			ldap_attrsonly = Z_LVAL_PP(attrsonly);

		case 4 : 
			if (Z_TYPE_PP(attrs) != IS_ARRAY) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected Array as last element");
				RETURN_FALSE;
			}

			num_attribs = zend_hash_num_elements(Z_ARRVAL_PP(attrs));
			if ((ldap_attrs = emalloc((num_attribs+1) * sizeof(char *))) == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not allocate memory");
				RETURN_FALSE;
			}

			for(i=0; i<num_attribs; i++) {
				if(zend_hash_index_find(Z_ARRVAL_PP(attrs), i, (void **) &attr) == FAILURE) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array initialization wrong");
					efree(ldap_attrs);
					RETURN_FALSE;
				}

				SEPARATE_ZVAL(attr);
				convert_to_string_ex(attr);
				ldap_attrs[i] = Z_STRVAL_PP(attr);
			}
			ldap_attrs[num_attribs] = NULL;
		
		case 3 :
			/* parallel search? */
			if (Z_TYPE_PP(link) != IS_ARRAY) {
				convert_to_string_ex(filter);
				ldap_filter = Z_STRVAL_PP(filter);
				convert_to_string_ex(base_dn);
				ldap_base_dn = Z_STRVAL_PP(base_dn);
			}
		break;

		default:
			WRONG_PARAM_COUNT;
		break;
	}

	/* parallel search? */
	if (Z_TYPE_PP(link) == IS_ARRAY) {
		int i, nlinks, nbases, nfilters, *rcs;
		ldap_linkdata **lds;
		zval **entry, *resource;
		
		nlinks = zend_hash_num_elements(Z_ARRVAL_PP(link));
		if (nlinks == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No links in link array");
			if (ldap_attrs != NULL) {
				efree(ldap_attrs);
			}
			RETURN_FALSE;
		}

		if (Z_TYPE_PP(base_dn) == IS_ARRAY) {
			nbases = zend_hash_num_elements(Z_ARRVAL_PP(base_dn));
			if (nbases != nlinks) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Base must either be a string, or an array with the same number of elements as the links array");
				if (ldap_attrs != NULL) {
					efree(ldap_attrs);
				}
				RETURN_FALSE;
			}
			zend_hash_internal_pointer_reset(Z_ARRVAL_PP(base_dn));
		} else {
			nbases = 0; /* this means string, not array */
			convert_to_string_ex(base_dn);
			ldap_base_dn = Z_STRLEN_PP(base_dn) < 1 ? NULL : Z_STRVAL_PP(base_dn);
		}

		if (Z_TYPE_PP(filter) == IS_ARRAY) {
			nfilters = zend_hash_num_elements(Z_ARRVAL_PP(filter));
			if (nfilters != nlinks) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filter must either be a string, or an array with the same number of elements as the links array");
				if (ldap_attrs != NULL) {
					efree(ldap_attrs);
				}
				RETURN_FALSE;
			}
			zend_hash_internal_pointer_reset(Z_ARRVAL_PP(filter));
		} else {
			nfilters = 0; /* this means string, not array */
			convert_to_string_ex(filter);
			ldap_filter = Z_STRVAL_PP(filter);
		}

		lds = emalloc(nlinks * sizeof(ldap_linkdata));
		rcs = emalloc(nlinks * sizeof(*rcs));
		
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(link));
		for (i=0; i<nlinks; i++) {
			zend_hash_get_current_data(Z_ARRVAL_PP(link), (void **)&entry);

			ld = (ldap_linkdata *) zend_fetch_resource(entry TSRMLS_CC, -1, "ldap link", NULL, 1, le_link);
			if (ld == NULL) {
				efree(lds);
				efree(rcs);
				if (ldap_attrs != NULL) {
					efree(ldap_attrs);
				}
				RETURN_FALSE;
			}
			if (nbases != 0) { /* base_dn an array? */
				zend_hash_get_current_data(Z_ARRVAL_PP(base_dn), (void **)&entry);
				zend_hash_move_forward(Z_ARRVAL_PP(base_dn));
				convert_to_string_ex(entry);
				ldap_base_dn = Z_STRLEN_PP(entry) < 1 ? NULL : Z_STRVAL_PP(entry);
			}
			if (nfilters != 0) { /* filter an array? */
				zend_hash_get_current_data(Z_ARRVAL_PP(filter), (void **)&entry);
				zend_hash_move_forward(Z_ARRVAL_PP(filter));
				convert_to_string_ex(entry);
				ldap_filter = Z_STRVAL_PP(entry);
			}

			php_set_opts(ld->link, ldap_sizelimit, ldap_timelimit, ldap_deref);

			/* Run the actual search */	
			rcs[i] = ldap_search(ld->link, ldap_base_dn, scope, ldap_filter, ldap_attrs, ldap_attrsonly);
			lds[i] = ld;
			zend_hash_move_forward(Z_ARRVAL_PP(link));
		}
		
		if (ldap_attrs != NULL) {
			efree(ldap_attrs);
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
		efree(lds);
		efree(rcs);
		return;
	}

	/* fix to make null base_dn's work */
	if ( strlen(ldap_base_dn) < 1 ) {
	  ldap_base_dn = NULL;
	}

	ld = (ldap_linkdata *) zend_fetch_resource(link TSRMLS_CC, -1, "ldap link", NULL, 1, le_link);
	if (ld == NULL) {
		if (ldap_attrs != NULL) {
			efree(ldap_attrs);
		}
		RETURN_FALSE;
	}

	php_set_opts(ld->link, ldap_sizelimit, ldap_timelimit, ldap_deref);

	/* Run the actual search */	
	errno = ldap_search_s(ld->link, ldap_base_dn, scope, ldap_filter, ldap_attrs, ldap_attrsonly, &ldap_res);

	if (ldap_attrs != NULL) {
		efree(ldap_attrs);
	}

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
		RETVAL_FALSE; 
	} else {
		if (errno == LDAP_SIZELIMIT_EXCEEDED)  {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Partial search results returned: Sizelimit exceeded.");
		}
#ifdef LDAP_ADMINLIMIT_EXCEEDED
		else if (errno == LDAP_ADMINLIMIT_EXCEEDED) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Partial search results returned: Adminlimit exceeded.");
		}
#endif
		
		ZEND_REGISTER_RESOURCE(return_value, ldap_res, le_result);
	}
}
/* }}} */

/* {{{ proto resource ldap_read(resource link, string base_dn, string filter [, array attrs [, int attrsonly [, int sizelimit [, int timelimit [, int deref]]]]])
   Read an entry */
PHP_FUNCTION(ldap_read)
{
	php_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_BASE);
}
/* }}} */

/* {{{ proto resource ldap_list(resource link, string base_dn, string filter [, array attrs [, int attrsonly [, int sizelimit [, int timelimit [, int deref]]]]])
   Single-level search */
PHP_FUNCTION(ldap_list)
{
	php_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_ONELEVEL);
}
/* }}} */

/* {{{ proto resource ldap_search(resource link, string base_dn, string filter [, array attrs [, int attrsonly [, int sizelimit [, int timelimit [, int deref]]]]])
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
	pval **result;
	LDAPMessage *ldap_result;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, result, -1, "ldap result", le_result);

	zend_list_delete(Z_LVAL_PP(result));  /* Delete list entry and call registered destructor function */
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int ldap_count_entries(resource link, resource result)
   Count the number of entries in a search result */
PHP_FUNCTION(ldap_count_entries)
{
	pval **link, **result;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &link, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, result, -1, "ldap result", le_result);

	RETURN_LONG(ldap_count_entries(ld->link, ldap_result));
}
/* }}} */

/* {{{ proto resource ldap_first_entry(resource link, resource result)
   Return first result id */
PHP_FUNCTION(ldap_first_entry)
{
	pval **link, **result;
	ldap_linkdata *ld;	
	ldap_resultentry *resultentry;
	LDAPMessage *ldap_result, *entry;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &link, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, result, -1, "ldap result", le_result);

	if ((entry = ldap_first_entry(ld->link, ldap_result)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry, le_result_entry);
		resultentry->id = Z_LVAL_PP(result);
		zend_list_addref(resultentry->id);
		resultentry->data = entry;
	}
}
/* }}} */

/* {{{ proto resource ldap_next_entry(resource link, resource result_entry)
   Get next result entry */
PHP_FUNCTION(ldap_next_entry)
{
	pval **link, **result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry, *resultentry_next;
	LDAPMessage *entry_next;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &link, &result_entry) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, result_entry, -1, "ldap result entry", le_result_entry);

	if ((entry_next = ldap_next_entry(ld->link, resultentry->data)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry_next = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry_next, le_result_entry);
		resultentry_next->id = resultentry->id;
		zend_list_addref(resultentry->id);
		resultentry_next->data = entry_next;
	}
}
/* }}} */

/* {{{ proto array ldap_get_entries(resource link, resource result)
   Get all result entries */
PHP_FUNCTION(ldap_get_entries)
{
	pval **link, **result;
	LDAPMessage *ldap_result, *ldap_result_entry;
	pval *tmp1, *tmp2;
	ldap_linkdata *ld;
	LDAP *ldap;
	int num_entries, num_attrib, num_values, i;
	BerElement *ber;
	char *attribute;
	size_t attr_len;
	char **ldap_value;
	char *dn;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &link, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, result, -1, "ldap result", le_result);

	ldap = ld->link;
	num_entries = ldap_count_entries(ldap, ldap_result);

	array_init(return_value);
	add_assoc_long(return_value, "count", num_entries);

	if (num_entries == 0) return;
	num_entries = 0;
	
	ldap_result_entry = ldap_first_entry(ldap, ldap_result);
	if (ldap_result_entry == NULL) RETURN_FALSE;

	while(ldap_result_entry != NULL) {

		MAKE_STD_ZVAL(tmp1);
		array_init(tmp1);

		num_attrib = 0;
		attribute = ldap_first_attribute(ldap, ldap_result_entry, &ber);

		while (attribute != NULL) {
			ldap_value = ldap_get_values(ldap, ldap_result_entry, attribute);
			num_values = ldap_count_values(ldap_value);

			MAKE_STD_ZVAL(tmp2);
			array_init(tmp2);
			add_assoc_long(tmp2, "count", num_values);
			for(i=0; i < num_values; i++) {
				add_index_string(tmp2, i, ldap_value[i], 1);
			}	
			ldap_value_free(ldap_value);

			attr_len = strlen(attribute);
			zend_hash_update(Z_ARRVAL_P(tmp1), php_strtolower(attribute, attr_len), attr_len+1, (void *) &tmp2, sizeof(pval *), NULL);
			add_index_string(tmp1, num_attrib, attribute, 1);

			num_attrib++;
#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP || WINDOWS
			ldap_memfree(attribute);
#endif
			attribute = ldap_next_attribute(ldap, ldap_result_entry, ber);
		}
#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP || WINDOWS
		if (ber != NULL)
			ber_free(ber, 0);
#endif

		add_assoc_long(tmp1, "count", num_attrib);
		dn = ldap_get_dn(ldap, ldap_result_entry);
		add_assoc_string(tmp1, "dn", dn, 1);
#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP || WINDOWS
		ldap_memfree(dn);
#else
		free(dn);
#endif

		zend_hash_index_update(Z_ARRVAL_P(return_value), num_entries, (void *) &tmp1, sizeof(pval *), NULL);
		
		num_entries++;
		ldap_result_entry = ldap_next_entry(ldap, ldap_result_entry);
	}

	add_assoc_long(return_value, "count", num_entries);

}
/* }}} */

/* {{{ proto string ldap_first_attribute(resource link, resource result_entry, int ber)
   Return first attribute */
PHP_FUNCTION(ldap_first_attribute)
{
	pval **link, **result_entry, **berp;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	BerElement *ber;
	char *attribute;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &link, &result_entry, &berp) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, result_entry, -1, "ldap result entry", le_result_entry);

	if ((attribute = ldap_first_attribute(ld->link, resultentry->data, &ber)) == NULL) {
		RETURN_FALSE;
	} else {
		ZEND_REGISTER_RESOURCE(*berp, ber, le_ber_entry);

		RETVAL_STRING(attribute, 1);
#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP || WINDOWS
		ldap_memfree(attribute);
#endif
	}
}
/* }}} */

/* {{{ proto string ldap_next_attribute(resource link, resource result_entry, resource ber)
   Get the next attribute in result */
PHP_FUNCTION(ldap_next_attribute)
{
	pval **link, **result_entry, **berp;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	BerElement *ber;
	char *attribute;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &link, &result_entry, &berp) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, result_entry, -1, "ldap result entry", le_result_entry);
	ZEND_FETCH_RESOURCE(ber, BerElement *, berp, -1, "ldap ber entry", le_ber_entry);

	if ((attribute = ldap_next_attribute(ld->link, resultentry->data, ber)) == NULL) {
		RETURN_FALSE;
	} else {
		ZEND_REGISTER_RESOURCE(*berp, ber, le_ber_entry);

		RETVAL_STRING(attribute, 1);
#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP || WINDOWS
		ldap_memfree(attribute);
#endif
	}
}
/* }}} */

/* {{{ proto array ldap_get_attributes(resource link, resource result_entry)
   Get attributes from a search result entry */
PHP_FUNCTION(ldap_get_attributes)
{
	pval **link, **result_entry;
	pval *tmp;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attribute;
	char **ldap_value;
	int i, num_values, num_attrib;
	BerElement *ber;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &link, &result_entry) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, result_entry, -1, "ldap result entry", le_result_entry);

	array_init(return_value);
	num_attrib = 0;
	
	attribute = ldap_first_attribute(ld->link, resultentry->data, &ber);
	while (attribute != NULL) {
		ldap_value = ldap_get_values(ld->link, resultentry->data, attribute);
		num_values = ldap_count_values(ldap_value);

		MAKE_STD_ZVAL(tmp);
		array_init(tmp);
		add_assoc_long(tmp, "count", num_values);
		for(i=0; i<num_values; i++) {
			add_index_string(tmp, i, ldap_value[i], 1);
		}
		ldap_value_free(ldap_value);

		zend_hash_update(Z_ARRVAL_P(return_value), attribute, strlen(attribute)+1, (void *) &tmp, sizeof(pval *), NULL);
		add_index_string(return_value, num_attrib, attribute, 1);

		num_attrib++;
#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP || WINDOWS
		ldap_memfree(attribute);
#endif
		attribute = ldap_next_attribute(ld->link, resultentry->data, ber);
	}
#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP || WINDOWS
		if (ber != NULL)
			ber_free(ber, 0);
#endif
	
	add_assoc_long(return_value, "count", num_attrib);
}
/* }}} */

/* {{{ proto array ldap_get_values(resource link, resource result_entry, string attribute)
   Get all values from a result entry */
PHP_FUNCTION(ldap_get_values)
{
	pval **link, **result_entry, **attr;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attribute;
	char **ldap_value;
	int i, num_values;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &link, &result_entry, &attr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, result_entry, -1, "ldap result entry", le_result_entry);

	convert_to_string_ex(attr);
	attribute = Z_STRVAL_PP(attr);

	if ((ldap_value = ldap_get_values(ld->link, resultentry->data, attribute)) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot get the value(s) of attribute %s", ldap_err2string(_get_lderrno(ld->link)));
		RETURN_FALSE;
	}

	num_values = ldap_count_values(ldap_value);

	array_init(return_value);

	for(i=0; i<num_values; i++) {
		add_next_index_string(return_value, ldap_value[i], 1);
	}
	
	add_assoc_long(return_value, "count", num_values);
	ldap_value_free(ldap_value);

}
/* }}} */

/* {{{ proto array ldap_get_values_len(resource link, resource result_entry, string attribute)
   Get all values with lengths from a result entry */
PHP_FUNCTION(ldap_get_values_len)
{
	pval **link, **result_entry, **attr;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char* attribute;
	struct berval **ldap_value_len;
	int i, num_values;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &link, &result_entry, &attr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, result_entry, -1, "ldap result entry", le_result_entry);

	convert_to_string_ex(attr);
	attribute = Z_STRVAL_PP(attr);
	
	if ((ldap_value_len = ldap_get_values_len(ld->link, resultentry->data, attribute)) == NULL) {
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
	pval **link, **result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *text;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &link, &result_entry) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, result_entry, -1, "ldap result entry", le_result_entry);

	text = ldap_get_dn(ld->link, resultentry->data);
	if ( text != NULL ) {
		RETVAL_STRING(text, 1);
#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP || WINDOWS
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
	pval **dn, **with_attrib;
	char **ldap_value;
	int i, count;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &dn, &with_attrib) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(dn);
	convert_to_long_ex(with_attrib);

	ldap_value = ldap_explode_dn(Z_STRVAL_PP(dn), Z_LVAL_PP(with_attrib));

	i=0;
	while(ldap_value[i] != NULL) i++;
	count = i;

	array_init(return_value);

	add_assoc_long(return_value, "count", count);
	for(i=0; i<count; i++) {
		add_index_string(return_value, i, ldap_value[i], 1);
	}

	ldap_value_free(ldap_value);
}
/* }}} */

/* {{{ proto string ldap_dn2ufn(string dn)
   Convert DN to User Friendly Naming format */
PHP_FUNCTION(ldap_dn2ufn)
{
	pval **dn;
	char *ufn;

	if (ZEND_NUM_ARGS() !=1 || zend_get_parameters_ex(1, &dn)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(dn);
	
	ufn = ldap_dn2ufn(Z_STRVAL_PP(dn));
	
	if (ufn !=NULL) {
		RETVAL_STRING(ufn, 1);
#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP || WINDOWS
		ldap_memfree(ufn);
#endif
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* added to fix use of ldap_modify_add for doing an ldap_add, gerrit thomson.   */
#define PHP_LD_FULL_ADD 0xff
/* {{{ php_ldap_do_modify
 */
static void php_ldap_do_modify(INTERNAL_FUNCTION_PARAMETERS, int oper)
{
	pval **link, **dn, **entry, **value, **ivalue;
	ldap_linkdata *ld;
	char *ldap_dn;
	LDAPMod **ldap_mods;
	int i, j, num_attribs, num_values;
	int *num_berval;
	char *attribute;
	ulong index;
	int is_full_add=0; /* flag for full add operation so ldap_mod_add can be put back into oper, gerrit THomson */
 
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &link, &dn, &entry) == FAILURE) {
		WRONG_PARAM_COUNT;
	}	

	if (Z_TYPE_PP(entry) != IS_ARRAY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected Array as the last element");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);

	convert_to_string_ex(dn);
	ldap_dn = Z_STRVAL_PP(dn);

	num_attribs = zend_hash_num_elements(Z_ARRVAL_PP(entry));
	ldap_mods = emalloc((num_attribs+1) * sizeof(LDAPMod *));
	num_berval = emalloc(num_attribs * sizeof(int));
	zend_hash_internal_pointer_reset(Z_ARRVAL_PP(entry));

	/* added by gerrit thomson to fix ldap_add using ldap_mod_add */
	if ( oper == PHP_LD_FULL_ADD ) {
		oper = LDAP_MOD_ADD;
		is_full_add = 1;
	}
	/* end additional , gerrit thomson */

	for(i=0; i<num_attribs; i++) {
		ldap_mods[i] = emalloc(sizeof(LDAPMod));
		ldap_mods[i]->mod_op = oper | LDAP_MOD_BVALUES;

		if (zend_hash_get_current_key(Z_ARRVAL_PP(entry), &attribute, &index, 0) == HASH_KEY_IS_STRING) {
			ldap_mods[i]->mod_type = estrdup(attribute);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unknown attribute in the data");
			/* Free allocated memory */
			while (i >= 0) {
				efree(ldap_mods[i--]);
			}
			efree(num_berval);
			efree(ldap_mods);	
			RETURN_FALSE;
		}

		zend_hash_get_current_data(Z_ARRVAL_PP(entry), (void **)&value);

		if (Z_TYPE_PP(value) != IS_ARRAY) {
			num_values = 1;
		} else {
			num_values = zend_hash_num_elements(Z_ARRVAL_PP(value));
		}
		
		num_berval[i] = num_values;
		ldap_mods[i]->mod_bvalues = emalloc((num_values + 1) * sizeof(struct berval *));

/* allow for arrays with one element, no allowance for arrays with none but probably not required, gerrit thomson. */
		if ((num_values == 1) && (Z_TYPE_PP(value) != IS_ARRAY)) {
			convert_to_string_ex(value);
			ldap_mods[i]->mod_bvalues[0] = (struct berval *) emalloc (sizeof(struct berval));
			ldap_mods[i]->mod_bvalues[0]->bv_len = Z_STRLEN_PP(value);
			ldap_mods[i]->mod_bvalues[0]->bv_val = Z_STRVAL_PP(value);
		} else {	
			for(j=0; j < num_values; j++) {
				if (zend_hash_index_find(Z_ARRVAL_PP(value), j, (void **) &ivalue) == FAILURE) {
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
		zend_hash_move_forward(Z_ARRVAL_PP(entry));
	}
	ldap_mods[num_attribs] = NULL;

/* check flag to see if do_mod was called to perform full add , gerrit thomson */
	if (is_full_add == 1) {
		if ((i = ldap_add_s(ld->link, ldap_dn, ldap_mods)) != LDAP_SUCCESS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Add: %s", ldap_err2string(i));
			RETVAL_FALSE;
		} else RETVAL_TRUE;
	} else {
		if ((i = ldap_modify_s(ld->link, ldap_dn, ldap_mods)) != LDAP_SUCCESS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Modify: %s", ldap_err2string(i));
			RETVAL_FALSE;
		} else RETVAL_TRUE;	
	}

errexit:
	for(i=0; i < num_attribs; i++) {
		efree(ldap_mods[i]->mod_type);
		for(j=0; j<num_berval[i]; j++) {
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
	pval **link, **dn;
	ldap_linkdata *ld;
	char *ldap_dn;
	int rc;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &link, &dn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);

	convert_to_string_ex(dn);
	ldap_dn = Z_STRVAL_PP(dn);

	if ((rc = ldap_delete_s(ld->link, ldap_dn)) != LDAP_SUCCESS) {
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
	pval **link;
	ldap_linkdata *ld;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ht, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);

	RETURN_LONG( _get_lderrno(ld->link) );
}
/* }}} */

/* {{{ proto string ldap_err2str(int errno)
   Convert error number to error string */
PHP_FUNCTION(ldap_err2str)
{
	zval **perrno;

	if ( ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ht, &perrno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(perrno);
	RETURN_STRING(ldap_err2string(Z_LVAL_PP(perrno)), 1);
}
/* }}} */

/* {{{ proto string ldap_error(resource link)
   Get the current ldap error string */
PHP_FUNCTION(ldap_error) 
{
	pval **link;
	ldap_linkdata *ld;
	int ld_errno;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ht, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);

	ld_errno = _get_lderrno(ld->link);

	RETURN_STRING(ldap_err2string(ld_errno), 1);
}
/* }}} */

/* {{{ proto bool ldap_compare(resource link, string dn, string attr, string value)
   Determine if an entry has a specific value for one of its attributes */
PHP_FUNCTION(ldap_compare) 
{
	pval **link, **dn, **attr, **value;
	char *ldap_dn, *ldap_attr, *ldap_value;
	ldap_linkdata *ld;
	int errno;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &link, &dn, &attr, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);

	convert_to_string_ex(dn);
	convert_to_string_ex(attr);
	convert_to_string_ex(value);

	ldap_dn = Z_STRVAL_PP(dn);
	ldap_attr = Z_STRVAL_PP(attr);
	ldap_value = Z_STRVAL_PP(value);

	errno = ldap_compare_s(ld->link, ldap_dn, ldap_attr, ldap_value);

	switch(errno) {
		case LDAP_COMPARE_TRUE :
			RETURN_TRUE;
		break;

		case LDAP_COMPARE_FALSE :
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrs", &link, &result, &sortfilter, &sflen) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (zend_hash_index_find(&EG(regular_list), Z_LVAL_P(result), (void **) &le) == FAILURE || le->type != le_result) {
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


#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP
/* {{{ proto bool ldap_get_option(resource link, int option, mixed retval)
   Get the current value of various session-wide parameters */
PHP_FUNCTION(ldap_get_option) 
{
	pval **link, **option, **retval;
	ldap_linkdata *ld;
	int opt;
	
	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &link, &option, &retval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);

	convert_to_long_ex(option);
	opt = Z_LVAL_PP(option);

	switch(opt) {
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
			if (ldap_get_option(ld->link, opt, &val)) {
				RETURN_FALSE;
			}
			zval_dtor(*retval);
                        ZVAL_LONG(*retval, val);
		} break;
		/* options with string value */
	case LDAP_OPT_HOST_NAME:
	case LDAP_OPT_ERROR_STRING:
#ifdef LDAP_OPT_MATCHED_DN
	case LDAP_OPT_MATCHED_DN:
#endif
	        {
			char *val;
			if (ldap_get_option(ld->link, opt, &val)) {
				RETURN_FALSE;
			}
			zval_dtor(*retval);
			ZVAL_STRING(*retval, val, 1);
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
	pval **link, **option, **newval;
	ldap_linkdata *ld;
	LDAP *ldap;
	int opt;
	
	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &link, &option, &newval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(link) == IS_NULL) {
		ldap = NULL;
	} else {
		ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
		ldap = ld->link;
	}

	convert_to_long_ex(option);
	opt = Z_LVAL_PP(option);

	switch(opt) {
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
			if (ldap_set_option(ldap, opt, &val)) {
				RETURN_FALSE;
			}
		} break;
		/* options with string value */
	case LDAP_OPT_HOST_NAME:
	case LDAP_OPT_ERROR_STRING:
#ifdef LDAP_OPT_MATCHED_DN
	case LDAP_OPT_MATCHED_DN:
#endif
		{
			char *val;
			convert_to_string_ex(newval);
			val = Z_STRVAL_PP(newval);
			if (ldap_set_option(ldap, opt, val)) {
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
			if (ldap_set_option(ldap, opt, val)) {
				RETURN_FALSE;
			}
		} break;
		/* options with control list value */
	case LDAP_OPT_SERVER_CONTROLS:
	case LDAP_OPT_CLIENT_CONTROLS:
		{
			LDAPControl *ctrl, **ctrls, **ctrlp;
			zval **ctrlval, **val;
			int ncontrols;
			char error=0;

			if ((Z_TYPE_PP(newval) != IS_ARRAY) || !(ncontrols = zend_hash_num_elements(Z_ARRVAL_PP(newval)))) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected non-empty array value for this option");
                                RETURN_FALSE;
                        }
			ctrls = emalloc((1 + ncontrols) * sizeof(*ctrls));
			*ctrls = NULL;
			ctrlp = ctrls;
			zend_hash_internal_pointer_reset(Z_ARRVAL_PP(newval));
			while (zend_hash_get_current_data(Z_ARRVAL_PP(newval), (void**)&ctrlval) == SUCCESS) {
				if (Z_TYPE_PP(ctrlval) != IS_ARRAY) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "The array value must contain only arrays, where each array is a control");
					error = 1;
					break;
				}
				if (zend_hash_find(Z_ARRVAL_PP(ctrlval), "oid", sizeof("oid"), (void **) &val) == FAILURE) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Control must have an oid key");
					error = 1;
					break;
				}
				ctrl = *ctrlp = emalloc(sizeof(**ctrlp));
				convert_to_string_ex(val);
				ctrl->ldctl_oid = Z_STRVAL_PP(val);
				if (zend_hash_find(Z_ARRVAL_PP(ctrlval), "value", sizeof("value"), (void **) &val) == SUCCESS) {
					convert_to_string_ex(val);
					ctrl->ldctl_value.bv_val = Z_STRVAL_PP(val);
					ctrl->ldctl_value.bv_len = Z_STRLEN_PP(val);
				} else {
					ctrl->ldctl_value.bv_val = NULL;
					ctrl->ldctl_value.bv_len = 0;
				}
				if (zend_hash_find(Z_ARRVAL_PP(ctrlval), "iscritical", sizeof("iscritical"), (void **) &val) == SUCCESS) {
					convert_to_boolean_ex(val);
					ctrl->ldctl_iscritical = Z_BVAL_PP(val);
				} else {
					ctrl->ldctl_iscritical = 0;
				}
				
				++ctrlp;
				*ctrlp = NULL;
				zend_hash_move_forward(Z_ARRVAL_PP(newval));
			}
			if (!error) {
				error = ldap_set_option(ldap, opt, ctrls);
			}
			ctrlp = ctrls;
			while ( *ctrlp ) {
				efree(*ctrlp);
				ctrlp++;
			}
			efree(ctrls);
			if (error) {
				RETURN_FALSE;
			}
		} break;
	default:
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ldap_parse_result(resource link, resource result, int errcode, string matcheddn, string errmsg, array referrals)
   Extract information from result */
PHP_FUNCTION(ldap_parse_result) 
{
	pval **link, **result, **errcode, **matcheddn, **errmsg, **referrals;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;
	char **lreferrals, **refp;
	char *lmatcheddn, *lerrmsg;
	int rc, lerrcode, myargcount = ZEND_NUM_ARGS();
  
	if (myargcount < 3 || myargcount > 6 || zend_get_parameters_ex(myargcount, &link, &result, &errcode, &matcheddn, &errmsg, &referrals) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, result, -1, "ldap result", le_result);

	rc = ldap_parse_result( ld->link, ldap_result, &lerrcode,
				myargcount > 3 ? &lmatcheddn : NULL,
				myargcount > 4 ? &lerrmsg : NULL,
				myargcount > 5 ? &lreferrals : NULL,
				NULL /* &serverctrls */,
				0 );
	if (rc != LDAP_SUCCESS ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to parse result: %s", ldap_err2string(rc));
		RETURN_FALSE;
	}

	zval_dtor(*errcode);
	ZVAL_LONG(*errcode, lerrcode);

	/* Reverse -> fall through */
	switch(myargcount) {
		case 6 :
			zval_dtor(*referrals);
			array_init(*referrals);
			if (lreferrals != NULL) {
				refp = lreferrals;
				while (*refp) {
					add_next_index_string(*referrals, *refp, 1);
					refp++;
				}
				ldap_value_free(lreferrals);
			}
		case 5 :
			zval_dtor(*errmsg);
			if (lerrmsg == NULL) {
				ZVAL_EMPTY_STRING(*errmsg);
			} else {
				ZVAL_STRING(*errmsg, lerrmsg, 1);
				ldap_memfree(lerrmsg);
			}
		case 4 : 
			zval_dtor(*matcheddn);
			if (lmatcheddn == NULL) {
				ZVAL_EMPTY_STRING(*matcheddn);
			} else {
				ZVAL_STRING(*matcheddn, lmatcheddn, 1);
				ldap_memfree(lmatcheddn);
			}
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource ldap_first_reference(resource link, resource result)
   Return first reference */
PHP_FUNCTION(ldap_first_reference)
{
	pval **link, **result;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	LDAPMessage *ldap_result, *entry;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &link, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, result, -1, "ldap result", le_result);

	if ((entry = ldap_first_reference(ld->link, ldap_result)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry, le_result_entry);
		resultentry->id = Z_LVAL_PP(result);
		zend_list_addref(resultentry->id);
		resultentry->data = entry;
	}
}
/* }}} */

/* {{{ proto resource ldap_next_reference(resource link, resource reference_entry)
   Get next reference */
PHP_FUNCTION(ldap_next_reference)
{
	pval **link, **result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry, *resultentry_next;
	LDAPMessage *entry_next;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &link, &result_entry) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, result_entry, -1, "ldap result entry", le_result_entry);

	if ((entry_next = ldap_next_reference(ld->link, resultentry->data)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry_next = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry_next, le_result_entry);
		resultentry_next->id = resultentry->id;
		zend_list_addref(resultentry->id);
		resultentry_next->data = entry_next;
	}
}
/* }}} */

#ifdef HAVE_LDAP_PARSE_REFERENCE
/* {{{ proto bool ldap_parse_reference(resource link, resource reference_entry, array referrals)
   Extract information from reference entry */
PHP_FUNCTION(ldap_parse_reference)
{
	pval **link, **result_entry, **referrals;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char **lreferrals, **refp;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &link, &result_entry, &referrals) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, result_entry, -1, "ldap result entry", le_result_entry);

	if (ldap_parse_reference(ld->link, resultentry->data, &lreferrals, NULL /* &serverctrls */, 0) != LDAP_SUCCESS) {
		RETURN_FALSE;
	}

	zval_dtor(*referrals);
	array_init(*referrals);
	if (lreferrals != NULL) {
		refp = lreferrals;
		while (*refp) {
			add_next_index_string(*referrals, *refp, 1);
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
	pval **link, **dn, **newrdn, **newparent, **deleteoldrdn;
	ldap_linkdata *ld;
	int rc;
	
	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &link, &dn, &newrdn, &newparent, &deleteoldrdn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);

	convert_to_string_ex(dn);
	convert_to_string_ex(newrdn);
	convert_to_string_ex(newparent);
	convert_to_boolean_ex(deleteoldrdn);

#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP	
	rc = ldap_rename_s(ld->link, Z_STRVAL_PP(dn), Z_STRVAL_PP(newrdn), Z_STRVAL_PP(newparent), Z_BVAL_PP(deleteoldrdn), NULL, NULL);
#else
	if (Z_STRLEN_PP(newparent) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "You are using old LDAP API, newparent must be the empty string, can only modify RDN");
		RETURN_FALSE;
	}
/* could support old APIs but need check for ldap_modrdn2()/ldap_modrdn() */
	rc = ldap_modrdn2_s(ld->link, Z_STRVAL_PP(dn), Z_STRVAL_PP(newrdn), Z_BVAL_PP(deleteoldrdn));
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
	pval **link;
	ldap_linkdata *ld;
	int rc;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, link, -1, "ldap link", le_link);

	if ((rc = ldap_start_tls_s(ld->link, NULL, NULL)) != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Unable to start TLS: %s", ldap_err2string(rc));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */
#endif
#endif /* ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP */


#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
int _ldap_rebind_proc(LDAP *ldap, const char *url, ber_tag_t req, ber_int_t msgid, void *params) {
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


/* {{{ proto bool ldap_set_rebind_proc(resource link, string callback)
   Set a callback function to do re-binds on referral chasing. */
PHP_FUNCTION(ldap_set_rebind_proc)
{
	zval *link, *callback;
	ldap_linkdata *ld;
	char *callback_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &link, &callback) == FAILURE) {
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
	if (!zend_is_callable(callback, 0, &callback_name)) {
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
#endif


#ifdef STR_TRANSLATION
/* {{{ php_ldap_do_translate
 */
static void php_ldap_do_translate(INTERNAL_FUNCTION_PARAMETERS, int way) 
{
	zval **value;
	char *ldap_buf;
	unsigned long ldap_len;
	int result;
		
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(value);
	ldap_buf = Z_STRVAL_PP(value);
	ldap_len = Z_STRLEN_PP(value);

	if(ldap_len == 0) {
		RETURN_FALSE;
	}

	if(way == 1) {
		result = ldap_8859_to_t61(&ldap_buf, &ldap_len, 0);
	} else {
		result = ldap_t61_to_8859(&ldap_buf, &ldap_len, 0);
	}

	if (result == LDAP_SUCCESS) {
		RETVAL_STRINGL(ldap_buf, ldap_len, 1);
		free(ldap_buf);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Conversion from iso-8859-1 to t61 failed: %s", ldap_err2string(result));
		RETVAL_FALSE;
	}

	return;
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
