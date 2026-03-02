/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: The typical suspects                                        |
   |          Marcus Boerger <helly@php.net>                              |
   |          Pollita <pollita@php.net>                                   |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_DNS_H
#define PHP_DNS_H

#if defined(HAVE_DNS_SEARCH)
#define php_dns_search(res, dname, class, type, answer, anslen) \
    	((int)dns_search(res, dname, class, type, (char *) answer, anslen, (struct sockaddr *)&from, &fromsize))
#define php_dns_free_handle(res) \
		dns_free(res)
#define php_dns_errno(handle) h_errno

#elif defined(HAVE_RES_NSEARCH)
#define php_dns_search(res, dname, class, type, answer, anslen) \
			res_nsearch(res, dname, class, type, answer, anslen);
#ifdef HAVE_RES_NDESTROY
#define php_dns_free_handle(res) \
			res_ndestroy(res); \
			php_dns_free_res(res)
#else
#define php_dns_free_handle(res) \
			res_nclose(res); \
			php_dns_free_res(res)
#endif
#define php_dns_errno(handle) handle->res_h_errno

#elif defined(HAVE_RES_SEARCH)
#define php_dns_search(res, dname, class, type, answer, anslen) \
			res_search(dname, class, type, answer, anslen)
#define php_dns_free_handle(res) /* noop */
#define php_dns_errno(handle) h_errno

#endif

#if defined(HAVE_DNS_SEARCH) || defined(HAVE_RES_NSEARCH) || defined(HAVE_RES_SEARCH)
#define HAVE_DNS_SEARCH_FUNC 1
#endif

#if defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_DN_EXPAND) && defined(HAVE_DN_SKIPNAME)
#define HAVE_FULL_DNS_FUNCS 1
#endif

#if defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS))
#define PHP_DNS_A      0x00000001
#define PHP_DNS_NS     0x00000002
#define PHP_DNS_CNAME  0x00000010
#define PHP_DNS_SOA    0x00000020
#define PHP_DNS_PTR    0x00000800
#define PHP_DNS_HINFO  0x00001000
#if !defined(PHP_WIN32)
# define PHP_DNS_CAA    0x00002000
#endif
#define PHP_DNS_MX     0x00004000
#define PHP_DNS_TXT    0x00008000
#define PHP_DNS_A6     0x01000000
#define PHP_DNS_SRV    0x02000000
#define PHP_DNS_NAPTR  0x04000000
#define PHP_DNS_AAAA   0x08000000
#define PHP_DNS_ANY    0x10000000

#if defined(PHP_WIN32)
# define PHP_DNS_NUM_TYPES	12	/* Number of DNS Types Supported by PHP currently */
# define PHP_DNS_ALL    (PHP_DNS_A|PHP_DNS_NS|PHP_DNS_CNAME|PHP_DNS_SOA|PHP_DNS_PTR|PHP_DNS_HINFO|PHP_DNS_MX|PHP_DNS_TXT|PHP_DNS_A6|PHP_DNS_SRV|PHP_DNS_NAPTR|PHP_DNS_AAAA)
#else
# define PHP_DNS_NUM_TYPES	13	/* Number of DNS Types Supported by PHP currently */
# define PHP_DNS_ALL   (PHP_DNS_A|PHP_DNS_NS|PHP_DNS_CNAME|PHP_DNS_SOA|PHP_DNS_PTR|PHP_DNS_HINFO|PHP_DNS_CAA|PHP_DNS_MX|PHP_DNS_TXT|PHP_DNS_A6|PHP_DNS_SRV|PHP_DNS_NAPTR|PHP_DNS_AAAA)
#endif
#endif

#ifndef INT16SZ
#define INT16SZ		2
#endif

#ifndef INT32SZ
#define INT32SZ		4
#endif

#endif /* PHP_DNS_H */
