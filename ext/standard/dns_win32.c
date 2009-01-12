/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2008-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Pierre A. Joye <pierre@php.net>                             |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#include <windows.h>
#include <Winbase.h >
#include <Windns.h>

#include "dns.h"

#define PHP_DNS_NUM_TYPES	12	/* Number of DNS Types Supported by PHP currently */

#define PHP_DNS_A      0x00000001
#define PHP_DNS_NS     0x00000002
#define PHP_DNS_CNAME  0x00000010
#define PHP_DNS_SOA    0x00000020
#define PHP_DNS_PTR    0x00000800
#define PHP_DNS_HINFO  0x00001000
#define PHP_DNS_MX     0x00004000
#define PHP_DNS_TXT    0x00008000
#define PHP_DNS_A6     0x01000000
#define PHP_DNS_SRV    0x02000000
#define PHP_DNS_NAPTR  0x04000000
#define PHP_DNS_AAAA   0x08000000
#define PHP_DNS_ANY    0x10000000
#define PHP_DNS_ALL    (PHP_DNS_A|PHP_DNS_NS|PHP_DNS_CNAME|PHP_DNS_SOA|PHP_DNS_PTR|PHP_DNS_HINFO|PHP_DNS_MX|PHP_DNS_TXT|PHP_DNS_A6|PHP_DNS_SRV|PHP_DNS_NAPTR|PHP_DNS_AAAA)

PHP_FUNCTION(dns_get_mx) /* {{{ */
{
	char *hostname;
	int hostname_len;
	zval *mx_list, *weight_list = NULL;

	DNS_STATUS      status;                 /* Return value of DnsQuery_A() function */
	PDNS_RECORD     pResult, pRec;          /* Pointer to DNS_RECORD structure */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &hostname, &hostname_len, &mx_list, &weight_list) == FAILURE) {
		return;
	}

	status = DnsQuery_A(hostname, DNS_TYPE_MX, DNS_QUERY_STANDARD, NULL, &pResult, NULL);

	if (status) {
		RETURN_FALSE;
	}

	zval_dtor(mx_list);
	array_init(mx_list);

	if (weight_list) {
		zval_dtor(weight_list);
		array_init(weight_list);
	}

	for (pRec = pResult; pRec; pRec = pRec->pNext) {
		DNS_SRV_DATA *srv = &pRec->Data.Srv;

		if (pRec->wType != DNS_TYPE_MX) {
			continue;
		}

		add_next_index_rt_string(mx_list, pRec->Data.MX.pNameExchange, ZSTR_DUPLICATE);
		if (weight_list) {
			add_next_index_long(weight_list, srv->wPriority);
		}
	}

	/* Free memory allocated for DNS records. */
	DnsRecordListFree(pResult, DnsFreeRecordListDeep);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool dns_check_record(string host [, string type])
   Check DNS records corresponding to a given Internet host name or IP address */
PHP_FUNCTION(dns_check_record)
{
	char *hostname, *rectype = NULL;
	int hostname_len, rectype_len = 0;
	int type = DNS_TYPE_MX;

	DNS_STATUS      status;                 /* Return value of DnsQuery_A() function */
	PDNS_RECORD     pResult, pRec;          /* Pointer to DNS_RECORD structure */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &hostname, &hostname_len, &rectype, &rectype_len) == FAILURE) {
		return;
	}

	if (rectype) {
		     if (!strcasecmp("A",     rectype)) type = DNS_TYPE_A;
		else if (!strcasecmp("NS",    rectype)) type = DNS_TYPE_NS;
		else if (!strcasecmp("MX",    rectype)) type = DNS_TYPE_MX;
		else if (!strcasecmp("PTR",   rectype)) type = DNS_TYPE_PTR;
		else if (!strcasecmp("ANY",   rectype)) type = DNS_TYPE_ANY;
		else if (!strcasecmp("SOA",   rectype)) type = DNS_TYPE_SOA;
		else if (!strcasecmp("TXT",   rectype)) type = DNS_TYPE_TEXT;
		else if (!strcasecmp("CNAME", rectype)) type = DNS_TYPE_CNAME;
		else if (!strcasecmp("AAAA",  rectype)) type = DNS_TYPE_AAAA;
		else if (!strcasecmp("SRV",   rectype)) type = DNS_TYPE_SRV;
		else if (!strcasecmp("NAPTR", rectype)) type = DNS_TYPE_NAPTR;
		else if (!strcasecmp("A6",    rectype)) type = DNS_TYPE_A6;
		else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Type '%s' not supported", rectype);
			RETURN_FALSE;
		}
	}

	status = DnsQuery_A(hostname, DNS_TYPE_MX, DNS_QUERY_STANDARD, NULL, &pResult, NULL);

	if (status) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#if 1
/* {{{ php_parserr */
static void php_parserr(PDNS_RECORD pRec, int type_to_fetch, int store, zval **subarray TSRMLS_DC)
{
	int type;
	u_long ttl;

	type = pRec->wType;
	ttl = pRec->dwTtl;

	if (type_to_fetch != DNS_TYPE_ANY && type != type_to_fetch) {
		return;
	}

	if (!store) {
		return;
	}

	ALLOC_INIT_ZVAL(*subarray);
	array_init(*subarray);

	add_ascii_assoc_rt_string(*subarray, "host", pRec->pName, ZSTR_DUPLICATE);
	switch (type) {
		case DNS_TYPE_A: {
			IN_ADDR ipaddr;
			ipaddr.S_un.S_addr = (pRec->Data.A.IpAddress);
			add_ascii_assoc_rt_string(*subarray, "type", "A", ZSTR_DUPLICATE);
			add_ascii_assoc_rt_string(*subarray, "ip", inet_ntoa(ipaddr), ZSTR_DUPLICATE);
			break;
		}

		case DNS_TYPE_MX:
			add_ascii_assoc_rt_string(*subarray, "type", "MX", ZSTR_DUPLICATE);
			add_ascii_assoc_long(*subarray, "pri", pRec->Data.Srv.wPriority);
			/* no break; */

		case DNS_TYPE_CNAME:
			if (type == DNS_TYPE_CNAME) {
				add_ascii_assoc_rt_string(*subarray, "type", "CNAME", ZSTR_DUPLICATE);
			}
			/* no break; */

		case DNS_TYPE_NS:
			if (type == DNS_TYPE_NS) {
				add_ascii_assoc_rt_string(*subarray, "type", "NS", ZSTR_DUPLICATE);
			}
			/* no break; */

		case DNS_TYPE_PTR:
			if (type == DNS_TYPE_PTR) {
				add_ascii_assoc_rt_string(*subarray, "type", "PTR", ZSTR_DUPLICATE);
			}
			add_ascii_assoc_rt_string(*subarray, "target", pRec->Data.MX.pNameExchange, ZSTR_DUPLICATE);
			break;

		/* Not available on windows, the query is possible but there is no DNS_HINFO_DATA structure */
		case DNS_TYPE_HINFO:
		case DNS_TYPE_TEXT:
			{
				int i = 0;
				DNS_TXT_DATA *data_txt = &pRec->Data.TXT;
				DWORD count = data_txt->dwStringCount;
				char *txt, *txt_dst;
				long txt_len = 0;
				zval *entries;

				add_ascii_assoc_rt_string(*subarray, "type", "TXT", ZSTR_DUPLICATE);
				
				ALLOC_INIT_ZVAL(entries);
				array_init(entries);

				for (i = 0; i < count; i++) {
					txt_len += strlen(data_txt->pStringArray[i]) + 1;
				}

				txt = ecalloc(txt_len * 2, 1);
				txt_dst = txt;
				for (i = 0; i < count; i++) {
					int len = strlen(data_txt->pStringArray[i]);
					memcpy(txt_dst, data_txt->pStringArray[i], len);
					add_next_index_rt_stringl(entries, data_txt->pStringArray[i], len, ZSTR_DUPLICATE);
					txt_dst += len;
				}

				add_ascii_assoc_rt_string(*subarray, "txt", txt, ZSTR_AUTOFREE);
				add_ascii_assoc_zval(*subarray, "entries", entries);
			}
			break;

		case DNS_TYPE_SOA:
			{
				DNS_SOA_DATA *data_soa = &pRec->Data.Soa;

				add_ascii_assoc_rt_string(*subarray, "type", "SOA", ZSTR_DUPLICATE);

				add_ascii_assoc_rt_string(*subarray, "mname", data_soa->pNamePrimaryServer, ZSTR_DUPLICATE);
				add_ascii_assoc_rt_string(*subarray, "rname", data_soa->pNameAdministrator, ZSTR_DUPLICATE);
				add_ascii_assoc_long(*subarray, "serial", data_soa->dwSerialNo);
				add_ascii_assoc_long(*subarray, "refresh", data_soa->dwRefresh);
				add_ascii_assoc_long(*subarray, "retry", data_soa->dwRetry);
				add_ascii_assoc_long(*subarray, "expire", data_soa->dwExpire);
				add_ascii_assoc_long(*subarray, "minimum-ttl", data_soa->dwDefaultTtl);
			}
			break;


		case DNS_TYPE_AAAA:
#if _WIN32_WINNT >= 0x0600
			{
				LPSTR str[MAXHOSTNAMELEN];
				DNS_AAAA_DATA *data_aaaa = &pRec->Data.AAAA;
				add_ascii_assoc_rt_string(*subarray, "type", "AAAA", ZSTR_DUPLICATE);
				add_ascii_assoc_rt_string(*subarray, "ipv6", RtlIpv6AddressToString(data_aaaa->Ip6Address, str), ZSTR_DUPLICATE);
			}
#endif
			break;

#if 0
		/* Not supported yet */
		case DNS_TYPE_A6:
			break;
#endif

		case DNS_TYPE_SRV:
			{
				DNS_SRV_DATA *data_srv = &pRec->Data.Srv;

				add_ascii_assoc_rt_string(*subarray, "type", "SRV", ZSTR_DUPLICATE);
				add_ascii_assoc_long(*subarray, "pri", data_srv->wPriority);
				add_ascii_assoc_long(*subarray, "weight", data_srv->wWeight);
				add_ascii_assoc_long(*subarray, "port", data_srv->wPort);
				add_ascii_assoc_rt_string(*subarray, "target", data_srv->pNameTarget, ZSTR_DUPLICATE);
			}
			break;

		case DNS_TYPE_NAPTR:
			{
#ifdef DNS_NAPTR_DATA
				DNS_NAPTR_DATA * data_naptr = &pRec->Data.Naptr;
				add_ascii_assoc_rt_string(*subarray, "type", "NAPTR", ZSTR_DUPLICATE);
				add_ascii_assoc_long(*subarray, "order", data_naptr->wOrder);
				add_ascii_assoc_long(*subarray, "pref", data_naptr->wPreference);
				add_ascii_assoc_rt_string(*subarray, "flags", data_naptr->pFlags, ZSTR_DUPLICATE);
				add_ascii_assoc_rt_string(*subarray, "services", data_naptr->pService, ZSTR_DUPLICATE);
				add_ascii_assoc_rt_string(*subarray, "regex", data_naptr->pRegularExpression, ZSTR_DUPLICATE);
				add_ascii_assoc_rt_string(*subarray, "replacement", data_naptr->pReplacement, ZSTR_DUPLICATE);
#endif
			}
			break;

		default:
			break;
	}

	add_ascii_assoc_rt_string(*subarray, "class", "IN", ZSTR_DUPLICATE);
	add_ascii_assoc_long(*subarray, "ttl", ttl);
}
/* }}} */
#endif

/* {{{ proto array|false dns_get_record(string hostname [, int type[, array authns, array addtl]])
   Get any Resource Record corresponding to a given Internet host name */
PHP_FUNCTION(dns_get_record)
{
	char *hostname;
	int hostname_len;
	long type_param = PHP_DNS_ANY;
	zval *authns = NULL, *addtl = NULL;
	int type, type_to_fetch, first_query = 1, store_results = 1;
	int addtl_recs = 0;
	IN_ADDR ipaddr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lzz", &hostname, &hostname_len, &type_param, &authns, &addtl) == FAILURE) {
		return;
	}

	if (authns) {
		zval_dtor(authns);
		array_init(authns);
	}
	if (addtl) {
		zval_dtor(addtl);
		array_init(addtl);
	}

	if (type_param & ~PHP_DNS_ALL && type_param != PHP_DNS_ANY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Type '%ld' not supported", type_param);
		RETURN_FALSE;
	}

	/* Initialize the return array */
	array_init(return_value);

	for (type = (type_param == PHP_DNS_ANY ? (PHP_DNS_NUM_TYPES + 1) : 0);
		type < (addtl_recs ? (PHP_DNS_NUM_TYPES + 2) : PHP_DNS_NUM_TYPES) || first_query;
		type++
	) {
		DNS_STATUS      status;                 /* Return value of DnsQuery_A() function */
		PDNS_RECORD     pResult, pRec;          /* Pointer to DNS_RECORD structure */

		first_query = 0;
		switch (type) {
			case 0:
				type_to_fetch = type_param&PHP_DNS_A     ? DNS_TYPE_A     : 0;
				break;
			case 1:
				type_to_fetch = type_param&PHP_DNS_NS    ? DNS_TYPE_NS    : 0;
				break;
			case 2:
				type_to_fetch = type_param&PHP_DNS_CNAME ? DNS_TYPE_CNAME : 0;
				break;
			case 3:
				type_to_fetch = type_param&PHP_DNS_SOA   ? DNS_TYPE_SOA   : 0;
				break;
			case 4:
				type_to_fetch = type_param&PHP_DNS_PTR   ? DNS_TYPE_PTR   : 0;
				break;
			case 5:
				type_to_fetch = type_param&PHP_DNS_HINFO ? DNS_TYPE_HINFO : 0;
				break;
			case 6:
				type_to_fetch = type_param&PHP_DNS_MX    ? DNS_TYPE_MX    : 0;
				break;
			case 7:
				type_to_fetch = type_param&PHP_DNS_TXT   ? DNS_TYPE_TEXT   : 0;
				break;
			case 8:
				type_to_fetch = type_param&PHP_DNS_AAAA	 ? DNS_TYPE_AAAA  : 0;
				break;
			case 9:
				type_to_fetch = type_param&PHP_DNS_SRV   ? DNS_TYPE_SRV   : 0;
				break;
			case 10:
				type_to_fetch = type_param&PHP_DNS_NAPTR ? DNS_TYPE_NAPTR : 0;
				break;
			case 11:
				type_to_fetch = type_param&PHP_DNS_A6	 ? DNS_TYPE_A6 : 0;
				break;
			case PHP_DNS_NUM_TYPES:
				store_results = 0;
				continue;
			default:
			case (PHP_DNS_NUM_TYPES + 1):
				type_to_fetch = DNS_TYPE_ANY;
				break;
		}

		if (type_to_fetch) {
			status = DnsQuery_A(hostname, type_to_fetch, DNS_QUERY_STANDARD, NULL, &pResult, NULL);

			if (status) {
				if (status == DNS_INFO_NO_RECORDS) {
					continue;
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Dns Query failed");
					zval_dtor(return_value);
					RETURN_FALSE;
				}
			}

			for (pRec = pResult; pRec; pRec = pRec->pNext) {
				DNS_SRV_DATA *srv = &pRec->Data.Srv;
				zval *retval = NULL;

				if (pRec->Flags.S.Section == DnsSectionAnswer) {
					php_parserr(pRec, type_to_fetch, store_results, &retval TSRMLS_CC);
					if (retval != NULL && store_results) {
						add_next_index_zval(return_value, retval);
					}
				}

				if (authns && pRec->Flags.S.Section == DnsSectionAuthority) {

					php_parserr(pRec, type_to_fetch, store_results, &retval TSRMLS_CC);
					if (retval != NULL) {
						add_next_index_zval(authns, retval);
					}
				}

/* Stupid typo in PSDK 6.1, WinDNS.h(1258)... */
#ifndef DnsSectionAdditional
# ifdef DnsSectionAddtional
#  define DnsSectionAdditional DnsSectionAddtional
# else
# define DnsSectionAdditional 3
# endif
#endif
				if (addtl && pRec->Flags.S.Section == DnsSectionAdditional) {
					php_parserr(pRec, type_to_fetch, store_results, &retval TSRMLS_CC);
					if (retval != NULL) {
						add_next_index_zval(addtl, retval);
					}
				}
			}
			/* Free memory allocated for DNS records. */
			DnsRecordListFree(pResult, DnsFreeRecordListDeep);
		}
	}
}
/* }}} */
