begin
  require "socket"
rescue LoadError
end

require "test/unit"
require "tempfile"
require "tmpdir"
require "thread"
require "io/nonblock"

class TestSocket_UNIXSocket < Test::Unit::TestCase
  def test_fd_passing
    r1, w = IO.pipe
    s1, s2 = UNIXSocket.pair
    begin
      s1.send_io(nil)
    rescue NotImplementedError
      assert_raise(NotImplementedError) { s2.recv_io }
    rescue TypeError
      s1.send_io(r1)
      r2 = s2.recv_io
      assert_equal(r1.stat.ino, r2.stat.ino)
      assert_not_equal(r1.fileno, r2.fileno)
      assert(r2.close_on_exec?)
      w.syswrite "a"
      assert_equal("a", r2.sysread(10))
    ensure
      s1.close
      s2.close
      w.close
      r1.close
      r2.close if r2 && !r2.closed?
    end
  end

  def test_fd_passing_n
    io_ary = []
    return if !defined?(Socket::SCM_RIGHTS)
    io_ary.concat IO.pipe
    io_ary.concat IO.pipe
    io_ary.concat IO.pipe
    send_io_ary = []
    io_ary.each {|io|
      send_io_ary << io
      UNIXSocket.pair {|s1, s2|
        begin
          ret = s1.sendmsg("\0", 0, nil, [Socket::SOL_SOCKET, Socket::SCM_RIGHTS,
                                          send_io_ary.map {|io2| io2.fileno }.pack("i!*")])
        rescue NotImplementedError
          return
        end
        assert_equal(1, ret)
        ret = s2.recvmsg(:scm_rights=>true)
        data, srcaddr, flags, *ctls = ret
        recv_io_ary = []
        ctls.each {|ctl|
          next if ctl.level != Socket::SOL_SOCKET || ctl.type != Socket::SCM_RIGHTS
          recv_io_ary.concat ctl.unix_rights
        }
        assert_equal(send_io_ary.length, recv_io_ary.length)
        send_io_ary.length.times {|i|
          assert_not_equal(send_io_ary[i].fileno, recv_io_ary[i].fileno)
          assert(File.identical?(send_io_ary[i], recv_io_ary[i]))
          assert(recv_io_ary[i].close_on_exec?)
        }
      }
    }
  ensure
    io_ary.each {|io| io.close if !io.closed? }
  end

  def test_fd_passing_n2
    io_ary = []
    return if !defined?(Socket::SCM_RIGHTS)
    return if !defined?(Socket::AncillaryData)
    io_ary.concat IO.pipe
    io_ary.concat IO.pipe
    io_ary.concat IO.pipe
    send_io_ary = []
    io_ary.each {|io|
      send_io_ary << io
      UNIXSocket.pair {|s1, s2|
        begin
          ancdata = Socket::AncillaryData.unix_rights(*send_io_ary)
          ret = s1.sendmsg("\0", 0, nil, ancdata)
        rescue NotImplementedError
          return
        end
        assert_equal(1, ret)
        ret = s2.recvmsg(:scm_rights=>true)
        data, srcaddr, flags, *ctls = ret
        recv_io_ary = []
        ctls.each {|ctl|
          next if ctl.level != Socket::SOL_SOCKET || ctl.type != Socket::SCM_RIGHTS
          recv_io_ary.concat ctl.unix_rights
        }
        assert_equal(send_io_ary.length, recv_io_ary.length)
        send_io_ary.length.times {|i|
          assert_not_equal(send_io_ary[i].fileno, recv_io_ary[i].fileno)
          assert(File.identical?(send_io_ary[i], recv_io_ary[i]))
          assert(recv_io_ary[i].close_on_exec?)
        }
      }
    }
  ensure
    io_ary.each {|io| io.close if !io.closed? }
  end

  def test_fd_passing_race_condition
    r1, w = IO.pipe
    s1, s2 = UNIXSocket.pair
    s1.nonblock = s2.nonblock = true
    lock = Mutex.new
    nr = 0
    x = 2
    y = 1000
    begin
      s1.send_io(nil)
    rescue NotImplementedError
      assert_raise(NotImplementedError) { s2.recv_io }
    rescue TypeError
      thrs = x.times.map do
        Thread.new do
          y.times do
            s2.recv_io.close
            lock.synchronize { nr += 1 }
          end
          true
        end
      end
      (x * y).times { s1.send_io r1 }
      assert_equal([true]*x, thrs.map { |t| t.value })
      assert_equal x * y, nr
    ensure
      s1.close
      s2.close
      w.close
      r1.close
    end
  end

  def test_sendmsg
    return if !defined?(Socket::SCM_RIGHTS)
    IO.pipe {|r1, w|
      UNIXSocket.pair {|s1, s2|
        begin
          ret = s1.sendmsg("\0", 0, nil, [Socket::SOL_SOCKET, Socket::SCM_RIGHTS, [r1.fileno].pack("i!")])
        rescue NotImplementedError
          return
        end
        assert_equal(1, ret)
        r2 = s2.recv_io
        begin
          assert(File.identical?(r1, r2))
          assert(r2.close_on_exec?)
        ensure
          r2.close
        end
      }
    }
  end

  def test_sendmsg_ancillarydata_int
    return if !defined?(Socket::SCM_RIGHTS)
    return if !defined?(Socket::AncillaryData)
    IO.pipe {|r1, w|
      UNIXSocket.pair {|s1, s2|
        begin
          ad = Socket::AncillaryData.int(:UNIX, :SOCKET, :RIGHTS, r1.fileno)
          ret = s1.sendmsg("\0", 0, nil, ad)
        rescue NotImplementedError
          return
        end
        assert_equal(1, ret)
        r2 = s2.recv_io
        begin
          assert(File.identical?(r1, r2))
        ensure
          r2.close
        end
      }
    }
  end

  def test_sendmsg_ancillarydata_unix_rights
    return if !defined?(Socket::SCM_RIGHTS)
    return if !defined?(Socket::AncillaryData)
    IO.pipe {|r1, w|
      UNIXSocket.pair {|s1, s2|
        begin
          ad = Socket::AncillaryData.unix_rights(r1)
          ret = s1.sendmsg("\0", 0, nil, ad)
        rescue NotImplementedError
          return
        end
        assert_equal(1, ret)
        r2 = s2.recv_io
        begin
          assert(File.identical?(r1, r2))
        ensure
          r2.close
        end
      }
    }
  end

  def test_recvmsg
    return if !defined?(Socket::SCM_RIGHTS)
    return if !defined?(Socket::AncillaryData)
    IO.pipe {|r1, w|
      UNIXSocket.pair {|s1, s2|
        s1.send_io(r1)
        ret = s2.recvmsg(:scm_rights=>true)
        data, srcaddr, flags, *ctls = ret
        assert_equal("\0", data)
	if flags == nil
	  # struct msghdr is 4.3BSD style (msg_accrights field).
	  assert_instance_of(Array, ctls)
	  assert_equal(0, ctls.length)
	else
	  # struct msghdr is POSIX/4.4BSD style (msg_control field).
	  assert_equal(0, flags & (Socket::MSG_TRUNC|Socket::MSG_CTRUNC))
	  assert_instance_of(Addrinfo, srcaddr)
	  assert_instance_of(Array, ctls)
	  assert_equal(1, ctls.length)
          ctl = ctls[0]
	  assert_instance_of(Socket::AncillaryData, ctl)
	  assert_equal(Socket::SOL_SOCKET, ctl.level)
	  assert_equal(Socket::SCM_RIGHTS, ctl.type)
	  assert_instance_of(String, ctl.data)
          ios = ctl.unix_rights
          assert_equal(1, ios.length)
	  r2 = ios[0]
	  begin
	    assert(File.identical?(r1, r2))
            assert(r2.close_on_exec?)
	  ensure
	    r2.close
	  end
	end
      }
    }
  end

  def bound_unix_socket(klass)
    tmpfile = Tempfile.new("testrubysock")
    path = tmpfile.path
    tmpfile.close(true)
    yield klass.new(path), path
  ensure
    File.unlink path if path && File.socket?(path)
  end

  def test_addr
    bound_unix_socket(UNIXServer) {|serv, path|
      c = UNIXSocket.new(path)
      s = serv.accept
      assert_equal(["AF_UNIX", path], c.peeraddr)
      assert_equal(["AF_UNIX", ""], c.addr)
      assert_equal(["AF_UNIX", ""], s.peeraddr)
      assert_equal(["AF_UNIX", path], s.addr)
      assert_equal(path, s.path)
      assert_equal("", c.path)
    }
  end

  def test_cloexec
    bound_unix_socket(UNIXServer) {|serv, path|
      c = UNIXSocket.new(path)
      s = serv.accept
      assert(serv.close_on_exec?)
      assert(c.close_on_exec?)
      assert(s.close_on_exec?)
    }
  end

  def test_noname_path
    s1, s2 = UNIXSocket.pair
    assert_equal("", s1.path)
    assert_equal("", s2.path)
  ensure
    s1.close
    s2.close
  end

  def test_noname_addr
    s1, s2 = UNIXSocket.pair
    assert_equal(["AF_UNIX", ""], s1.addr)
    assert_equal(["AF_UNIX", ""], s2.addr)
  ensure
    s1.close
    s2.close
  end

  def test_noname_peeraddr
    s1, s2 = UNIXSocket.pair
    assert_equal(["AF_UNIX", ""], s1.peeraddr)
    assert_equal(["AF_UNIX", ""], s2.peeraddr)
  ensure
    s1.close
    s2.close
  end

  def test_noname_unpack_sockaddr_un
    s1, s2 = UNIXSocket.pair
    n = nil
    assert_equal("", Socket.unpack_sockaddr_un(n)) if (n = s1.getsockname) != ""
    assert_equal("", Socket.unpack_sockaddr_un(n)) if (n = s1.getsockname) != ""
    assert_equal("", Socket.unpack_sockaddr_un(n)) if (n = s2.getsockname) != ""
    assert_equal("", Socket.unpack_sockaddr_un(n)) if (n = s1.getpeername) != ""
    assert_equal("", Socket.unpack_sockaddr_un(n)) if (n = s2.getpeername) != ""
  ensure
    s1.close
    s2.close
  end

  def test_noname_recvfrom
    s1, s2 = UNIXSocket.pair
    s2.write("a")
    assert_equal(["a", ["AF_UNIX", ""]], s1.recvfrom(10))
  ensure
    s1.close
    s2.close
  end

  def test_noname_recv_nonblock
    s1, s2 = UNIXSocket.pair
    s2.write("a")
    IO.select [s1]
    assert_equal("a", s1.recv_nonblock(10))
  ensure
    s1.close
    s2.close
  end

  def test_too_long_path
    assert_raise(ArgumentError) { Socket.sockaddr_un("a" * 300) }
    assert_raise(ArgumentError) { UNIXServer.new("a" * 300) }
  end

  def test_nul
    assert_raise(ArgumentError) { Socket.sockaddr_un("a\0b") }
  end

  def test_dgram_pair
    s1, s2 = UNIXSocket.pair(Socket::SOCK_DGRAM)
    assert_raise(Errno::EAGAIN) { s1.recv_nonblock(10) }
    s2.send("", 0)
    s2.send("haha", 0)
    s2.send("", 0)
    s2.send("", 0)
    assert_equal("", s1.recv(10))
    assert_equal("haha", s1.recv(10))
    assert_equal("", s1.recv(10))
    assert_equal("", s1.recv(10))
    assert_raise(Errno::EAGAIN) { s1.recv_nonblock(10) }
  ensure
    s1.close if s1
    s2.close if s2
  end

  def test_epipe # [ruby-dev:34619]
    s1, s2 = UNIXSocket.pair
    s1.shutdown(Socket::SHUT_WR)
    assert_raise(Errno::EPIPE) { s1.write "a" }
    assert_equal(nil, s2.read(1))
    s2.write "a"
    assert_equal("a", s1.read(1))
  end

  def test_socket_pair_with_block
    pair = nil
    ret = Socket.pair(Socket::AF_UNIX, Socket::SOCK_STREAM, 0) {|s1, s2|
      pair = [s1, s2]
      :return_value
    }
    assert_equal(:return_value, ret)
    assert_kind_of(Socket, pair[0])
    assert_kind_of(Socket, pair[1])
  end

  def test_unix_socket_pair_with_block
    pair = nil
    UNIXSocket.pair {|s1, s2|
      pair = [s1, s2]
    }
    assert_kind_of(UNIXSocket, pair[0])
    assert_kind_of(UNIXSocket, pair[1])
  end

  def test_unix_socket_pair_close_on_exec
    pair = nil
    UNIXSocket.pair {|s1, s2|
      assert(s1.close_on_exec?)
      assert(s2.close_on_exec?)
    }
  end

  def test_initialize
    Dir.mktmpdir {|d|
      Socket.open(Socket::AF_UNIX, Socket::SOCK_STREAM, 0) {|s|
	s.bind(Socket.pack_sockaddr_un("#{d}/s1"))
	addr = s.getsockname
	assert_nothing_raised { Socket.unpack_sockaddr_un(addr) }
	assert_raise(ArgumentError) { Socket.unpack_sockaddr_in(addr) }
      }
      Socket.open("AF_UNIX", "SOCK_STREAM", 0) {|s|
	s.bind(Socket.pack_sockaddr_un("#{d}/s2"))
	addr = s.getsockname
	assert_nothing_raised { Socket.unpack_sockaddr_un(addr) }
	assert_raise(ArgumentError) { Socket.unpack_sockaddr_in(addr) }
      }
    }
  end

  def test_unix_server_socket
    Dir.mktmpdir {|d|
      path = "#{d}/sock"
      s0 = nil
      Socket.unix_server_socket(path) {|s|
        assert_equal(path, s.local_address.unix_path)
        assert(File.socket?(path))
        s0 = s
      }
      assert(s0.closed?)
      assert_raise(Errno::ENOENT) { File.stat path }
    }
  end

  def test_getcred_ucred
    return if /linux/ !~ RUBY_PLATFORM
    Dir.mktmpdir {|d|
      sockpath = "#{d}/sock"
      serv = Socket.unix_server_socket(sockpath)
      c = Socket.unix(sockpath)
      s, = serv.accept
      cred = s.getsockopt(:SOCKET, :PEERCRED)
      inspect = cred.inspect
      assert_match(/ pid=#{$$} /, inspect)
      assert_match(/ euid=#{Process.euid} /, inspect)
      assert_match(/ egid=#{Process.egid} /, inspect)
      assert_match(/ \(ucred\)/, inspect)
    }
  end

  def test_getcred_xucred
    return if /freebsd|darwin/ !~ RUBY_PLATFORM
    Dir.mktmpdir {|d|
      sockpath = "#{d}/sock"
      serv = Socket.unix_server_socket(sockpath)
      c = Socket.unix(sockpath)
      s, = serv.accept
      cred = s.getsockopt(0, Socket::LOCAL_PEERCRED)
      inspect = cred.inspect
      assert_match(/ euid=#{Process.euid} /, inspect)
      assert_match(/ \(xucred\)/, inspect)
    }
  end

  def test_sendcred_ucred
    return if /linux/ !~ RUBY_PLATFORM
    Dir.mktmpdir {|d|
      sockpath = "#{d}/sock"
      serv = Socket.unix_server_socket(sockpath)
      c = Socket.unix(sockpath)
      s, = serv.accept
      s.setsockopt(:SOCKET, :PASSCRED, 1)
      c.print "a"
      msg, cliend_ai, rflags, cred = s.recvmsg
      inspect = cred.inspect
      assert_equal("a", msg)
      assert_match(/ pid=#{$$} /, inspect)
      assert_match(/ uid=#{Process.uid} /, inspect)
      assert_match(/ gid=#{Process.gid} /, inspect)
      assert_match(/ \(ucred\)/, inspect)
    }
  end

  def test_sendcred_sockcred
    return if /netbsd|freebsd/ !~ RUBY_PLATFORM
    Dir.mktmpdir {|d|
      sockpath = "#{d}/sock"
      serv = Socket.unix_server_socket(sockpath)
      c = Socket.unix(sockpath)
      s, = serv.accept
      s.setsockopt(0, Socket::LOCAL_CREDS, 1)
      c.print "a"
      msg, cliend_ai, rflags, cred = s.recvmsg
      assert_equal("a", msg)
      inspect = cred.inspect
      assert_match(/ uid=#{Process.uid} /, inspect)
      assert_match(/ euid=#{Process.euid} /, inspect)
      assert_match(/ gid=#{Process.gid} /, inspect)
      assert_match(/ egid=#{Process.egid} /, inspect)
      assert_match(/ \(sockcred\)/, inspect)
    }
  end

  def test_sendcred_cmsgcred
    return if /freebsd/ !~ RUBY_PLATFORM
    Dir.mktmpdir {|d|
      sockpath = "#{d}/sock"
      serv = Socket.unix_server_socket(sockpath)
      c = Socket.unix(sockpath)
      s, = serv.accept
      c.sendmsg("a", 0, nil, [:SOCKET, Socket::SCM_CREDS, ""])
      msg, cliend_ai, rflags, cred = s.recvmsg
      assert_equal("a", msg)
      inspect = cred.inspect
      assert_match(/ pid=#{$$} /, inspect)
      assert_match(/ uid=#{Process.uid} /, inspect)
      assert_match(/ euid=#{Process.euid} /, inspect)
      assert_match(/ gid=#{Process.gid} /, inspect)
      assert_match(/ \(cmsgcred\)/, inspect)
    }
  end

  def test_getpeereid
    Dir.mktmpdir {|d|
      path = "#{d}/sock"
      serv = Socket.unix_server_socket(path)
      c = Socket.unix(path)
      s, = serv.accept
      begin
        assert_equal([Process.euid, Process.egid], c.getpeereid)
        assert_equal([Process.euid, Process.egid], s.getpeereid)
      rescue NotImplementedError
      end
    }
  end

end if defined?(UNIXSocket) && /cygwin/ !~ RUBY_PLATFORM
