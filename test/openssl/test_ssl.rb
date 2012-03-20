require_relative "utils"

if defined?(OpenSSL)

class OpenSSL::TestSSL < OpenSSL::SSLTestCase
  def test_ctx_setup
    ctx = OpenSSL::SSL::SSLContext.new
    assert_equal(ctx.setup, true)
    assert_equal(ctx.setup, nil)
  end

  def test_ctx_setup_no_compression
    ctx = OpenSSL::SSL::SSLContext.new
    ctx.options = OpenSSL::SSL::OP_ALL | OpenSSL::SSL::OP_NO_COMPRESSION
    assert_equal(ctx.setup, true)
    assert_equal(ctx.setup, nil)
    assert_equal(OpenSSL::SSL::OP_NO_COMPRESSION,
                 ctx.options & OpenSSL::SSL::OP_NO_COMPRESSION)
  end if defined?(OpenSSL::SSL::OP_NO_COMPRESSION)

  def test_not_started_session
    skip "non socket argument of SSLSocket.new is not supported on this platform" if /mswin|mingw/ =~ RUBY_PLATFORM
    open(__FILE__) do |f|
      assert_nil OpenSSL::SSL::SSLSocket.new(f).cert
    end
  end

  def test_ssl_read_nonblock
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true) { |server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock)
      ssl.sync_close = true
      ssl.connect
      assert_raise(IO::WaitReadable) { ssl.read_nonblock(100) }
      ssl.write("abc\n")
      IO.select [ssl]
      assert_equal('a', ssl.read_nonblock(1))
      assert_equal("bc\n", ssl.read_nonblock(100))
      assert_raise(IO::WaitReadable) { ssl.read_nonblock(100) }
    }
  end

  def test_connect_and_close
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock)
      assert(ssl.connect)
      ssl.close
      assert(!sock.closed?)
      sock.close

      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock)
      ssl.sync_close = true  # !!
      assert(ssl.connect)
      ssl.close
      assert(sock.closed?)
    }
  end

  def test_read_and_write
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock)
      ssl.sync_close = true
      ssl.connect

      # syswrite and sysread
      ITERATIONS.times{|i|
        str = "x" * 100 + "\n"
        ssl.syswrite(str)
        assert_equal(str, ssl.sysread(str.size))

        str = "x" * i * 100 + "\n"
        buf = ""
        ssl.syswrite(str)
        assert_equal(buf.object_id, ssl.sysread(str.size, buf).object_id)
        assert_equal(str, buf)
      }

      # puts and gets
      ITERATIONS.times{
        str = "x" * 100 + "\n"
        ssl.puts(str)
        assert_equal(str, ssl.gets)

        str = "x" * 100
        ssl.puts(str)
        assert_equal(str, ssl.gets("\n", 100))
        assert_equal("\n", ssl.gets)
      }

      # read and write
      ITERATIONS.times{|i|
        str = "x" * 100 + "\n"
        ssl.write(str)
        assert_equal(str, ssl.read(str.size))

        str = "x" * i * 100 + "\n"
        buf = ""
        ssl.write(str)
        assert_equal(buf.object_id, ssl.read(str.size, buf).object_id)
        assert_equal(str, buf)
      }

      ssl.close
    }
  end

  def test_client_auth
    vflag = OpenSSL::SSL::VERIFY_PEER|OpenSSL::SSL::VERIFY_FAIL_IF_NO_PEER_CERT
    start_server(PORT, vflag, true){|server, port|
      assert_raise(OpenSSL::SSL::SSLError, Errno::ECONNRESET){
        sock = TCPSocket.new("127.0.0.1", port)
        ssl = OpenSSL::SSL::SSLSocket.new(sock)
        ssl.connect
      }

      ctx = OpenSSL::SSL::SSLContext.new
      ctx.key = @cli_key
      ctx.cert = @cli_cert
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      ssl.sync_close = true
      ssl.connect
      ssl.puts("foo")
      assert_equal("foo\n", ssl.gets)
      ssl.close

      called = nil
      ctx = OpenSSL::SSL::SSLContext.new
      ctx.client_cert_cb = Proc.new{ |sslconn|
        called = true
        [@cli_cert, @cli_key]
      }
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      ssl.sync_close = true
      ssl.connect
      assert(called)
      ssl.puts("foo")
      assert_equal("foo\n", ssl.gets)
      ssl.close
    }
  end

  def test_client_ca
    ctx_proc = Proc.new do |ctx|
      ctx.client_ca = [@ca_cert]
    end

    vflag = OpenSSL::SSL::VERIFY_PEER|OpenSSL::SSL::VERIFY_FAIL_IF_NO_PEER_CERT
    start_server(PORT, vflag, true, :ctx_proc => ctx_proc){|server, port|
      ctx = OpenSSL::SSL::SSLContext.new
      client_ca_from_server = nil
      ctx.client_cert_cb = Proc.new do |sslconn|
        client_ca_from_server = sslconn.client_ca
        [@cli_cert, @cli_key]
      end
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      ssl.sync_close = true
      ssl.connect
      assert_equal([@ca], client_ca_from_server)
      ssl.close
    }
  end

  def test_starttls
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, false){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock)
      ssl.sync_close = true
      str = "x" * 1000 + "\n"

      OpenSSL::TestUtils.silent do
        ITERATIONS.times{
          ssl.puts(str)
          assert_equal(str, ssl.gets)
        }
        starttls(ssl)
      end

      ITERATIONS.times{
        ssl.puts(str)
        assert_equal(str, ssl.gets)
      }

      ssl.close
    }
  end

  def test_parallel
    GC.start
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true){|server, port|
      ssls = []
      10.times{
        sock = TCPSocket.new("127.0.0.1", port)
        ssl = OpenSSL::SSL::SSLSocket.new(sock)
        ssl.connect
        ssl.sync_close = true
        ssls << ssl
      }
      str = "x" * 1000 + "\n"
      ITERATIONS.times{
        ssls.each{|ssl|
          ssl.puts(str)
          assert_equal(str, ssl.gets)
        }
      }
      ssls.each{|ssl| ssl.close }
    }
  end

  def test_verify_result
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ctx = OpenSSL::SSL::SSLContext.new
      ctx.set_params
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      assert_raise(OpenSSL::SSL::SSLError){ ssl.connect }
      assert_equal(OpenSSL::X509::V_ERR_SELF_SIGNED_CERT_IN_CHAIN, ssl.verify_result)

      sock = TCPSocket.new("127.0.0.1", port)
      ctx = OpenSSL::SSL::SSLContext.new
      ctx.set_params(
        :verify_callback => Proc.new do |preverify_ok, store_ctx|
          store_ctx.error = OpenSSL::X509::V_OK
          true
        end
      )
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      ssl.connect
      assert_equal(OpenSSL::X509::V_OK, ssl.verify_result)

      sock = TCPSocket.new("127.0.0.1", port)
      ctx = OpenSSL::SSL::SSLContext.new
      ctx.set_params(
        :verify_callback => Proc.new do |preverify_ok, store_ctx|
          store_ctx.error = OpenSSL::X509::V_ERR_APPLICATION_VERIFICATION
          false
        end
      )
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      assert_raise(OpenSSL::SSL::SSLError){ ssl.connect }
      assert_equal(OpenSSL::X509::V_ERR_APPLICATION_VERIFICATION, ssl.verify_result)
    }
  end

  def test_exception_in_verify_callback_is_ignored
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ctx = OpenSSL::SSL::SSLContext.new
      ctx.set_params(
        :verify_callback => Proc.new do |preverify_ok, store_ctx|
          store_ctx.error = OpenSSL::X509::V_OK
          raise RuntimeError
        end
      )
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      OpenSSL::TestUtils.silent do
        # SSLError, not RuntimeError
        assert_raise(OpenSSL::SSL::SSLError) { ssl.connect }
      end
      assert_equal(OpenSSL::X509::V_ERR_CERT_REJECTED, ssl.verify_result)
      ssl.close
    }
  end

  def test_sslctx_set_params
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ctx = OpenSSL::SSL::SSLContext.new
      ctx.set_params
      assert_equal(OpenSSL::SSL::VERIFY_PEER, ctx.verify_mode)
      assert_equal(OpenSSL::SSL::OP_ALL, ctx.options)
      ciphers = ctx.ciphers
      ciphers_versions = ciphers.collect{|_, v, _, _| v }
      ciphers_names = ciphers.collect{|v, _, _, _| v }
      assert(ciphers_names.all?{|v| /ADH/ !~ v })
      assert(ciphers_versions.all?{|v| /SSLv2/ !~ v })
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      assert_raise(OpenSSL::SSL::SSLError){ ssl.connect }
      assert_equal(OpenSSL::X509::V_ERR_SELF_SIGNED_CERT_IN_CHAIN, ssl.verify_result)
    }
  end

  def test_post_connection_check
    sslerr = OpenSSL::SSL::SSLError

    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock)
      ssl.connect
      assert_raise(sslerr){ssl.post_connection_check("localhost.localdomain")}
      assert_raise(sslerr){ssl.post_connection_check("127.0.0.1")}
      assert(ssl.post_connection_check("localhost"))
      assert_raise(sslerr){ssl.post_connection_check("foo.example.com")}

      cert = ssl.peer_cert
      assert(!OpenSSL::SSL.verify_certificate_identity(cert, "localhost.localdomain"))
      assert(!OpenSSL::SSL.verify_certificate_identity(cert, "127.0.0.1"))
      assert(OpenSSL::SSL.verify_certificate_identity(cert, "localhost"))
      assert(!OpenSSL::SSL.verify_certificate_identity(cert, "foo.example.com"))
    }

    now = Time.now
    exts = [
      ["keyUsage","keyEncipherment,digitalSignature",true],
      ["subjectAltName","DNS:localhost.localdomain",false],
      ["subjectAltName","IP:127.0.0.1",false],
    ]
    @svr_cert = issue_cert(@svr, @svr_key, 4, now, now+1800, exts,
                           @ca_cert, @ca_key, OpenSSL::Digest::SHA1.new)
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock)
      ssl.connect
      assert(ssl.post_connection_check("localhost.localdomain"))
      assert(ssl.post_connection_check("127.0.0.1"))
      assert_raise(sslerr){ssl.post_connection_check("localhost")}
      assert_raise(sslerr){ssl.post_connection_check("foo.example.com")}

      cert = ssl.peer_cert
      assert(OpenSSL::SSL.verify_certificate_identity(cert, "localhost.localdomain"))
      assert(OpenSSL::SSL.verify_certificate_identity(cert, "127.0.0.1"))
      assert(!OpenSSL::SSL.verify_certificate_identity(cert, "localhost"))
      assert(!OpenSSL::SSL.verify_certificate_identity(cert, "foo.example.com"))
    }

    now = Time.now
    exts = [
      ["keyUsage","keyEncipherment,digitalSignature",true],
      ["subjectAltName","DNS:*.localdomain",false],
    ]
    @svr_cert = issue_cert(@svr, @svr_key, 5, now, now+1800, exts,
                           @ca_cert, @ca_key, OpenSSL::Digest::SHA1.new)
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock)
      ssl.connect
      assert(ssl.post_connection_check("localhost.localdomain"))
      assert_raise(sslerr){ssl.post_connection_check("127.0.0.1")}
      assert_raise(sslerr){ssl.post_connection_check("localhost")}
      assert_raise(sslerr){ssl.post_connection_check("foo.example.com")}
      cert = ssl.peer_cert
      assert(OpenSSL::SSL.verify_certificate_identity(cert, "localhost.localdomain"))
      assert(!OpenSSL::SSL.verify_certificate_identity(cert, "127.0.0.1"))
      assert(!OpenSSL::SSL.verify_certificate_identity(cert, "localhost"))
      assert(!OpenSSL::SSL.verify_certificate_identity(cert, "foo.example.com"))
    }
  end

  def test_tlsext_hostname
    return unless OpenSSL::SSL::SSLSocket.instance_methods.include?(:hostname)

    ctx_proc = Proc.new do |ctx, ssl|
      foo_ctx = ctx.dup

      ctx.servername_cb = Proc.new do |ssl2, hostname|
        case hostname
        when 'foo.example.com'
          foo_ctx
        when 'bar.example.com'
          nil
        else
          raise "unknown hostname #{hostname.inspect}"
        end
      end
    end

    server_proc = Proc.new do |ctx, ssl|
      readwrite_loop(ctx, ssl)
    end

    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true, :ctx_proc => ctx_proc, :server_proc => server_proc) do |server, port|
      2.times do |i|
        sock = TCPSocket.new("127.0.0.1", port)
        ctx = OpenSSL::SSL::SSLContext.new
        if defined?(OpenSSL::SSL::OP_NO_TICKET)
          # disable RFC4507 support
          ctx.options = OpenSSL::SSL::OP_NO_TICKET
        end
        ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
        ssl.sync_close = true
        ssl.hostname = (i & 1 == 0) ? 'foo.example.com' : 'bar.example.com'
        ssl.connect

        str = "x" * 100 + "\n"
        ssl.puts(str)
        assert_equal(str, ssl.gets)

        ssl.close
      end
    end
  end

  def test_multibyte_read_write
    #German a umlaut
    auml = [%w{ C3 A4 }.join('')].pack('H*')
    auml.force_encoding(Encoding::UTF_8)

    [10, 1000, 100000].each {|i|
      str = nil
      num_written = nil
      server_proc = Proc.new {|ctx, ssl|
        cmp = ssl.read
        raw_size = cmp.size
        cmp.force_encoding(Encoding::UTF_8)
        assert_equal(str, cmp)
        assert_equal(num_written, raw_size)
        ssl.close
      }
      start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true, :server_proc => server_proc){|server, port|
        sock = TCPSocket.new("127.0.0.1", port)
        ssl = OpenSSL::SSL::SSLSocket.new(sock)
        ssl.sync_close = true
        ssl.connect
        str = auml * i
        num_written = ssl.write(str)
        ssl.close
      }
    }
  end

  def test_unset_OP_ALL
    ctx_proc = Proc.new { |ctx|
      ctx.options = OpenSSL::SSL::OP_ALL & ~OpenSSL::SSL::OP_DONT_INSERT_EMPTY_FRAGMENTS
    }
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true, :ctx_proc => ctx_proc){|server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock)
      ssl.sync_close = true
      ssl.connect
      ssl.puts('hello')
      assert_equal("hello\n", ssl.gets)
      ssl.close
    }
  end
end

end
