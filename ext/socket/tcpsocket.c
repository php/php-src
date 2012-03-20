/************************************************

  tcpsocket.c -

  created at: Thu Mar 31 12:21:29 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

************************************************/

#include "rubysocket.h"

/*
 * call-seq:
 *    TCPSocket.new(remote_host, remote_port, local_host=nil, local_port=nil)
 *
 * Opens a TCP connection to +remote_host+ on +remote_port+.  If +local_host+
 * and +local_port+ are specified, then those parameters are used on the local
 * end to establish the connection.
 */
static VALUE
tcp_init(int argc, VALUE *argv, VALUE sock)
{
    VALUE remote_host, remote_serv;
    VALUE local_host, local_serv;

    rb_scan_args(argc, argv, "22", &remote_host, &remote_serv,
			&local_host, &local_serv);

    return rsock_init_inetsock(sock, remote_host, remote_serv,
			       local_host, local_serv, INET_CLIENT);
}

static VALUE
tcp_sockaddr(struct sockaddr *addr, size_t len)
{
    return rsock_make_ipaddr(addr);
}

/*
 * call-seq:
 *   TCPSocket.gethostbyname(hostname) => [official_hostname, alias_hostnames, address_family, *address_list]
 *
 * Lookups host information by _hostname_.
 *
 *   TCPSocket.gethostbyname("localhost")
 *   #=> ["localhost", ["hal"], 2, "127.0.0.1"]
 *
 */
static VALUE
tcp_s_gethostbyname(VALUE obj, VALUE host)
{
    rb_secure(3);
    return rsock_make_hostent(host, rsock_addrinfo(host, Qnil, SOCK_STREAM, AI_CANONNAME),
			tcp_sockaddr);
}

void
rsock_init_tcpsocket(void)
{
    /*
     * Document-class: TCPSocket < IPSocket
     *
     * TCPSocket represents a TCP/IP client socket.
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
     */
    rb_cTCPSocket = rb_define_class("TCPSocket", rb_cIPSocket);
    rb_define_singleton_method(rb_cTCPSocket, "gethostbyname", tcp_s_gethostbyname, 1);
    rb_define_method(rb_cTCPSocket, "initialize", tcp_init, -1);
}
