/************************************************

  unixsocket.c -

  created at: Thu Mar 31 12:21:29 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

************************************************/

#include "rubysocket.h"

#ifdef HAVE_SYS_UN_H
struct unixsock_arg {
    struct sockaddr_un *sockaddr;
    int fd;
};

static VALUE
unixsock_connect_internal(VALUE a)
{
    struct unixsock_arg *arg = (struct unixsock_arg *)a;
    return (VALUE)rsock_connect(arg->fd, (struct sockaddr*)arg->sockaddr,
			        (socklen_t)sizeof(*arg->sockaddr), 0);
}

VALUE
rsock_init_unixsock(VALUE sock, VALUE path, int server)
{
    struct sockaddr_un sockaddr;
    int fd, status;
    rb_io_t *fptr;

    SafeStringValue(path);
    fd = rsock_socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
	rb_sys_fail("socket(2)");
    }

    MEMZERO(&sockaddr, struct sockaddr_un, 1);
    sockaddr.sun_family = AF_UNIX;
    if (sizeof(sockaddr.sun_path) <= (size_t)RSTRING_LEN(path)) {
        rb_raise(rb_eArgError, "too long unix socket path (max: %dbytes)",
            (int)sizeof(sockaddr.sun_path)-1);
    }
    memcpy(sockaddr.sun_path, RSTRING_PTR(path), RSTRING_LEN(path));

    if (server) {
        status = bind(fd, (struct sockaddr*)&sockaddr, (socklen_t)sizeof(sockaddr));
    }
    else {
	int prot;
	struct unixsock_arg arg;
	arg.sockaddr = &sockaddr;
	arg.fd = fd;
        status = (int)rb_protect(unixsock_connect_internal, (VALUE)&arg, &prot);
	if (prot) {
	    close(fd);
	    rb_jump_tag(prot);
	}
    }

    if (status < 0) {
	close(fd);
	rb_sys_fail_str(path);
    }

    if (server) {
	if (listen(fd, SOMAXCONN) < 0) {
	    close(fd);
	    rb_sys_fail("listen(2)");
	}
    }

    rsock_init_sock(sock, fd);
    if (server) {
	GetOpenFile(sock, fptr);
        fptr->pathv = rb_str_new_frozen(path);
    }

    return sock;
}

/*
 * call-seq:
 *   UNIXSocket.new(path) => unixsocket
 *
 * Creates a new UNIX client socket connected to _path_.
 *
 *   s = UNIXSocket.new("/tmp/sock")
 *   s.send "hello", 0
 *
 */
static VALUE
unix_init(VALUE sock, VALUE path)
{
    return rsock_init_unixsock(sock, path, 0);
}

/*
 * call-seq:
 *   unixsocket.path => path
 *
 * Returns the path of the local address of unixsocket.
 *
 *   s = UNIXServer.new("/tmp/sock")
 *   p s.path #=> "/tmp/sock"
 *
 */
static VALUE
unix_path(VALUE sock)
{
    rb_io_t *fptr;

    GetOpenFile(sock, fptr);
    if (NIL_P(fptr->pathv)) {
	struct sockaddr_un addr;
	socklen_t len = (socklen_t)sizeof(addr);
	if (getsockname(fptr->fd, (struct sockaddr*)&addr, &len) < 0)
	    rb_sys_fail(0);
	fptr->pathv = rb_obj_freeze(rb_str_new_cstr(rsock_unixpath(&addr, len)));
    }
    return rb_str_dup(fptr->pathv);
}

/*
 * call-seq:
 *   unixsocket.recvfrom(maxlen [, flags]) => [mesg, unixaddress]
 *
 * Receives a message via _unixsocket_.
 *
 * _maxlen_ is the maximum number of bytes to receive.
 *
 * _flags_ should be a bitwise OR of Socket::MSG_* constants.
 *
 *   s1 = Socket.new(:UNIX, :DGRAM, 0)
 *   s1_ai = Addrinfo.unix("/tmp/sock1")
 *   s1.bind(s1_ai)
 *
 *   s2 = Socket.new(:UNIX, :DGRAM, 0)
 *   s2_ai = Addrinfo.unix("/tmp/sock2")
 *   s2.bind(s2_ai)
 *   s3 = UNIXSocket.for_fd(s2.fileno)
 *
 *   s1.send "a", 0, s2_ai
 *   p s3.recvfrom(10) #=> ["a", ["AF_UNIX", "/tmp/sock1"]]
 *
 */
static VALUE
unix_recvfrom(int argc, VALUE *argv, VALUE sock)
{
    return rsock_s_recvfrom(sock, argc, argv, RECV_UNIX);
}

#if defined(HAVE_ST_MSG_CONTROL) && defined(SCM_RIGHTS)
#define FD_PASSING_BY_MSG_CONTROL 1
#else
#define FD_PASSING_BY_MSG_CONTROL 0
#endif

#if defined(HAVE_ST_MSG_ACCRIGHTS)
#define FD_PASSING_BY_MSG_ACCRIGHTS 1
#else
#define FD_PASSING_BY_MSG_ACCRIGHTS 0
#endif

struct iomsg_arg {
    int fd;
    struct msghdr msg;
};

#if defined(HAVE_SENDMSG) && (FD_PASSING_BY_MSG_CONTROL || FD_PASSING_BY_MSG_ACCRIGHTS)
static VALUE
sendmsg_blocking(void *data)
{
    struct iomsg_arg *arg = data;
    return sendmsg(arg->fd, &arg->msg, 0);
}

/*
 * call-seq:
 *   unixsocket.send_io(io) => nil
 *
 * Sends _io_ as file descriptor passing.
 *
 *   s1, s2 = UNIXSocket.pair
 *
 *   s1.send_io STDOUT
 *   stdout = s2.recv_io
 *
 *   p STDOUT.fileno #=> 1
 *   p stdout.fileno #=> 6
 *
 *   stdout.puts "hello" # outputs "hello\n" to standard output.
 */
static VALUE
unix_send_io(VALUE sock, VALUE val)
{
    int fd;
    rb_io_t *fptr;
    struct iomsg_arg arg;
    struct iovec vec[1];
    char buf[1];

#if FD_PASSING_BY_MSG_CONTROL
    struct {
	struct cmsghdr hdr;
        char pad[8+sizeof(int)+8];
    } cmsg;
#endif

    if (rb_obj_is_kind_of(val, rb_cIO)) {
        rb_io_t *valfptr;
	GetOpenFile(val, valfptr);
	fd = valfptr->fd;
    }
    else if (FIXNUM_P(val)) {
        fd = FIX2INT(val);
    }
    else {
	rb_raise(rb_eTypeError, "neither IO nor file descriptor");
    }

    GetOpenFile(sock, fptr);

    arg.msg.msg_name = NULL;
    arg.msg.msg_namelen = 0;

    /* Linux and Solaris doesn't work if msg_iov is NULL. */
    buf[0] = '\0';
    vec[0].iov_base = buf;
    vec[0].iov_len = 1;
    arg.msg.msg_iov = vec;
    arg.msg.msg_iovlen = 1;

#if FD_PASSING_BY_MSG_CONTROL
    arg.msg.msg_control = (caddr_t)&cmsg;
    arg.msg.msg_controllen = (socklen_t)CMSG_LEN(sizeof(int));
    arg.msg.msg_flags = 0;
    MEMZERO((char*)&cmsg, char, sizeof(cmsg));
    cmsg.hdr.cmsg_len = (socklen_t)CMSG_LEN(sizeof(int));
    cmsg.hdr.cmsg_level = SOL_SOCKET;
    cmsg.hdr.cmsg_type = SCM_RIGHTS;
    memcpy(CMSG_DATA(&cmsg.hdr), &fd, sizeof(int));
#else
    arg.msg.msg_accrights = (caddr_t)&fd;
    arg.msg.msg_accrightslen = sizeof(fd);
#endif

    arg.fd = fptr->fd;
    while ((int)BLOCKING_REGION_FD(sendmsg_blocking, &arg) == -1) {
	if (!rb_io_wait_writable(arg.fd))
	    rb_sys_fail("sendmsg(2)");
    }

    return Qnil;
}
#else
#define unix_send_io rb_f_notimplement
#endif

#if defined(HAVE_RECVMSG) && (FD_PASSING_BY_MSG_CONTROL || FD_PASSING_BY_MSG_ACCRIGHTS)
static VALUE
recvmsg_blocking(void *data)
{
    struct iomsg_arg *arg = data;
    int flags = 0;
    return rsock_recvmsg(arg->fd, &arg->msg, flags);
}

/*
 * call-seq:
 *   unixsocket.recv_io([klass [, mode]]) => io
 *
 *   UNIXServer.open("/tmp/sock") {|serv|
 *     UNIXSocket.open("/tmp/sock") {|c|
 *       s = serv.accept
 *
 *       c.send_io STDOUT
 *       stdout = s.recv_io
 *
 *       p STDOUT.fileno #=> 1
 *       p stdout.fileno #=> 7
 *
 *       stdout.puts "hello" # outputs "hello\n" to standard output.
 *     }
 *   }
 *
 */
static VALUE
unix_recv_io(int argc, VALUE *argv, VALUE sock)
{
    VALUE klass, mode;
    rb_io_t *fptr;
    struct iomsg_arg arg;
    struct iovec vec[2];
    char buf[1];

    int fd;
#if FD_PASSING_BY_MSG_CONTROL
    struct {
	struct cmsghdr hdr;
        char pad[8+sizeof(int)+8];
    } cmsg;
#endif

    rb_scan_args(argc, argv, "02", &klass, &mode);
    if (argc == 0)
	klass = rb_cIO;
    if (argc <= 1)
	mode = Qnil;

    GetOpenFile(sock, fptr);

    arg.msg.msg_name = NULL;
    arg.msg.msg_namelen = 0;

    vec[0].iov_base = buf;
    vec[0].iov_len = sizeof(buf);
    arg.msg.msg_iov = vec;
    arg.msg.msg_iovlen = 1;

#if FD_PASSING_BY_MSG_CONTROL
    arg.msg.msg_control = (caddr_t)&cmsg;
    arg.msg.msg_controllen = (socklen_t)CMSG_SPACE(sizeof(int));
    arg.msg.msg_flags = 0;
    cmsg.hdr.cmsg_len = (socklen_t)CMSG_LEN(sizeof(int));
    cmsg.hdr.cmsg_level = SOL_SOCKET;
    cmsg.hdr.cmsg_type = SCM_RIGHTS;
    fd = -1;
    memcpy(CMSG_DATA(&cmsg.hdr), &fd, sizeof(int));
#else
    arg.msg.msg_accrights = (caddr_t)&fd;
    arg.msg.msg_accrightslen = sizeof(fd);
    fd = -1;
#endif

    arg.fd = fptr->fd;
    while ((int)BLOCKING_REGION_FD(recvmsg_blocking, &arg) == -1) {
	if (!rb_io_wait_readable(arg.fd))
	    rb_sys_fail("recvmsg(2)");
    }

#if FD_PASSING_BY_MSG_CONTROL
    if (arg.msg.msg_controllen < (socklen_t)sizeof(struct cmsghdr)) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (msg_controllen=%d smaller than sizeof(struct cmsghdr)=%d)",
		 (int)arg.msg.msg_controllen, (int)sizeof(struct cmsghdr));
    }
    if (cmsg.hdr.cmsg_level != SOL_SOCKET) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (cmsg_level=%d, %d expected)",
		 cmsg.hdr.cmsg_level, SOL_SOCKET);
    }
    if (cmsg.hdr.cmsg_type != SCM_RIGHTS) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (cmsg_type=%d, %d expected)",
		 cmsg.hdr.cmsg_type, SCM_RIGHTS);
    }
    if (arg.msg.msg_controllen < (socklen_t)CMSG_LEN(sizeof(int))) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (msg_controllen=%d smaller than CMSG_LEN(sizeof(int))=%d)",
		 (int)arg.msg.msg_controllen, (int)CMSG_LEN(sizeof(int)));
    }
    if ((socklen_t)CMSG_SPACE(sizeof(int)) < arg.msg.msg_controllen) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (msg_controllen=%d bigger than CMSG_SPACE(sizeof(int))=%d)",
		 (int)arg.msg.msg_controllen, (int)CMSG_SPACE(sizeof(int)));
    }
    if (cmsg.hdr.cmsg_len != CMSG_LEN(sizeof(int))) {
	rsock_discard_cmsg_resource(&arg.msg, 0);
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (cmsg_len=%d, %d expected)",
		 (int)cmsg.hdr.cmsg_len, (int)CMSG_LEN(sizeof(int)));
    }
#else
    if (arg.msg.msg_accrightslen != sizeof(fd)) {
	rb_raise(rb_eSocket,
		 "file descriptor was not passed (accrightslen) : %d != %d",
		 arg.msg.msg_accrightslen, (int)sizeof(fd));
    }
#endif

#if FD_PASSING_BY_MSG_CONTROL
    memcpy(&fd, CMSG_DATA(&cmsg.hdr), sizeof(int));
#endif
    rb_fd_fix_cloexec(fd);

    if (klass == Qnil)
	return INT2FIX(fd);
    else {
	ID for_fd;
	int ff_argc;
	VALUE ff_argv[2];
	CONST_ID(for_fd, "for_fd");
	ff_argc = mode == Qnil ? 1 : 2;
	ff_argv[0] = INT2FIX(fd);
	ff_argv[1] = mode;
        return rb_funcall2(klass, for_fd, ff_argc, ff_argv);
    }
}
#else
#define unix_recv_io rb_f_notimplement
#endif

/*
 * call-seq:
 *   unixsocket.addr => [address_family, unix_path]
 *
 * Returns the local address as an array which contains
 * address_family and unix_path.
 *
 * Example
 *   serv = UNIXServer.new("/tmp/sock")
 *   p serv.addr #=> ["AF_UNIX", "/tmp/sock"]
 */
static VALUE
unix_addr(VALUE sock)
{
    rb_io_t *fptr;
    struct sockaddr_un addr;
    socklen_t len = (socklen_t)sizeof addr;

    GetOpenFile(sock, fptr);

    if (getsockname(fptr->fd, (struct sockaddr*)&addr, &len) < 0)
	rb_sys_fail("getsockname(2)");
    return rsock_unixaddr(&addr, len);
}

/*
 * call-seq:
 *   unixsocket.peeraddr => [address_family, unix_path]
 *
 * Returns the remote address as an array which contains
 * address_family and unix_path.
 *
 * Example
 *   serv = UNIXServer.new("/tmp/sock")
 *   c = UNIXSocket.new("/tmp/sock")
 *   p c.peeraddr #=> ["AF_UNIX", "/tmp/sock"]
 */
static VALUE
unix_peeraddr(VALUE sock)
{
    rb_io_t *fptr;
    struct sockaddr_un addr;
    socklen_t len = (socklen_t)sizeof addr;

    GetOpenFile(sock, fptr);

    if (getpeername(fptr->fd, (struct sockaddr*)&addr, &len) < 0)
	rb_sys_fail("getpeername(2)");
    return rsock_unixaddr(&addr, len);
}

/*
 * call-seq:
 *   UNIXSocket.pair([type [, protocol]])       => [unixsocket1, unixsocket2]
 *   UNIXSocket.socketpair([type [, protocol]]) => [unixsocket1, unixsocket2]
 *
 * Creates a pair of sockets connected each other.
 *
 * _socktype_ should be a socket type such as: :STREAM, :DGRAM, :RAW, etc.
 *
 * _protocol_ should be a protocol defined in the domain.
 * 0 is default protocol for the domain.
 *
 *   s1, s2 = UNIXSocket.pair
 *   s1.send "a", 0
 *   s1.send "b", 0
 *   p s2.recv(10) #=> "ab"
 *
 */
static VALUE
unix_s_socketpair(int argc, VALUE *argv, VALUE klass)
{
    VALUE domain, type, protocol;
    VALUE args[3];

    domain = INT2FIX(PF_UNIX);
    rb_scan_args(argc, argv, "02", &type, &protocol);
    if (argc == 0)
	type = INT2FIX(SOCK_STREAM);
    if (argc <= 1)
	protocol = INT2FIX(0);

    args[0] = domain;
    args[1] = type;
    args[2] = protocol;

    return rsock_sock_s_socketpair(3, args, klass);
}
#endif

void
rsock_init_unixsocket(void)
{
#ifdef HAVE_SYS_UN_H
    /*
     * Document-class: UNIXSocket < BasicSocket
     *
     * UNIXSocket represents a UNIX domain stream client socket.
     */
    rb_cUNIXSocket = rb_define_class("UNIXSocket", rb_cBasicSocket);
    rb_define_method(rb_cUNIXSocket, "initialize", unix_init, 1);
    rb_define_method(rb_cUNIXSocket, "path", unix_path, 0);
    rb_define_method(rb_cUNIXSocket, "addr", unix_addr, 0);
    rb_define_method(rb_cUNIXSocket, "peeraddr", unix_peeraddr, 0);
    rb_define_method(rb_cUNIXSocket, "recvfrom", unix_recvfrom, -1);
    rb_define_method(rb_cUNIXSocket, "send_io", unix_send_io, 1);
    rb_define_method(rb_cUNIXSocket, "recv_io", unix_recv_io, -1);
    rb_define_singleton_method(rb_cUNIXSocket, "socketpair", unix_s_socketpair, -1);
    rb_define_singleton_method(rb_cUNIXSocket, "pair", unix_s_socketpair, -1);
#endif
}
