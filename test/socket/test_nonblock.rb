begin
  require "socket"
rescue LoadError
end

require "test/unit"
require "tempfile"
require "timeout"

class TestSocketNonblock < Test::Unit::TestCase
  def test_accept_nonblock
    serv = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    serv.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    serv.listen(5)
    assert_raise(IO::WaitReadable) { serv.accept_nonblock }
    c = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    c.connect(serv.getsockname)
    begin
      s, sockaddr = serv.accept_nonblock
    rescue IO::WaitReadable
      IO.select [serv]
      s, sockaddr = serv.accept_nonblock
    end
    assert_equal(Socket.unpack_sockaddr_in(c.getsockname), Socket.unpack_sockaddr_in(sockaddr))
  ensure
    serv.close if serv
    c.close if c
    s.close if s
  end

  def test_connect_nonblock
    serv = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    serv.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    serv.listen(5)
    c = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    servaddr = serv.getsockname
    begin
      c.connect_nonblock(servaddr)
    rescue IO::WaitWritable
      IO.select nil, [c]
      assert_nothing_raised {
        begin
          c.connect_nonblock(servaddr)
        rescue Errno::EISCONN
        end
      }
    end
    s, sockaddr = serv.accept
    assert_equal(Socket.unpack_sockaddr_in(c.getsockname), Socket.unpack_sockaddr_in(sockaddr))
  ensure
    serv.close if serv
    c.close if c
    s.close if s
  end

  def test_udp_recvfrom_nonblock
    u1 = UDPSocket.new
    u2 = UDPSocket.new
    u1.bind("127.0.0.1", 0)
    assert_raise(IO::WaitReadable) { u1.recvfrom_nonblock(100) }
    assert_raise(IO::WaitReadable, Errno::EINVAL) { u2.recvfrom_nonblock(100) }
    u2.send("aaa", 0, u1.getsockname)
    IO.select [u1]
    mesg, inet_addr = u1.recvfrom_nonblock(100)
    assert_equal(4, inet_addr.length)
    assert_equal("aaa", mesg)
    af, port, host, addr = inet_addr
    u2_port, u2_addr = Socket.unpack_sockaddr_in(u2.getsockname)
    assert_equal(u2_port, port)
    assert_raise(IO::WaitReadable) { u1.recvfrom_nonblock(100) }
    u2.send("", 0, u1.getsockname)
    assert_nothing_raised("cygwin 1.5.19 has a problem to send an empty UDP packet. [ruby-dev:28915]") {
      timeout(1) { IO.select [u1] }
    }
    mesg, inet_addr = u1.recvfrom_nonblock(100)
    assert_equal("", mesg)
  ensure
    u1.close if u1
    u2.close if u2
  end

  def test_udp_recv_nonblock
    u1 = UDPSocket.new
    u2 = UDPSocket.new
    u1.bind("127.0.0.1", 0)
    assert_raise(IO::WaitReadable) { u1.recv_nonblock(100) }
    assert_raise(IO::WaitReadable, Errno::EINVAL) { u2.recv_nonblock(100) }
    u2.send("aaa", 0, u1.getsockname)
    IO.select [u1]
    mesg = u1.recv_nonblock(100)
    assert_equal("aaa", mesg)
    assert_raise(IO::WaitReadable) { u1.recv_nonblock(100) }
    u2.send("", 0, u1.getsockname)
    assert_nothing_raised("cygwin 1.5.19 has a problem to send an empty UDP packet. [ruby-dev:28915]") {
      timeout(1) { IO.select [u1] }
    }
    mesg = u1.recv_nonblock(100)
    assert_equal("", mesg)
  ensure
    u1.close if u1
    u2.close if u2
  end

  def test_socket_recvfrom_nonblock
    s1 = Socket.new(Socket::AF_INET, Socket::SOCK_DGRAM, 0)
    s1.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    s2 = Socket.new(Socket::AF_INET, Socket::SOCK_DGRAM, 0)
    assert_raise(IO::WaitReadable) { s1.recvfrom_nonblock(100) }
    assert_raise(IO::WaitReadable, Errno::EINVAL) { s2.recvfrom_nonblock(100) }
    s2.send("aaa", 0, s1.getsockname)
    IO.select [s1]
    mesg, sockaddr = s1.recvfrom_nonblock(100)
    assert_equal("aaa", mesg)
    port, addr = Socket.unpack_sockaddr_in(sockaddr)
    s2_port, s2_addr = Socket.unpack_sockaddr_in(s2.getsockname)
    assert_equal(s2_port, port)
  ensure
    s1.close if s1
    s2.close if s2
  end

  def tcp_pair
    serv = TCPServer.new("127.0.0.1", 0)
    af, port, host, addr = serv.addr
    c = TCPSocket.new(addr, port)
    s = serv.accept
    if block_given?
      begin
        yield c, s
      ensure
        c.close if !c.closed?
        s.close if !s.closed?
      end
    else
      return c, s
    end
  ensure
    serv.close if serv && !serv.closed?
  end

  def udp_pair
    s1 = UDPSocket.new
    s1.bind('127.0.0.1', 0)
    af, port1, host, addr1 = s1.addr

    s2 = UDPSocket.new
    s2.bind('127.0.0.1', 0)
    af, port2, host, addr2 = s2.addr

    s1.connect(addr2, port2)
    s2.connect(addr1, port1)

    if block_given?
      begin
        yield s1, s2
      ensure
        s1.close if !s1.closed?
        s2.close if !s2.closed?
      end
    else
      return s1, s2
    end
  end

  def test_tcp_recv_nonblock
    c, s = tcp_pair
    assert_raise(IO::WaitReadable) { c.recv_nonblock(100) }
    assert_raise(IO::WaitReadable) { s.recv_nonblock(100) }
    c.write("abc")
    IO.select [s]
    assert_equal("a", s.recv_nonblock(1))
    assert_equal("bc", s.recv_nonblock(100))
    assert_raise(IO::WaitReadable) { s.recv_nonblock(100) }
  ensure
    c.close if c
    s.close if s
  end

  def test_read_nonblock
    c, s = tcp_pair
    assert_raise(IO::WaitReadable) { c.read_nonblock(100) }
    assert_raise(IO::WaitReadable) { s.read_nonblock(100) }
    c.write("abc")
    IO.select [s]
    assert_equal("a", s.read_nonblock(1))
    assert_equal("bc", s.read_nonblock(100))
    assert_raise(IO::WaitReadable) { s.read_nonblock(100) }
  ensure
    c.close if c
    s.close if s
  end

=begin
  def test_write_nonblock
    c, s = tcp_pair
    str = "a" * 10000
    _, ws, _ = IO.select(nil, [c], nil)
    assert_equal([c], ws)
    ret = c.write_nonblock(str)
    assert_operator(ret, :>, 0)
    loop {
      assert_raise(IO::WaitWritable) {
        loop {
          ret = c.write_nonblock(str)
          assert_operator(ret, :>, 0)
        }
      }
      _, ws, _ = IO.select(nil, [c], nil, 0)
      break if !ws
    }
  ensure
    c.close if c
    s.close if s
  end
=end

  def test_sendmsg_nonblock_error
    udp_pair {|s1, s2|
      begin
        loop {
          s1.sendmsg_nonblock("a" * 100000)
        }
      rescue NotImplementedError, Errno::ENOSYS
        skip "sendmsg not implemented on this platform."
      rescue Errno::EMSGSIZE
        # UDP has 64K limit (if no Jumbograms).  No problem.
      rescue Errno::EWOULDBLOCK
        assert_kind_of(IO::WaitWritable, $!)
      end
    }
  end

  def test_recvmsg_nonblock_error
    udp_pair {|s1, s2|
      begin
        s1.recvmsg_nonblock(4096)
      rescue NotImplementedError
        skip "recvmsg not implemented on this platform."
      rescue Errno::EWOULDBLOCK
        assert_kind_of(IO::WaitReadable, $!)
      end
    }
  end

  def test_recv_nonblock_error
    tcp_pair {|c, s|
      begin
        c.recv_nonblock(4096)
      rescue Errno::EWOULDBLOCK
        assert_kind_of(IO::WaitReadable, $!)
      end
    }
  end

  def test_connect_nonblock_error
    serv = TCPServer.new("127.0.0.1", 0)
    af, port, host, addr = serv.addr
    c = Socket.new(:INET, :STREAM)
    begin
      c.connect_nonblock(Socket.sockaddr_in(port, "127.0.0.1"))
    rescue Errno::EINPROGRESS
      assert_kind_of(IO::WaitWritable, $!)
    end
  ensure
    serv.close if serv && !serv.closed?
    c.close if c && !c.closed?
  end

  def test_accept_nonblock_error
    serv = Socket.new(:INET, :STREAM)
    serv.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    serv.listen(5)
    port = serv.local_address.ip_port
    begin
      s, _ = serv.accept_nonblock
    rescue Errno::EWOULDBLOCK
      assert_kind_of(IO::WaitReadable, $!)
    end
  ensure
    serv.close if serv && !serv.closed?
    s.close if s && !s.closed?
  end

end if defined?(Socket)
