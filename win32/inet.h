#if _MSC_VER >= 1500
# include <In6addr.h>
#endif
#include <Ws2tcpip.h>

#if (_WIN32_WINNT <= 0x500)
PHPAPI int inet_pton(int af, const char* src, void* dst);
PHPAPI const char* inet_ntop(int af, const void* src, char* dst, size_t size);
PHPAPI int inet_aton(const char *cp, struct in_addr *inp);
#endif
