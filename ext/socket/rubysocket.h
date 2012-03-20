#ifndef RUBY_SOCKET_H
#define RUBY_SOCKET_H 1

#include "ruby/ruby.h"
#include "ruby/io.h"
#include "ruby/util.h"
#include "internal.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#ifdef HAVE_XTI_H
#include <xti.h>
#endif

#ifndef _WIN32
#if defined(__BEOS__) && !defined(__HAIKU__) && !defined(BONE)
# include <net/socket.h>
#else
# include <sys/socket.h>
#endif
#include <netinet/in.h>
#ifdef HAVE_NETINET_IN_SYSTM_H
# include <netinet/in_systm.h>
#endif
#ifdef HAVE_NETINET_TCP_H
# include <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
# include <netinet/udp.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#include <netdb.h>
#endif
#include <errno.h>
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif

#if defined(HAVE_FCNTL)
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#endif

#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_UCRED_H
#include <sys/ucred.h>
#endif
#ifdef HAVE_UCRED_H
#include <ucred.h>
#endif

#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif

/*
 * workaround for NetBSD, OpenBSD and etc.
 * The problem is since 4.4BSD-Lite.
 * FreeBSD fix the problem at FreeBSD 2.2.0.
 * NetBSD fix the problem at NetBSD 3.0 by kern/29624.
 * OpenBSD fix the problem at OpenBSD 3.8.
 */
#define pseudo_AF_FTIP pseudo_AF_RTIP

#ifndef HAVE_GETADDRINFO
#include "addrinfo.h"
#endif
#include "sockport.h"

#ifndef NI_MAXHOST
# define NI_MAXHOST 1025
#endif
#ifndef NI_MAXSERV
# define NI_MAXSERV 32
#endif

#ifdef AF_INET6
# define IS_IP_FAMILY(af) ((af) == AF_INET || (af) == AF_INET6)
#else
# define IS_IP_FAMILY(af) ((af) == AF_INET)
#endif

#ifndef HAVE_SOCKADDR_STORAGE
/*
 * RFC 2553: protocol-independent placeholder for socket addresses
 */
#define _SS_MAXSIZE     128
#define _SS_ALIGNSIZE   (sizeof(double))
#define _SS_PAD1SIZE    (_SS_ALIGNSIZE - sizeof(unsigned char) * 2)
#define _SS_PAD2SIZE    (_SS_MAXSIZE - sizeof(unsigned char) * 2 - \
                                _SS_PAD1SIZE - _SS_ALIGNSIZE)

struct sockaddr_storage {
#ifdef HAVE_SA_LEN
        unsigned char ss_len;           /* address length */
        unsigned char ss_family;        /* address family */
#else
        unsigned short ss_family;
#endif
        char    __ss_pad1[_SS_PAD1SIZE];
        double  __ss_align;     /* force desired structure storage alignment */
        char    __ss_pad2[_SS_PAD2SIZE];
};
#endif

#if defined __APPLE__ && defined __MACH__
/*
 * CMSG_ macros are broken on 64bit darwin, because __DARWIN_ALIGN
 * aligns up to __darwin_size_t which is 64bit, but CMSG_DATA is
 * 32bit-aligned.
 */
#undef __DARWIN_ALIGNBYTES
#define __DARWIN_ALIGNBYTES (sizeof(unsigned int) - 1)
#endif

#if defined(_AIX)
#ifndef CMSG_SPACE
# define CMSG_SPACE(len) (_CMSG_ALIGN(sizeof(struct cmsghdr)) + _CMSG_ALIGN(len))
#endif
#ifndef CMSG_LEN
# define CMSG_LEN(len) (_CMSG_ALIGN(sizeof(struct cmsghdr)) + (len))
#endif
#endif

#ifdef __BEOS__
#undef close
#define close closesocket
#endif

#define INET_CLIENT 0
#define INET_SERVER 1
#define INET_SOCKS  2

extern int rsock_do_not_reverse_lookup;
#define FMODE_NOREVLOOKUP 0x100

extern VALUE rb_cBasicSocket;
extern VALUE rb_cIPSocket;
extern VALUE rb_cTCPSocket;
extern VALUE rb_cTCPServer;
extern VALUE rb_cUDPSocket;
#ifdef HAVE_SYS_UN_H
extern VALUE rb_cUNIXSocket;
extern VALUE rb_cUNIXServer;
#endif
extern VALUE rb_cSocket;
extern VALUE rb_cAddrinfo;
extern VALUE rb_cSockOpt;

extern VALUE rb_eSocket;

#ifdef SOCKS
extern VALUE rb_cSOCKSSocket;
#ifdef SOCKS5
#include <socks.h>
#else
void SOCKSinit();
int Rconnect();
#endif
#endif

#include "constdefs.h"

#define BLOCKING_REGION(func, arg) (long)rb_thread_blocking_region((func), (arg), RUBY_UBF_IO, 0)
#define BLOCKING_REGION_FD(func, arg) (long)rb_thread_io_blocking_region((func), (arg), (arg)->fd)

#define SockAddrStringValue(v) rsock_sockaddr_string_value(&(v))
#define SockAddrStringValuePtr(v) rsock_sockaddr_string_value_ptr(&(v))
VALUE rsock_sockaddr_string_value(volatile VALUE *);
char *rsock_sockaddr_string_value_ptr(volatile VALUE *);
VALUE rb_check_sockaddr_string_type(VALUE);

NORETURN(void rsock_raise_socket_error(const char *, int));

int rsock_family_arg(VALUE domain);
int rsock_socktype_arg(VALUE type);
int rsock_level_arg(int family, VALUE level);
int rsock_optname_arg(int family, int level, VALUE optname);
int rsock_cmsg_type_arg(int family, int level, VALUE type);
int rsock_shutdown_how_arg(VALUE how);

int rsock_getfamily(int sockfd);

int rb_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
int rb_getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, size_t hostlen, char *serv, size_t servlen, int flags);
struct addrinfo *rsock_addrinfo(VALUE host, VALUE port, int socktype, int flags);
struct addrinfo *rsock_getaddrinfo(VALUE host, VALUE port, struct addrinfo *hints, int socktype_hack);
VALUE rsock_fd_socket_addrinfo(int fd, struct sockaddr *addr, socklen_t len);
VALUE rsock_io_socket_addrinfo(VALUE io, struct sockaddr *addr, socklen_t len);

VALUE rsock_addrinfo_new(struct sockaddr *addr, socklen_t len, int family, int socktype, int protocol, VALUE canonname, VALUE inspectname);

VALUE rsock_make_ipaddr(struct sockaddr *addr);
VALUE rsock_ipaddr(struct sockaddr *sockaddr, int norevlookup);
VALUE rsock_make_hostent(VALUE host, struct addrinfo *addr, VALUE (*ipaddr)(struct sockaddr *, size_t));

int rsock_revlookup_flag(VALUE revlookup, int *norevlookup);

#ifdef HAVE_SYS_UN_H
const char* rsock_unixpath(struct sockaddr_un *sockaddr, socklen_t len);
VALUE rsock_unixaddr(struct sockaddr_un *sockaddr, socklen_t len);
#endif

int rsock_socket(int domain, int type, int proto);
VALUE rsock_init_sock(VALUE sock, int fd);
VALUE rsock_sock_s_socketpair(int argc, VALUE *argv, VALUE klass);
VALUE rsock_init_inetsock(VALUE sock, VALUE remote_host, VALUE remote_serv, VALUE local_host, VALUE local_serv, int type);
VALUE rsock_init_unixsock(VALUE sock, VALUE path, int server);

struct rsock_send_arg {
    int fd, flags;
    VALUE mesg;
    struct sockaddr *to;
    socklen_t tolen;
};

VALUE rsock_sendto_blocking(void *data);
VALUE rsock_send_blocking(void *data);
VALUE rsock_bsock_send(int argc, VALUE *argv, VALUE sock);

enum sock_recv_type {
    RECV_RECV,                  /* BasicSocket#recv(no from) */
    RECV_IP,                    /* IPSocket#recvfrom */
    RECV_UNIX,                  /* UNIXSocket#recvfrom */
    RECV_SOCKET                 /* Socket#recvfrom */
};

VALUE rsock_s_recvfrom_nonblock(VALUE sock, int argc, VALUE *argv, enum sock_recv_type from);
VALUE rsock_s_recvfrom(VALUE sock, int argc, VALUE *argv, enum sock_recv_type from);

int rsock_connect(int fd, const struct sockaddr *sockaddr, int len, int socks);

VALUE rsock_s_accept(VALUE klass, int fd, struct sockaddr *sockaddr, socklen_t *len);
VALUE rsock_s_accept_nonblock(VALUE klass, rb_io_t *fptr, struct sockaddr *sockaddr, socklen_t *len);
VALUE rsock_sock_listen(VALUE sock, VALUE log);

VALUE rsock_sockopt_new(int family, int level, int optname, VALUE data);

#if defined(HAVE_SENDMSG)
VALUE rsock_bsock_sendmsg(int argc, VALUE *argv, VALUE sock);
VALUE rsock_bsock_sendmsg_nonblock(int argc, VALUE *argv, VALUE sock);
#else
#define rsock_bsock_sendmsg rb_f_notimplement
#define rsock_bsock_sendmsg_nonblock rb_f_notimplement
#endif
#if defined(HAVE_RECVMSG)
VALUE rsock_bsock_recvmsg(int argc, VALUE *argv, VALUE sock);
VALUE rsock_bsock_recvmsg_nonblock(int argc, VALUE *argv, VALUE sock);
ssize_t rsock_recvmsg(int socket, struct msghdr *message, int flags);
#else
#define rsock_bsock_recvmsg rb_f_notimplement
#define rsock_bsock_recvmsg_nonblock rb_f_notimplement
#endif

#ifdef HAVE_ST_MSG_CONTROL
void rsock_discard_cmsg_resource(struct msghdr *mh, int msg_peek_p);
#endif

void rsock_init_basicsocket(void);
void rsock_init_ipsocket(void);
void rsock_init_tcpsocket(void);
void rsock_init_tcpserver(void);
void rsock_init_sockssocket(void);
void rsock_init_udpsocket(void);
void rsock_init_unixsocket(void);
void rsock_init_unixserver(void);
void rsock_init_socket_constants(void);
void rsock_init_ancdata(void);
void rsock_init_addrinfo(void);
void rsock_init_sockopt(void);
void rsock_init_socket_init(void);

#endif
