#include "inet.h"

int inet_aton(const char *cp, struct in_addr *inp) {
  if ((inp->s_addr= inet_addr(cp)) == INADDR_NONE) {
          return 0;
  }

  return 1;
}
