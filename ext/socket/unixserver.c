/************************************************

  unixserver.c -

  created at: Thu Mar 31 12:21:29 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

************************************************/

#include "rubysocket.h"

#ifdef HAVE_SYS_UN_H
/*
 * call-seq:
 *   UNIXServer.new(path) => unixserver
 *
 * Creates a new UNIX server socket bound to _path_.
 *
 *   serv = UNIXServer.new("/tmp/sock")
 *   s = serv.accept
 *   p s.read
 */
static VALUE
unix_svr_init(VALUE sock, VALUE path)
{
    return rsock_init_unixsock(sock, path, 1);
}

/*
 * call-seq:
 *   unixserver.accept => unixsocket
 *
 * Accepts a new connection.
 * It returns new UNIXSocket object.
 *
 *   UNIXServer.open("/tmp/sock") {|serv|
 *     UNIXSocket.open("/tmp/sock") {|c|
 *       s = serv.accept
 *       s.puts "hi"
 *       s.close
 *       p c.read #=> "hi\n"
 *     }
 *   }
 *
 */
static VALUE
unix_accept(VALUE sock)
{
    rb_io_t *fptr;
    struct sockaddr_un from;
    socklen_t fromlen;

    GetOpenFile(sock, fptr);
    fromlen = (socklen_t)sizeof(struct sockaddr_un);
    return rsock_s_accept(rb_cUNIXSocket, fptr->fd,
		          (struct sockaddr*)&from, &fromlen);
}

/*
 * call-seq:
 * 	unixserver.accept_nonblock => unixsocket
 *
 * Accepts an incoming connection using accept(2) after
 * O_NONBLOCK is set for the underlying file descriptor.
 * It returns an accepted UNIXSocket for the incoming connection.
 *
 * === Example
 * 	require 'socket'
 * 	serv = UNIXServer.new("/tmp/sock")
 * 	begin # emulate blocking accept
 * 	  sock = serv.accept_nonblock
 * 	rescue IO::WaitReadable, Errno::EINTR
 * 	  IO.select([serv])
 * 	  retry
 * 	end
 * 	# sock is an accepted socket.
 *
 * Refer to Socket#accept for the exceptions that may be thrown if the call
 * to UNIXServer#accept_nonblock fails.
 *
 * UNIXServer#accept_nonblock may raise any error corresponding to accept(2) failure,
 * including Errno::EWOULDBLOCK.
 *
 * If the exception is Errno::EWOULDBLOCK, Errno::AGAIN, Errno::ECONNABORTED or Errno::EPROTO,
 * it is extended by IO::WaitReadable.
 * So IO::WaitReadable can be used to rescue the exceptions for retrying accept_nonblock.
 *
 * === See
 * * UNIXServer#accept
 * * Socket#accept
 */
static VALUE
unix_accept_nonblock(VALUE sock)
{
    rb_io_t *fptr;
    struct sockaddr_un from;
    socklen_t fromlen;

    GetOpenFile(sock, fptr);
    fromlen = (socklen_t)sizeof(from);
    return rsock_s_accept_nonblock(rb_cUNIXSocket, fptr,
			           (struct sockaddr *)&from, &fromlen);
}

/*
 * call-seq:
 *   unixserver.sysaccept => file_descriptor
 *
 * Accepts a new connection.
 * It returns the new file descriptor which is an integer.
 *
 *   UNIXServer.open("/tmp/sock") {|serv|
 *     UNIXSocket.open("/tmp/sock") {|c|
 *       fd = serv.sysaccept
 *       s = IO.new(fd)
 *       s.puts "hi"
 *       s.close
 *       p c.read #=> "hi\n"
 *     }
 *   }
 *
 */
static VALUE
unix_sysaccept(VALUE sock)
{
    rb_io_t *fptr;
    struct sockaddr_un from;
    socklen_t fromlen;

    GetOpenFile(sock, fptr);
    fromlen = (socklen_t)sizeof(struct sockaddr_un);
    return rsock_s_accept(0, fptr->fd, (struct sockaddr*)&from, &fromlen);
}

#endif

void
rsock_init_unixserver(void)
{
#ifdef HAVE_SYS_UN_H
    /*
     * Document-class: UNIXServer < UNIXSocket
     *
     * UNIXServer represents a UNIX domain stream server socket.
     *
     */
    rb_cUNIXServer = rb_define_class("UNIXServer", rb_cUNIXSocket);
    rb_define_method(rb_cUNIXServer, "initialize", unix_svr_init, 1);
    rb_define_method(rb_cUNIXServer, "accept", unix_accept, 0);
    rb_define_method(rb_cUNIXServer, "accept_nonblock", unix_accept_nonblock, 0);
    rb_define_method(rb_cUNIXServer, "sysaccept", unix_sysaccept, 0);
    rb_define_method(rb_cUNIXServer, "listen", rsock_sock_listen, 1); /* in socket.c */
#endif
}
