/* thread safe version of some common functions */

/* for thread safe my_inet_ntoa */
#if !defined(MSDOS) && !defined(__WIN32__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif 

void my_inet_ntoa(struct in_addr in, char *buf);
