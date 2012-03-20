require "test/unit"
require "net/http"
require "webrick"
require "webrick/httpproxy"
begin
  require "webrick/ssl"
  require "net/https"
  require File.expand_path("../openssl/utils.rb", File.dirname(__FILE__))
rescue LoadError
  # test_connect will be skipped
end
require File.expand_path("utils.rb", File.dirname(__FILE__))

class TestWEBrickHTTPProxy < Test::Unit::TestCase
  def test_fake_proxy
    assert_nil(WEBrick::FakeProxyURI.scheme)
    assert_nil(WEBrick::FakeProxyURI.host)
    assert_nil(WEBrick::FakeProxyURI.port)
    assert_nil(WEBrick::FakeProxyURI.path)
    assert_nil(WEBrick::FakeProxyURI.userinfo)
    assert_raise(NoMethodError){ WEBrick::FakeProxyURI.foo }
  end

  def test_proxy
    # Testing GET or POST to the proxy server
    # Note that the proxy server works as the origin server.
    #                    +------+
    #                    V      |
    #  client -------> proxy ---+
    #        GET / POST     GET / POST
    #
    proxy_handler_called = request_handler_called = 0
    config = {
      :ServerName => "localhost.localdomain",
      :ProxyContentHandler => Proc.new{|req, res| proxy_handler_called += 1 },
      :RequestCallback => Proc.new{|req, res| request_handler_called += 1 }
    }
    TestWEBrick.start_httpproxy(config){|server, addr, port, log|
      server.mount_proc("/"){|req, res|
        res.body = "#{req.request_method} #{req.path} #{req.body}"
      }
      http = Net::HTTP.new(addr, port, addr, port)

      req = Net::HTTP::Get.new("/")
      http.request(req){|res|
        assert_equal("1.1 localhost.localdomain:#{port}", res["via"], log.call)
        assert_equal("GET / ", res.body, log.call)
      }
      assert_equal(1, proxy_handler_called, log.call)
      assert_equal(2, request_handler_called, log.call)

      req = Net::HTTP::Head.new("/")
      http.request(req){|res|
        assert_equal("1.1 localhost.localdomain:#{port}", res["via"], log.call)
        assert_nil(res.body, log.call)
      }
      assert_equal(2, proxy_handler_called, log.call)
      assert_equal(4, request_handler_called, log.call)

      req = Net::HTTP::Post.new("/")
      req.body = "post-data"
      http.request(req){|res|
        assert_equal("1.1 localhost.localdomain:#{port}", res["via"], log.call)
        assert_equal("POST / post-data", res.body, log.call)
      }
      assert_equal(3, proxy_handler_called, log.call)
      assert_equal(6, request_handler_called, log.call)
    }
  end

  def test_no_proxy
    # Testing GET or POST to the proxy server without proxy request.
    #
    #  client -------> proxy
    #        GET / POST
    #
    proxy_handler_called = request_handler_called = 0
    config = {
      :ServerName => "localhost.localdomain",
      :ProxyContentHandler => Proc.new{|req, res| proxy_handler_called += 1 },
      :RequestCallback => Proc.new{|req, res| request_handler_called += 1 }
    }
    TestWEBrick.start_httpproxy(config){|server, addr, port, log|
      server.mount_proc("/"){|req, res|
        res.body = "#{req.request_method} #{req.path} #{req.body}"
      }
      http = Net::HTTP.new(addr, port)

      req = Net::HTTP::Get.new("/")
      http.request(req){|res|
        assert_nil(res["via"], log.call)
        assert_equal("GET / ", res.body, log.call)
      }
      assert_equal(0, proxy_handler_called, log.call)
      assert_equal(1, request_handler_called, log.call)

      req = Net::HTTP::Head.new("/")
      http.request(req){|res|
        assert_nil(res["via"], log.call)
        assert_nil(res.body, log.call)
      }
      assert_equal(0, proxy_handler_called, log.call)
      assert_equal(2, request_handler_called, log.call)

      req = Net::HTTP::Post.new("/")
      req.body = "post-data"
      http.request(req){|res|
        assert_nil(res["via"], log.call)
        assert_equal("POST / post-data", res.body, log.call)
      }
      assert_equal(0, proxy_handler_called, log.call)
      assert_equal(3, request_handler_called, log.call)
    }
  end

  def make_certificate(key, cn)
    subject = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=#{cn}")
    exts = [
      ["keyUsage", "keyEncipherment,digitalSignature", true],
    ]
    cert = OpenSSL::TestUtils.issue_cert(
      subject, key, 1, Time.now, Time.now + 3600, exts,
      nil, nil, OpenSSL::Digest::SHA1.new
    )
    return cert
  end

  def test_connect
    # Testing CONNECT to proxy server
    #
    #  client -----------> proxy -----------> https
    #    1.     CONNECT          establish TCP
    #    2.   ---- establish SSL session --->
    #    3.   ------- GET or POST ---------->
    #
    key = OpenSSL::TestUtils::TEST_KEY_RSA1024
    cert = make_certificate(key, "127.0.0.1")
    s_config = {
      :SSLEnable =>true,
      :ServerName => "localhost",
      :SSLCertificate => cert,
      :SSLPrivateKey => key,
    }
    config = {
      :ServerName => "localhost.localdomain",
      :RequestCallback => Proc.new{|req, res|
        assert_equal("CONNECT", req.request_method)
      },
    }
    TestWEBrick.start_httpserver(s_config){|s_server, s_addr, s_port, s_log|
      s_server.mount_proc("/"){|req, res|
        res.body = "SSL #{req.request_method} #{req.path} #{req.body}"
      }
      TestWEBrick.start_httpproxy(config){|server, addr, port, log|
        http = Net::HTTP.new("127.0.0.1", s_port, addr, port)
        http.use_ssl = true
        http.verify_callback = Proc.new do |preverify_ok, store_ctx|
          store_ctx.current_cert.to_der == cert.to_der
        end

        req = Net::HTTP::Get.new("/")
        http.request(req){|res|
          assert_equal("SSL GET / ", res.body, s_log.call + log.call)
        }

        req = Net::HTTP::Post.new("/")
        req.body = "post-data"
        http.request(req){|res|
          assert_equal("SSL POST / post-data", res.body, s_log.call + log.call)
        }
      }
    }
  end if defined?(OpenSSL)

  def test_upstream_proxy
    # Testing GET or POST through the upstream proxy server
    # Note that the upstream proxy server works as the origin server.
    #                                   +------+
    #                                   V      |
    #  client -------> proxy -------> proxy ---+
    #        GET / POST     GET / POST     GET / POST
    #
    up_proxy_handler_called = up_request_handler_called = 0
    proxy_handler_called = request_handler_called = 0
    up_config = {
      :ServerName => "localhost.localdomain",
      :ProxyContentHandler => Proc.new{|req, res| up_proxy_handler_called += 1},
      :RequestCallback => Proc.new{|req, res| up_request_handler_called += 1}
    }
    TestWEBrick.start_httpproxy(up_config){|up_server, up_addr, up_port, up_log|
      up_server.mount_proc("/"){|req, res|
        res.body = "#{req.request_method} #{req.path} #{req.body}"
      }
      config = {
        :ServerName => "localhost.localdomain",
        :ProxyURI => URI.parse("http://localhost:#{up_port}"),
        :ProxyContentHandler => Proc.new{|req, res| proxy_handler_called += 1},
        :RequestCallback => Proc.new{|req, res| request_handler_called += 1},
      }
      TestWEBrick.start_httpproxy(config){|server, addr, port, log|
        http = Net::HTTP.new(up_addr, up_port, addr, port)

        req = Net::HTTP::Get.new("/")
        http.request(req){|res|
          skip res.message unless res.code == '200'
          via = res["via"].split(/,\s+/)
          assert(via.include?("1.1 localhost.localdomain:#{up_port}"), up_log.call + log.call)
          assert(via.include?("1.1 localhost.localdomain:#{port}"), up_log.call + log.call)
          assert_equal("GET / ", res.body)
        }
        assert_equal(1, up_proxy_handler_called, up_log.call + log.call)
        assert_equal(2, up_request_handler_called, up_log.call + log.call)
        assert_equal(1, proxy_handler_called, up_log.call + log.call)
        assert_equal(1, request_handler_called, up_log.call + log.call)

        req = Net::HTTP::Head.new("/")
        http.request(req){|res|
          via = res["via"].split(/,\s+/)
          assert(via.include?("1.1 localhost.localdomain:#{up_port}"), up_log.call + log.call)
          assert(via.include?("1.1 localhost.localdomain:#{port}"), up_log.call + log.call)
          assert_nil(res.body, up_log.call + log.call)
        }
        assert_equal(2, up_proxy_handler_called, up_log.call + log.call)
        assert_equal(4, up_request_handler_called, up_log.call + log.call)
        assert_equal(2, proxy_handler_called, up_log.call + log.call)
        assert_equal(2, request_handler_called, up_log.call + log.call)

        req = Net::HTTP::Post.new("/")
        req.body = "post-data"
        http.request(req){|res|
          via = res["via"].split(/,\s+/)
          assert(via.include?("1.1 localhost.localdomain:#{up_port}"), up_log.call + log.call)
          assert(via.include?("1.1 localhost.localdomain:#{port}"), up_log.call + log.call)
          assert_equal("POST / post-data", res.body, up_log.call + log.call)
        }
        assert_equal(3, up_proxy_handler_called, up_log.call + log.call)
        assert_equal(6, up_request_handler_called, up_log.call + log.call)
        assert_equal(3, proxy_handler_called, up_log.call + log.call)
        assert_equal(3, request_handler_called, up_log.call + log.call)

        if defined?(OpenSSL)
          # Testing CONNECT to the upstream proxy server
          #
          #  client -------> proxy -------> proxy -------> https
          #    1.   CONNECT        CONNECT      establish TCP
          #    2.   -------- establish SSL session ------>
          #    3.   ---------- GET or POST -------------->
          #
          key = OpenSSL::TestUtils::TEST_KEY_RSA1024
          cert = make_certificate(key, "127.0.0.1")
          s_config = {
            :SSLEnable =>true,
            :ServerName => "localhost",
            :SSLCertificate => cert,
            :SSLPrivateKey => key,
          }
          TestWEBrick.start_httpserver(s_config){|s_server, s_addr, s_port, s_log|
            s_server.mount_proc("/"){|req2, res|
              res.body = "SSL #{req2.request_method} #{req2.path} #{req2.body}"
            }
            http = Net::HTTP.new("127.0.0.1", s_port, addr, port, up_log.call + log.call + s_log.call)
            http.use_ssl = true
            http.verify_callback = Proc.new do |preverify_ok, store_ctx|
              store_ctx.current_cert.to_der == cert.to_der
            end

            req2 = Net::HTTP::Get.new("/")
            http.request(req2){|res|
              assert_equal("SSL GET / ", res.body, up_log.call + log.call + s_log.call)
            }

            req2 = Net::HTTP::Post.new("/")
            req2.body = "post-data"
            http.request(req2){|res|
              assert_equal("SSL POST / post-data", res.body, up_log.call + log.call + s_log.call)
            }
          }
        end
      }
    }
  end
end
