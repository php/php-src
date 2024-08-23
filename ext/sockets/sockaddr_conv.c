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

bool php_set_common_addr(struct sockaddr *sin, int family, char *string, php_socket *php_sock) /* {{{ */
{
#ifdef HAVE_GETADDRINFO
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)sin;
	struct sockaddr_in *sin4 = (struct sockaddr_in*)sin;
	struct in6_addr tmp6;
	struct in_addr tmp4;

	struct addrinfo hints;
	struct addrinfo *addrinfo = NULL;

	if (family == AF_INET6 && inet_pton(AF_INET6, string, &tmp6)) {
		memcpy(&(sin6->sin6_addr.s6_addr), &(tmp6.s6_addr), sizeof(struct in6_addr));
	} else if (family == AF_INET && inet_pton(AF_INET, string, &tmp4)) {
		sin4->sin_addr.s_addr = tmp4.s_addr;
	} else {
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = family;
#ifdef AI_V4MAPPED
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
			return false;
		}
		if (addrinfo->ai_family != family) {
			php_error_docref(NULL, E_WARNING, "Host lookup failed: Wrong address family returned for socket");
			freeaddrinfo(addrinfo);
			return false;
		}

		if (addrinfo->ai_family == AF_INET6) {
			memcpy(&(sin6->sin6_addr.s6_addr), ((struct sockaddr_in6*)(addrinfo->ai_addr))->sin6_addr.s6_addr, sizeof(struct in6_addr));
		} else if (addrinfo->ai_family == AF_INET) {
			memcpy(&(sin4->sin_addr.s_addr), &((struct sockaddr_in*)(addrinfo->ai_addr))->sin_addr.s_addr, sizeof(struct in_addr));
		}
		freeaddrinfo(addrinfo);
	}

	return true;
#else
	php_error_docref(NULL, E_WARNING, "Host lookup failed: getaddrinfo() not available on this system");
	return true;
#endif
}
/* }}} */

#ifdef HAVE_IPV6
/* Sets addr by hostname, or by ip in string form (AF_INET6) */
bool php_set_inet6_addr(struct sockaddr_in6 *sin6, char *string, php_socket *php_sock) /* {{{ */
{
	char *scope = strchr(string, '%');

	bool ret = php_set_common_addr((struct sockaddr*)sin6, AF_INET6, string, php_sock);
	if (!ret) {
		return false;
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
bool php_set_inet_addr(struct sockaddr_in *sin, char *string, php_socket *php_sock) /* {{{ */
{
	return php_set_common_addr((struct sockaddr*)sin, AF_INET, string, php_sock);
}
/* }}} */

/* Sets addr by hostname or by ip in string form (AF_INET or AF_INET6,
 * depending on the socket) */
bool php_set_inet46_addr(php_sockaddr_storage *ss, socklen_t *ss_len, char *string, php_socket *php_sock) /* {{{ */
{
	if (php_sock->type == AF_INET) {
		struct sockaddr_in t = {0};
		if (php_set_inet_addr(&t, string, php_sock)) {
			memcpy(ss, &t, sizeof t);
			ss->ss_family = AF_INET;
			*ss_len = sizeof(t);
			return true;
		}
	}
#ifdef HAVE_IPV6
	else if (php_sock->type == AF_INET6) {
		struct sockaddr_in6 t = {0};
		if (php_set_inet6_addr(&t, string, php_sock)) {
			memcpy(ss, &t, sizeof t);
			ss->ss_family = AF_INET6;
			*ss_len = sizeof(t);
			return true;
		}
	}
#endif
	else {
		php_error_docref(NULL, E_WARNING,
			"IP address used in the context of an unexpected type of socket");
	}
	return false;
}
