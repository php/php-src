require_relative "utils"

if defined?(OpenSSL)

class OpenSSL::TestSSLSession < OpenSSL::SSLTestCase
  def test_session_equals
    session = OpenSSL::SSL::Session.new <<-SESSION
-----BEGIN SSL SESSION PARAMETERS-----
MIIDFgIBAQICAwEEAgA5BCCY3pW6iTkPoD5SENuztz/gZjhvey6XnHbsxd22k0Ol
dgQw8uaN3hCRnlhoIKPWInCFzrp/tQsDRFs9jDjc9pwpy/oKHmJdQQMQA1g8FYnO
gpdVoQYCBE52ikKiBAICASyjggKOMIICijCCAXKgAwIBAgIBAjANBgkqhkiG9w0B
AQUFADA9MRMwEQYKCZImiZPyLGQBGRYDb3JnMRkwFwYKCZImiZPyLGQBGRYJcnVi
eS1sYW5nMQswCQYDVQQDDAJDQTAeFw0xMTA5MTkwMDE4MTBaFw0xMTA5MTkwMDQ4
MTBaMEQxEzARBgoJkiaJk/IsZAEZFgNvcmcxGTAXBgoJkiaJk/IsZAEZFglydWJ5
LWxhbmcxEjAQBgNVBAMMCWxvY2FsaG9zdDCBnzANBgkqhkiG9w0BAQEFAAOBjQAw
gYkCgYEAy8LEsNRApz7U/j5DoB4XBgO9Z8Atv5y/OVQRp0ag8Tqo1YewsWijxEWB
7JOATwpBN267U4T1nPZIxxEEO7n/WNa2ws9JWsjah8ssEBFSxZqdXKSLf0N4Hi7/
GQ/aYoaMCiQ8jA4jegK2FJmXM71uPe+jFN/peeBOpRfyXxRFOYcCAwEAAaMSMBAw
DgYDVR0PAQH/BAQDAgWgMA0GCSqGSIb3DQEBBQUAA4IBAQARC7GP7InX1t7VEXz2
I8RI57S0/HSJL4fDIYP3zFpitHX1PZeo+7XuzMilvPjjBo/ky9Jzo8TYiY+N+JEz
mY/A/zPA4ZsJ7KYj6/FEdIc/vRlS0CvsbClbNjw1jl/PoB2FLr2b3uuBcZEsyZeP
yq154ijq37Ajf8K5Mi5FgshoP41BPtRPj+VVf61rv1IcEnNWdDCS6DR4XsaNC+zt
G6AqCqkytIXWRuDw6n6vYLF3A/tn2sldLo7/scY0PMDNbo63O/LTxkDHmPhSkD68
8m9SsMeTR+RCiDEZWFPVcAH/8mDfi+5k8uN3qS+gOU/PPrmHGgl5ykiSFgqs4v61
tddwpBAEDjcwMzA5NTYzMTU1MzAwpQMCARM=
-----END SSL SESSION PARAMETERS-----
    SESSION

    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true) { |_, port|
      ctx = OpenSSL::SSL::SSLContext.new
      ctx.session_cache_mode = OpenSSL::SSL::SSLContext::SESSION_CACHE_CLIENT
      ctx.session_id_context = self.object_id.to_s

      sock = TCPSocket.new '127.0.0.1', port
      ssl = OpenSSL::SSL::SSLSocket.new sock, ctx
      ssl.session = session

      assert_equal session, ssl.session
      sock.close
    }
  end

  def test_session
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true) do |server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ctx = OpenSSL::SSL::SSLContext.new("TLSv1")
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      ssl.sync_close = true
      ssl.connect
      session = ssl.session
      assert(session == OpenSSL::SSL::Session.new(session.to_pem))
      assert(session == OpenSSL::SSL::Session.new(ssl))
      assert_equal(300, session.timeout)
      session.timeout = 5
      assert_equal(5, session.timeout)
      assert_not_nil(session.time)
      # SSL_SESSION_time keeps long value so we can't keep nsec fragment.
      session.time = t1 = Time.now.to_i
      assert_equal(Time.at(t1), session.time)
      if session.respond_to?(:id)
        assert_not_nil(session.id)
      end
      pem = session.to_pem
      assert_match(/\A-----BEGIN SSL SESSION PARAMETERS-----/, pem)
      assert_match(/-----END SSL SESSION PARAMETERS-----\Z/, pem)
      pem.gsub!(/-----(BEGIN|END) SSL SESSION PARAMETERS-----/, '').gsub!(/[\r\n]+/m, '')
      assert_equal(session.to_der, pem.unpack('m*')[0])
      assert_not_nil(session.to_text)
      ssl.close
    end
  end

  DUMMY_SESSION = <<__EOS__
-----BEGIN SSL SESSION PARAMETERS-----
MIIDzQIBAQICAwEEAgA5BCAF219w9ZEV8dNA60cpEGOI34hJtIFbf3bkfzSgMyad
MQQwyGLbkCxE4OiMLdKKem+pyh8V7ifoP7tCxhdmwoDlJxI1v6nVCjai+FGYuncy
NNSWoQYCBE4DDWuiAwIBCqOCAo4wggKKMIIBcqADAgECAgECMA0GCSqGSIb3DQEB
BQUAMD0xEzARBgoJkiaJk/IsZAEZFgNvcmcxGTAXBgoJkiaJk/IsZAEZFglydWJ5
LWxhbmcxCzAJBgNVBAMMAkNBMB4XDTExMDYyMzA5NTQ1MVoXDTExMDYyMzEwMjQ1
MVowRDETMBEGCgmSJomT8ixkARkWA29yZzEZMBcGCgmSJomT8ixkARkWCXJ1Ynkt
bGFuZzESMBAGA1UEAwwJbG9jYWxob3N0MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCB
iQKBgQDLwsSw1ECnPtT+PkOgHhcGA71nwC2/nL85VBGnRqDxOqjVh7CxaKPERYHs
k4BPCkE3brtThPWc9kjHEQQ7uf9Y1rbCz0layNqHyywQEVLFmp1cpIt/Q3geLv8Z
D9pihowKJDyMDiN6ArYUmZczvW4976MU3+l54E6lF/JfFEU5hwIDAQABoxIwEDAO
BgNVHQ8BAf8EBAMCBaAwDQYJKoZIhvcNAQEFBQADggEBACj5WhoZ/ODVeHpwgq1d
8fW/13ICRYHYpv6dzlWihyqclGxbKMlMnaVCPz+4JaVtMz3QB748KJQgL3Llg3R1
ek+f+n1MBCMfFFsQXJ2gtLB84zD6UCz8aaCWN5/czJCd7xMz7fRLy3TOIW5boXAU
zIa8EODk+477K1uznHm286ab0Clv+9d304hwmBZgkzLg6+31Of6d6s0E0rwLGiS2
sOWYg34Y3r4j8BS9Ak4jzpoLY6cJ0QAKCOJCgmjGr4XHpyXMLbicp3ga1uSbwtVO
gF/gTfpLhJC+y0EQ5x3Ftl88Cq7ZJuLBDMo/TLIfReJMQu/HlrTT7+LwtneSWGmr
KkSkAgQApQMCAROqgcMEgcAuDkAVfj6QAJMz9yqTzW5wPFyty7CxUEcwKjUqj5UP
/Yvky1EkRuM/eQfN7ucY+MUvMqv+R8ZSkHPsnjkBN5ChvZXjrUSZKFVjR4eFVz2V
jismLEJvIFhQh6pqTroRrOjMfTaM5Lwoytr2FTGobN9rnjIRsXeFQW1HLFbXn7Dh
8uaQkMwIVVSGRB8T7t6z6WIdWruOjCZ6G5ASI5XoqAHwGezhLodZuvJEfsVyCF9y
j+RBGfCFrrQbBdnkFI/ztgM=
-----END SSL SESSION PARAMETERS-----
__EOS__

  DUMMY_SESSION_NO_EXT = <<-__EOS__
-----BEGIN SSL SESSION PARAMETERS-----
MIIDCAIBAQICAwAEAgA5BCDyAW7rcpzMjDSosH+Tv6sukymeqgq3xQVVMez628A+
lAQw9TrKzrIqlHEh6ltuQaqv/Aq83AmaAlogYktZgXAjOGnhX7ifJDNLMuCfQq53
hPAaoQYCBE4iDeeiBAICASyjggKOMIICijCCAXKgAwIBAgIBAjANBgkqhkiG9w0B
AQUFADA9MRMwEQYKCZImiZPyLGQBGRYDb3JnMRkwFwYKCZImiZPyLGQBGRYJcnVi
eS1sYW5nMQswCQYDVQQDDAJDQTAeFw0xMTA3MTYyMjE3MTFaFw0xMTA3MTYyMjQ3
MTFaMEQxEzARBgoJkiaJk/IsZAEZFgNvcmcxGTAXBgoJkiaJk/IsZAEZFglydWJ5
LWxhbmcxEjAQBgNVBAMMCWxvY2FsaG9zdDCBnzANBgkqhkiG9w0BAQEFAAOBjQAw
gYkCgYEAy8LEsNRApz7U/j5DoB4XBgO9Z8Atv5y/OVQRp0ag8Tqo1YewsWijxEWB
7JOATwpBN267U4T1nPZIxxEEO7n/WNa2ws9JWsjah8ssEBFSxZqdXKSLf0N4Hi7/
GQ/aYoaMCiQ8jA4jegK2FJmXM71uPe+jFN/peeBOpRfyXxRFOYcCAwEAAaMSMBAw
DgYDVR0PAQH/BAQDAgWgMA0GCSqGSIb3DQEBBQUAA4IBAQA3TRzABRG3kz8jEEYr
tDQqXgsxwTsLhTT5d1yF0D8uFw+y15hJAJnh6GJHjqhWBrF4zNoTApFo+4iIL6g3
q9C3mUsxIVAHx41DwZBh/FI7J4FqlAoGOguu7892CNVY3ZZjc3AXMTdKjcNoWPzz
FCdj5fNT24JMMe+ZdGZK97ChahJsdn/6B3j6ze9NK9mfYEbiJhejGTPLOFVHJCGR
KYYZ3ZcKhLDr9ql4d7cCo1gBtemrmFQGPui7GttNEqmXqUKvV8mYoa8farf5i7T4
L6a/gp2cVZTaDIS1HjbJsA/Ag7AajZqiN6LfqShNUVsrMZ+5CoV8EkBDTZPJ9MSr
a3EqpAIEAKUDAgET
-----END SSL SESSION PARAMETERS-----
__EOS__


  def test_session_time
    sess = OpenSSL::SSL::Session.new(DUMMY_SESSION_NO_EXT)
    sess.time = (now = Time.now)
    assert_equal(now.to_i, sess.time.to_i)
    sess.time = 1
    assert_equal(1, sess.time.to_i)
    sess.time = 1.2345
    assert_equal(1, sess.time.to_i)
    # Can OpenSSL handle t>2038y correctly? Version?
    sess.time = 2**31 - 1
    assert_equal(2**31 - 1, sess.time.to_i)
  end

  def test_session_timeout
    sess = OpenSSL::SSL::Session.new(DUMMY_SESSION_NO_EXT)
    assert_raise(TypeError) do
      sess.timeout = (now = Time.now)
    end
    sess.timeout = 1
    assert_equal(1, sess.timeout.to_i)
    sess.timeout = 1.2345
    assert_equal(1, sess.timeout.to_i)
    sess.timeout = 2**31 - 1
    assert_equal(2**31 - 1, sess.timeout.to_i)
  end

  def test_session_exts_read
    assert(OpenSSL::SSL::Session.new(DUMMY_SESSION))
  end if OpenSSL::OPENSSL_VERSION_NUMBER >= 0x009080bf

  def test_client_session
    last_session = nil
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true) do |server, port|
      2.times do
        sock = TCPSocket.new("127.0.0.1", port)
        # Debian's openssl 0.9.8g-13 failed at assert(ssl.session_reused?),
        # when use default SSLContext. [ruby-dev:36167]
        ctx = OpenSSL::SSL::SSLContext.new("TLSv1")
        ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
        ssl.sync_close = true
        ssl.session = last_session if last_session
        ssl.connect

        session = ssl.session
        if last_session
          assert(ssl.session_reused?)

          if session.respond_to?(:id)
            assert_equal(session.id, last_session.id)
          end
          assert_equal(session.to_pem, last_session.to_pem)
          assert_equal(session.to_der, last_session.to_der)
          # Older version of OpenSSL may not be consistent.  Look up which versions later.
          assert_equal(session.to_text, last_session.to_text)
        else
          assert(!ssl.session_reused?)
        end
        last_session = session

        str = "x" * 100 + "\n"
        ssl.puts(str)
        assert_equal(str, ssl.gets)

        ssl.close
      end
    end
  end

  def test_server_session
    connections = 0
    saved_session = nil

    ctx_proc = Proc.new do |ctx, ssl|
# add test for session callbacks here
    end

    server_proc = Proc.new do |ctx, ssl|
      session = ssl.session
      stats = ctx.session_cache_stats

      case connections
      when 0
        assert_equal(stats[:cache_num], 1)
        assert_equal(stats[:cache_hits], 0)
        assert_equal(stats[:cache_misses], 0)
        assert(!ssl.session_reused?)
      when 1
        assert_equal(stats[:cache_num], 1)
        assert_equal(stats[:cache_hits], 1)
        assert_equal(stats[:cache_misses], 0)
        assert(ssl.session_reused?)
        ctx.session_remove(session)
        saved_session = session
      when 2
        assert_equal(stats[:cache_num], 1)
        assert_equal(stats[:cache_hits], 1)
        assert_equal(stats[:cache_misses], 1)
        assert(!ssl.session_reused?)
        ctx.session_add(saved_session)
      when 3
        assert_equal(stats[:cache_num], 2)
        assert_equal(stats[:cache_hits], 2)
        assert_equal(stats[:cache_misses], 1)
        assert(ssl.session_reused?)
        ctx.flush_sessions(Time.now + 5000)
      when 4
        assert_equal(stats[:cache_num], 1)
        assert_equal(stats[:cache_hits], 2)
        assert_equal(stats[:cache_misses], 2)
        assert(!ssl.session_reused?)
        ctx.session_add(saved_session)
      end
      connections += 1

      readwrite_loop(ctx, ssl)
    end

    first_session = nil
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true, :ctx_proc => ctx_proc, :server_proc => server_proc) do |server, port|
      10.times do |i|
        sock = TCPSocket.new("127.0.0.1", port)
        ctx = OpenSSL::SSL::SSLContext.new
        if defined?(OpenSSL::SSL::OP_NO_TICKET)
          # disable RFC4507 support
          ctx.options = OpenSSL::SSL::OP_NO_TICKET
        end
        ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
        ssl.sync_close = true
        ssl.session = first_session if first_session
        ssl.connect

        session = ssl.session
        if first_session
          case i
          when 1; assert(ssl.session_reused?)
          when 2; assert(!ssl.session_reused?)
          when 3; assert(ssl.session_reused?)
          when 4; assert(!ssl.session_reused?)
          when 5..10; assert(ssl.session_reused?)
          end
        end
        first_session ||= session

        str = "x" * 100 + "\n"
        ssl.puts(str)
        assert_equal(str, ssl.gets)

        ssl.close
      end
    end
  end

  def test_ctx_client_session_cb
    called = {}
    ctx = OpenSSL::SSL::SSLContext.new("SSLv3")
    ctx.session_cache_mode = OpenSSL::SSL::SSLContext::SESSION_CACHE_CLIENT

    ctx.session_new_cb = lambda { |ary|
      sock, sess = ary
      called[:new] = [sock, sess]
    }

    ctx.session_remove_cb = lambda { |ary|
      ctx, sess = ary
      called[:remove] = [ctx, sess]
      # any resulting value is OK (ignored)
    }

    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true) do |server, port|
      sock = TCPSocket.new("127.0.0.1", port)
      ssl = OpenSSL::SSL::SSLSocket.new(sock, ctx)
      ssl.sync_close = true
      ssl.connect
      assert_equal(1, ctx.session_cache_stats[:cache_num])
      assert_equal(1, ctx.session_cache_stats[:connect_good])
      assert_equal([ssl, ssl.session], called[:new])
      assert(ctx.session_remove(ssl.session))
      assert(!ctx.session_remove(ssl.session))
      assert_equal([ctx, ssl.session], called[:remove])
      ssl.close
    end
  end

  def test_ctx_server_session_cb
    called = {}

    ctx_proc = Proc.new { |ctx, ssl|
      ctx.session_cache_mode = OpenSSL::SSL::SSLContext::SESSION_CACHE_SERVER
      last_server_session = nil

      # get_cb is called whenever a client proposed to resume a session but
      # the session could not be found in the internal session cache.
      ctx.session_get_cb = lambda { |ary|
        sess, data = ary
        if last_server_session
          called[:get2] = [sess, data]
          last_server_session
        else
          called[:get1] = [sess, data]
          last_server_session = sess
          nil
        end
      }

      ctx.session_new_cb = lambda { |ary|
        sock, sess = ary
        called[:new] = [sock, sess]
        # SSL server doesn't cache sessions so get_cb is called next time.
        ctx.session_remove(sess)
      }

      ctx.session_remove_cb = lambda { |ary|
        ctx, sess = ary
        called[:remove] = [ctx, sess]
      }
    }

    server_proc = Proc.new { |c, ssl|
      session = ssl.session
      stats = c.session_cache_stats
      readwrite_loop(c, ssl)
    }
    start_server(PORT, OpenSSL::SSL::VERIFY_NONE, true, :ctx_proc => ctx_proc, :server_proc => server_proc) do |server, port|
      last_client_session = nil
      3.times do
        sock = TCPSocket.new("127.0.0.1", port)
        ssl = OpenSSL::SSL::SSLSocket.new(sock, OpenSSL::SSL::SSLContext.new("SSLv3"))
        ssl.sync_close = true
        ssl.session = last_client_session if last_client_session
        ssl.connect
        last_client_session = ssl.session
        ssl.close
        Thread.pass # try to ensure server calls callbacks
        assert(called.delete(:new))
        assert(called.delete(:remove))
      end
    end
    assert(called[:get1])
    assert(called[:get2])
  end
end

end
