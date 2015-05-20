#include "inet.h"

int inet_aton(const char *cp, struct in_addr *inp) {
  inp->s_addr = inet_addr(cp);

  if (inp->s_addr == INADDR_NONE) {
          return 0;
  }

  return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
