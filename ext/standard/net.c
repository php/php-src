/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_network.h"
#include "ext/standard/net.h"

#if HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#if HAVE_NET_IF_H
# include <net/if.h>
#endif

#if HAVE_GETIFADDRS
# include <ifaddrs.h>
#endif

#ifdef PHP_WIN32
# include <intrin.h>
# include <winsock2.h>
# include <ws2ipdef.h>
# include <Ws2tcpip.h>
# include <iphlpapi.h>
#else
# include <netdb.h>
#endif

PHPAPI zend_string* php_inet_ntop(const struct sockaddr *addr) {
	socklen_t addrlen = sizeof(struct sockaddr_in);

	if (!addr) { return NULL; }

	/* Prefer inet_ntop() as it's more task-specific and doesn't have to be demangled */
#if HAVE_INET_NTOP
	switch (addr->sa_family) {
#ifdef AF_INET6
		case AF_INET6: {
			zend_string *ret = zend_string_alloc(INET6_ADDRSTRLEN, 0);
			if (inet_ntop(AF_INET6, &(((struct sockaddr_in6*)addr)->sin6_addr), ZSTR_VAL(ret), INET6_ADDRSTRLEN)) {
				ZSTR_LEN(ret) = strlen(ZSTR_VAL(ret));
				return ret;
			}
			zend_string_free(ret);
			break;
		}
#endif
		case AF_INET: {
			zend_string *ret = zend_string_alloc(INET_ADDRSTRLEN, 0);
			if (inet_ntop(AF_INET, &(((struct sockaddr_in*)addr)->sin_addr), ZSTR_VAL(ret), INET_ADDRSTRLEN)) {
				ZSTR_LEN(ret) = strlen(ZSTR_VAL(ret));
				return ret;
			}
			zend_string_free(ret);
			break;
		}
	}
#endif

	/* Fallback on getnameinfo() */
	switch (addr->sa_family) {
#ifdef AF_INET6
		case AF_INET6:
			addrlen = sizeof(struct sockaddr_in6);
			/* fallthrough */
#endif
		case AF_INET: {
			zend_string *ret = zend_string_alloc(NI_MAXHOST, 0);
			if (getnameinfo(addr, addrlen, ZSTR_VAL(ret), NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == SUCCESS) {
				/* Also demangle numeric host with %name suffix */
				char *colon = strchr(ZSTR_VAL(ret), '%');
				if (colon) { *colon = 0; }
				ZSTR_LEN(ret) = strlen(ZSTR_VAL(ret));
				return ret;
			}
			zend_string_free(ret);
			break;
		}
	}

	return NULL;
}

#if defined(PHP_WIN32) || HAVE_GETIFADDRS
static void iface_append_unicast(zval *unicast, zend_long flags,
                                 struct sockaddr *addr, struct sockaddr *netmask,
                                 struct sockaddr *broadcast, struct sockaddr *ptp) {
	zend_string *host;
	zval u;

	array_init(&u);
	add_assoc_long(&u, "flags", flags);

	if (addr) {
		add_assoc_long(&u, "family", addr->sa_family);
		if ((host = php_inet_ntop(addr))) {
			add_assoc_str(&u, "address", host);
		}
	}
	if ((host = php_inet_ntop(netmask))) {
		add_assoc_str(&u, "netmask", host);
	}

	if ((host = php_inet_ntop(broadcast))) {
		add_assoc_str(&u, "broadcast", host);
	}

	if ((host = php_inet_ntop(ptp))) {
		add_assoc_str(&u, "ptp", host);
	}

	add_next_index_zval(unicast, &u);
}
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_net_get_interfaces, 0, 0, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

/* {{{ proto array|false net_get_interfaces([ int flags ])
Returns an array in the form:
array(
  'ifacename' => array(
    'description' => 'Awesome interface', // Win32 only
    'mac' => '00:11:22:33:44:55',         // Win32 only
    'mtu' => 1234,                        // Win32 only
    'unicast' => array(
      0 => array(
        'family' => 2,                    // e.g. AF_INET, AF_INET6, AF_PACKET
        'address' => '127.0.0.1',
        'netmnask' => '255.0.0.0',
        'broadcast' => '127.255.255.255', // POSIX only
        'ptp' => '127.0.0.2',             // POSIX only
      ), // etc...
    ),
  ), // etc...
)
*/
PHP_FUNCTION(net_get_interfaces) {
#ifdef PHP_WIN32
# define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
# define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
	ULONG family = AF_UNSPEC;
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL, p;
	PIP_ADAPTER_UNICAST_ADDRESS u = NULL;
	ULONG outBufLen = 0;
	DWORD dwRetVal = 0;
	zend_long filter = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(filter)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	// Make an initial call to GetAdaptersAddresses to get the
	// size needed into the outBufLen variable
	if (GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
	}

	if (pAddresses == NULL) {
		zend_error(E_WARNING, "Memory allocation failed for IP_ADAPTER_ADDRESSES struct");
		RETURN_FALSE;
	}

	dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

	if (NO_ERROR != dwRetVal) {
		/* TODO check GetLastError() */
		FREE(pAddresses);
		RETURN_FALSE;
	}

	array_init(return_value);
	for (p = pAddresses; p; p = p->Next) {
		zval iface, unicast;

		if ((p->Flags & filter) != filter) {
			continue;
		}
		if ((IF_TYPE_ETHERNET_CSMACD != p->IfType) && (IF_TYPE_SOFTWARE_LOOPBACK != p->IfType)) {
			continue;
		}

		array_init(&iface);

		if (p->Description) {
			char tmp[256];
			memset(tmp, 0, sizeof(tmp));
			wcstombs(tmp, p->Description, sizeof(tmp));
			add_assoc_string(&iface, "description", tmp);
		}

		if (p->PhysicalAddressLength > 0) {
			zend_string *mac = zend_string_alloc(p->PhysicalAddressLength * 3, 0);
			char *s = ZSTR_VAL(mac);
			ULONG i;
			for (i = 0; i < p->PhysicalAddressLength; ++i) {
				s += snprintf(s, 4, "%02X:", p->PhysicalAddress[i]);
			}
			*(--s) = 0;
			ZSTR_LEN(mac) = s - ZSTR_VAL(mac);
			add_assoc_str(&iface, "mac", mac);
		}

		/* Flags could be placed at this level,
		 * but we repeat it in the unicast subarray
		 * for consistency with the POSIX version.
		 */
		add_assoc_long(&iface, "mtu", p->Mtu);

		array_init(&unicast);
		for (u = p->FirstUnicastAddress; u; u = u->Next) {
			switch (u->Address.lpSockaddr->sa_family) {
				case AF_INET: {
					ULONG mask;
					struct sockaddr_in sin_mask;

					ConvertLengthToIpv4Mask(u->OnLinkPrefixLength, &mask);
					sin_mask.sin_family = AF_INET;
					sin_mask.sin_addr.s_addr = mask;

					iface_append_unicast(&unicast, p->Flags,
					                     (struct sockaddr*)u->Address.lpSockaddr,
					                     (struct sockaddr*)&sin_mask, NULL, NULL);
					break;
				}
				case AF_INET6: {
					ULONG i, j;
					struct sockaddr_in6 sin6_mask;

					memset(&sin6_mask, 0, sizeof(sin6_mask));
					sin6_mask.sin6_family = AF_INET6;
					for (i = u->OnLinkPrefixLength, j = 0; i > 0; i -= 8, ++j) {
						sin6_mask.sin6_addr.s6_addr[j] = (i >= 8) ? 0xff : ((ULONG)((0xffU << (8 - i)) & 0xffU));
					}

					iface_append_unicast(&unicast, p->Flags,
					                     (struct sockaddr*)u->Address.lpSockaddr,
										 (struct sockaddr*)&sin6_mask, NULL, NULL);
					break;
				}
			}
		}
		add_assoc_zval(&iface, "unicast", &unicast);

		add_assoc_zval(return_value, p->AdapterName, &iface);
	}

	FREE(pAddresses);
#undef MALLOC
#undef FREE
#elif HAVE_GETIFADDRS /* !PHP_WIN32 */
	struct ifaddrs *addrs = NULL, *p;
	zend_long filter = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(filter)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);


	if (getifaddrs(&addrs)) {
		php_error(E_WARNING, "getifaddrs() failed %d: %s", errno, strerror(errno));
		RETURN_FALSE;
	}

	array_init(return_value);
	for (p = addrs; p; p = p->ifa_next) {
		if ((p->ifa_flags & filter) != filter) {
			continue;
		}
		zval *iface = zend_hash_str_find(Z_ARR_P(return_value), p->ifa_name, strlen(p->ifa_name));
		zval *unicast;

		if (!iface) {
			zval newif;
			array_init(&newif);
			iface = zend_hash_str_add(Z_ARR_P(return_value), p->ifa_name, strlen(p->ifa_name), &newif);
		}

		unicast = zend_hash_str_find(Z_ARR_P(iface), "unicast", sizeof("unicast") - 1);
		if (!unicast) {
			zval newuni;
			array_init(&newuni);
			unicast = zend_hash_str_add(Z_ARR_P(iface), "unicast", sizeof("unicast") - 1, &newuni);
		}

		iface_append_unicast(unicast,
		                     p->ifa_flags,
		                     p->ifa_addr, p->ifa_netmask,
		                     (p->ifa_flags & IFF_BROADCAST) ? p->ifa_broadaddr : NULL,
					         (p->ifa_flags & IFF_POINTOPOINT) ? p->ifa_dstaddr : NULL);
	}

	freeifaddrs(addrs);
#else
	/* Should never happen as we never register the function */
	php_error(E_WARNING, "No support for net_get_interfaces");
	RETURN_FALSE;
#endif
}
/* }}} */

const zend_function_entry basic_net_functions[] = { /* {{{ */
	PHP_FE(net_get_interfaces,												arginfo_net_get_interfaces)
	PHP_FE_END
};
/* }}} */

PHP_MINIT_FUNCTION(standard_net)
{
#if defined(PHP_WIN32) || HAVE_GETIFADDRS
#ifdef IFF_UP
	REGISTER_LONG_CONSTANT("IFF_UP",          IFF_UP,          CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_BROADCAST
	REGISTER_LONG_CONSTANT("IFF_BROADCAST",   IFF_BROADCAST,   CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_DEBUG
	REGISTER_LONG_CONSTANT("IFF_DEBUG",       IFF_DEBUG,       CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_LOOPBACK
	REGISTER_LONG_CONSTANT("IFF_LOOPBACK",    IFF_LOOPBACK,    CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_POINTOPOINT
	REGISTER_LONG_CONSTANT("IFF_POINTOPOINT", IFF_POINTOPOINT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_NOTRAILERS
	REGISTER_LONG_CONSTANT("IFF_NOTRAILERS",  IFF_NOTRAILERS,  CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_RUNNING
	REGISTER_LONG_CONSTANT("IFF_RUNNING",     IFF_RUNNING,     CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_NOARP
	REGISTER_LONG_CONSTANT("IFF_NOARP",       IFF_NOARP,       CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_PROMISC
	REGISTER_LONG_CONSTANT("IFF_PROMISC",     IFF_PROMISC,     CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_ALLMULTI
	REGISTER_LONG_CONSTANT("IFF_ALLMULTI",    IFF_ALLMULTI,    CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_MASTER
	REGISTER_LONG_CONSTANT("IFF_MASTER",      IFF_MASTER,      CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_SLAVE
	REGISTER_LONG_CONSTANT("IFF_SLAVE",       IFF_SLAVE,       CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_MULTICAST
	REGISTER_LONG_CONSTANT("IFF_MULTICAST",   IFF_MULTICAST,   CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_PORTSEL
	REGISTER_LONG_CONSTANT("IFF_PORTSEL",     IFF_PORTSEL,     CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_AUTOMEDIA
	REGISTER_LONG_CONSTANT("IFF_AUTOMEDIA",   IFF_AUTOMEDIA,   CONST_CS | CONST_PERSISTENT);
#endif
#ifdef IFF_DYNAMIC
	REGISTER_LONG_CONSTANT("IFF_DYNAMIC",     IFF_DYNAMIC,     CONST_CS | CONST_PERSISTENT);
#endif
/* IFF_LOWER_UP, IFF_DORMANT, IFF_ECHO are not in net/if.h but in linux/if.h */

    zend_register_functions(NULL, basic_net_functions, NULL, MODULE_PERSISTENT);
#endif
}
