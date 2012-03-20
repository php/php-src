/************************************************

  udpsocket.c -

  created at: Thu Mar 31 12:21:29 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

************************************************/

#include "rubysocket.h"

/*
 * call-seq:
 *   UDPSocket.new([address_family]) => socket
 *
 * Creates a new UDPSocket object.
 *
 * _address_family_ should be an integer, a string or a symbol:
 * Socket::AF_INET, "AF_INET", :INET, etc.
 *
 *   UDPSocket.new                   #=> #<UDPSocket:fd 3>
 *   UDPSocket.new(Socket::AF_INET6) #=> #<UDPSocket:fd 4>
 *
 */
static VALUE
udp_init(int argc, VALUE *argv, VALUE sock)
{
    VALUE arg;
    int family = AF_INET;
    int fd;

    rb_secure(3);
    if (rb_scan_args(argc, argv, "01", &arg) == 1) {
	family = rsock_family_arg(arg);
    }
    fd = rsock_socket(family, SOCK_DGRAM, 0);
    if (fd < 0) {
	rb_sys_fail("socket(2) - udp");
    }

    return rsock_init_sock(sock, fd);
}

struct udp_arg
{
    struct addrinfo *res;
    int fd;
};

static VALUE
udp_connect_internal(struct udp_arg *arg)
{
    int fd = arg->fd;
    struct addrinfo *res;

    for (res = arg->res; res; res = res->ai_next) {
	if (rsock_connect(fd, res->ai_addr, res->ai_addrlen, 0) >= 0) {
	    return Qtrue;
	}
    }
    return Qfalse;
}

VALUE rsock_freeaddrinfo(struct addrinfo *addr);

/*
 * call-seq:
 *   udpsocket.connect(host, port) => 0
 *
 * Connects _udpsocket_ to _host_:_port_.
 *
 * This makes possible to send without destination address.
 *
 *   u1 = UDPSocket.new
 *   u1.bind("127.0.0.1", 4913)
 *   u2 = UDPSocket.new
 *   u2.connect("127.0.0.1", 4913)
 *   u2.send "uuuu", 0
 *   p u1.recvfrom(10) #=> ["uuuu", ["AF_INET", 33230, "localhost", "127.0.0.1"]]
 *
 */
static VALUE
udp_connect(VALUE sock, VALUE host, VALUE port)
{
    rb_io_t *fptr;
    struct udp_arg arg;
    VALUE ret;

    rb_secure(3);
    arg.res = rsock_addrinfo(host, port, SOCK_DGRAM, 0);
    GetOpenFile(sock, fptr);
    arg.fd = fptr->fd;
    ret = rb_ensure(udp_connect_internal, (VALUE)&arg,
		    rsock_freeaddrinfo, (VALUE)arg.res);
    if (!ret) rb_sys_fail("connect(2)");
    return INT2FIX(0);
}

/*
 * call-seq:
 *   udpsocket.bind(host, port) #=> 0
 *
 * Binds _udpsocket_ to _host_:_port_.
 *
 *   u1 = UDPSocket.new
 *   u1.bind("127.0.0.1", 4913)
 *   u1.send "message-to-self", 0, "127.0.0.1", 4913
 *   p u1.recvfrom(10) #=> ["message-to", ["AF_INET", 4913, "localhost", "127.0.0.1"]]
 *
 */
static VALUE
udp_bind(VALUE sock, VALUE host, VALUE port)
{
    rb_io_t *fptr;
    struct addrinfo *res0, *res;

    rb_secure(3);
    res0 = rsock_addrinfo(host, port, SOCK_DGRAM, 0);
    GetOpenFile(sock, fptr);
    for (res = res0; res; res = res->ai_next) {
	if (bind(fptr->fd, res->ai_addr, res->ai_addrlen) < 0) {
	    continue;
	}
	freeaddrinfo(res0);
	return INT2FIX(0);
    }
    freeaddrinfo(res0);
    rb_sys_fail("bind(2)");
    return INT2FIX(0);
}

/*
 * call-seq:
 *   udpsocket.send(mesg, flags, host, port)  => numbytes_sent
 *   udpsocket.send(mesg, flags, sockaddr_to) => numbytes_sent
 *   udpsocket.send(mesg, flags)              => numbytes_sent
 *
 * Sends _mesg_ via _udpsocket_.
 *
 * _flags_ should be a bitwise OR of Socket::MSG_* constants.
 *
 *   u1 = UDPSocket.new
 *   u1.bind("127.0.0.1", 4913)
 *
 *   u2 = UDPSocket.new
 *   u2.send "hi", 0, "127.0.0.1", 4913
 *
 *   mesg, addr = u1.recvfrom(10)
 *   u1.send mesg, 0, addr[3], addr[1]
 *
 *   p u2.recv(100) #=> "hi"
 *
 */
static VALUE
udp_send(int argc, VALUE *argv, VALUE sock)
{
    VALUE flags, host, port;
    rb_io_t *fptr;
    int n;
    struct addrinfo *res0, *res;
    struct rsock_send_arg arg;

    if (argc == 2 || argc == 3) {
	return rsock_bsock_send(argc, argv, sock);
    }
    rb_secure(4);
    rb_scan_args(argc, argv, "4", &arg.mesg, &flags, &host, &port);

    StringValue(arg.mesg);
    res0 = rsock_addrinfo(host, port, SOCK_DGRAM, 0);
    GetOpenFile(sock, fptr);
    arg.fd = fptr->fd;
    arg.flags = NUM2INT(flags);
    for (res = res0; res; res = res->ai_next) {
      retry:
	arg.to = res->ai_addr;
	arg.tolen = res->ai_addrlen;
	rb_thread_fd_writable(arg.fd);
	n = (int)BLOCKING_REGION_FD(rsock_sendto_blocking, &arg);
	if (n >= 0) {
	    freeaddrinfo(res0);
	    return INT2FIX(n);
	}
	if (rb_io_wait_writable(fptr->fd)) {
	    goto retry;
	}
    }
    freeaddrinfo(res0);
    rb_sys_fail("sendto(2)");
    return INT2FIX(n);
}

/*
 * call-seq:
 * 	udpsocket.recvfrom_nonblock(maxlen) => [mesg, sender_inet_addr]
 * 	udpsocket.recvfrom_nonblock(maxlen, flags) => [mesg, sender_inet_addr]
 *
 * Receives up to _maxlen_ bytes from +udpsocket+ using recvfrom(2) after
 * O_NONBLOCK is set for the underlying file descriptor.
 * If _maxlen_ is omitted, its default value is 65536.
 * _flags_ is zero or more of the +MSG_+ options.
 * The first element of the results, _mesg_, is the data received.
 * The second element, _sender_inet_addr_, is an array to represent the sender address.
 *
 * When recvfrom(2) returns 0,
 * Socket#recvfrom_nonblock returns an empty string as data.
 * It means an empty packet.
 *
 * === Parameters
 * * +maxlen+ - the number of bytes to receive from the socket
 * * +flags+ - zero or more of the +MSG_+ options
 *
 * === Example
 * 	require 'socket'
 * 	s1 = UDPSocket.new
 * 	s1.bind("127.0.0.1", 0)
 * 	s2 = UDPSocket.new
 * 	s2.bind("127.0.0.1", 0)
 * 	s2.connect(*s1.addr.values_at(3,1))
 * 	s1.connect(*s2.addr.values_at(3,1))
 * 	s1.send "aaa", 0
 * 	begin # emulate blocking recvfrom
 * 	  p s2.recvfrom_nonblock(10)  #=> ["aaa", ["AF_INET", 33302, "localhost.localdomain", "127.0.0.1"]]
 * 	rescue IO::WaitReadable
 * 	  IO.select([s2])
 * 	  retry
 * 	end
 *
 * Refer to Socket#recvfrom for the exceptions that may be thrown if the call
 * to _recvfrom_nonblock_ fails.
 *
 * UDPSocket#recvfrom_nonblock may raise any error corresponding to recvfrom(2) failure,
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
udp_recvfrom_nonblock(int argc, VALUE *argv, VALUE sock)
{
    return rsock_s_recvfrom_nonblock(sock, argc, argv, RECV_IP);
}

void
rsock_init_udpsocket(void)
{
    /*
     * Document-class: UDPSocket < IPSocket
     *
     * UDPSocket represents a UDP/IP socket.
     *
     */
    rb_cUDPSocket = rb_define_class("UDPSocket", rb_cIPSocket);
    rb_define_method(rb_cUDPSocket, "initialize", udp_init, -1);
    rb_define_method(rb_cUDPSocket, "connect", udp_connect, 2);
    rb_define_method(rb_cUDPSocket, "bind", udp_bind, 2);
    rb_define_method(rb_cUDPSocket, "send", udp_send, -1);
    rb_define_method(rb_cUDPSocket, "recvfrom_nonblock", udp_recvfrom_nonblock, -1);
}

