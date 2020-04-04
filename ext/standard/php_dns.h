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

#elif defined(HAVE_RES_NSEARCH)
#define php_dns_search(res, dname, class, type, answer, anslen) \
			res_nsearch(res, dname, class, type, answer, anslen);
#if HAVE_RES_NDESTROY
#define php_dns_free_handle(res) \
			res_ndestroy(res); \
			php_dns_free_res(res)
#else
#define php_dns_free_handle(res) \
			res_nclose(res); \
			php_dns_free_res(res)
#endif

#elif defined(HAVE_RES_SEARCH)
#define php_dns_search(res, dname, class, type, answer, anslen) \
			res_search(dname, class, type, answer, anslen)
#define php_dns_free_handle(res) /* noop */

#endif

#if defined(HAVE_DNS_SEARCH) || defined(HAVE_RES_NSEARCH) || defined(HAVE_RES_SEARCH)
#define HAVE_DNS_SEARCH_FUNC 1
#endif

#if HAVE_DNS_SEARCH_FUNC && HAVE_DN_EXPAND && HAVE_DN_SKIPNAME
#define HAVE_FULL_DNS_FUNCS 1
#endif

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
# if defined(PHP_WIN32) || HAVE_FULL_DNS_FUNCS
PHP_MINIT_FUNCTION(dns);
# endif
#endif /* defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC */

#ifndef INT16SZ
#define INT16SZ		2
#endif

#ifndef INT32SZ
#define INT32SZ		4
#endif

#endif /* PHP_DNS_H */
