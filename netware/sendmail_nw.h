
#ifdef USE_WINSOCK
/*#include <ws2nlm.h>*/
#include <novsock2.h>
#else
#include <sys/socket.h>	/* For struct sockaddr, 'PF_INET' and 'AF_INET' */
#include <netinet\in.h>	/* For struct sockaddr_in */
#include <netdb.h>		/* For struct hostent */
/*#include <ws2name.h>*/
#endif	/* USE_WINSOCK */

typedef int SOCKET;	/* Borrowed from winsock\novsock2.h */
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr * LPSOCKADDR;
typedef struct hostent * LPHOSTENT;

#define INVALID_SOCKET  (SOCKET)(~0)	/* Borrowed from winsock\novsock2.h */
