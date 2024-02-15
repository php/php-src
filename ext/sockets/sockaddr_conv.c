#include <php.h>
#include <php_network.h>
#include "php_sockets.h"

#ifdef PHP_WIN32
#include "windows_common.h"
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

extern zend_result php_string_to_if_index(const char *val, unsigned *out);

#if HAVE_IPV6
/* Sets addr by hostname, or by ip in string form (AF_INET6) */
int php_set_inet6_addr(struct sockaddr_in6 *sin6, char *string, php_socket *php_sock) /* {{{ */
{
	struct in6_addr tmp;
#if HAVE_GETADDRINFO
	struct addrinfo hints;
	struct addrinfo *addrinfo = NULL;
#endif
	char *scope = strchr(string, '%');

	if (inet_pton(AF_INET6, string, &tmp)) {
		memcpy(&(sin6->sin6_addr.s6_addr), &(tmp.s6_addr), sizeof(struct in6_addr));
	} else {
#if HAVE_GETADDRINFO

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET6;
#if HAVE_AI_V4MAPPED
		hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
#else
		hints.ai_flags = AI_ADDRCONFIG;
#endif
		getaddrinfo(string, NULL, &hints, &addrinfo);
		if (!addrinfo) {
#ifdef PHP_WIN32
			PHP_SOCKET_ERROR(php_sock, "Host lookup failed", WSAGetLastError());
#else
			PHP_SOCKET_ERROR(php_sock, "Host lookup failed", (-10000 - h_errno));
#endif
			return 0;
		}
		if (addrinfo->ai_family != PF_INET6 || addrinfo->ai_addrlen != sizeof(struct sockaddr_in6)) {
			php_error_docref(NULL, E_WARNING, "Host lookup failed: Non AF_INET6 domain returned on AF_INET6 socket");
			freeaddrinfo(addrinfo);
			return 0;
		}

		memcpy(&(sin6->sin6_addr.s6_addr), ((struct sockaddr_in6*)(addrinfo->ai_addr))->sin6_addr.s6_addr, sizeof(struct in6_addr));
		freeaddrinfo(addrinfo);

#else
		/* No IPv6 specific hostname resolution is available on this system? */
		php_error_docref(NULL, E_WARNING, "Host lookup failed: getaddrinfo() not available on this system");
		return 0;
#endif

	}

	if (scope) {
		zend_long lval = 0;
		double dval = 0;
		unsigned scope_id = 0;

		scope++;

		if (IS_LONG == is_numeric_string(scope, strlen(scope), &lval, &dval, 0)) {
			if (lval > 0 && (zend_ulong)lval <= UINT_MAX) {
				scope_id = lval;
			}
		} else {
			php_string_to_if_index(scope, &scope_id);
		}

		sin6->sin6_scope_id = scope_id;
	}

	return 1;
}
/* }}} */
#endif

/* Sets addr by hostname, or by ip in string form (AF_INET)  */
int php_set_inet_addr(struct sockaddr_in *sin, char *string, php_socket *php_sock) /* {{{ */
{
	struct in_addr tmp;
	struct hostent *host_entry;

	if (inet_pton(AF_INET, string, &tmp)) {
		sin->sin_addr.s_addr = tmp.s_addr;
	} else {
		if (strlen(string) > MAXFQDNLEN || ! (host_entry = php_network_gethostbyname(string))) {
			/* Note: < -10000 indicates a host lookup error */
#ifdef PHP_WIN32
			PHP_SOCKET_ERROR(php_sock, "Host lookup failed", WSAGetLastError());
#else
			PHP_SOCKET_ERROR(php_sock, "Host lookup failed", (-10000 - h_errno));
#endif
			return 0;
		}
		if (host_entry->h_addrtype != AF_INET) {
			php_error_docref(NULL, E_WARNING, "Host lookup failed: Non AF_INET domain returned on AF_INET socket");
			return 0;
		}
		memcpy(&(sin->sin_addr.s_addr), host_entry->h_addr_list[0], host_entry->h_length);
	}

	return 1;
}
/* }}} */

/* Sets addr by hostname or by ip in string form (AF_INET or AF_INET6,
 * depending on the socket) */
int php_set_inet46_addr(php_sockaddr_storage *ss, socklen_t *ss_len, char *string, php_socket *php_sock) /* {{{ */
{
	if (php_sock->type == AF_INET) {
		struct sockaddr_in t = {0};
		if (php_set_inet_addr(&t, string, php_sock)) {
			memcpy(ss, &t, sizeof t);
			ss->ss_family = AF_INET;
			*ss_len = sizeof(t);
			return 1;
		}
	}
#if HAVE_IPV6
	else if (php_sock->type == AF_INET6) {
		struct sockaddr_in6 t = {0};
		if (php_set_inet6_addr(&t, string, php_sock)) {
			memcpy(ss, &t, sizeof t);
			ss->ss_family = AF_INET6;
			*ss_len = sizeof(t);
			return 1;
		}
	}
#endif
	else {
		php_error_docref(NULL, E_WARNING,
			"IP address used in the context of an unexpected type of socket");
	}
	return 0;
}
