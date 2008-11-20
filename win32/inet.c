#include "config.w32.h"
#if (_WIN32_WINNT < 0x0600) /* Vista/2k8 have these functions */
#include "php.h"
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>

#include "inet.h"

PHPAPI int inet_pton(int af, const char* src, void* dst)
{
	int address_length;
	struct sockaddr_storage sa;
	struct sockaddr_in *sin = (struct sockaddr_in *)&sa;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&sa;

	switch (af) {
		case AF_INET:
			address_length = sizeof (struct sockaddr_in);
			break;

		case AF_INET6:
			address_length = sizeof (struct sockaddr_in6);
			break;

		default:
			return -1;
	}

	if (WSAStringToAddress ((LPTSTR) src, af, NULL, (LPSOCKADDR) &sa, &address_length) == 0) {
		switch (af) {
			case AF_INET:
				memcpy (dst, &sin->sin_addr, sizeof (struct in_addr));
				break;

			case AF_INET6:
				memcpy (dst, &sin6->sin6_addr, sizeof (struct in6_addr));
				break;
		}
		return 1;
	}

	return 0;
}

PHPAPI const char* inet_ntop(int af, const void* src, char* dst, size_t size)
{
	int address_length;
	DWORD string_length = size;
	struct sockaddr_storage sa;
	struct sockaddr_in *sin = (struct sockaddr_in *)&sa;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&sa;

	memset (&sa, 0, sizeof (sa));
	switch (af) {
		case AF_INET:
			address_length = sizeof (struct sockaddr_in);
			sin->sin_family = af;
			memcpy (&sin->sin_addr, src, sizeof (struct in_addr));
			break;

		case AF_INET6:
			address_length = sizeof (struct sockaddr_in6);
			sin6->sin6_family = af;
			memcpy (&sin6->sin6_addr, src, sizeof (struct in6_addr));
			break;

		default:
			return NULL;
	}

	if (WSAAddressToString ((LPSOCKADDR) &sa, address_length, NULL, dst, &string_length) == 0) {
		return dst;
	}

	return NULL;
}

int inet_aton(const char *cp, struct in_addr *inp) {
  inp->s_addr = inet_addr(cp);

  if (inp->s_addr == INADDR_NONE) {
          return 0;
  }

  return 1;
}
#endif
