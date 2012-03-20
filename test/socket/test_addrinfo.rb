begin
  require "socket"
rescue LoadError
end

require "test/unit"

class TestSocketAddrinfo < Test::Unit::TestCase
  HAS_UNIXSOCKET = defined?(UNIXSocket) && /cygwin/ !~ RUBY_PLATFORM

  def tcp_unspecified_to_loopback(addrinfo)
    if addrinfo.ipv4? && addrinfo.ip_address == "0.0.0.0"
      Addrinfo.tcp("127.0.0.1", addrinfo.ip_port)
    elsif addrinfo.ipv6? && addrinfo.ipv6_unspecified?
      Addrinfo.tcp("::1", addrinfo.ip_port)
    elsif addrinfo.ipv6? && (ai = addrinfo.ipv6_to_ipv4) && ai.ip_address == "0.0.0.0"
      Addrinfo.tcp("127.0.0.1", addrinfo.ip_port)
    else
      addrinfo
    end
  end

  def test_addrinfo_ip
    ai = Addrinfo.ip("127.0.0.1")
    assert_equal([0, "127.0.0.1"], Socket.unpack_sockaddr_in(ai))
    assert_equal(Socket::AF_INET, ai.afamily)
    assert_equal(Socket::PF_INET, ai.pfamily)
    assert_equal(0, ai.socktype)
    assert_equal(0, ai.protocol)
  end

  def test_addrinfo_tcp
    ai = Addrinfo.tcp("127.0.0.1", 80)
    assert_equal([80, "127.0.0.1"], Socket.unpack_sockaddr_in(ai))
    assert_equal(Socket::AF_INET, ai.afamily)
    assert_equal(Socket::PF_INET, ai.pfamily)
    assert_equal(Socket::SOCK_STREAM, ai.socktype)
    assert_include([0, Socket::IPPROTO_TCP], ai.protocol)
  end

  def test_addrinfo_udp
    ai = Addrinfo.udp("127.0.0.1", 80)
    assert_equal([80, "127.0.0.1"], Socket.unpack_sockaddr_in(ai))
    assert_equal(Socket::AF_INET, ai.afamily)
    assert_equal(Socket::PF_INET, ai.pfamily)
    assert_equal(Socket::SOCK_DGRAM, ai.socktype)
    assert_include([0, Socket::IPPROTO_UDP], ai.protocol)
  end

  def test_addrinfo_ip_unpack
    ai = Addrinfo.tcp("127.0.0.1", 80)
    assert_equal(["127.0.0.1", 80], ai.ip_unpack)
    assert_equal("127.0.0.1", ai.ip_address)
    assert_equal(80, ai.ip_port)
  end

  def test_addrinfo_inspect_sockaddr
    ai = Addrinfo.tcp("127.0.0.1", 80)
    assert_equal("127.0.0.1:80", ai.inspect_sockaddr)
  end

  def test_addrinfo_new_inet
    ai = Addrinfo.new(["AF_INET", 46102, "localhost.localdomain", "127.0.0.2"])
    assert_equal([46102, "127.0.0.2"], Socket.unpack_sockaddr_in(ai))
    assert_equal(Socket::AF_INET, ai.afamily)
    assert_equal(Socket::PF_INET, ai.pfamily)
    assert_equal(0, ai.socktype)
    assert_equal(0, ai.protocol)
  end

  def test_addrinfo_predicates
    ipv4_ai = Addrinfo.new(Socket.sockaddr_in(80, "192.168.0.1"))
    assert(ipv4_ai.ip?)
    assert(ipv4_ai.ipv4?)
    assert(!ipv4_ai.ipv6?)
    assert(!ipv4_ai.unix?)
  end

  def test_ipv4_address_predicates
    list = [
      [:ipv4_private?, "10.0.0.0", "10.255.255.255",
                       "172.16.0.0", "172.31.255.255",
                       "192.168.0.0", "192.168.255.255"],
      [:ipv4_loopback?, "127.0.0.1", "127.0.0.0", "127.255.255.255"],
      [:ipv4_multicast?, "224.0.0.0", "224.255.255.255"]
    ]
    list.each {|meth, *addrs|
      addrs.each {|addr|
        assert(Addrinfo.ip(addr).send(meth), "Addrinfo.ip(#{addr.inspect}).#{meth}")
        list.each {|meth2,|
          next if meth == meth2
          assert(!Addrinfo.ip(addr).send(meth2), "!Addrinfo.ip(#{addr.inspect}).#{meth2}")
        }
      }
    }
  end

  def test_basicsocket_send
    s1 = Socket.new(:INET, :DGRAM, 0)
    s1.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    sa = s1.getsockname
    ai = Addrinfo.new(sa)
    s2 = Socket.new(:INET, :DGRAM, 0)
    s2.send("test-basicsocket-send", 0, ai)
    assert_equal("test-basicsocket-send", s1.recv(100))
  ensure
    s1.close if s1 && !s1.closed?
    s2.close if s2 && !s2.closed?
  end

  def test_udpsocket_send
    s1 = UDPSocket.new
    s1.bind("127.0.0.1", 0)
    ai = Addrinfo.new(s1.getsockname)
    s2 = UDPSocket.new
    s2.send("test-udp-send", 0, ai)
    assert_equal("test-udp-send", s1.recv(100))
  ensure
    s1.close if s1 && !s1.closed?
    s2.close if s2 && !s2.closed?
  end

  def test_socket_bind
    s1 = Socket.new(:INET, :DGRAM, 0)
    sa = Socket.sockaddr_in(0, "127.0.0.1")
    ai = Addrinfo.new(sa)
    s1.bind(ai)
    s2 = UDPSocket.new
    s2.send("test-socket-bind", 0, s1.getsockname)
    assert_equal("test-socket-bind", s1.recv(100))
  ensure
    s1.close if s1 && !s1.closed?
    s2.close if s2 && !s2.closed?
  end

  def test_socket_connect
    s1 = Socket.new(:INET, :STREAM, 0)
    s1.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    s1.listen(5)
    ai = Addrinfo.new(s1.getsockname)
    s2 = Socket.new(:INET, :STREAM, 0)
    s2.connect(ai)
    s3, sender_addr = s1.accept
    s2.send("test-socket-connect", 0)
    assert_equal("test-socket-connect", s3.recv(100))
  ensure
    s1.close if s1 && !s1.closed?
    s2.close if s2 && !s2.closed?
    s3.close if s3 && !s3.closed?
  end

  def test_socket_connect_nonblock
    s1 = Socket.new(:INET, :STREAM, 0)
    s1.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    s1.listen(5)
    ai = Addrinfo.new(s1.getsockname)
    s2 = Socket.new(:INET, :STREAM, 0)
    begin
      s2.connect_nonblock(ai)
    rescue IO::WaitWritable
      IO.select(nil, [s2])
      begin
        s2.connect_nonblock(ai)
      rescue Errno::EISCONN
      end
    end
    s3, sender_addr = s1.accept
    s2.send("test-socket-connect-nonblock", 0)
    assert_equal("test-socket-connect-nonblock", s3.recv(100))
  ensure
    s1.close if s1 && !s1.closed?
    s2.close if s2 && !s2.closed?
    s3.close if s3 && !s3.closed?
  end

  def test_socket_getnameinfo
     ai = Addrinfo.udp("127.0.0.1", 8888)
     assert_equal(["127.0.0.1", "8888"], Socket.getnameinfo(ai, Socket::NI_NUMERICHOST|Socket::NI_NUMERICSERV))
  end

  def test_basicsocket_local_address
    s1 = Socket.new(:INET, :DGRAM, 0)
    s1.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    e = Socket.unpack_sockaddr_in(s1.getsockname)
    a = Socket.unpack_sockaddr_in(s1.local_address.to_sockaddr)
    assert_equal(e, a)
    assert_equal(Socket::AF_INET, s1.local_address.afamily)
    assert_equal(Socket::PF_INET, s1.local_address.pfamily)
    assert_equal(Socket::SOCK_DGRAM, s1.local_address.socktype)
  ensure
    s1.close if s1 && !s1.closed?
  end

  def test_basicsocket_remote_address
    s1 = TCPServer.new("127.0.0.1", 0)
    s2 = Socket.new(:INET, :STREAM, 0)
    s2.connect(s1.getsockname)
    s3, _ = s1.accept
    e = Socket.unpack_sockaddr_in(s2.getsockname)
    a = Socket.unpack_sockaddr_in(s3.remote_address.to_sockaddr)
    assert_equal(e, a)
    assert_equal(Socket::AF_INET, s3.remote_address.afamily)
    assert_equal(Socket::PF_INET, s3.remote_address.pfamily)
    assert_equal(Socket::SOCK_STREAM, s3.remote_address.socktype)
  ensure
    s1.close if s1 && !s1.closed?
    s2.close if s2 && !s2.closed?
    s3.close if s3 && !s3.closed?
  end

  def test_socket_accept
    serv = Socket.new(:INET, :STREAM, 0)
    serv.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    serv.listen(5)
    c = Socket.new(:INET, :STREAM, 0)
    c.connect(serv.local_address)
    ret = serv.accept
    s, ai = ret
    assert_kind_of(Array, ret)
    assert_equal(2, ret.length)
    assert_kind_of(Addrinfo, ai)
    e = Socket.unpack_sockaddr_in(c.getsockname)
    a = Socket.unpack_sockaddr_in(ai.to_sockaddr)
    assert_equal(e, a)
  ensure
    serv.close if serv && !serv.closed?
    s.close if s && !s.closed?
    c.close if c && !c.closed?
  end

  def test_socket_accept_nonblock
    serv = Socket.new(:INET, :STREAM, 0)
    serv.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    serv.listen(5)
    c = Socket.new(:INET, :STREAM, 0)
    c.connect(serv.local_address)
    begin
      ret = serv.accept_nonblock
    rescue IO::WaitReadable, Errno::EINTR
      IO.select([serv])
      retry
    end
    s, ai = ret
    assert_kind_of(Array, ret)
    assert_equal(2, ret.length)
    assert_kind_of(Addrinfo, ai)
    e = Socket.unpack_sockaddr_in(c.getsockname)
    a = Socket.unpack_sockaddr_in(ai.to_sockaddr)
    assert_equal(e, a)
  ensure
    serv.close if serv && !serv.closed?
    s.close if s && !s.closed?
    c.close if c && !c.closed?
  end

  def test_socket_sysaccept
    serv = Socket.new(:INET, :STREAM, 0)
    serv.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    serv.listen(5)
    c = Socket.new(:INET, :STREAM, 0)
    c.connect(serv.local_address)
    ret = serv.sysaccept
    fd, ai = ret
    s = IO.new(fd)
    assert_kind_of(Array, ret)
    assert_equal(2, ret.length)
    assert_kind_of(Addrinfo, ai)
    e = Socket.unpack_sockaddr_in(c.getsockname)
    a = Socket.unpack_sockaddr_in(ai.to_sockaddr)
    assert_equal(e, a)
  ensure
    serv.close if serv && !serv.closed?
    s.close if s && !s.closed?
    c.close if c && !c.closed?
  end

  def test_socket_recvfrom
    s1 = Socket.new(:INET, :DGRAM, 0)
    s1.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    s2 = Socket.new(:INET, :DGRAM, 0)
    s2.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    s2.send("test-socket-recvfrom", 0, s1.getsockname)
    data, ai = s1.recvfrom(100)
    assert_equal("test-socket-recvfrom", data)
    assert_kind_of(Addrinfo, ai)
    e = Socket.unpack_sockaddr_in(s2.getsockname)
    a = Socket.unpack_sockaddr_in(ai.to_sockaddr)
    assert_equal(e, a)
  ensure
    s1.close if s1 && !s1.closed?
    s2.close if s2 && !s2.closed?
  end

  def test_socket_recvfrom_nonblock
    s1 = Socket.new(:INET, :DGRAM, 0)
    s1.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    s2 = Socket.new(:INET, :DGRAM, 0)
    s2.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    s2.send("test-socket-recvfrom", 0, s1.getsockname)
    begin
      data, ai = s1.recvfrom_nonblock(100)
    rescue IO::WaitReadable
      IO.select([s1])
      retry
    end
    assert_equal("test-socket-recvfrom", data)
    assert_kind_of(Addrinfo, ai)
    e = Socket.unpack_sockaddr_in(s2.getsockname)
    a = Socket.unpack_sockaddr_in(ai.to_sockaddr)
    assert_equal(e, a)
  ensure
    s1.close if s1 && !s1.closed?
    s2.close if s2 && !s2.closed?
  end

  def test_family_addrinfo
    ai = Addrinfo.tcp("0.0.0.0", 4649).family_addrinfo("127.0.0.1", 80)
    assert_equal(["127.0.0.1", 80], ai.ip_unpack)
    assert_equal(Socket::SOCK_STREAM, ai.socktype)
    return unless Addrinfo.respond_to?(:unix)
    ai = Addrinfo.unix("/testdir/sock").family_addrinfo("/testdir/sock2")
    assert_equal("/testdir/sock2", ai.unix_path)
    assert_equal(Socket::SOCK_STREAM, ai.socktype)
    assert_raise(SocketError) { Addrinfo.tcp("0.0.0.0", 4649).family_addrinfo("::1", 80) }
  end

  def random_port
    # IANA suggests dynamic port for 49152 to 65535
    # http://www.iana.org/assignments/port-numbers
    49152 + rand(65535-49152+1)
  end

  def errors_addrinuse
    [Errno::EADDRINUSE]
  end

  def test_connect_from
    TCPServer.open("0.0.0.0", 0) {|serv|
      serv_ai = Addrinfo.new(serv.getsockname, :INET, :STREAM)
      serv_ai = tcp_unspecified_to_loopback(serv_ai)
      port = random_port
      begin
        serv_ai.connect_from("0.0.0.0", port) {|s1|
          s2 = serv.accept
          begin
            assert_equal(port, s2.remote_address.ip_port)
          ensure
            s2.close
          end
        }
      rescue *errors_addrinuse
        # not test failure
      end
    }

    TCPServer.open("0.0.0.0", 0) {|serv|
      serv_ai = Addrinfo.new(serv.getsockname, :INET, :STREAM)
      serv_ai = tcp_unspecified_to_loopback(serv_ai)
      port = random_port
      begin
        serv_ai.connect_from(Addrinfo.tcp("0.0.0.0", port)) {|s1|
          s2 = serv.accept
          begin
            assert_equal(port, s2.remote_address.ip_port)
          ensure
            s2.close
          end
        }
      rescue *errors_addrinuse
        # not test failure
      end
    }
  end

  def test_connect_to
    TCPServer.open("0.0.0.0", 0) {|serv|
      serv_ai = Addrinfo.new(serv.getsockname, :INET, :STREAM)
      serv_ai = tcp_unspecified_to_loopback(serv_ai)
      port = random_port
      client_ai = Addrinfo.tcp("0.0.0.0", port)
      begin
        client_ai.connect_to(*serv_ai.ip_unpack) {|s1|
          s2 = serv.accept
          begin
            assert_equal(port, s2.remote_address.ip_port)
          ensure
            s2.close
          end
        }
      rescue *errors_addrinuse
        # not test failure
      end
    }
  end

  def test_connect
    TCPServer.open("0.0.0.0", 0) {|serv|
      serv_ai = Addrinfo.new(serv.getsockname, :INET, :STREAM)
      serv_ai = tcp_unspecified_to_loopback(serv_ai)
      begin
        serv_ai.connect {|s1|
          s2 = serv.accept
          begin
            assert_equal(s1.local_address.ip_unpack, s2.remote_address.ip_unpack)
            assert_equal(s2.local_address.ip_unpack, s1.remote_address.ip_unpack)
          ensure
            s2.close
          end
        }
      rescue *errors_addrinuse
        # not test failure
      end
    }
  end

  def test_bind
    port = random_port
    client_ai = Addrinfo.tcp("0.0.0.0", port)
    begin
      client_ai.bind {|s|
        assert_equal(port, s.local_address.ip_port)
      }
    rescue *errors_addrinuse
      # not test failure
    end
  end

  def test_listen
    port = random_port
    client_ai = Addrinfo.tcp("0.0.0.0", port)
    begin
      client_ai.listen {|serv|
        assert_equal(port, serv.local_address.ip_port)
        serv_addr, serv_port = serv.local_address.ip_unpack
        case serv_addr
	when "0.0.0.0" then serv_addr = "127.0.0.1"
	end
        TCPSocket.open(serv_addr, serv_port) {|s1|
          s2, addr = serv.accept
          begin
            assert_equal(s1.local_address.ip_unpack, addr.ip_unpack)
          ensure
            s2.close
          end
        }
      }
    rescue *errors_addrinuse
      # not test failure
    end
  end

  def test_s_foreach
    Addrinfo.foreach(nil, 80, nil, :STREAM) {|ai|
      assert_kind_of(Addrinfo, ai)
    }
  end

  def test_marshal
    ai1 = Addrinfo.tcp("127.0.0.1", 80)
    ai2 = Marshal.load(Marshal.dump(ai1))
    assert_equal(ai1.afamily, ai2.afamily)
    assert_equal(ai1.ip_unpack, ai2.ip_unpack)
    assert_equal(ai1.pfamily, ai2.pfamily)
    assert_equal(ai1.socktype, ai2.socktype)
    assert_equal(ai1.protocol, ai2.protocol)
    assert_equal(ai1.canonname, ai2.canonname)
  end

  if Socket.const_defined?("AF_INET6") && Socket::AF_INET6.is_a?(Integer)

    def test_addrinfo_new_inet6
      ai = Addrinfo.new(["AF_INET6", 42304, "ip6-localhost", "::1"])
      assert_equal([42304, "::1"], Socket.unpack_sockaddr_in(ai))
      assert_equal(Socket::AF_INET6, ai.afamily)
      assert_equal(Socket::PF_INET6, ai.pfamily)
      assert_equal(0, ai.socktype)
      assert_equal(0, ai.protocol)
    end

    def test_addrinfo_ip_unpack_inet6
      ai = Addrinfo.tcp("::1", 80)
      assert_equal(["::1", 80], ai.ip_unpack)
      assert_equal("::1", ai.ip_address)
      assert_equal(80, ai.ip_port)
    end

    def test_addrinfo_inspect_sockaddr_inet6
      ai = Addrinfo.tcp("::1", 80)
      assert_equal("[::1]:80", ai.inspect_sockaddr)
    end

    def test_marshal_inet6
      ai1 = Addrinfo.tcp("::1", 80)
      ai2 = Marshal.load(Marshal.dump(ai1))
      assert_equal(ai1.afamily, ai2.afamily)
      assert_equal(ai1.ip_unpack, ai2.ip_unpack)
      assert_equal(ai1.pfamily, ai2.pfamily)
      assert_equal(ai1.socktype, ai2.socktype)
      assert_equal(ai1.protocol, ai2.protocol)
      assert_equal(ai1.canonname, ai2.canonname)
    end

    def ipv6(str)
      Addrinfo.getaddrinfo(str, nil, :INET6, :DGRAM).fetch(0)
    end

    def test_ipv6_address_predicates
      list = [
        [:ipv6_unspecified?, "::"],
        [:ipv6_loopback?, "::1"],
        [:ipv6_v4compat?, "::0.0.0.2", "::255.255.255.255"],
        [:ipv6_v4mapped?, "::ffff:0.0.0.0", "::ffff:255.255.255.255"],
        [:ipv6_linklocal?, "fe80::", "febf::"],
        [:ipv6_sitelocal?, "fec0::", "feef::"],
        [:ipv6_multicast?, "ff00::", "ffff::"]
      ]
      mlist = [
        [:ipv6_mc_nodelocal?, "ff01::", "ff11::"],
        [:ipv6_mc_linklocal?, "ff02::", "ff12::"],
        [:ipv6_mc_sitelocal?, "ff05::", "ff15::"],
        [:ipv6_mc_orglocal?, "ff08::", "ff18::"],
        [:ipv6_mc_global?, "ff0e::", "ff1e::"]
      ]
      list.each {|meth, *addrs|
        addrs.each {|addr|
          addr_exp = "Addrinfo.getaddrinfo(#{addr.inspect}, nil, :INET6, :DGRAM).fetch(0)"
	  if meth == :ipv6_v4compat? || meth == :ipv6_v4mapped?
	    # MacOS X returns IPv4 address for ::ffff:1.2.3.4 and ::1.2.3.4.
            # Solaris returns IPv4 address for ::ffff:1.2.3.4.
	    ai = ipv6(addr)
	    assert(ai.ipv4? || ai.send(meth), "ai=#{addr_exp}; ai.ipv4? || .#{meth}")
	  else
	    assert(ipv6(addr).send(meth), "#{addr_exp}.#{meth}")
            assert_equal(addr, ipv6(addr).ip_address)
	  end
          list.each {|meth2,|
            next if meth == meth2
            assert(!ipv6(addr).send(meth2), "!#{addr_exp}.#{meth2}")
          }
        }
      }
      mlist.each {|meth, *addrs|
        addrs.each {|addr|
          addr_exp = "Addrinfo.getaddrinfo(#{addr.inspect}, nil, :INET6, :DGRAM).fetch(0)"
          assert(ipv6(addr).send(meth), "#{addr_exp}.#{meth}")
          assert(ipv6(addr).ipv6_multicast?, "#{addr_exp}.ipv6_multicast?")
          mlist.each {|meth2,|
            next if meth == meth2
            assert(!ipv6(addr).send(meth2), "!#{addr_exp}.#{meth2}")
          }
          list.each {|meth2,|
            next if :ipv6_multicast? == meth2
            assert(!ipv6(addr).send(meth2), "!#{addr_exp}.#{meth2}")
          }
        }
      }
    end

    def test_ipv6_to_ipv4
      ai = Addrinfo.ip("::192.0.2.3")
      ai = ai.ipv6_to_ipv4 if !ai.ipv4?
      assert(ai.ipv4?)
      assert_equal("192.0.2.3", ai.ip_address)

      ai = Addrinfo.ip("::ffff:192.0.2.3")
      ai = ai.ipv6_to_ipv4 if !ai.ipv4?
      assert(ai.ipv4?)
      assert_equal("192.0.2.3", ai.ip_address)

      assert_nil(Addrinfo.ip("::1").ipv6_to_ipv4)
      assert_nil(Addrinfo.ip("192.0.2.3").ipv6_to_ipv4)
      if HAS_UNIXSOCKET
        assert_nil(Addrinfo.unix("/testdir/sock").ipv6_to_ipv4)
      end
    end
  end

  if HAS_UNIXSOCKET

    def test_addrinfo_unix
      ai = Addrinfo.unix("/testdir/sock")
      assert_equal("/testdir/sock", Socket.unpack_sockaddr_un(ai))
      assert_equal(Socket::AF_UNIX, ai.afamily)
      assert_equal(Socket::PF_UNIX, ai.pfamily)
      assert_equal(Socket::SOCK_STREAM, ai.socktype)
      assert_equal(0, ai.protocol)
    end

    def test_addrinfo_unix_dgram
      ai = Addrinfo.unix("/testdir/sock", :DGRAM)
      assert_equal("/testdir/sock", Socket.unpack_sockaddr_un(ai))
      assert_equal(Socket::AF_UNIX, ai.afamily)
      assert_equal(Socket::PF_UNIX, ai.pfamily)
      assert_equal(Socket::SOCK_DGRAM, ai.socktype)
      assert_equal(0, ai.protocol)
    end

    def test_addrinfo_unix_path
      ai = Addrinfo.unix("/testdir/sock1")
      assert_equal("/testdir/sock1", ai.unix_path)
    end

    def test_addrinfo_inspect_sockaddr_unix
      ai = Addrinfo.unix("/testdir/test_addrinfo_inspect_sockaddr_unix")
      assert_equal("/testdir/test_addrinfo_inspect_sockaddr_unix", ai.inspect_sockaddr)
    end

    def test_addrinfo_new_unix
      ai = Addrinfo.new(["AF_UNIX", "/testdir/sock"])
      assert_equal("/testdir/sock", Socket.unpack_sockaddr_un(ai))
      assert_equal(Socket::AF_UNIX, ai.afamily)
      assert_equal(Socket::PF_UNIX, ai.pfamily)
      assert_equal(Socket::SOCK_STREAM, ai.socktype) # UNIXSocket/UNIXServer is SOCK_STREAM only.
      assert_equal(0, ai.protocol)
    end

    def test_addrinfo_predicates_unix
      unix_ai = Addrinfo.new(Socket.sockaddr_un("/testdir/sososo"))
      assert(!unix_ai.ip?)
      assert(!unix_ai.ipv4?)
      assert(!unix_ai.ipv6?)
      assert(unix_ai.unix?)
    end

    def test_marshal_unix
      ai1 = Addrinfo.unix("/testdir/sock")
      ai2 = Marshal.load(Marshal.dump(ai1))
      assert_equal(ai1.afamily, ai2.afamily)
      assert_equal(ai1.unix_path, ai2.unix_path)
      assert_equal(ai1.pfamily, ai2.pfamily)
      assert_equal(ai1.socktype, ai2.socktype)
      assert_equal(ai1.protocol, ai2.protocol)
      assert_equal(ai1.canonname, ai2.canonname)
    end

  end
end
