begin
  require "socket"
  require "tmpdir"
  require "fcntl"
  require "test/unit"
rescue LoadError
end

class TestSocket < Test::Unit::TestCase
  def test_socket_new
    begin
      s = Socket.new(:INET, :STREAM)
      assert_kind_of(Socket, s)
    ensure
      s.close
    end
  end

  def test_socket_new_cloexec
    return unless defined? Fcntl::FD_CLOEXEC
    begin
      s = Socket.new(:INET, :STREAM)
      assert(s.close_on_exec?)
    ensure
      s.close
    end
  end

  def test_unpack_sockaddr
    sockaddr_in = Socket.sockaddr_in(80, "")
    assert_raise(ArgumentError) { Socket.unpack_sockaddr_un(sockaddr_in) }
    sockaddr_un = Socket.sockaddr_un("/testdir/s")
    assert_raise(ArgumentError) { Socket.unpack_sockaddr_in(sockaddr_un) }
    assert_raise(ArgumentError) { Socket.unpack_sockaddr_in("") }
    assert_raise(ArgumentError) { Socket.unpack_sockaddr_un("") }
  end if Socket.respond_to?(:sockaddr_un)

  def test_sysaccept
    serv = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    serv.bind(Socket.sockaddr_in(0, "127.0.0.1"))
    serv.listen 5
    c = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    c.connect(serv.getsockname)
    fd, peeraddr = serv.sysaccept
    assert_equal(c.getsockname, peeraddr.to_sockaddr)
  ensure
    serv.close if serv
    c.close if c
    IO.for_fd(fd).close if fd
  end

  def test_initialize
    Socket.open(Socket::AF_INET, Socket::SOCK_STREAM, 0) {|s|
      s.bind(Socket.sockaddr_in(0, "127.0.0.1"))
      addr = s.getsockname
      assert_nothing_raised { Socket.unpack_sockaddr_in(addr) }
      assert_raise(ArgumentError, NoMethodError) { Socket.unpack_sockaddr_un(addr) }
    }
    Socket.open("AF_INET", "SOCK_STREAM", 0) {|s|
      s.bind(Socket.sockaddr_in(0, "127.0.0.1"))
      addr = s.getsockname
      assert_nothing_raised { Socket.unpack_sockaddr_in(addr) }
      assert_raise(ArgumentError, NoMethodError) { Socket.unpack_sockaddr_un(addr) }
    }
    Socket.open(:AF_INET, :SOCK_STREAM, 0) {|s|
      s.bind(Socket.sockaddr_in(0, "127.0.0.1"))
      addr = s.getsockname
      assert_nothing_raised { Socket.unpack_sockaddr_in(addr) }
      assert_raise(ArgumentError, NoMethodError) { Socket.unpack_sockaddr_un(addr) }
    }
  end

  def test_getaddrinfo
    # This should not send a DNS query because AF_UNIX.
    assert_raise(SocketError) { Socket.getaddrinfo("www.kame.net", 80, "AF_UNIX") }
  end

  def test_getaddrinfo_raises_no_errors_on_port_argument_of_0 # [ruby-core:29427]
    assert_nothing_raised('[ruby-core:29427]'){ Socket.getaddrinfo('localhost', 0, Socket::AF_INET, Socket::SOCK_STREAM, nil, Socket::AI_CANONNAME) }
    assert_nothing_raised('[ruby-core:29427]'){ Socket.getaddrinfo('localhost', '0', Socket::AF_INET, Socket::SOCK_STREAM, nil, Socket::AI_CANONNAME) }
    assert_nothing_raised('[ruby-core:29427]'){ Socket.getaddrinfo('localhost', '00', Socket::AF_INET, Socket::SOCK_STREAM, nil, Socket::AI_CANONNAME) }
    assert_raise(SocketError, '[ruby-core:29427]'){ Socket.getaddrinfo(nil, nil, Socket::AF_INET, Socket::SOCK_STREAM, nil, Socket::AI_CANONNAME) }
    assert_nothing_raised('[ruby-core:29427]'){ TCPServer.open('localhost', 0) {} }
  end


  def test_getnameinfo
    assert_raise(SocketError) { Socket.getnameinfo(["AF_UNIX", 80, "0.0.0.0"]) }
  end

  def test_ip_address_list
    begin
      list = Socket.ip_address_list
    rescue NotImplementedError
      return
    end
    list.each {|ai|
      assert_instance_of(Addrinfo, ai)
      assert(ai.ip?)
    }
  end

  def test_tcp
    TCPServer.open(0) {|serv|
      addr = serv.connect_address
      addr.connect {|s1|
        s2 = serv.accept
        begin
          assert_equal(s2.remote_address.ip_unpack, s1.local_address.ip_unpack)
        ensure
          s2.close
        end
      }
    }
  end

  def test_tcp_cloexec
    return unless defined? Fcntl::FD_CLOEXEC
    TCPServer.open(0) {|serv|
      addr = serv.connect_address
      addr.connect {|s1|
        s2 = serv.accept
        begin
          assert(s2.close_on_exec?)
        ensure
          s2.close
        end
      }

    }
  end

  def random_port
    # IANA suggests dynamic port for 49152 to 65535
    # http://www.iana.org/assignments/port-numbers
    49152 + rand(65535-49152+1)
  end

  def errors_addrinuse
    [Errno::EADDRINUSE]
  end

  def test_tcp_server_sockets
    port = random_port
    begin
      sockets = Socket.tcp_server_sockets(port)
    rescue *errors_addrinuse
      return # not test failure
    end
    begin
      sockets.each {|s|
        assert_equal(port, s.local_address.ip_port)
      }
    ensure
      sockets.each {|s|
        s.close
      }
    end
  end

  def test_tcp_server_sockets_port0
    sockets = Socket.tcp_server_sockets(0)
    ports = sockets.map {|s| s.local_address.ip_port }
    the_port = ports.first
    ports.each {|port|
      assert_equal(the_port, port)
    }
  ensure
    if sockets
      sockets.each {|s|
        s.close
      }
    end
  end

  if defined? UNIXSocket
    def test_unix
      Dir.mktmpdir {|tmpdir|
        path = "#{tmpdir}/sock"
        UNIXServer.open(path) {|serv|
          Socket.unix(path) {|s1|
            s2 = serv.accept
            begin
              s2raddr = s2.remote_address
              s1laddr = s1.local_address
              assert(s2raddr.to_sockaddr.empty? ||
                     s1laddr.to_sockaddr.empty? ||
                     s2raddr.unix_path == s1laddr.unix_path)
              assert(s2.close_on_exec?)
            ensure
              s2.close
            end
          }
        }
      }
    end

    def test_unix_server_socket
      Dir.mktmpdir {|tmpdir|
        path = "#{tmpdir}/sock"
        2.times {
          serv = Socket.unix_server_socket(path)
          begin
            assert_kind_of(Socket, serv)
            assert(File.socket?(path))
            assert_equal(path, serv.local_address.unix_path)
          ensure
            serv.close
          end
        }
      }
    end

    def test_accept_loop_with_unix
      Dir.mktmpdir {|tmpdir|
        tcp_servers = []
        clients = []
        accepted = []
        begin
          tcp_servers = Socket.tcp_server_sockets(0)
          unix_server = Socket.unix_server_socket("#{tmpdir}/sock")
          tcp_servers.each {|s|
            addr = s.connect_address
            assert_nothing_raised("connect to #{addr.inspect}") {
              clients << addr.connect
            }
          }
          addr = unix_server.connect_address
          assert_nothing_raised("connect to #{addr.inspect}") {
            clients << addr.connect
          }
          Socket.accept_loop(tcp_servers, unix_server) {|s|
            accepted << s
            break if clients.length == accepted.length
          }
          assert_equal(clients.length, accepted.length)
        ensure
          tcp_servers.each {|s| s.close if !s.closed?  }
          unix_server.close if unix_server && !unix_server.closed?
          clients.each {|s| s.close if !s.closed?  }
          accepted.each {|s| s.close if !s.closed?  }
        end
      }
    end
  end

  def test_accept_loop
    servers = []
    begin
      servers = Socket.tcp_server_sockets(0)
      port = servers[0].local_address.ip_port
      Socket.tcp("localhost", port) {|s1|
        Socket.accept_loop(servers) {|s2, client_ai|
          begin
            assert_equal(s1.local_address.ip_unpack, client_ai.ip_unpack)
          ensure
            s2.close
          end
          break
        }
      }
    ensure
      servers.each {|s| s.close if !s.closed?  }
    end
  end

  def test_accept_loop_multi_port
    servers = []
    begin
      servers = Socket.tcp_server_sockets(0)
      port = servers[0].local_address.ip_port
      servers2 = Socket.tcp_server_sockets(0)
      servers.concat servers2
      port2 = servers2[0].local_address.ip_port

      Socket.tcp("localhost", port) {|s1|
        Socket.accept_loop(servers) {|s2, client_ai|
          begin
            assert_equal(s1.local_address.ip_unpack, client_ai.ip_unpack)
          ensure
            s2.close
          end
          break
        }
      }
      Socket.tcp("localhost", port2) {|s1|
        Socket.accept_loop(servers) {|s2, client_ai|
          begin
            assert_equal(s1.local_address.ip_unpack, client_ai.ip_unpack)
          ensure
            s2.close
          end
          break
        }
      }
    ensure
      servers.each {|s| s.close if !s.closed?  }
    end
  end

  def test_udp_server
    begin
      ip_addrs = Socket.ip_address_list
    rescue NotImplementedError
      skip "Socket.ip_address_list not implemented"
    end

    Socket.udp_server_sockets(0) {|sockets|
      famlies = {}
      sockets.each {|s| famlies[s.local_address.afamily] = true }
      ip_addrs.reject! {|ai| !famlies[ai.afamily] }
      skipped = false
      begin
        port = sockets.first.local_address.ip_port

        th = Thread.new {
          Socket.udp_server_loop_on(sockets) {|msg, msg_src|
            break if msg == "exit"
            rmsg = Marshal.dump([msg, msg_src.remote_address, msg_src.local_address])
            msg_src.reply rmsg
          }
        }

        ip_addrs.each {|ai|
          if /linux/ =~ RUBY_PLATFORM && ai.ip_address.include?('%') &&
            (`uname -r`[/[0-9.]+/].split('.').map(&:to_i) <=> [2,6,18]) <= 0
            # Cent OS 5.6 (2.6.18-238.19.1.el5xen) doesn't correctly work
            # sendmsg with pktinfo for link-local ipv6 addresses
            next
          end
          Addrinfo.udp(ai.ip_address, port).connect {|s|
            msg1 = "<<<#{ai.inspect}>>>"
            s.sendmsg msg1
            unless IO.select([s], nil, nil, 10)
              raise "no response from #{ai.inspect}"
            end
            msg2, addr = s.recvmsg
            msg2, remote_address, local_address = Marshal.load(msg2)
            assert_equal(msg1, msg2)
            assert_equal(ai.ip_address, addr.ip_address)
          }
        }
      rescue NotImplementedError, Errno::ENOSYS
        skipped = true
        skip "need sendmsg and recvmsg"
      ensure
        if th
          if skipped
            Thread.kill th unless th.join(10)
          else
            Addrinfo.udp("127.0.0.1", port).connect {|s| s.sendmsg "exit" }
            unless th.join(10)
              Thread.kill th
              th.join(10)
              raise "thread killed"
            end
          end
        end
      end
    }
  end

  def test_linger
    opt = Socket::Option.linger(true, 0)
    assert_equal([true, 0], opt.linger)
    Addrinfo.tcp("127.0.0.1", 0).listen {|serv|
      serv.local_address.connect {|s1|
        s2, _ = serv.accept
        begin
          s1.setsockopt(opt)
          s1.close
          assert_raise(Errno::ECONNRESET) { s2.read }
        ensure
          s2.close
        end
      }
    }
  end

  def test_timestamp
    return if /linux|freebsd|netbsd|openbsd|solaris|darwin/ !~ RUBY_PLATFORM
    return if !defined?(Socket::AncillaryData)
    t1 = Time.now.strftime("%Y-%m-%d")
    stamp = nil
    Addrinfo.udp("127.0.0.1", 0).bind {|s1|
      Addrinfo.udp("127.0.0.1", 0).bind {|s2|
        s1.setsockopt(:SOCKET, :TIMESTAMP, true)
        s2.send "a", 0, s1.local_address
        msg, addr, rflags, stamp = s1.recvmsg
        assert_equal("a", msg)
        assert(stamp.cmsg_is?(:SOCKET, :TIMESTAMP))
      }
    }
    t2 = Time.now.strftime("%Y-%m-%d")
    pat = Regexp.union([t1, t2].uniq)
    assert_match(pat, stamp.inspect)
    t = stamp.timestamp
    assert_match(pat, t.strftime("%Y-%m-%d"))
    pat = /\.#{"%06d" % t.usec}/
    assert_match(pat, stamp.inspect)
  end

  def test_timestampns
    return if /linux/ !~ RUBY_PLATFORM || !defined?(Socket::SO_TIMESTAMPNS)
    t1 = Time.now.strftime("%Y-%m-%d")
    stamp = nil
    Addrinfo.udp("127.0.0.1", 0).bind {|s1|
      Addrinfo.udp("127.0.0.1", 0).bind {|s2|
        begin
          s1.setsockopt(:SOCKET, :TIMESTAMPNS, true)
        rescue Errno::ENOPROTOOPT
          # SO_TIMESTAMPNS is available since Linux 2.6.22
          return
        end
        s2.send "a", 0, s1.local_address
        msg, addr, rflags, stamp = s1.recvmsg
        assert_equal("a", msg)
        assert(stamp.cmsg_is?(:SOCKET, :TIMESTAMPNS))
      }
    }
    t2 = Time.now.strftime("%Y-%m-%d")
    pat = Regexp.union([t1, t2].uniq)
    assert_match(pat, stamp.inspect)
    t = stamp.timestamp
    assert_match(pat, t.strftime("%Y-%m-%d"))
    pat = /\.#{"%09d" % t.nsec}/
    assert_match(pat, stamp.inspect)
  end

  def test_bintime
    return if /freebsd/ !~ RUBY_PLATFORM
    t1 = Time.now.strftime("%Y-%m-%d")
    stamp = nil
    Addrinfo.udp("127.0.0.1", 0).bind {|s1|
      Addrinfo.udp("127.0.0.1", 0).bind {|s2|
        s1.setsockopt(:SOCKET, :BINTIME, true)
        s2.send "a", 0, s1.local_address
        msg, addr, rflags, stamp = s1.recvmsg
        assert_equal("a", msg)
        assert(stamp.cmsg_is?(:SOCKET, :BINTIME))
      }
    }
    t2 = Time.now.strftime("%Y-%m-%d")
    pat = Regexp.union([t1, t2].uniq)
    assert_match(pat, stamp.inspect)
    t = stamp.timestamp
    assert_match(pat, t.strftime("%Y-%m-%d"))
    assert_equal(stamp.data[-8,8].unpack("Q")[0], t.subsec * 2**64)
  end

  def test_closed_read
    require 'timeout'
    require 'socket'
    bug4390 = '[ruby-core:35203]'
    server = TCPServer.new("localhost", 0)
    serv_thread = Thread.new {server.accept}
    begin sleep(0.1) end until serv_thread.stop?
    sock = TCPSocket.new("localhost", server.addr[1])
    client_thread = Thread.new do
      sock.readline
    end
    begin sleep(0.1) end until client_thread.stop?
    Timeout.timeout(1) do
      sock.close
      sock = nil
      assert_raise(IOError, bug4390) {client_thread.join}
    end
  ensure
    server.close
  end

  def test_connect_timeout
    host = "127.0.0.1"
    server = TCPServer.new(host, 0)
    port = server.addr[1]
    serv_thread = Thread.new {server.accept}
    sock = Socket.tcp(host, port, :connect_timeout => 30)
    accepted = serv_thread.value
    assert_kind_of TCPSocket, accepted
    assert_equal sock, IO.select(nil, [ sock ])[1][0], "not writable"
    sock.close

    # some platforms may not timeout when the listener queue overflows,
    # but we know Linux does with the default listen backlog of SOMAXCONN for
    # TCPServer.
    assert_raises(Errno::ETIMEDOUT) do
      (Socket::SOMAXCONN*2).times do |i|
        sock = Socket.tcp(host, port, :connect_timeout => 0)
        assert_equal sock, IO.select(nil, [ sock ])[1][0],
                     "not writable (#{i})"
        sock.close
      end
    end if RUBY_PLATFORM =~ /linux/
  ensure
    server.close
    accepted.close if accepted
    sock.close if sock && ! sock.closed?
  end
end if defined?(Socket)
