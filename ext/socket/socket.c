/************************************************

  socket.c -

  created at: Thu Mar 31 12:21:29 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

************************************************/

#include "rubysocket.h"

static void
setup_domain_and_type(VALUE domain, int *dv, VALUE type, int *tv)
{
    *dv = rsock_family_arg(domain);
    *tv = rsock_socktype_arg(type);
}

/*
 * call-seq:
 *   Socket.new(domain, socktype [, protocol]) => socket
 *
 * Creates a new socket object.
 *
 * _domain_ should be a communications domain such as: :INET, :INET6, :UNIX, etc.
 *
 * _socktype_ should be a socket type such as: :STREAM, :DGRAM, :RAW, etc.
 *
 * _protocol_ should be a protocol defined in the domain.
 * This is optional.
 * If it is not given, 0 is used internally.
 *
 *   Socket.new(:INET, :STREAM) # TCP socket
 *   Socket.new(:INET, :DGRAM)  # UDP socket
 *   Socket.new(:UNIX, :STREAM) # UNIX stream socket
 *   Socket.new(:UNIX, :DGRAM)  # UNIX datagram socket
 */
static VALUE
sock_initialize(int argc, VALUE *argv, VALUE sock)
{
    VALUE domain, type, protocol;
    int fd;
    int d, t;

    rb_scan_args(argc, argv, "21", &domain, &type, &protocol);
    if (NIL_P(protocol))
        protocol = INT2FIX(0);

    rb_secure(3);
    setup_domain_and_type(domain, &d, type, &t);
    fd = rsock_socket(d, t, NUM2INT(protocol));
    if (fd < 0) rb_sys_fail("socket(2)");

    return rsock_init_sock(sock, fd);
}

#if defined HAVE_SOCKETPAIR
static VALUE
io_call_close(VALUE io)
{
    return rb_funcall(io, rb_intern("close"), 0, 0);
}

static VALUE
io_close(VALUE io)
{
    return rb_rescue(io_call_close, io, 0, 0);
}

static VALUE
pair_yield(VALUE pair)
{
    return rb_ensure(rb_yield, pair, io_close, rb_ary_entry(pair, 1));
}
#endif

#if defined HAVE_SOCKETPAIR

static int
rsock_socketpair0(int domain, int type, int protocol, int sv[2])
{
    int ret;

#ifdef SOCK_CLOEXEC
    static int try_sock_cloexec = 1;
    if (try_sock_cloexec) {
        ret = socketpair(domain, type|SOCK_CLOEXEC, protocol, sv);
        if (ret == -1 && errno == EINVAL) {
            /* SOCK_CLOEXEC is available since Linux 2.6.27.  Linux 2.6.18 fails with EINVAL */
            ret = socketpair(domain, type, protocol, sv);
            if (ret != -1) {
                /* The reason of EINVAL may be other than SOCK_CLOEXEC.
                 * So disable SOCK_CLOEXEC only if socketpair() succeeds without SOCK_CLOEXEC.
                 * Ex. Socket.pair(:UNIX, 0xff) fails with EINVAL.
                 */
                try_sock_cloexec = 0;
            }
        }
    }
    else {
        ret = socketpair(domain, type, protocol, sv);
    }
#else
    ret = socketpair(domain, type, protocol, sv);
#endif

    if (ret == -1) {
        return -1;
    }

    rb_fd_fix_cloexec(sv[0]);
    rb_fd_fix_cloexec(sv[1]);

    return ret;
}

static int
rsock_socketpair(int domain, int type, int protocol, int sv[2])
{
    int ret;

    ret = rsock_socketpair0(domain, type, protocol, sv);
    if (ret < 0 && (errno == EMFILE || errno == ENFILE)) {
        rb_gc();
        ret = rsock_socketpair0(domain, type, protocol, sv);
    }

    return ret;
}

/*
 * call-seq:
 *   Socket.pair(domain, type, protocol)       => [socket1, socket2]
 *   Socket.socketpair(domain, type, protocol) => [socket1, socket2]
 *
 * Creates a pair of sockets connected each other.
 *
 * _domain_ should be a communications domain such as: :INET, :INET6, :UNIX, etc.
 *
 * _socktype_ should be a socket type such as: :STREAM, :DGRAM, :RAW, etc.
 *
 * _protocol_ should be a protocol defined in the domain.
 * 0 is default protocol for the domain.
 *
 *   s1, s2 = Socket.pair(:UNIX, :DGRAM, 0)
 *   s1.send "a", 0
 *   s1.send "b", 0
 *   p s2.recv(10) #=> "a"
 *   p s2.recv(10) #=> "b"
 *
 */
VALUE
rsock_sock_s_socketpair(int argc, VALUE *argv, VALUE klass)
{
    VALUE domain, type, protocol;
    int d, t, p, sp[2];
    int ret;
    VALUE s1, s2, r;

    rb_scan_args(argc, argv, "21", &domain, &type, &protocol);
    if (NIL_P(protocol))
        protocol = INT2FIX(0);

    setup_domain_and_type(domain, &d, type, &t);
    p = NUM2INT(protocol);
    ret = rsock_socketpair(d, t, p, sp);
    if (ret < 0) {
	rb_sys_fail("socketpair(2)");
    }
    rb_fd_fix_cloexec(sp[0]);
    rb_fd_fix_cloexec(sp[1]);

    s1 = rsock_init_sock(rb_obj_alloc(klass), sp[0]);
    s2 = rsock_init_sock(rb_obj_alloc(klass), sp[1]);
    r = rb_assoc_new(s1, s2);
    if (rb_block_given_p()) {
        return rb_ensure(pair_yield, r, io_close, s1);
    }
    return r;
}
#else
#define rsock_sock_s_socketpair rb_f_notimplement
#endif

/*
 * call-seq:
 * 	socket.connect(remote_sockaddr) => 0
 *
 * Requests a connection to be made on the given +remote_sockaddr+. Returns 0 if
 * successful, otherwise an exception is raised.
 *
 * === Parameter
 * * +remote_sockaddr+ - the +struct+ sockaddr contained in a string or Addrinfo object
 *
 * === Example:
 * 	# Pull down Google's web page
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new( AF_INET, SOCK_STREAM, 0 )
 * 	sockaddr = Socket.pack_sockaddr_in( 80, 'www.google.com' )
 * 	socket.connect( sockaddr )
 * 	socket.write( "GET / HTTP/1.0\r\n\r\n" )
 * 	results = socket.read
 *
 * === Unix-based Exceptions
 * On unix-based systems the following system exceptions may be raised if
 * the call to _connect_ fails:
 * * Errno::EACCES - search permission is denied for a component of the prefix
 *   path or write access to the +socket+ is denied
 * * Errno::EADDRINUSE - the _sockaddr_ is already in use
 * * Errno::EADDRNOTAVAIL - the specified _sockaddr_ is not available from the
 *   local machine
 * * Errno::EAFNOSUPPORT - the specified _sockaddr_ is not a valid address for
 *   the address family of the specified +socket+
 * * Errno::EALREADY - a connection is already in progress for the specified
 *   socket
 * * Errno::EBADF - the +socket+ is not a valid file descriptor
 * * Errno::ECONNREFUSED - the target _sockaddr_ was not listening for connections
 *   refused the connection request
 * * Errno::ECONNRESET - the remote host reset the connection request
 * * Errno::EFAULT - the _sockaddr_ cannot be accessed
 * * Errno::EHOSTUNREACH - the destination host cannot be reached (probably
 *   because the host is down or a remote router cannot reach it)
 * * Errno::EINPROGRESS - the O_NONBLOCK is set for the +socket+ and the
 *   connection cannot be immediately established; the connection will be
 *   established asynchronously
 * * Errno::EINTR - the attempt to establish the connection was interrupted by
 *   delivery of a signal that was caught; the connection will be established
 *   asynchronously
 * * Errno::EISCONN - the specified +socket+ is already connected
 * * Errno::EINVAL - the address length used for the _sockaddr_ is not a valid
 *   length for the address family or there is an invalid family in _sockaddr_
 * * Errno::ENAMETOOLONG - the pathname resolved had a length which exceeded
 *   PATH_MAX
 * * Errno::ENETDOWN - the local interface used to reach the destination is down
 * * Errno::ENETUNREACH - no route to the network is present
 * * Errno::ENOBUFS - no buffer space is available
 * * Errno::ENOSR - there were insufficient STREAMS resources available to
 *   complete the operation
 * * Errno::ENOTSOCK - the +socket+ argument does not refer to a socket
 * * Errno::EOPNOTSUPP - the calling +socket+ is listening and cannot be connected
 * * Errno::EPROTOTYPE - the _sockaddr_ has a different type than the socket
 *   bound to the specified peer address
 * * Errno::ETIMEDOUT - the attempt to connect time out before a connection
 *   was made.
 *
 * On unix-based systems if the address family of the calling +socket+ is
 * AF_UNIX the follow exceptions may be raised if the call to _connect_
 * fails:
 * * Errno::EIO - an i/o error occurred while reading from or writing to the
 *   file system
 * * Errno::ELOOP - too many symbolic links were encountered in translating
 *   the pathname in _sockaddr_
 * * Errno::ENAMETOOLLONG - a component of a pathname exceeded NAME_MAX
 *   characters, or an entire pathname exceeded PATH_MAX characters
 * * Errno::ENOENT - a component of the pathname does not name an existing file
 *   or the pathname is an empty string
 * * Errno::ENOTDIR - a component of the path prefix of the pathname in _sockaddr_
 *   is not a directory
 *
 * === Windows Exceptions
 * On Windows systems the following system exceptions may be raised if
 * the call to _connect_ fails:
 * * Errno::ENETDOWN - the network is down
 * * Errno::EADDRINUSE - the socket's local address is already in use
 * * Errno::EINTR - the socket was cancelled
 * * Errno::EINPROGRESS - a blocking socket is in progress or the service provider
 *   is still processing a callback function. Or a nonblocking connect call is
 *   in progress on the +socket+.
 * * Errno::EALREADY - see Errno::EINVAL
 * * Errno::EADDRNOTAVAIL - the remote address is not a valid address, such as
 *   ADDR_ANY TODO check ADDRANY TO INADDR_ANY
 * * Errno::EAFNOSUPPORT - addresses in the specified family cannot be used with
 *   with this +socket+
 * * Errno::ECONNREFUSED - the target _sockaddr_ was not listening for connections
 *   refused the connection request
 * * Errno::EFAULT - the socket's internal address or address length parameter
 *   is too small or is not a valid part of the user space address
 * * Errno::EINVAL - the +socket+ is a listening socket
 * * Errno::EISCONN - the +socket+ is already connected
 * * Errno::ENETUNREACH - the network cannot be reached from this host at this time
 * * Errno::EHOSTUNREACH - no route to the network is present
 * * Errno::ENOBUFS - no buffer space is available
 * * Errno::ENOTSOCK - the +socket+ argument does not refer to a socket
 * * Errno::ETIMEDOUT - the attempt to connect time out before a connection
 *   was made.
 * * Errno::EWOULDBLOCK - the socket is marked as nonblocking and the
 *   connection cannot be completed immediately
 * * Errno::EACCES - the attempt to connect the datagram socket to the
 *   broadcast address failed
 *
 * === See
 * * connect manual pages on unix-based systems
 * * connect function in Microsoft's Winsock functions reference
 */
static VALUE
sock_connect(VALUE sock, VALUE addr)
{
    rb_io_t *fptr;
    int fd, n;

    SockAddrStringValue(addr);
    addr = rb_str_new4(addr);
    GetOpenFile(sock, fptr);
    fd = fptr->fd;
    n = rsock_connect(fd, (struct sockaddr*)RSTRING_PTR(addr), RSTRING_LENINT(addr), 0);
    if (n < 0) {
	rb_sys_fail("connect(2)");
    }

    return INT2FIX(n);
}

/*
 * call-seq:
 * 	socket.connect_nonblock(remote_sockaddr) => 0
 *
 * Requests a connection to be made on the given +remote_sockaddr+ after
 * O_NONBLOCK is set for the underlying file descriptor.
 * Returns 0 if successful, otherwise an exception is raised.
 *
 * === Parameter
 * * +remote_sockaddr+ - the +struct+ sockaddr contained in a string or Addrinfo object
 *
 * === Example:
 * 	# Pull down Google's web page
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new(AF_INET, SOCK_STREAM, 0)
 * 	sockaddr = Socket.sockaddr_in(80, 'www.google.com')
 * 	begin # emulate blocking connect
 * 	  socket.connect_nonblock(sockaddr)
 * 	rescue IO::WaitWritable
 * 	  IO.select(nil, [socket]) # wait 3-way handshake completion
 * 	  begin
 * 	    socket.connect_nonblock(sockaddr) # check connection failure
 * 	  rescue Errno::EISCONN
 * 	  end
 * 	end
 * 	socket.write("GET / HTTP/1.0\r\n\r\n")
 * 	results = socket.read
 *
 * Refer to Socket#connect for the exceptions that may be thrown if the call
 * to _connect_nonblock_ fails.
 *
 * Socket#connect_nonblock may raise any error corresponding to connect(2) failure,
 * including Errno::EINPROGRESS.
 *
 * If the exception is Errno::EINPROGRESS,
 * it is extended by IO::WaitWritable.
 * So IO::WaitWritable can be used to rescue the exceptions for retrying connect_nonblock.
 *
 * === See
 * * Socket#connect
 */
static VALUE
sock_connect_nonblock(VALUE sock, VALUE addr)
{
    rb_io_t *fptr;
    int n;

    SockAddrStringValue(addr);
    addr = rb_str_new4(addr);
    GetOpenFile(sock, fptr);
    rb_io_set_nonblock(fptr);
    n = connect(fptr->fd, (struct sockaddr*)RSTRING_PTR(addr), RSTRING_LENINT(addr));
    if (n < 0) {
        if (errno == EINPROGRESS)
            rb_mod_sys_fail(rb_mWaitWritable, "connect(2) would block");
	rb_sys_fail("connect(2)");
    }

    return INT2FIX(n);
}

/*
 * call-seq:
 * 	socket.bind(local_sockaddr) => 0
 *
 * Binds to the given local address.
 *
 * === Parameter
 * * +local_sockaddr+ - the +struct+ sockaddr contained in a string or an Addrinfo object
 *
 * === Example
 * 	require 'socket'
 *
 * 	# use Addrinfo
 * 	socket = Socket.new(:INET, :STREAM, 0)
 * 	socket.bind(Addrinfo.tcp("127.0.0.1", 2222))
 * 	p socket.local_address #=> #<Addrinfo: 127.0.0.1:2222 TCP>
 *
 * 	# use struct sockaddr
 * 	include Socket::Constants
 * 	socket = Socket.new( AF_INET, SOCK_STREAM, 0 )
 * 	sockaddr = Socket.pack_sockaddr_in( 2200, 'localhost' )
 * 	socket.bind( sockaddr )
 *
 * === Unix-based Exceptions
 * On unix-based based systems the following system exceptions may be raised if
 * the call to _bind_ fails:
 * * Errno::EACCES - the specified _sockaddr_ is protected and the current
 *   user does not have permission to bind to it
 * * Errno::EADDRINUSE - the specified _sockaddr_ is already in use
 * * Errno::EADDRNOTAVAIL - the specified _sockaddr_ is not available from the
 *   local machine
 * * Errno::EAFNOSUPPORT - the specified _sockaddr_ is not a valid address for
 *   the family of the calling +socket+
 * * Errno::EBADF - the _sockaddr_ specified is not a valid file descriptor
 * * Errno::EFAULT - the _sockaddr_ argument cannot be accessed
 * * Errno::EINVAL - the +socket+ is already bound to an address, and the
 *   protocol does not support binding to the new _sockaddr_ or the +socket+
 *   has been shut down.
 * * Errno::EINVAL - the address length is not a valid length for the address
 *   family
 * * Errno::ENAMETOOLONG - the pathname resolved had a length which exceeded
 *   PATH_MAX
 * * Errno::ENOBUFS - no buffer space is available
 * * Errno::ENOSR - there were insufficient STREAMS resources available to
 *   complete the operation
 * * Errno::ENOTSOCK - the +socket+ does not refer to a socket
 * * Errno::EOPNOTSUPP - the socket type of the +socket+ does not support
 *   binding to an address
 *
 * On unix-based based systems if the address family of the calling +socket+ is
 * Socket::AF_UNIX the follow exceptions may be raised if the call to _bind_
 * fails:
 * * Errno::EACCES - search permission is denied for a component of the prefix
 *   path or write access to the +socket+ is denied
 * * Errno::EDESTADDRREQ - the _sockaddr_ argument is a null pointer
 * * Errno::EISDIR - same as Errno::EDESTADDRREQ
 * * Errno::EIO - an i/o error occurred
 * * Errno::ELOOP - too many symbolic links were encountered in translating
 *   the pathname in _sockaddr_
 * * Errno::ENAMETOOLLONG - a component of a pathname exceeded NAME_MAX
 *   characters, or an entire pathname exceeded PATH_MAX characters
 * * Errno::ENOENT - a component of the pathname does not name an existing file
 *   or the pathname is an empty string
 * * Errno::ENOTDIR - a component of the path prefix of the pathname in _sockaddr_
 *   is not a directory
 * * Errno::EROFS - the name would reside on a read only filesystem
 *
 * === Windows Exceptions
 * On Windows systems the following system exceptions may be raised if
 * the call to _bind_ fails:
 * * Errno::ENETDOWN-- the network is down
 * * Errno::EACCES - the attempt to connect the datagram socket to the
 *   broadcast address failed
 * * Errno::EADDRINUSE - the socket's local address is already in use
 * * Errno::EADDRNOTAVAIL - the specified address is not a valid address for this
 *   computer
 * * Errno::EFAULT - the socket's internal address or address length parameter
 *   is too small or is not a valid part of the user space addressed
 * * Errno::EINVAL - the +socket+ is already bound to an address
 * * Errno::ENOBUFS - no buffer space is available
 * * Errno::ENOTSOCK - the +socket+ argument does not refer to a socket
 *
 * === See
 * * bind manual pages on unix-based systems
 * * bind function in Microsoft's Winsock functions reference
 */
static VALUE
sock_bind(VALUE sock, VALUE addr)
{
    rb_io_t *fptr;

    SockAddrStringValue(addr);
    GetOpenFile(sock, fptr);
    if (bind(fptr->fd, (struct sockaddr*)RSTRING_PTR(addr), RSTRING_LENINT(addr)) < 0)
	rb_sys_fail("bind(2)");

    return INT2FIX(0);
}

/*
 * call-seq:
 * 	socket.listen( int ) => 0
 *
 * Listens for connections, using the specified +int+ as the backlog. A call
 * to _listen_ only applies if the +socket+ is of type SOCK_STREAM or
 * SOCK_SEQPACKET.
 *
 * === Parameter
 * * +backlog+ - the maximum length of the queue for pending connections.
 *
 * === Example 1
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new( AF_INET, SOCK_STREAM, 0 )
 * 	sockaddr = Socket.pack_sockaddr_in( 2200, 'localhost' )
 * 	socket.bind( sockaddr )
 * 	socket.listen( 5 )
 *
 * === Example 2 (listening on an arbitrary port, unix-based systems only):
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new( AF_INET, SOCK_STREAM, 0 )
 * 	socket.listen( 1 )
 *
 * === Unix-based Exceptions
 * On unix based systems the above will work because a new +sockaddr+ struct
 * is created on the address ADDR_ANY, for an arbitrary port number as handed
 * off by the kernel. It will not work on Windows, because Windows requires that
 * the +socket+ is bound by calling _bind_ before it can _listen_.
 *
 * If the _backlog_ amount exceeds the implementation-dependent maximum
 * queue length, the implementation's maximum queue length will be used.
 *
 * On unix-based based systems the following system exceptions may be raised if the
 * call to _listen_ fails:
 * * Errno::EBADF - the _socket_ argument is not a valid file descriptor
 * * Errno::EDESTADDRREQ - the _socket_ is not bound to a local address, and
 *   the protocol does not support listening on an unbound socket
 * * Errno::EINVAL - the _socket_ is already connected
 * * Errno::ENOTSOCK - the _socket_ argument does not refer to a socket
 * * Errno::EOPNOTSUPP - the _socket_ protocol does not support listen
 * * Errno::EACCES - the calling process does not have appropriate privileges
 * * Errno::EINVAL - the _socket_ has been shut down
 * * Errno::ENOBUFS - insufficient resources are available in the system to
 *   complete the call
 *
 * === Windows Exceptions
 * On Windows systems the following system exceptions may be raised if
 * the call to _listen_ fails:
 * * Errno::ENETDOWN - the network is down
 * * Errno::EADDRINUSE - the socket's local address is already in use. This
 *   usually occurs during the execution of _bind_ but could be delayed
 *   if the call to _bind_ was to a partially wildcard address (involving
 *   ADDR_ANY) and if a specific address needs to be committed at the
 *   time of the call to _listen_
 * * Errno::EINPROGRESS - a Windows Sockets 1.1 call is in progress or the
 *   service provider is still processing a callback function
 * * Errno::EINVAL - the +socket+ has not been bound with a call to _bind_.
 * * Errno::EISCONN - the +socket+ is already connected
 * * Errno::EMFILE - no more socket descriptors are available
 * * Errno::ENOBUFS - no buffer space is available
 * * Errno::ENOTSOC - +socket+ is not a socket
 * * Errno::EOPNOTSUPP - the referenced +socket+ is not a type that supports
 *   the _listen_ method
 *
 * === See
 * * listen manual pages on unix-based systems
 * * listen function in Microsoft's Winsock functions reference
 */
VALUE
rsock_sock_listen(VALUE sock, VALUE log)
{
    rb_io_t *fptr;
    int backlog;

    rb_secure(4);
    backlog = NUM2INT(log);
    GetOpenFile(sock, fptr);
    if (listen(fptr->fd, backlog) < 0)
	rb_sys_fail("listen(2)");

    return INT2FIX(0);
}

/*
 * call-seq:
 * 	socket.recvfrom(maxlen) => [mesg, sender_addrinfo]
 * 	socket.recvfrom(maxlen, flags) => [mesg, sender_addrinfo]
 *
 * Receives up to _maxlen_ bytes from +socket+. _flags_ is zero or more
 * of the +MSG_+ options. The first element of the results, _mesg_, is the data
 * received. The second element, _sender_addrinfo_, contains protocol-specific
 * address information of the sender.
 *
 * === Parameters
 * * +maxlen+ - the maximum number of bytes to receive from the socket
 * * +flags+ - zero or more of the +MSG_+ options
 *
 * === Example
 * 	# In one file, start this first
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new( AF_INET, SOCK_STREAM, 0 )
 * 	sockaddr = Socket.pack_sockaddr_in( 2200, 'localhost' )
 * 	socket.bind( sockaddr )
 * 	socket.listen( 5 )
 * 	client, client_addrinfo = socket.accept
 * 	data = client.recvfrom( 20 )[0].chomp
 * 	puts "I only received 20 bytes '#{data}'"
 * 	sleep 1
 * 	socket.close
 *
 * 	# In another file, start this second
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new( AF_INET, SOCK_STREAM, 0 )
 * 	sockaddr = Socket.pack_sockaddr_in( 2200, 'localhost' )
 * 	socket.connect( sockaddr )
 * 	socket.puts "Watch this get cut short!"
 * 	socket.close
 *
 * === Unix-based Exceptions
 * On unix-based based systems the following system exceptions may be raised if the
 * call to _recvfrom_ fails:
 * * Errno::EAGAIN - the +socket+ file descriptor is marked as O_NONBLOCK and no
 *   data is waiting to be received; or MSG_OOB is set and no out-of-band data
 *   is available and either the +socket+ file descriptor is marked as
 *   O_NONBLOCK or the +socket+ does not support blocking to wait for
 *   out-of-band-data
 * * Errno::EWOULDBLOCK - see Errno::EAGAIN
 * * Errno::EBADF - the +socket+ is not a valid file descriptor
 * * Errno::ECONNRESET - a connection was forcibly closed by a peer
 * * Errno::EFAULT - the socket's internal buffer, address or address length
 *   cannot be accessed or written
 * * Errno::EINTR - a signal interrupted _recvfrom_ before any data was available
 * * Errno::EINVAL - the MSG_OOB flag is set and no out-of-band data is available
 * * Errno::EIO - an i/o error occurred while reading from or writing to the
 *   filesystem
 * * Errno::ENOBUFS - insufficient resources were available in the system to
 *   perform the operation
 * * Errno::ENOMEM - insufficient memory was available to fulfill the request
 * * Errno::ENOSR - there were insufficient STREAMS resources available to
 *   complete the operation
 * * Errno::ENOTCONN - a receive is attempted on a connection-mode socket that
 *   is not connected
 * * Errno::ENOTSOCK - the +socket+ does not refer to a socket
 * * Errno::EOPNOTSUPP - the specified flags are not supported for this socket type
 * * Errno::ETIMEDOUT - the connection timed out during connection establishment
 *   or due to a transmission timeout on an active connection
 *
 * === Windows Exceptions
 * On Windows systems the following system exceptions may be raised if
 * the call to _recvfrom_ fails:
 * * Errno::ENETDOWN - the network is down
 * * Errno::EFAULT - the internal buffer and from parameters on +socket+ are not
 *   part of the user address space, or the internal fromlen parameter is
 *   too small to accommodate the peer address
 * * Errno::EINTR - the (blocking) call was cancelled by an internal call to
 *   the WinSock function WSACancelBlockingCall
 * * Errno::EINPROGRESS - a blocking Windows Sockets 1.1 call is in progress or
 *   the service provider is still processing a callback function
 * * Errno::EINVAL - +socket+ has not been bound with a call to _bind_, or an
 *   unknown flag was specified, or MSG_OOB was specified for a socket with
 *   SO_OOBINLINE enabled, or (for byte stream-style sockets only) the internal
 *   len parameter on +socket+ was zero or negative
 * * Errno::EISCONN - +socket+ is already connected. The call to _recvfrom_ is
 *   not permitted with a connected socket on a socket that is connection
 *   oriented or connectionless.
 * * Errno::ENETRESET - the connection has been broken due to the keep-alive
 *   activity detecting a failure while the operation was in progress.
 * * Errno::EOPNOTSUPP - MSG_OOB was specified, but +socket+ is not stream-style
 *   such as type SOCK_STREAM. OOB data is not supported in the communication
 *   domain associated with +socket+, or +socket+ is unidirectional and
 *   supports only send operations
 * * Errno::ESHUTDOWN - +socket+ has been shutdown. It is not possible to
 *   call _recvfrom_ on a socket after _shutdown_ has been invoked.
 * * Errno::EWOULDBLOCK - +socket+ is marked as nonblocking and a  call to
 *   _recvfrom_ would block.
 * * Errno::EMSGSIZE - the message was too large to fit into the specified buffer
 *   and was truncated.
 * * Errno::ETIMEDOUT - the connection has been dropped, because of a network
 *   failure or because the system on the other end went down without
 *   notice
 * * Errno::ECONNRESET - the virtual circuit was reset by the remote side
 *   executing a hard or abortive close. The application should close the
 *   socket; it is no longer usable. On a UDP-datagram socket this error
 *   indicates a previous send operation resulted in an ICMP Port Unreachable
 *   message.
 */
static VALUE
sock_recvfrom(int argc, VALUE *argv, VALUE sock)
{
    return rsock_s_recvfrom(sock, argc, argv, RECV_SOCKET);
}

/*
 * call-seq:
 * 	socket.recvfrom_nonblock(maxlen) => [mesg, sender_addrinfo]
 * 	socket.recvfrom_nonblock(maxlen, flags) => [mesg, sender_addrinfo]
 *
 * Receives up to _maxlen_ bytes from +socket+ using recvfrom(2) after
 * O_NONBLOCK is set for the underlying file descriptor.
 * _flags_ is zero or more of the +MSG_+ options.
 * The first element of the results, _mesg_, is the data received.
 * The second element, _sender_addrinfo_, contains protocol-specific address
 * information of the sender.
 *
 * When recvfrom(2) returns 0, Socket#recvfrom_nonblock returns
 * an empty string as data.
 * The meaning depends on the socket: EOF on TCP, empty packet on UDP, etc.
 *
 * === Parameters
 * * +maxlen+ - the maximum number of bytes to receive from the socket
 * * +flags+ - zero or more of the +MSG_+ options
 *
 * === Example
 * 	# In one file, start this first
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new(AF_INET, SOCK_STREAM, 0)
 * 	sockaddr = Socket.sockaddr_in(2200, 'localhost')
 * 	socket.bind(sockaddr)
 * 	socket.listen(5)
 * 	client, client_addrinfo = socket.accept
 * 	begin # emulate blocking recvfrom
 * 	  pair = client.recvfrom_nonblock(20)
 * 	rescue IO::WaitReadable
 * 	  IO.select([client])
 * 	  retry
 * 	end
 * 	data = pair[0].chomp
 * 	puts "I only received 20 bytes '#{data}'"
 * 	sleep 1
 * 	socket.close
 *
 * 	# In another file, start this second
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new(AF_INET, SOCK_STREAM, 0)
 * 	sockaddr = Socket.sockaddr_in(2200, 'localhost')
 * 	socket.connect(sockaddr)
 * 	socket.puts "Watch this get cut short!"
 * 	socket.close
 *
 * Refer to Socket#recvfrom for the exceptions that may be thrown if the call
 * to _recvfrom_nonblock_ fails.
 *
 * Socket#recvfrom_nonblock may raise any error corresponding to recvfrom(2) failure,
 * including Errno::EWOULDBLOCK.
 *
 * If the exception is Errno::EWOULDBLOCK or Errno::AGAIN,
 * it is extended by IO::WaitReadable.
 * So IO::WaitReadable can be used to rescue the exceptions for retrying recvfrom_nonblock.
 *
 * === See
 * * Socket#recvfrom
 */
static VALUE
sock_recvfrom_nonblock(int argc, VALUE *argv, VALUE sock)
{
    return rsock_s_recvfrom_nonblock(sock, argc, argv, RECV_SOCKET);
}

/*
 * call-seq:
 *   socket.accept => [client_socket, client_addrinfo]
 *
 * Accepts a next connection.
 * Returns a new Socket object and Addrinfo object.
 *
 *   serv = Socket.new(:INET, :STREAM, 0)
 *   serv.listen(5)
 *   c = Socket.new(:INET, :STREAM, 0)
 *   c.connect(serv.connect_address)
 *   p serv.accept #=> [#<Socket:fd 6>, #<Addrinfo: 127.0.0.1:48555 TCP>]
 *
 */
static VALUE
sock_accept(VALUE sock)
{
    rb_io_t *fptr;
    VALUE sock2;
    struct sockaddr_storage buf;
    socklen_t len = (socklen_t)sizeof buf;

    GetOpenFile(sock, fptr);
    sock2 = rsock_s_accept(rb_cSocket,fptr->fd,(struct sockaddr*)&buf,&len);

    return rb_assoc_new(sock2, rsock_io_socket_addrinfo(sock2, (struct sockaddr*)&buf, len));
}

/*
 * call-seq:
 * 	socket.accept_nonblock => [client_socket, client_addrinfo]
 *
 * Accepts an incoming connection using accept(2) after
 * O_NONBLOCK is set for the underlying file descriptor.
 * It returns an array containing the accepted socket
 * for the incoming connection, _client_socket_,
 * and an Addrinfo, _client_addrinfo_.
 *
 * === Example
 * 	# In one script, start this first
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new(AF_INET, SOCK_STREAM, 0)
 * 	sockaddr = Socket.sockaddr_in(2200, 'localhost')
 * 	socket.bind(sockaddr)
 * 	socket.listen(5)
 * 	begin # emulate blocking accept
 * 	  client_socket, client_addrinfo = socket.accept_nonblock
 * 	rescue IO::WaitReadable, Errno::EINTR
 * 	  IO.select([socket])
 * 	  retry
 * 	end
 * 	puts "The client said, '#{client_socket.readline.chomp}'"
 * 	client_socket.puts "Hello from script one!"
 * 	socket.close
 *
 * 	# In another script, start this second
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new(AF_INET, SOCK_STREAM, 0)
 * 	sockaddr = Socket.sockaddr_in(2200, 'localhost')
 * 	socket.connect(sockaddr)
 * 	socket.puts "Hello from script 2."
 * 	puts "The server said, '#{socket.readline.chomp}'"
 * 	socket.close
 *
 * Refer to Socket#accept for the exceptions that may be thrown if the call
 * to _accept_nonblock_ fails.
 *
 * Socket#accept_nonblock may raise any error corresponding to accept(2) failure,
 * including Errno::EWOULDBLOCK.
 *
 * If the exception is Errno::EWOULDBLOCK, Errno::AGAIN, Errno::ECONNABORTED or Errno::EPROTO,
 * it is extended by IO::WaitReadable.
 * So IO::WaitReadable can be used to rescue the exceptions for retrying accept_nonblock.
 *
 * === See
 * * Socket#accept
 */
static VALUE
sock_accept_nonblock(VALUE sock)
{
    rb_io_t *fptr;
    VALUE sock2;
    struct sockaddr_storage buf;
    socklen_t len = (socklen_t)sizeof buf;

    GetOpenFile(sock, fptr);
    sock2 = rsock_s_accept_nonblock(rb_cSocket, fptr, (struct sockaddr *)&buf, &len);
    return rb_assoc_new(sock2, rsock_io_socket_addrinfo(sock2, (struct sockaddr*)&buf, len));
}

/*
 * call-seq:
 * 	socket.sysaccept => [client_socket_fd, client_addrinfo]
 *
 * Accepts an incoming connection returning an array containing the (integer)
 * file descriptor for the incoming connection, _client_socket_fd_,
 * and an Addrinfo, _client_addrinfo_.
 *
 * === Example
 * 	# In one script, start this first
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new( AF_INET, SOCK_STREAM, 0 )
 * 	sockaddr = Socket.pack_sockaddr_in( 2200, 'localhost' )
 * 	socket.bind( sockaddr )
 * 	socket.listen( 5 )
 * 	client_fd, client_addrinfo = socket.sysaccept
 * 	client_socket = Socket.for_fd( client_fd )
 * 	puts "The client said, '#{client_socket.readline.chomp}'"
 * 	client_socket.puts "Hello from script one!"
 * 	socket.close
 *
 * 	# In another script, start this second
 * 	require 'socket'
 * 	include Socket::Constants
 * 	socket = Socket.new( AF_INET, SOCK_STREAM, 0 )
 * 	sockaddr = Socket.pack_sockaddr_in( 2200, 'localhost' )
 * 	socket.connect( sockaddr )
 * 	socket.puts "Hello from script 2."
 * 	puts "The server said, '#{socket.readline.chomp}'"
 * 	socket.close
 *
 * Refer to Socket#accept for the exceptions that may be thrown if the call
 * to _sysaccept_ fails.
 *
 * === See
 * * Socket#accept
 */
static VALUE
sock_sysaccept(VALUE sock)
{
    rb_io_t *fptr;
    VALUE sock2;
    struct sockaddr_storage buf;
    socklen_t len = (socklen_t)sizeof buf;

    GetOpenFile(sock, fptr);
    sock2 = rsock_s_accept(0,fptr->fd,(struct sockaddr*)&buf,&len);

    return rb_assoc_new(sock2, rsock_io_socket_addrinfo(sock2, (struct sockaddr*)&buf, len));
}

#ifdef HAVE_GETHOSTNAME
/*
 * call-seq:
 *   Socket.gethostname => hostname
 *
 * Returns the hostname.
 *
 *   p Socket.gethostname #=> "hal"
 *
 * Note that it is not guaranteed to be able to convert to IP address using gethostbyname, getaddrinfo, etc.
 * If you need local IP address, use Socket.ip_address_list.
 */
static VALUE
sock_gethostname(VALUE obj)
{
#ifndef HOST_NAME_MAX
#  define HOST_NAME_MAX 1024
#endif
    char buf[HOST_NAME_MAX+1];

    rb_secure(3);
    if (gethostname(buf, (int)sizeof buf - 1) < 0)
	rb_sys_fail("gethostname");

    buf[sizeof buf - 1] = '\0';
    return rb_str_new2(buf);
}
#else
#ifdef HAVE_UNAME

#include <sys/utsname.h>

static VALUE
sock_gethostname(VALUE obj)
{
    struct utsname un;

    rb_secure(3);
    uname(&un);
    return rb_str_new2(un.nodename);
}
#else
#define sock_gethostname rb_f_notimplement
#endif
#endif

static VALUE
make_addrinfo(struct addrinfo *res0, int norevlookup)
{
    VALUE base, ary;
    struct addrinfo *res;

    if (res0 == NULL) {
	rb_raise(rb_eSocket, "host not found");
    }
    base = rb_ary_new();
    for (res = res0; res; res = res->ai_next) {
	ary = rsock_ipaddr(res->ai_addr, norevlookup);
	if (res->ai_canonname) {
	    RARRAY_PTR(ary)[2] = rb_str_new2(res->ai_canonname);
	}
	rb_ary_push(ary, INT2FIX(res->ai_family));
	rb_ary_push(ary, INT2FIX(res->ai_socktype));
	rb_ary_push(ary, INT2FIX(res->ai_protocol));
	rb_ary_push(base, ary);
    }
    return base;
}

static VALUE
sock_sockaddr(struct sockaddr *addr, size_t len)
{
    char *ptr;

    switch (addr->sa_family) {
      case AF_INET:
	ptr = (char*)&((struct sockaddr_in*)addr)->sin_addr.s_addr;
	len = sizeof(((struct sockaddr_in*)addr)->sin_addr.s_addr);
	break;
#ifdef AF_INET6
      case AF_INET6:
	ptr = (char*)&((struct sockaddr_in6*)addr)->sin6_addr.s6_addr;
	len = sizeof(((struct sockaddr_in6*)addr)->sin6_addr.s6_addr);
	break;
#endif
      default:
        rb_raise(rb_eSocket, "unknown socket family:%d", addr->sa_family);
	break;
    }
    return rb_str_new(ptr, len);
}

/*
 * call-seq:
 *   Socket.gethostbyname(hostname) => [official_hostname, alias_hostnames, address_family, *address_list]
 *
 * Obtains the host information for _hostname_.
 *
 *   p Socket.gethostbyname("hal") #=> ["localhost", ["hal"], 2, "\x7F\x00\x00\x01"]
 *
 */
static VALUE
sock_s_gethostbyname(VALUE obj, VALUE host)
{
    rb_secure(3);
    return rsock_make_hostent(host, rsock_addrinfo(host, Qnil, SOCK_STREAM, AI_CANONNAME), sock_sockaddr);
}

/*
 * call-seq:
 *   Socket.gethostbyaddr(address_string [, address_family]) => hostent
 *
 * Obtains the host information for _address_.
 *
 *   p Socket.gethostbyaddr([221,186,184,68].pack("CCCC"))
 *   #=> ["carbon.ruby-lang.org", [], 2, "\xDD\xBA\xB8D"]
 */
static VALUE
sock_s_gethostbyaddr(int argc, VALUE *argv)
{
    VALUE addr, family;
    struct hostent *h;
    struct sockaddr *sa;
    char **pch;
    VALUE ary, names;
    int t = AF_INET;

    rb_scan_args(argc, argv, "11", &addr, &family);
    sa = (struct sockaddr*)StringValuePtr(addr);
    if (!NIL_P(family)) {
	t = rsock_family_arg(family);
    }
#ifdef AF_INET6
    else if (RSTRING_LEN(addr) == 16) {
	t = AF_INET6;
    }
#endif
    h = gethostbyaddr(RSTRING_PTR(addr), RSTRING_LENINT(addr), t);
    if (h == NULL) {
#ifdef HAVE_HSTRERROR
	extern int h_errno;
	rb_raise(rb_eSocket, "%s", (char*)hstrerror(h_errno));
#else
	rb_raise(rb_eSocket, "host not found");
#endif
    }
    ary = rb_ary_new();
    rb_ary_push(ary, rb_str_new2(h->h_name));
    names = rb_ary_new();
    rb_ary_push(ary, names);
    if (h->h_aliases != NULL) {
	for (pch = h->h_aliases; *pch; pch++) {
	    rb_ary_push(names, rb_str_new2(*pch));
	}
    }
    rb_ary_push(ary, INT2NUM(h->h_addrtype));
#ifdef h_addr
    for (pch = h->h_addr_list; *pch; pch++) {
	rb_ary_push(ary, rb_str_new(*pch, h->h_length));
    }
#else
    rb_ary_push(ary, rb_str_new(h->h_addr, h->h_length));
#endif

    return ary;
}

/*
 * call-seq:
 *   Socket.getservbyname(service_name)                => port_number
 *   Socket.getservbyname(service_name, protocol_name) => port_number
 *
 * Obtains the port number for _service_name_.
 *
 * If _protocol_name_ is not given, "tcp" is assumed.
 *
 *   Socket.getservbyname("smtp")          #=> 25
 *   Socket.getservbyname("shell")         #=> 514
 *   Socket.getservbyname("syslog", "udp") #=> 514
 */
static VALUE
sock_s_getservbyname(int argc, VALUE *argv)
{
    VALUE service, proto;
    struct servent *sp;
    long port;
    const char *servicename, *protoname = "tcp";

    rb_scan_args(argc, argv, "11", &service, &proto);
    StringValue(service);
    if (!NIL_P(proto)) StringValue(proto);
    servicename = StringValueCStr(service);
    if (!NIL_P(proto)) protoname = StringValueCStr(proto);
    sp = getservbyname(servicename, protoname);
    if (sp) {
	port = ntohs(sp->s_port);
    }
    else {
	char *end;

	port = STRTOUL(servicename, &end, 0);
	if (*end != '\0') {
	    rb_raise(rb_eSocket, "no such service %s/%s", servicename, protoname);
	}
    }
    return INT2FIX(port);
}

/*
 * call-seq:
 *   Socket.getservbyport(port [, protocol_name]) => service
 *
 * Obtains the port number for _port_.
 *
 * If _protocol_name_ is not given, "tcp" is assumed.
 *
 *   Socket.getservbyport(80)         #=> "www"
 *   Socket.getservbyport(514, "tcp") #=> "shell"
 *   Socket.getservbyport(514, "udp") #=> "syslog"
 *
 */
static VALUE
sock_s_getservbyport(int argc, VALUE *argv)
{
    VALUE port, proto;
    struct servent *sp;
    long portnum;
    const char *protoname = "tcp";

    rb_scan_args(argc, argv, "11", &port, &proto);
    portnum = NUM2LONG(port);
    if (portnum != (uint16_t)portnum) {
	const char *s = portnum > 0 ? "big" : "small";
	rb_raise(rb_eRangeError, "integer %ld too %s to convert into `int16_t'", portnum, s);
    }
    if (!NIL_P(proto)) protoname = StringValueCStr(proto);

    sp = getservbyport((int)htons((uint16_t)portnum), protoname);
    if (!sp) {
	rb_raise(rb_eSocket, "no such service for port %d/%s", (int)portnum, protoname);
    }
    return rb_tainted_str_new2(sp->s_name);
}

/*
 * call-seq:
 *   Socket.getaddrinfo(nodename, servname[, family[, socktype[, protocol[, flags[, reverse_lookup]]]]]) => array
 *
 * Obtains address information for _nodename_:_servname_.
 *
 * _family_ should be an address family such as: :INET, :INET6, :UNIX, etc.
 *
 * _socktype_ should be a socket type such as: :STREAM, :DGRAM, :RAW, etc.
 *
 * _protocol_ should be a protocol defined in the family.
 * 0 is default protocol for the family.
 *
 * _flags_ should be bitwise OR of Socket::AI_* constants.
 *
 *   Socket.getaddrinfo("www.ruby-lang.org", "http", nil, :STREAM)
 *   #=> [["AF_INET", 80, "carbon.ruby-lang.org", "221.186.184.68", 2, 1, 6]] # PF_INET/SOCK_STREAM/IPPROTO_TCP
 *
 *   Socket.getaddrinfo("localhost", nil)
 *   #=> [["AF_INET", 0, "localhost", "127.0.0.1", 2, 1, 6],  # PF_INET/SOCK_STREAM/IPPROTO_TCP
 *   #    ["AF_INET", 0, "localhost", "127.0.0.1", 2, 2, 17], # PF_INET/SOCK_DGRAM/IPPROTO_UDP
 *   #    ["AF_INET", 0, "localhost", "127.0.0.1", 2, 3, 0]]  # PF_INET/SOCK_RAW/IPPROTO_IP
 *
 * _reverse_lookup_ directs the form of the third element, and has to
 * be one of below.
 * If it is ommitted, the default value is +nil+.
 *
 *   +true+, +:hostname+:  hostname is obtained from numeric address using reverse lookup, which may take a time.
 *   +false+, +:numeric+:  hostname is same as numeric address.
 *   +nil+:              obey to the current +do_not_reverse_lookup+ flag.
 *
 * If Addrinfo object is preferred, use Addrinfo.getaddrinfo.
 */
static VALUE
sock_s_getaddrinfo(int argc, VALUE *argv)
{
    VALUE host, port, family, socktype, protocol, flags, ret, revlookup;
    struct addrinfo hints, *res;
    int norevlookup;

    rb_scan_args(argc, argv, "25", &host, &port, &family, &socktype, &protocol, &flags, &revlookup);

    MEMZERO(&hints, struct addrinfo, 1);
    hints.ai_family = NIL_P(family) ? PF_UNSPEC : rsock_family_arg(family);

    if (!NIL_P(socktype)) {
	hints.ai_socktype = rsock_socktype_arg(socktype);
    }
    if (!NIL_P(protocol)) {
	hints.ai_protocol = NUM2INT(protocol);
    }
    if (!NIL_P(flags)) {
	hints.ai_flags = NUM2INT(flags);
    }
    if (NIL_P(revlookup) || !rsock_revlookup_flag(revlookup, &norevlookup)) {
	norevlookup = rsock_do_not_reverse_lookup;
    }
    res = rsock_getaddrinfo(host, port, &hints, 0);

    ret = make_addrinfo(res, norevlookup);
    freeaddrinfo(res);
    return ret;
}

/*
 * call-seq:
 *   Socket.getnameinfo(sockaddr [, flags]) => [hostname, servicename]
 *
 * Obtains name information for _sockaddr_.
 *
 * _sockaddr_ should be one of follows.
 * - packed sockaddr string such as Socket.sockaddr_in(80, "127.0.0.1")
 * - 3-elements array such as ["AF_INET", 80, "127.0.0.1"]
 * - 4-elements array such as ["AF_INET", 80, ignored, "127.0.0.1"]
 *
 * _flags_ should be bitwise OR of Socket::NI_* constants.
 *
 * Note that the last form is compatible with IPSocket#{addr,peeraddr}.
 *
 *   Socket.getnameinfo(Socket.sockaddr_in(80, "127.0.0.1"))       #=> ["localhost", "www"]
 *   Socket.getnameinfo(["AF_INET", 80, "127.0.0.1"])              #=> ["localhost", "www"]
 *   Socket.getnameinfo(["AF_INET", 80, "localhost", "127.0.0.1"]) #=> ["localhost", "www"]
 *
 * If Addrinfo object is preferred, use Addrinfo#getnameinfo.
 */
static VALUE
sock_s_getnameinfo(int argc, VALUE *argv)
{
    VALUE sa, af = Qnil, host = Qnil, port = Qnil, flags, tmp;
    char *hptr, *pptr;
    char hbuf[1024], pbuf[1024];
    int fl;
    struct addrinfo hints, *res = NULL, *r;
    int error;
    struct sockaddr_storage ss;
    struct sockaddr *sap;

    sa = flags = Qnil;
    rb_scan_args(argc, argv, "11", &sa, &flags);

    fl = 0;
    if (!NIL_P(flags)) {
	fl = NUM2INT(flags);
    }
    tmp = rb_check_sockaddr_string_type(sa);
    if (!NIL_P(tmp)) {
	sa = tmp;
	if (sizeof(ss) < (size_t)RSTRING_LEN(sa)) {
	    rb_raise(rb_eTypeError, "sockaddr length too big");
	}
	memcpy(&ss, RSTRING_PTR(sa), RSTRING_LEN(sa));
	if ((size_t)RSTRING_LEN(sa) != SS_LEN(&ss)) {
	    rb_raise(rb_eTypeError, "sockaddr size differs - should not happen");
	}
	sap = (struct sockaddr*)&ss;
	goto call_nameinfo;
    }
    tmp = rb_check_array_type(sa);
    if (!NIL_P(tmp)) {
	sa = tmp;
	MEMZERO(&hints, struct addrinfo, 1);
	if (RARRAY_LEN(sa) == 3) {
	    af = RARRAY_PTR(sa)[0];
	    port = RARRAY_PTR(sa)[1];
	    host = RARRAY_PTR(sa)[2];
	}
	else if (RARRAY_LEN(sa) >= 4) {
	    af = RARRAY_PTR(sa)[0];
	    port = RARRAY_PTR(sa)[1];
	    host = RARRAY_PTR(sa)[3];
	    if (NIL_P(host)) {
		host = RARRAY_PTR(sa)[2];
	    }
	    else {
		/*
		 * 4th element holds numeric form, don't resolve.
		 * see rsock_ipaddr().
		 */
#ifdef AI_NUMERICHOST /* AIX 4.3.3 doesn't have AI_NUMERICHOST. */
		hints.ai_flags |= AI_NUMERICHOST;
#endif
	    }
	}
	else {
	    rb_raise(rb_eArgError, "array size should be 3 or 4, %ld given",
		     RARRAY_LEN(sa));
	}
	/* host */
	if (NIL_P(host)) {
	    hptr = NULL;
	}
	else {
	    strncpy(hbuf, StringValuePtr(host), sizeof(hbuf));
	    hbuf[sizeof(hbuf) - 1] = '\0';
	    hptr = hbuf;
	}
	/* port */
	if (NIL_P(port)) {
	    strcpy(pbuf, "0");
	    pptr = NULL;
	}
	else if (FIXNUM_P(port)) {
	    snprintf(pbuf, sizeof(pbuf), "%ld", NUM2LONG(port));
	    pptr = pbuf;
	}
	else {
	    strncpy(pbuf, StringValuePtr(port), sizeof(pbuf));
	    pbuf[sizeof(pbuf) - 1] = '\0';
	    pptr = pbuf;
	}
	hints.ai_socktype = (fl & NI_DGRAM) ? SOCK_DGRAM : SOCK_STREAM;
	/* af */
        hints.ai_family = NIL_P(af) ? PF_UNSPEC : rsock_family_arg(af);
	error = rb_getaddrinfo(hptr, pptr, &hints, &res);
	if (error) goto error_exit_addr;
	sap = res->ai_addr;
    }
    else {
	rb_raise(rb_eTypeError, "expecting String or Array");
    }

  call_nameinfo:
    error = rb_getnameinfo(sap, SA_LEN(sap), hbuf, sizeof(hbuf),
			   pbuf, sizeof(pbuf), fl);
    if (error) goto error_exit_name;
    if (res) {
	for (r = res->ai_next; r; r = r->ai_next) {
	    char hbuf2[1024], pbuf2[1024];

	    sap = r->ai_addr;
	    error = rb_getnameinfo(sap, SA_LEN(sap), hbuf2, sizeof(hbuf2),
				   pbuf2, sizeof(pbuf2), fl);
	    if (error) goto error_exit_name;
	    if (strcmp(hbuf, hbuf2) != 0|| strcmp(pbuf, pbuf2) != 0) {
		freeaddrinfo(res);
		rb_raise(rb_eSocket, "sockaddr resolved to multiple nodename");
	    }
	}
	freeaddrinfo(res);
    }
    return rb_assoc_new(rb_str_new2(hbuf), rb_str_new2(pbuf));

  error_exit_addr:
    if (res) freeaddrinfo(res);
    rsock_raise_socket_error("getaddrinfo", error);

  error_exit_name:
    if (res) freeaddrinfo(res);
    rsock_raise_socket_error("getnameinfo", error);
}

/*
 * call-seq:
 *   Socket.sockaddr_in(port, host)      => sockaddr
 *   Socket.pack_sockaddr_in(port, host) => sockaddr
 *
 * Packs _port_ and _host_ as an AF_INET/AF_INET6 sockaddr string.
 *
 *   Socket.sockaddr_in(80, "127.0.0.1")
 *   #=> "\x02\x00\x00P\x7F\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00"
 *
 *   Socket.sockaddr_in(80, "::1")
 *   #=> "\n\x00\x00P\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00"
 *
 */
static VALUE
sock_s_pack_sockaddr_in(VALUE self, VALUE port, VALUE host)
{
    struct addrinfo *res = rsock_addrinfo(host, port, 0, 0);
    VALUE addr = rb_str_new((char*)res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);
    OBJ_INFECT(addr, port);
    OBJ_INFECT(addr, host);

    return addr;
}

/*
 * call-seq:
 *   Socket.unpack_sockaddr_in(sockaddr) => [port, ip_address]
 *
 * Unpacks _sockaddr_ into port and ip_address.
 *
 * _sockaddr_ should be a string or an addrinfo for AF_INET/AF_INET6.
 *
 *   sockaddr = Socket.sockaddr_in(80, "127.0.0.1")
 *   p sockaddr #=> "\x02\x00\x00P\x7F\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00"
 *   p Socket.unpack_sockaddr_in(sockaddr) #=> [80, "127.0.0.1"]
 *
 */
static VALUE
sock_s_unpack_sockaddr_in(VALUE self, VALUE addr)
{
    struct sockaddr_in * sockaddr;
    VALUE host;

    sockaddr = (struct sockaddr_in*)SockAddrStringValuePtr(addr);
    if (RSTRING_LEN(addr) <
        (char*)&((struct sockaddr *)sockaddr)->sa_family +
        sizeof(((struct sockaddr *)sockaddr)->sa_family) -
        (char*)sockaddr)
        rb_raise(rb_eArgError, "too short sockaddr");
    if (((struct sockaddr *)sockaddr)->sa_family != AF_INET
#ifdef INET6
        && ((struct sockaddr *)sockaddr)->sa_family != AF_INET6
#endif
        ) {
#ifdef INET6
        rb_raise(rb_eArgError, "not an AF_INET/AF_INET6 sockaddr");
#else
        rb_raise(rb_eArgError, "not an AF_INET sockaddr");
#endif
    }
    host = rsock_make_ipaddr((struct sockaddr*)sockaddr);
    OBJ_INFECT(host, addr);
    return rb_assoc_new(INT2NUM(ntohs(sockaddr->sin_port)), host);
}

#ifdef HAVE_SYS_UN_H

/*
 * call-seq:
 *   Socket.sockaddr_un(path)      => sockaddr
 *   Socket.pack_sockaddr_un(path) => sockaddr
 *
 * Packs _path_ as an AF_UNIX sockaddr string.
 *
 *   Socket.sockaddr_un("/tmp/sock") #=> "\x01\x00/tmp/sock\x00\x00..."
 *
 */
static VALUE
sock_s_pack_sockaddr_un(VALUE self, VALUE path)
{
    struct sockaddr_un sockaddr;
    char *sun_path;
    VALUE addr;

    MEMZERO(&sockaddr, struct sockaddr_un, 1);
    sockaddr.sun_family = AF_UNIX;
    sun_path = StringValueCStr(path);
    if (sizeof(sockaddr.sun_path) <= strlen(sun_path)) {
        rb_raise(rb_eArgError, "too long unix socket path (max: %dbytes)",
            (int)sizeof(sockaddr.sun_path)-1);
    }
    strncpy(sockaddr.sun_path, sun_path, sizeof(sockaddr.sun_path)-1);
    addr = rb_str_new((char*)&sockaddr, sizeof(sockaddr));
    OBJ_INFECT(addr, path);

    return addr;
}

/*
 * call-seq:
 *   Socket.unpack_sockaddr_un(sockaddr) => path
 *
 * Unpacks _sockaddr_ into path.
 *
 * _sockaddr_ should be a string or an addrinfo for AF_UNIX.
 *
 *   sockaddr = Socket.sockaddr_un("/tmp/sock")
 *   p Socket.unpack_sockaddr_un(sockaddr) #=> "/tmp/sock"
 *
 */
static VALUE
sock_s_unpack_sockaddr_un(VALUE self, VALUE addr)
{
    struct sockaddr_un * sockaddr;
    const char *sun_path;
    VALUE path;

    sockaddr = (struct sockaddr_un*)SockAddrStringValuePtr(addr);
    if (RSTRING_LEN(addr) <
        (char*)&((struct sockaddr *)sockaddr)->sa_family +
        sizeof(((struct sockaddr *)sockaddr)->sa_family) -
        (char*)sockaddr)
        rb_raise(rb_eArgError, "too short sockaddr");
    if (((struct sockaddr *)sockaddr)->sa_family != AF_UNIX) {
        rb_raise(rb_eArgError, "not an AF_UNIX sockaddr");
    }
    if (sizeof(struct sockaddr_un) < (size_t)RSTRING_LEN(addr)) {
	rb_raise(rb_eTypeError, "too long sockaddr_un - %ld longer than %d",
		 RSTRING_LEN(addr), (int)sizeof(struct sockaddr_un));
    }
    sun_path = rsock_unixpath(sockaddr, RSTRING_LENINT(addr));
    if (sizeof(struct sockaddr_un) == RSTRING_LEN(addr) &&
        sun_path == sockaddr->sun_path &&
        sun_path + strlen(sun_path) == RSTRING_PTR(addr) + RSTRING_LEN(addr)) {
        rb_raise(rb_eArgError, "sockaddr_un.sun_path not NUL terminated");
    }
    path = rb_str_new2(sun_path);
    OBJ_INFECT(path, addr);
    return path;
}
#endif

#if defined(HAVE_GETIFADDRS) || defined(SIOCGLIFCONF) || defined(SIOCGIFCONF) || defined(_WIN32)
static VALUE
sockaddr_obj(struct sockaddr *addr)
{
    socklen_t len;
#if defined(AF_INET6) && defined(__KAME__)
    struct sockaddr_in6 addr6;
#endif

    if (addr == NULL)
        return Qnil;

    switch (addr->sa_family) {
      case AF_INET:
        len = (socklen_t)sizeof(struct sockaddr_in);
        break;

#ifdef AF_INET6
      case AF_INET6:
        len = (socklen_t)sizeof(struct sockaddr_in6);
#  ifdef __KAME__
	/* KAME uses the 2nd 16bit word of link local IPv6 address as interface index internally */
        /* http://orange.kame.net/dev/cvsweb.cgi/kame/IMPLEMENTATION */
	/* convert fe80:1::1 to fe80::1%1 */
	memcpy(&addr6, addr, len);
	addr = (struct sockaddr *)&addr6;
	if (IN6_IS_ADDR_LINKLOCAL(&addr6.sin6_addr) &&
	    addr6.sin6_scope_id == 0 &&
	    (addr6.sin6_addr.s6_addr[2] || addr6.sin6_addr.s6_addr[3])) {
	    addr6.sin6_scope_id = (addr6.sin6_addr.s6_addr[2] << 8) | addr6.sin6_addr.s6_addr[3];
	    addr6.sin6_addr.s6_addr[2] = 0;
	    addr6.sin6_addr.s6_addr[3] = 0;
	}
#  endif
        break;
#endif

#ifdef HAVE_SYS_UN_H
      case AF_UNIX:
        len = (socklen_t)sizeof(struct sockaddr_un);
        break;
#endif

      default:
        len = (socklen_t)sizeof(struct sockaddr_in);
        break;
    }
#ifdef SA_LEN
    if (len < (socklen_t)SA_LEN(addr))
	len = (socklen_t)SA_LEN(addr);
#endif

    return rsock_addrinfo_new(addr, len, addr->sa_family, 0, 0, Qnil, Qnil);
}
#endif

#if defined(HAVE_GETIFADDRS) || (defined(SIOCGLIFCONF) && defined(SIOCGLIFNUM) && !defined(__hpux)) || defined(SIOCGIFCONF) ||  defined(_WIN32)
/*
 * call-seq:
 *   Socket.ip_address_list => array
 *
 * Returns local IP addresses as an array.
 *
 * The array contains Addrinfo objects.
 *
 *  pp Socket.ip_address_list
 *  #=> [#<Addrinfo: 127.0.0.1>,
 *       #<Addrinfo: 192.168.0.128>,
 *       #<Addrinfo: ::1>,
 *       ...]
 *
 */
static VALUE
socket_s_ip_address_list(VALUE self)
{
#if defined(HAVE_GETIFADDRS)
    struct ifaddrs *ifp = NULL;
    struct ifaddrs *p;
    int ret;
    VALUE list;

    ret = getifaddrs(&ifp);
    if (ret == -1) {
        rb_sys_fail("getifaddrs");
    }

    list = rb_ary_new();
    for (p = ifp; p; p = p->ifa_next) {
        if (p->ifa_addr != NULL && IS_IP_FAMILY(p->ifa_addr->sa_family)) {
            rb_ary_push(list, sockaddr_obj(p->ifa_addr));
        }
    }

    freeifaddrs(ifp);

    return list;
#elif defined(SIOCGLIFCONF) && defined(SIOCGLIFNUM) && !defined(__hpux)
    /* Solaris if_tcp(7P) */
    /* HP-UX has SIOCGLIFCONF too.  But it uses different struct */
    int fd = -1;
    int ret;
    struct lifnum ln;
    struct lifconf lc;
    char *reason = NULL;
    int save_errno;
    int i;
    VALUE list = Qnil;

    lc.lifc_buf = NULL;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        rb_sys_fail("socket");

    memset(&ln, 0, sizeof(ln));
    ln.lifn_family = AF_UNSPEC;

    ret = ioctl(fd, SIOCGLIFNUM, &ln);
    if (ret == -1) {
	reason = "SIOCGLIFNUM";
	goto finish;
    }

    memset(&lc, 0, sizeof(lc));
    lc.lifc_family = AF_UNSPEC;
    lc.lifc_flags = 0;
    lc.lifc_len = sizeof(struct lifreq) * ln.lifn_count;
    lc.lifc_req = xmalloc(lc.lifc_len);

    ret = ioctl(fd, SIOCGLIFCONF, &lc);
    if (ret == -1) {
	reason = "SIOCGLIFCONF";
	goto finish;
    }

    list = rb_ary_new();
    for (i = 0; i < ln.lifn_count; i++) {
	struct lifreq *req = &lc.lifc_req[i];
        if (IS_IP_FAMILY(req->lifr_addr.ss_family)) {
            if (req->lifr_addr.ss_family == AF_INET6 &&
                IN6_IS_ADDR_LINKLOCAL(&((struct sockaddr_in6 *)(&req->lifr_addr))->sin6_addr) &&
                ((struct sockaddr_in6 *)(&req->lifr_addr))->sin6_scope_id == 0) {
                struct lifreq req2;
                memcpy(req2.lifr_name, req->lifr_name, LIFNAMSIZ);
                ret = ioctl(fd, SIOCGLIFINDEX, &req2);
                if (ret == -1) {
                    reason = "SIOCGLIFINDEX";
                    goto finish;
                }
                ((struct sockaddr_in6 *)(&req->lifr_addr))->sin6_scope_id = req2.lifr_index;
            }
            rb_ary_push(list, sockaddr_obj((struct sockaddr *)&req->lifr_addr));
        }
    }

  finish:
    save_errno = errno;
    if (lc.lifc_buf != NULL)
	xfree(lc.lifc_req);
    if (fd != -1)
	close(fd);
    errno = save_errno;

    if (reason)
	rb_sys_fail(reason);
    return list;

#elif defined(SIOCGIFCONF)
    int fd = -1;
    int ret;
#define EXTRA_SPACE (sizeof(struct ifconf) + sizeof(struct sockaddr_storage))
    char initbuf[4096+EXTRA_SPACE];
    char *buf = initbuf;
    int bufsize;
    struct ifconf conf;
    struct ifreq *req;
    VALUE list = Qnil;
    const char *reason = NULL;
    int save_errno;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        rb_sys_fail("socket");

    bufsize = sizeof(initbuf);
    buf = initbuf;

  retry:
    conf.ifc_len = bufsize;
    conf.ifc_req = (struct ifreq *)buf;

    /* fprintf(stderr, "bufsize: %d\n", bufsize); */

    ret = ioctl(fd, SIOCGIFCONF, &conf);
    if (ret == -1) {
        reason = "SIOCGIFCONF";
        goto finish;
    }

    /* fprintf(stderr, "conf.ifc_len: %d\n", conf.ifc_len); */

    if (bufsize - EXTRA_SPACE < conf.ifc_len) {
	if (bufsize < conf.ifc_len) {
	    /* NetBSD returns required size for all interfaces. */
	    bufsize = conf.ifc_len + EXTRA_SPACE;
	}
	else {
	    bufsize = bufsize << 1;
	}
	if (buf == initbuf)
	    buf = NULL;
	buf = xrealloc(buf, bufsize);
	goto retry;
    }

    close(fd);
    fd = -1;

    list = rb_ary_new();
    req = conf.ifc_req;
    while ((char*)req < (char*)conf.ifc_req + conf.ifc_len) {
	struct sockaddr *addr = &req->ifr_addr;
        if (IS_IP_FAMILY(addr->sa_family)) {
	    rb_ary_push(list, sockaddr_obj(addr));
	}
#ifdef HAVE_SA_LEN
# ifndef _SIZEOF_ADDR_IFREQ
#  define _SIZEOF_ADDR_IFREQ(r) \
          (sizeof(struct ifreq) + \
           (sizeof(struct sockaddr) < (r).ifr_addr.sa_len ? \
            (r).ifr_addr.sa_len - sizeof(struct sockaddr) : \
            0))
# endif
	req = (struct ifreq *)((char*)req + _SIZEOF_ADDR_IFREQ(*req));
#else
	req = (struct ifreq *)((char*)req + sizeof(struct ifreq));
#endif
    }

  finish:

    save_errno = errno;
    if (buf != initbuf)
        xfree(buf);
    if (fd != -1)
	close(fd);
    errno = save_errno;

    if (reason)
	rb_sys_fail(reason);
    return list;

#undef EXTRA_SPACE
#elif defined(_WIN32)
    typedef struct ip_adapter_unicast_address_st {
	unsigned LONG_LONG dummy0;
	struct ip_adapter_unicast_address_st *Next;
	struct {
	    struct sockaddr *lpSockaddr;
	    int iSockaddrLength;
	} Address;
	int dummy1;
	int dummy2;
	int dummy3;
	long dummy4;
	long dummy5;
	long dummy6;
    } ip_adapter_unicast_address_t;
    typedef struct ip_adapter_anycast_address_st {
	unsigned LONG_LONG dummy0;
	struct ip_adapter_anycast_address_st *Next;
	struct {
	    struct sockaddr *lpSockaddr;
	    int iSockaddrLength;
	} Address;
    } ip_adapter_anycast_address_t;
    typedef struct ip_adapter_addresses_st {
	unsigned LONG_LONG dummy0;
	struct ip_adapter_addresses_st *Next;
	void *dummy1;
	ip_adapter_unicast_address_t *FirstUnicastAddress;
	ip_adapter_anycast_address_t *FirstAnycastAddress;
	void *dummy2;
	void *dummy3;
	void *dummy4;
	void *dummy5;
	void *dummy6;
	BYTE dummy7[8];
	DWORD dummy8;
	DWORD dummy9;
	DWORD dummy10;
	DWORD IfType;
	int OperStatus;
	DWORD dummy12;
	DWORD dummy13[16];
	void *dummy14;
    } ip_adapter_addresses_t;
    typedef ULONG (WINAPI *GetAdaptersAddresses_t)(ULONG, ULONG, PVOID, ip_adapter_addresses_t *, PULONG);
    HMODULE h;
    GetAdaptersAddresses_t pGetAdaptersAddresses;
    ULONG len;
    DWORD ret;
    ip_adapter_addresses_t *adapters;
    VALUE list;

    h = LoadLibrary("iphlpapi.dll");
    if (!h)
	rb_notimplement();
    pGetAdaptersAddresses = (GetAdaptersAddresses_t)GetProcAddress(h, "GetAdaptersAddresses");
    if (!pGetAdaptersAddresses) {
	FreeLibrary(h);
	rb_notimplement();
    }

    ret = pGetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &len);
    if (ret != ERROR_SUCCESS && ret != ERROR_BUFFER_OVERFLOW) {
	errno = rb_w32_map_errno(ret);
	FreeLibrary(h);
	rb_sys_fail("GetAdaptersAddresses");
    }
    adapters = (ip_adapter_addresses_t *)ALLOCA_N(BYTE, len);
    ret = pGetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapters, &len);
    if (ret != ERROR_SUCCESS) {
	errno = rb_w32_map_errno(ret);
	FreeLibrary(h);
	rb_sys_fail("GetAdaptersAddresses");
    }

    list = rb_ary_new();
    for (; adapters; adapters = adapters->Next) {
	ip_adapter_unicast_address_t *uni;
	ip_adapter_anycast_address_t *any;
	if (adapters->OperStatus != 1)	/* 1 means IfOperStatusUp */
	    continue;
	for (uni = adapters->FirstUnicastAddress; uni; uni = uni->Next) {
#ifndef INET6
	    if (uni->Address.lpSockaddr->sa_family == AF_INET)
#else
	    if (IS_IP_FAMILY(uni->Address.lpSockaddr->sa_family))
#endif
		rb_ary_push(list, sockaddr_obj(uni->Address.lpSockaddr));
	}
	for (any = adapters->FirstAnycastAddress; any; any = any->Next) {
#ifndef INET6
	    if (any->Address.lpSockaddr->sa_family == AF_INET)
#else
	    if (IS_IP_FAMILY(any->Address.lpSockaddr->sa_family))
#endif
		rb_ary_push(list, sockaddr_obj(any->Address.lpSockaddr));
	}
    }

    FreeLibrary(h);
    return list;
#endif
}
#else
#define socket_s_ip_address_list rb_f_notimplement
#endif

void
Init_socket()
{
    rsock_init_basicsocket();

    /*
     * Document-class: Socket < BasicSocket
     *
     * Class +Socket+ provides access to the underlying operating system
     * socket implementations.  It can be used to provide more operating system
     * specific functionality than the protocol-specific socket classes.
     *
     * The constants defined under Socket::Constants are also defined under
     * Socket.  For example, Socket::AF_INET is usable as well as
     * Socket::Constants::AF_INET.  See Socket::Constants for the list of
     * constants.
     *
     * === What's a socket?
     *
     * Sockets are endpoints of a bidirectionnal communication channel.
     * Sockets can communicate within a process, between processes on the same
     * machine or between different machines.  There are many types of socket:
     * TCPSocket, UDPSocket or UNIXSocket for example.
     *
     * Sockets have their own vocabulary:
     * domain::
     *   The family of protocols:  Socket::PF_INET, Socket::PF_INET6,
     *   Socket::PF_UNIX, etc.
     * type::
     *   The type of communications between the two endpoints, typically
     *   Socket::SOCK_STREAM or Socket::SOCK_DGRAM.
     * protocol::
     *   Typically zero.  This may be used to identify a variant of a
     *   protocol.
     * hostname::
     *   The identifier of a network interface:
     *     * a string (hostname, IPv4 or IPv6 adress or <tt><broadcast></tt>
     *       which specifies a broadcast address)
     *     * a zero-length string which specifies INADDR_ANY
     *     * an integer (interpreted as binary address in host byte order).
     *
     * === Quick start
     *
     * Some classes such as TCPSocket, UDPSocket or UNIXSocket ease use of
     * sockets of these types compared to C programming.
     *
     *   # Creating a TCP socket in a C-like manner
     *   s = Socket.new Socket::INET, Socket::SOCK_STREAM
     *   s.connect Socket.pack_sockaddr_in(80, 'example.com')
     *
     *   # Using TCPSocket
     *   s = TCPSocket.new 'example.com', 80
     *
     * A simple server would look like:
     *
     *   require 'socket'
     *
     *   server = TCPServer.new 2000 # Server bound to port 2000
     *
     *   loop do
     *     client = server.accept    # Wait for a client to connect
     *     client.puts "Hello !"
     *     client.puts "Time is #{Time.now}"
     *     client.close
     *   end
     *
     * A simple client may look like:
     *
     *   require 'socket'
     *
     *   s = TCPSocket.new 'localhost', 2000
     *
     *   while line = s.gets # Read lines from socket
     *     puts line         # and print them
     *   end
     *
     *   s.close             # close socket when done
     *
     * === Exception Handling
     *
     * Ruby's Socket implementation raises exceptions based on the error
     * generated by the system dependent implementation.  This is why the
     * methods are documented in a way that isolate Unix-based system
     * exceptions from Windows based exceptions. If more information on
     * particular exception is needed please refer to the Unix manual pages or
     * the Windows WinSock reference.
     *
     * === Convenient methods
     *
     * Although the general way to create socket is Socket.new,
     * there are several methods for socket creation for most cases.
     *
     * TCP client socket::
     *   Socket.tcp, TCPSocket.open
     * TCP server socket::
     *   Socket.tcp_server_loop, TCPServer.open
     * UNIX client socket::
     *   Socket.unix, UNIXSocket.open
     * UNIX server socket::
     *   Socket.unix_server_loop, UNIXServer.open
     *
     * === Documentation by
     *
     * * Zach Dennis
     * * Sam Roberts
     * * <em>Programming Ruby</em> from The Pragmatic Bookshelf.
     *
     * Much material in this documentation is taken with permission from
     * <em>Programming Ruby</em> from The Pragmatic Bookshelf.
     */
    rb_cSocket = rb_define_class("Socket", rb_cBasicSocket);

    rsock_init_socket_init();

    rb_define_method(rb_cSocket, "initialize", sock_initialize, -1);
    rb_define_method(rb_cSocket, "connect", sock_connect, 1);
    rb_define_method(rb_cSocket, "connect_nonblock", sock_connect_nonblock, 1);
    rb_define_method(rb_cSocket, "bind", sock_bind, 1);
    rb_define_method(rb_cSocket, "listen", rsock_sock_listen, 1);
    rb_define_method(rb_cSocket, "accept", sock_accept, 0);
    rb_define_method(rb_cSocket, "accept_nonblock", sock_accept_nonblock, 0);
    rb_define_method(rb_cSocket, "sysaccept", sock_sysaccept, 0);

    rb_define_method(rb_cSocket, "recvfrom", sock_recvfrom, -1);
    rb_define_method(rb_cSocket, "recvfrom_nonblock", sock_recvfrom_nonblock, -1);

    rb_define_singleton_method(rb_cSocket, "socketpair", rsock_sock_s_socketpair, -1);
    rb_define_singleton_method(rb_cSocket, "pair", rsock_sock_s_socketpair, -1);
    rb_define_singleton_method(rb_cSocket, "gethostname", sock_gethostname, 0);
    rb_define_singleton_method(rb_cSocket, "gethostbyname", sock_s_gethostbyname, 1);
    rb_define_singleton_method(rb_cSocket, "gethostbyaddr", sock_s_gethostbyaddr, -1);
    rb_define_singleton_method(rb_cSocket, "getservbyname", sock_s_getservbyname, -1);
    rb_define_singleton_method(rb_cSocket, "getservbyport", sock_s_getservbyport, -1);
    rb_define_singleton_method(rb_cSocket, "getaddrinfo", sock_s_getaddrinfo, -1);
    rb_define_singleton_method(rb_cSocket, "getnameinfo", sock_s_getnameinfo, -1);
    rb_define_singleton_method(rb_cSocket, "sockaddr_in", sock_s_pack_sockaddr_in, 2);
    rb_define_singleton_method(rb_cSocket, "pack_sockaddr_in", sock_s_pack_sockaddr_in, 2);
    rb_define_singleton_method(rb_cSocket, "unpack_sockaddr_in", sock_s_unpack_sockaddr_in, 1);
#ifdef HAVE_SYS_UN_H
    rb_define_singleton_method(rb_cSocket, "sockaddr_un", sock_s_pack_sockaddr_un, 1);
    rb_define_singleton_method(rb_cSocket, "pack_sockaddr_un", sock_s_pack_sockaddr_un, 1);
    rb_define_singleton_method(rb_cSocket, "unpack_sockaddr_un", sock_s_unpack_sockaddr_un, 1);
#endif

    rb_define_singleton_method(rb_cSocket, "ip_address_list", socket_s_ip_address_list, 0);
}
