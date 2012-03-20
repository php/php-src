/************************************************

  init.c -

  created at: Thu Mar 31 12:21:29 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

************************************************/

#include "rubysocket.h"

VALUE rb_cBasicSocket;
VALUE rb_cIPSocket;
VALUE rb_cTCPSocket;
VALUE rb_cTCPServer;
VALUE rb_cUDPSocket;
#ifdef AF_UNIX
VALUE rb_cUNIXSocket;
VALUE rb_cUNIXServer;
#endif
VALUE rb_cSocket;
VALUE rb_cAddrinfo;

VALUE rb_eSocket;

#ifdef SOCKS
VALUE rb_cSOCKSSocket;
#endif

int rsock_do_not_reverse_lookup = 1;

void
rsock_raise_socket_error(const char *reason, int error)
{
#ifdef EAI_SYSTEM
    if (error == EAI_SYSTEM) rb_sys_fail(reason);
#endif
    rb_raise(rb_eSocket, "%s: %s", reason, gai_strerror(error));
}

VALUE
rsock_init_sock(VALUE sock, int fd)
{
    rb_io_t *fp;
#ifndef _WIN32
    struct stat sbuf;

    if (fstat(fd, &sbuf) < 0)
        rb_sys_fail(0);
    rb_update_max_fd(fd);
    if (!S_ISSOCK(sbuf.st_mode))
        rb_raise(rb_eArgError, "not a socket file descriptor");
#else
    if (!rb_w32_is_socket(fd))
        rb_raise(rb_eArgError, "not a socket file descriptor");
#endif

    MakeOpenFile(sock, fp);
    fp->fd = fd;
    fp->mode = FMODE_READWRITE|FMODE_DUPLEX;
    rb_io_ascii8bit_binmode(sock);
    if (rsock_do_not_reverse_lookup) {
	fp->mode |= FMODE_NOREVLOOKUP;
    }
    rb_io_synchronized(fp);

    return sock;
}

VALUE
rsock_sendto_blocking(void *data)
{
    struct rsock_send_arg *arg = data;
    VALUE mesg = arg->mesg;
    return (VALUE)sendto(arg->fd, RSTRING_PTR(mesg), RSTRING_LEN(mesg),
                         arg->flags, arg->to, arg->tolen);
}

VALUE
rsock_send_blocking(void *data)
{
    struct rsock_send_arg *arg = data;
    VALUE mesg = arg->mesg;
    return (VALUE)send(arg->fd, RSTRING_PTR(mesg), RSTRING_LEN(mesg),
                       arg->flags);
}

struct recvfrom_arg {
    int fd, flags;
    VALUE str;
    socklen_t alen;
    struct sockaddr_storage buf;
};

static VALUE
recvfrom_blocking(void *data)
{
    struct recvfrom_arg *arg = data;
    return (VALUE)recvfrom(arg->fd, RSTRING_PTR(arg->str), RSTRING_LEN(arg->str),
			   arg->flags, (struct sockaddr*)&arg->buf, &arg->alen);
}

VALUE
rsock_s_recvfrom(VALUE sock, int argc, VALUE *argv, enum sock_recv_type from)
{
    rb_io_t *fptr;
    VALUE str, klass;
    struct recvfrom_arg arg;
    VALUE len, flg;
    long buflen;
    long slen;

    rb_scan_args(argc, argv, "11", &len, &flg);

    if (flg == Qnil) arg.flags = 0;
    else             arg.flags = NUM2INT(flg);
    buflen = NUM2INT(len);

    GetOpenFile(sock, fptr);
    if (rb_io_read_pending(fptr)) {
	rb_raise(rb_eIOError, "recv for buffered IO");
    }
    arg.fd = fptr->fd;
    arg.alen = (socklen_t)sizeof(arg.buf);

    arg.str = str = rb_tainted_str_new(0, buflen);
    klass = RBASIC(str)->klass;
    RBASIC(str)->klass = 0;

    while (rb_io_check_closed(fptr),
	   rb_thread_wait_fd(arg.fd),
	   (slen = BLOCKING_REGION_FD(recvfrom_blocking, &arg)) < 0) {
        if (!rb_io_wait_readable(fptr->fd)) {
            rb_sys_fail("recvfrom(2)");
        }
	if (RBASIC(str)->klass || RSTRING_LEN(str) != buflen) {
	    rb_raise(rb_eRuntimeError, "buffer string modified");
	}
    }

    RBASIC(str)->klass = klass;
    if (slen < RSTRING_LEN(str)) {
	rb_str_set_len(str, slen);
    }
    rb_obj_taint(str);
    switch (from) {
      case RECV_RECV:
	return str;
      case RECV_IP:
#if 0
	if (arg.alen != sizeof(struct sockaddr_in)) {
	    rb_raise(rb_eTypeError, "sockaddr size differs - should not happen");
	}
#endif
	if (arg.alen && arg.alen != sizeof(arg.buf)) /* OSX doesn't return a from result for connection-oriented sockets */
	    return rb_assoc_new(str, rsock_ipaddr((struct sockaddr*)&arg.buf, fptr->mode & FMODE_NOREVLOOKUP));
	else
	    return rb_assoc_new(str, Qnil);

#ifdef HAVE_SYS_UN_H
      case RECV_UNIX:
        return rb_assoc_new(str, rsock_unixaddr((struct sockaddr_un*)&arg.buf, arg.alen));
#endif
      case RECV_SOCKET:
	return rb_assoc_new(str, rsock_io_socket_addrinfo(sock, (struct sockaddr*)&arg.buf, arg.alen));
      default:
	rb_bug("rsock_s_recvfrom called with bad value");
    }
}

VALUE
rsock_s_recvfrom_nonblock(VALUE sock, int argc, VALUE *argv, enum sock_recv_type from)
{
    rb_io_t *fptr;
    VALUE str;
    struct sockaddr_storage buf;
    socklen_t alen = (socklen_t)sizeof buf;
    VALUE len, flg;
    long buflen;
    long slen;
    int fd, flags;
    VALUE addr = Qnil;

    rb_scan_args(argc, argv, "11", &len, &flg);

    if (flg == Qnil) flags = 0;
    else             flags = NUM2INT(flg);
    buflen = NUM2INT(len);

#ifdef MSG_DONTWAIT
    /* MSG_DONTWAIT avoids the race condition between fcntl and recvfrom.
       It is not portable, though. */
    flags |= MSG_DONTWAIT;
#endif

    GetOpenFile(sock, fptr);
    if (rb_io_read_pending(fptr)) {
	rb_raise(rb_eIOError, "recvfrom for buffered IO");
    }
    fd = fptr->fd;

    str = rb_tainted_str_new(0, buflen);

    rb_io_check_closed(fptr);
    rb_io_set_nonblock(fptr);
    slen = recvfrom(fd, RSTRING_PTR(str), buflen, flags, (struct sockaddr*)&buf, &alen);

    if (slen < 0) {
	switch (errno) {
	  case EAGAIN:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
	  case EWOULDBLOCK:
#endif
            rb_mod_sys_fail(rb_mWaitReadable, "recvfrom(2) would block");
	}
	rb_sys_fail("recvfrom(2)");
    }
    if (slen < RSTRING_LEN(str)) {
	rb_str_set_len(str, slen);
    }
    rb_obj_taint(str);
    switch (from) {
      case RECV_RECV:
        return str;

      case RECV_IP:
        if (alen && alen != sizeof(buf)) /* connection-oriented socket may not return a from result */
            addr = rsock_ipaddr((struct sockaddr*)&buf, fptr->mode & FMODE_NOREVLOOKUP);
        break;

      case RECV_SOCKET:
        addr = rsock_io_socket_addrinfo(sock, (struct sockaddr*)&buf, alen);
        break;

      default:
        rb_bug("rsock_s_recvfrom_nonblock called with bad value");
    }
    return rb_assoc_new(str, addr);
}

static int
rsock_socket0(int domain, int type, int proto)
{
    int ret;

#ifdef SOCK_CLOEXEC
    static int try_sock_cloexec = 1;
    if (try_sock_cloexec) {
        ret = socket(domain, type|SOCK_CLOEXEC, proto);
        if (ret == -1 && errno == EINVAL) {
            /* SOCK_CLOEXEC is available since Linux 2.6.27.  Linux 2.6.18 fails with EINVAL */
            ret = socket(domain, type, proto);
            if (ret != -1) {
                try_sock_cloexec = 0;
            }
        }
    }
    else {
        ret = socket(domain, type, proto);
    }
#else
    ret = socket(domain, type, proto);
#endif
    if (ret == -1)
        return -1;

    rb_fd_fix_cloexec(ret);

    return ret;

}

int
rsock_socket(int domain, int type, int proto)
{
    int fd;

    fd = rsock_socket0(domain, type, proto);
    if (fd < 0) {
       if (errno == EMFILE || errno == ENFILE) {
           rb_gc();
           fd = rsock_socket0(domain, type, proto);
       }
    }
    if (0 <= fd)
        rb_update_max_fd(fd);
    return fd;
}

static int
wait_connectable(int fd)
{
    int sockerr;
    socklen_t sockerrlen;
    int revents;
    int ret;

    for (;;) {
	/*
	 * Stevens book says, succuessful finish turn on RB_WAITFD_OUT and
	 * failure finish turn on both RB_WAITFD_IN and RB_WAITFD_OUT.
	 */
	revents = rb_wait_for_single_fd(fd, RB_WAITFD_IN|RB_WAITFD_OUT, NULL);

	if (revents & (RB_WAITFD_IN|RB_WAITFD_OUT)) {
	    sockerrlen = (socklen_t)sizeof(sockerr);
	    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&sockerr, &sockerrlen);

	    /*
	     * Solaris getsockopt(SO_ERROR) return -1 and set errno
	     * in getsockopt(). Let's return immediately.
	     */
	    if (ret < 0)
		break;
	    if (sockerr == 0)
		continue;	/* workaround for winsock */

	    /* BSD and Linux use sockerr. */
	    errno = sockerr;
	    ret = -1;
	    break;
	}

	if ((revents & (RB_WAITFD_IN|RB_WAITFD_OUT)) == RB_WAITFD_OUT) {
	    ret = 0;
	    break;
	}
    }

    return ret;
}

#ifdef __CYGWIN__
#define WAIT_IN_PROGRESS 10
#endif
#ifdef __APPLE__
#define WAIT_IN_PROGRESS 10
#endif
#ifdef __linux__
/* returns correct error */
#define WAIT_IN_PROGRESS 0
#endif
#ifndef WAIT_IN_PROGRESS
/* BSD origin code apparently has a problem */
#define WAIT_IN_PROGRESS 1
#endif

struct connect_arg {
    int fd;
    const struct sockaddr *sockaddr;
    socklen_t len;
};

static VALUE
connect_blocking(void *data)
{
    struct connect_arg *arg = data;
    return (VALUE)connect(arg->fd, arg->sockaddr, arg->len);
}

#if defined(SOCKS) && !defined(SOCKS5)
static VALUE
socks_connect_blocking(void *data)
{
    struct connect_arg *arg = data;
    return (VALUE)Rconnect(arg->fd, arg->sockaddr, arg->len);
}
#endif

int
rsock_connect(int fd, const struct sockaddr *sockaddr, int len, int socks)
{
    int status;
    rb_blocking_function_t *func = connect_blocking;
    struct connect_arg arg;
#if WAIT_IN_PROGRESS > 0
    int wait_in_progress = -1;
    int sockerr;
    socklen_t sockerrlen;
#endif

    arg.fd = fd;
    arg.sockaddr = sockaddr;
    arg.len = len;
#if defined(SOCKS) && !defined(SOCKS5)
    if (socks) func = socks_connect_blocking;
#endif
    for (;;) {
	status = (int)BLOCKING_REGION_FD(func, &arg);
	if (status < 0) {
	    switch (errno) {
	      case EINTR:
#if defined(ERESTART)
	      case ERESTART:
#endif
		continue;

	      case EAGAIN:
#ifdef EINPROGRESS
	      case EINPROGRESS:
#endif
#if WAIT_IN_PROGRESS > 0
		sockerrlen = (socklen_t)sizeof(sockerr);
		status = getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&sockerr, &sockerrlen);
		if (status) break;
		if (sockerr) {
		    status = -1;
		    errno = sockerr;
		    break;
		}
#endif
#ifdef EALREADY
	      case EALREADY:
#endif
#if WAIT_IN_PROGRESS > 0
		wait_in_progress = WAIT_IN_PROGRESS;
#endif
		status = wait_connectable(fd);
		if (status) {
		    break;
		}
		errno = 0;
		continue;

#if WAIT_IN_PROGRESS > 0
	      case EINVAL:
		if (wait_in_progress-- > 0) {
		    /*
		     * connect() after EINPROGRESS returns EINVAL on
		     * some platforms, need to check true error
		     * status.
		     */
		    sockerrlen = (socklen_t)sizeof(sockerr);
		    status = getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&sockerr, &sockerrlen);
		    if (!status && !sockerr) {
			struct timeval tv = {0, 100000};
			rb_thread_wait_for(tv);
			continue;
		    }
		    status = -1;
		    errno = sockerr;
		}
		break;
#endif

#ifdef EISCONN
	      case EISCONN:
		status = 0;
		errno = 0;
		break;
#endif
	      default:
		break;
	    }
	}
	return status;
    }
}

static void
make_fd_nonblock(int fd)
{
    int flags;
#ifdef F_GETFL
    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        rb_sys_fail(0);
    }
#else
    flags = 0;
#endif
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        rb_sys_fail(0);
    }
}

static int
cloexec_accept(int socket, struct sockaddr *address, socklen_t *address_len)
{
    int ret;
#ifdef HAVE_ACCEPT4
    static int try_accept4 = 1;
    if (try_accept4) {
        ret = accept4(socket, address, address_len, SOCK_CLOEXEC);
        /* accept4 is available since Linux 2.6.28, glibc 2.10. */
        if (ret != -1) {
            if (ret <= 2)
                rb_maygvl_fd_fix_cloexec(ret);
            return ret;
        }
        if (errno == ENOSYS) {
            try_accept4 = 0;
            ret = accept(socket, address, address_len);
        }
    }
    else {
        ret = accept(socket, address, address_len);
    }
#else
    ret = accept(socket, address, address_len);
#endif
    if (ret == -1) return -1;
    rb_maygvl_fd_fix_cloexec(ret);
    return ret;
}


VALUE
rsock_s_accept_nonblock(VALUE klass, rb_io_t *fptr, struct sockaddr *sockaddr, socklen_t *len)
{
    int fd2;

    rb_secure(3);
    rb_io_set_nonblock(fptr);
    fd2 = cloexec_accept(fptr->fd, (struct sockaddr*)sockaddr, len);
    if (fd2 < 0) {
	switch (errno) {
	  case EAGAIN:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
	  case EWOULDBLOCK:
#endif
	  case ECONNABORTED:
#if defined EPROTO
	  case EPROTO:
#endif
            rb_mod_sys_fail(rb_mWaitReadable, "accept(2) would block");
	}
        rb_sys_fail("accept(2)");
    }
    rb_update_max_fd(fd2);
    make_fd_nonblock(fd2);
    return rsock_init_sock(rb_obj_alloc(klass), fd2);
}

struct accept_arg {
    int fd;
    struct sockaddr *sockaddr;
    socklen_t *len;
};

static VALUE
accept_blocking(void *data)
{
    struct accept_arg *arg = data;
    return (VALUE)cloexec_accept(arg->fd, arg->sockaddr, arg->len);
}

VALUE
rsock_s_accept(VALUE klass, int fd, struct sockaddr *sockaddr, socklen_t *len)
{
    int fd2;
    int retry = 0;
    struct accept_arg arg;

    rb_secure(3);
    arg.fd = fd;
    arg.sockaddr = sockaddr;
    arg.len = len;
  retry:
    rb_thread_wait_fd(fd);
    fd2 = (int)BLOCKING_REGION_FD(accept_blocking, &arg);
    if (fd2 < 0) {
	switch (errno) {
	  case EMFILE:
	  case ENFILE:
	    if (retry) break;
	    rb_gc();
	    retry = 1;
	    goto retry;
	  default:
	    if (!rb_io_wait_readable(fd)) break;
	    retry = 0;
	    goto retry;
	}
	rb_sys_fail(0);
    }
    rb_update_max_fd(fd2);
    if (!klass) return INT2NUM(fd2);
    return rsock_init_sock(rb_obj_alloc(klass), fd2);
}

int
rsock_getfamily(int sockfd)
{
    struct sockaddr_storage ss;
    socklen_t sslen = (socklen_t)sizeof(ss);

    ss.ss_family = AF_UNSPEC;
    if (getsockname(sockfd, (struct sockaddr*)&ss, &sslen) < 0)
        return AF_UNSPEC;

    return ss.ss_family;
}

void
rsock_init_socket_init()
{
    /*
     * SocketError is the error class for socket.
     */
    rb_eSocket = rb_define_class("SocketError", rb_eStandardError);
    rsock_init_ipsocket();
    rsock_init_tcpsocket();
    rsock_init_tcpserver();
    rsock_init_sockssocket();
    rsock_init_udpsocket();
    rsock_init_unixsocket();
    rsock_init_unixserver();
    rsock_init_sockopt();
    rsock_init_ancdata();
    rsock_init_addrinfo();
    rsock_init_socket_constants();
}
