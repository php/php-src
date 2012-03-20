require 'test/unit'
require 'open-uri'
require 'webrick'
require 'webrick/httpproxy'
require 'zlib'

class TestOpenURI < Test::Unit::TestCase

  NullLog = Object.new
  def NullLog.<<(arg)
  end

  def with_http
    Dir.mktmpdir {|dr|
      srv = WEBrick::HTTPServer.new({
        :DocumentRoot => dr,
        :ServerType => Thread,
        :Logger => WEBrick::Log.new(NullLog),
        :AccessLog => [[NullLog, ""]],
        :BindAddress => '127.0.0.1',
        :Port => 0})
      _, port, _, host = srv.listeners[0].addr
      begin
        th = srv.start
        yield srv, dr, "http://#{host}:#{port}"
      ensure
        srv.shutdown
      end
    }
  end

  def with_env(h)
    begin
      old = {}
      h.each_key {|k| old[k] = ENV[k] }
      h.each {|k, v| ENV[k] = v }
      yield
    ensure
      h.each_key {|k| ENV[k] = old[k] }
    end
  end

  def setup
    @proxies = %w[http_proxy HTTP_PROXY ftp_proxy FTP_PROXY no_proxy]
    @old_proxies = @proxies.map {|k| ENV[k] }
    @proxies.each {|k| ENV[k] = nil }
  end

  def teardown
    @proxies.each_with_index {|k, i| ENV[k] = @old_proxies[i] }
  end

  def test_200
    with_http {|srv, dr, url|
      open("#{dr}/foo200", "w") {|f| f << "foo200" }
      open("#{url}/foo200") {|f|
        assert_equal("200", f.status[0])
        assert_equal("foo200", f.read)
      }
    }
  end

  def test_200big
    with_http {|srv, dr, url|
      content = "foo200big"*10240
      open("#{dr}/foo200big", "w") {|f| f << content }
      open("#{url}/foo200big") {|f|
        assert_equal("200", f.status[0])
        assert_equal(content, f.read)
      }
    }
  end

  def test_404
    with_http {|srv, dr, url|
      exc = assert_raise(OpenURI::HTTPError) { open("#{url}/not-exist") {} }
      assert_equal("404", exc.io.status[0])
    }
  end

  def test_open_uri
    with_http {|srv, dr, url|
      open("#{dr}/foo_ou", "w") {|f| f << "foo_ou" }
      u = URI("#{url}/foo_ou")
      open(u) {|f|
        assert_equal("200", f.status[0])
        assert_equal("foo_ou", f.read)
      }
    }
  end

  def test_open_too_many_arg
    assert_raise(ArgumentError) { open("http://192.0.2.1/tma", "r", 0666, :extra) {} }
  end

  def test_read_timeout
    TCPServer.open("127.0.0.1", 0) {|serv|
      port = serv.addr[1]
      th = Thread.new {
        sock = serv.accept
        begin
          req = sock.gets("\r\n\r\n")
          assert_match(%r{\AGET /foo/bar }, req)
          sock.print "HTTP/1.0 200 OK\r\n"
          sock.print "Content-Length: 4\r\n\r\n"
          sleep 1
          sock.print "ab\r\n"
        ensure
          sock.close
        end
      }
      begin
        assert_raise(Timeout::Error) { URI("http://127.0.0.1:#{port}/foo/bar").read(:read_timeout=>0.01) }
      ensure
        Thread.kill(th)
        th.join
      end
    }
  end

  def test_invalid_option
    assert_raise(ArgumentError) { open("http://127.0.0.1/", :invalid_option=>true) {} }
  end

  def test_mode
    with_http {|srv, dr, url|
      open("#{dr}/mode", "w") {|f| f << "mode" }
      open("#{url}/mode", "r") {|f|
        assert_equal("200", f.status[0])
        assert_equal("mode", f.read)
      }
      open("#{url}/mode", "r", 0600) {|f|
        assert_equal("200", f.status[0])
        assert_equal("mode", f.read)
      }
      assert_raise(ArgumentError) { open("#{url}/mode", "a") {} }
      open("#{url}/mode", "r:us-ascii") {|f|
        assert_equal(Encoding::US_ASCII, f.read.encoding)
      }
      open("#{url}/mode", "r:utf-8") {|f|
        assert_equal(Encoding::UTF_8, f.read.encoding)
      }
      assert_raise(ArgumentError) { open("#{url}/mode", "r:invalid-encoding") {} }
    }
  end

  def test_without_block
    with_http {|srv, dr, url|
      open("#{dr}/without_block", "w") {|g| g << "without_block" }
      begin
        f = open("#{url}/without_block")
        assert_equal("200", f.status[0])
        assert_equal("without_block", f.read)
      ensure
        f.close
      end
    }
  end

  def test_header
    myheader1 = 'barrrr'
    myheader2 = nil
    with_http {|srv, dr, url|
      srv.mount_proc("/h/") {|req, res| myheader2 = req['myheader']; res.body = "foo" }
      open("#{url}/h/", 'MyHeader'=>myheader1) {|f|
        assert_equal("foo", f.read)
        assert_equal(myheader1, myheader2)
      }
    }
  end

  def test_multi_proxy_opt
    assert_raise(ArgumentError) {
      open("http://127.0.0.1/", :proxy_http_basic_authentication=>true, :proxy=>true) {}
    }
  end

  def test_non_http_proxy
    assert_raise(RuntimeError) {
      open("http://127.0.0.1/", :proxy=>URI("ftp://127.0.0.1/")) {}
    }
  end

  def test_proxy
    with_http {|srv, dr, url|
      log = ''
      proxy = WEBrick::HTTPProxyServer.new({
        :ServerType => Thread,
        :Logger => WEBrick::Log.new(NullLog),
        :AccessLog => [[NullLog, ""]],
        :ProxyAuthProc => lambda {|req, res|
          log << req.request_line
        },
        :BindAddress => '127.0.0.1',
        :Port => 0})
      _, proxy_port, _, proxy_host = proxy.listeners[0].addr
      proxy_url = "http://#{proxy_host}:#{proxy_port}/"
      begin
        th = proxy.start
        open("#{dr}/proxy", "w") {|f| f << "proxy" }
        open("#{url}/proxy", :proxy=>proxy_url) {|f|
          assert_equal("200", f.status[0])
          assert_equal("proxy", f.read)
        }
        assert_match(/#{Regexp.quote url}/, log); log.clear
        open("#{url}/proxy", :proxy=>URI(proxy_url)) {|f|
          assert_equal("200", f.status[0])
          assert_equal("proxy", f.read)
        }
        assert_match(/#{Regexp.quote url}/, log); log.clear
        open("#{url}/proxy", :proxy=>nil) {|f|
          assert_equal("200", f.status[0])
          assert_equal("proxy", f.read)
        }
        assert_equal("", log); log.clear
        assert_raise(ArgumentError) {
          open("#{url}/proxy", :proxy=>:invalid) {}
        }
        assert_equal("", log); log.clear
        with_env("http_proxy"=>proxy_url) {
          # should not use proxy for 127.0.0.0/8.
          open("#{url}/proxy") {|f|
            assert_equal("200", f.status[0])
            assert_equal("proxy", f.read)
          }
        }
        assert_equal("", log); log.clear
      ensure
        proxy.shutdown
      end
    }
  end

  def test_proxy_http_basic_authentication
    with_http {|srv, dr, url|
      log = ''
      proxy = WEBrick::HTTPProxyServer.new({
        :ServerType => Thread,
        :Logger => WEBrick::Log.new(NullLog),
        :AccessLog => [[NullLog, ""]],
        :ProxyAuthProc => lambda {|req, res|
          log << req.request_line
          if req["Proxy-Authorization"] != "Basic #{['user:pass'].pack('m').chomp}"
            raise WEBrick::HTTPStatus::ProxyAuthenticationRequired
          end
        },
        :BindAddress => '127.0.0.1',
        :Port => 0})
      _, proxy_port, _, proxy_host = proxy.listeners[0].addr
      proxy_url = "http://#{proxy_host}:#{proxy_port}/"
      begin
        th = proxy.start
        open("#{dr}/proxy", "w") {|f| f << "proxy" }
        exc = assert_raise(OpenURI::HTTPError) { open("#{url}/proxy", :proxy=>proxy_url) {} }
        assert_equal("407", exc.io.status[0])
        assert_match(/#{Regexp.quote url}/, log); log.clear
        open("#{url}/proxy",
            :proxy_http_basic_authentication=>[proxy_url, "user", "pass"]) {|f|
          assert_equal("200", f.status[0])
          assert_equal("proxy", f.read)
        }
        assert_match(/#{Regexp.quote url}/, log); log.clear
        assert_raise(ArgumentError) {
          open("#{url}/proxy",
              :proxy_http_basic_authentication=>[true, "user", "pass"]) {}
        }
        assert_equal("", log); log.clear
      ensure
        proxy.shutdown
      end
    }
  end

  def test_redirect
    with_http {|srv, dr, url|
      srv.mount_proc("/r1/") {|req, res| res.status = 301; res["location"] = "#{url}/r2"; res.body = "r1" }
      srv.mount_proc("/r2/") {|req, res| res.body = "r2" }
      srv.mount_proc("/to-file/") {|req, res| res.status = 301; res["location"] = "file:///foo" }
      open("#{url}/r1/") {|f|
        assert_equal("#{url}/r2", f.base_uri.to_s)
        assert_equal("r2", f.read)
      }
      assert_raise(OpenURI::HTTPRedirect) { open("#{url}/r1/", :redirect=>false) {} }
      assert_raise(RuntimeError) { open("#{url}/to-file/") {} }
    }
  end

  def test_redirect_loop
    with_http {|srv, dr, url|
      srv.mount_proc("/r1/") {|req, res| res.status = 301; res["location"] = "#{url}/r2"; res.body = "r1" }
      srv.mount_proc("/r2/") {|req, res| res.status = 301; res["location"] = "#{url}/r1"; res.body = "r2" }
      assert_raise(RuntimeError) { open("#{url}/r1/") {} }
    }
  end

  def test_redirect_relative
    TCPServer.open("127.0.0.1", 0) {|serv|
      port = serv.addr[1]
      th = Thread.new {
        sock = serv.accept
        begin
          req = sock.gets("\r\n\r\n")
          assert_match(%r{\AGET /foo/bar }, req)
          sock.print "HTTP/1.0 302 Found\r\n"
          sock.print "Location: ../baz\r\n\r\n"
        ensure
          sock.close
        end
        sock = serv.accept
        begin
          req = sock.gets("\r\n\r\n")
          assert_match(%r{\AGET /baz }, req)
          sock.print "HTTP/1.0 200 OK\r\n"
          sock.print "Content-Length: 4\r\n\r\n"
          sock.print "ab\r\n"
        ensure
          sock.close
        end
      }
      begin
        content = URI("http://127.0.0.1:#{port}/foo/bar").read
        assert_equal("ab\r\n", content)
      ensure
        Thread.kill(th)
        th.join
      end
    }
  end

  def test_redirect_invalid
    TCPServer.open("127.0.0.1", 0) {|serv|
      port = serv.addr[1]
      th = Thread.new {
        sock = serv.accept
        begin
          req = sock.gets("\r\n\r\n")
          assert_match(%r{\AGET /foo/bar }, req)
          sock.print "HTTP/1.0 302 Found\r\n"
          sock.print "Location: ::\r\n\r\n"
        ensure
          sock.close
        end
      }
      begin
        assert_raise(OpenURI::HTTPError) {
          URI("http://127.0.0.1:#{port}/foo/bar").read
        }
      ensure
        Thread.kill(th)
        th.join
      end
    }
  end

  def test_redirect_auth
    with_http {|srv, dr, url|
      srv.mount_proc("/r1/") {|req, res| res.status = 301; res["location"] = "#{url}/r2" }
      srv.mount_proc("/r2/") {|req, res|
        if req["Authorization"] != "Basic #{['user:pass'].pack('m').chomp}"
          raise WEBrick::HTTPStatus::Unauthorized
        end
        res.body = "r2"
      }
      exc = assert_raise(OpenURI::HTTPError) { open("#{url}/r2/") {} }
      assert_equal("401", exc.io.status[0])
      open("#{url}/r2/", :http_basic_authentication=>['user', 'pass']) {|f|
        assert_equal("r2", f.read)
      }
      exc = assert_raise(OpenURI::HTTPError) { open("#{url}/r1/", :http_basic_authentication=>['user', 'pass']) {} }
      assert_equal("401", exc.io.status[0])
    }
  end

  def test_userinfo
    if "1.9.0" <= RUBY_VERSION
      assert_raise(ArgumentError) { open("http://user:pass@127.0.0.1/") {} }
    end
  end

  def test_progress
    with_http {|srv, dr, url|
      content = "a" * 100000
      srv.mount_proc("/data/") {|req, res| res.body = content }
      length = []
      progress = []
      open("#{url}/data/",
           :content_length_proc => lambda {|n| length << n },
           :progress_proc => lambda {|n| progress << n }
          ) {|f|
        assert_equal(1, length.length)
        assert_equal(content.length, length[0])
        assert(progress.length>1,"maybe test is wrong")
        assert(progress.sort == progress,"monotone increasing expected but was\n#{progress.inspect}")
        assert_equal(content.length, progress[-1])
        assert_equal(content, f.read)
      }
    }
  end

  def test_progress_chunked
    with_http {|srv, dr, url|
      content = "a" * 100000
      srv.mount_proc("/data/") {|req, res| res.body = content; res.chunked = true }
      length = []
      progress = []
      open("#{url}/data/",
           :content_length_proc => lambda {|n| length << n },
           :progress_proc => lambda {|n| progress << n }
          ) {|f|
        assert_equal(1, length.length)
        assert_equal(nil, length[0])
        assert(progress.length>1,"maybe test is worng")
        assert(progress.sort == progress,"monotone increasing expected but was\n#{progress.inspect}")
        assert_equal(content.length, progress[-1])
        assert_equal(content, f.read)
      }
    }
  end

  def test_uri_read
    with_http {|srv, dr, url|
      open("#{dr}/uriread", "w") {|f| f << "uriread" }
      data = URI("#{url}/uriread").read
      assert_equal("200", data.status[0])
      assert_equal("uriread", data)
    }
  end

  def test_encoding
    with_http {|srv, dr, url|
      content_u8 = "\u3042"
      content_ej = "\xa2\xa4".force_encoding("euc-jp")
      srv.mount_proc("/u8/") {|req, res| res.body = content_u8; res['content-type'] = 'text/plain; charset=utf-8' }
      srv.mount_proc("/ej/") {|req, res| res.body = content_ej; res['content-type'] = 'TEXT/PLAIN; charset=EUC-JP' }
      srv.mount_proc("/nc/") {|req, res| res.body = "aa"; res['content-type'] = 'Text/Plain' }
      open("#{url}/u8/") {|f|
        assert_equal(content_u8, f.read)
        assert_equal("text/plain", f.content_type)
        assert_equal("utf-8", f.charset)
      }
      open("#{url}/ej/") {|f|
        assert_equal(content_ej, f.read)
        assert_equal("text/plain", f.content_type)
        assert_equal("euc-jp", f.charset)
      }
      open("#{url}/nc/") {|f|
        assert_equal("aa", f.read)
        assert_equal("text/plain", f.content_type)
        assert_equal("iso-8859-1", f.charset)
        assert_equal("unknown", f.charset { "unknown" })
      }
    }
  end

  def test_quoted_attvalue
    with_http {|srv, dr, url|
      content_u8 = "\u3042"
      srv.mount_proc("/qu8/") {|req, res| res.body = content_u8; res['content-type'] = 'text/plain; charset="utf\-8"' }
      open("#{url}/qu8/") {|f|
        assert_equal(content_u8, f.read)
        assert_equal("text/plain", f.content_type)
        assert_equal("utf-8", f.charset)
      }
    }
  end

  def test_last_modified
    with_http {|srv, dr, url|
      srv.mount_proc("/data/") {|req, res| res.body = "foo"; res['last-modified'] = 'Fri, 07 Aug 2009 06:05:04 GMT' }
      open("#{url}/data/") {|f|
        assert_equal("foo", f.read)
        assert_equal(Time.utc(2009,8,7,6,5,4), f.last_modified)
      }
    }
  end

  def test_content_encoding
    with_http {|srv, dr, url|
      content = "abc" * 10000
      Zlib::GzipWriter.wrap(StringIO.new(content_gz="".force_encoding("ascii-8bit"))) {|z| z.write content }
      srv.mount_proc("/data/") {|req, res| res.body = content_gz; res['content-encoding'] = 'gzip' }
      srv.mount_proc("/data2/") {|req, res| res.body = content_gz; res['content-encoding'] = 'gzip'; res.chunked = true }
      srv.mount_proc("/noce/") {|req, res| res.body = content_gz }
      open("#{url}/data/") {|f|
        assert_equal ['gzip'], f.content_encoding
        assert_equal(content_gz, f.read.force_encoding("ascii-8bit"))
      }
      open("#{url}/data2/") {|f|
        assert_equal ['gzip'], f.content_encoding
        assert_equal(content_gz, f.read.force_encoding("ascii-8bit"))
      }
      open("#{url}/noce/") {|f|
        assert_equal [], f.content_encoding
        assert_equal(content_gz, f.read.force_encoding("ascii-8bit"))
      }
    }
  end

  # 192.0.2.0/24 is TEST-NET.  [RFC3330]

  def test_find_proxy
    assert_nil(URI("http://192.0.2.1/").find_proxy)
    assert_nil(URI("ftp://192.0.2.1/").find_proxy)
    with_env('http_proxy'=>'http://127.0.0.1:8080') {
      assert_equal(URI('http://127.0.0.1:8080'), URI("http://192.0.2.1/").find_proxy)
      assert_nil(URI("ftp://192.0.2.1/").find_proxy)
    }
    with_env('ftp_proxy'=>'http://127.0.0.1:8080') {
      assert_nil(URI("http://192.0.2.1/").find_proxy)
      assert_equal(URI('http://127.0.0.1:8080'), URI("ftp://192.0.2.1/").find_proxy)
    }
    with_env('REQUEST_METHOD'=>'GET') {
      assert_nil(URI("http://192.0.2.1/").find_proxy)
    }
    with_env('CGI_HTTP_PROXY'=>'http://127.0.0.1:8080', 'REQUEST_METHOD'=>'GET') {
      assert_equal(URI('http://127.0.0.1:8080'), URI("http://192.0.2.1/").find_proxy)
    }
    with_env('http_proxy'=>'http://127.0.0.1:8080', 'no_proxy'=>'192.0.2.2') {
      assert_equal(URI('http://127.0.0.1:8080'), URI("http://192.0.2.1/").find_proxy)
      assert_nil(URI("http://192.0.2.2/").find_proxy)
    }
  end

  def test_find_proxy_case_sensitive_env
    with_env('http_proxy'=>'http://127.0.0.1:8080', 'REQUEST_METHOD'=>'GET') {
      assert_equal(URI('http://127.0.0.1:8080'), URI("http://192.0.2.1/").find_proxy)
    }
    with_env('HTTP_PROXY'=>'http://127.0.0.1:8081', 'REQUEST_METHOD'=>'GET') {
      assert_nil(nil, URI("http://192.0.2.1/").find_proxy)
    }
    with_env('http_proxy'=>'http://127.0.0.1:8080', 'HTTP_PROXY'=>'http://127.0.0.1:8081', 'REQUEST_METHOD'=>'GET') {
      assert_equal(URI('http://127.0.0.1:8080'), URI("http://192.0.2.1/").find_proxy)
    }
  end unless RUBY_PLATFORM =~ /mswin|mingw/

  def test_ftp_invalid_request
    assert_raise(ArgumentError) { URI("ftp://127.0.0.1/").read }
    assert_raise(ArgumentError) { URI("ftp://127.0.0.1/a%0Db").read }
    assert_raise(ArgumentError) { URI("ftp://127.0.0.1/a%0Ab").read }
    assert_raise(ArgumentError) { URI("ftp://127.0.0.1/a%0Db/f").read }
    assert_raise(ArgumentError) { URI("ftp://127.0.0.1/a%0Ab/f").read }
    assert_raise(URI::InvalidComponentError) { URI("ftp://127.0.0.1/d/f;type=x") }
  end

  def test_ftp
    TCPServer.open("127.0.0.1", 0) {|serv|
      _, port, _, host = serv.addr
      th = Thread.new {
        s = serv.accept
        begin
          s.print "220 Test FTP Server\r\n"
          assert_equal("USER anonymous\r\n", s.gets); s.print "331 name ok\r\n"
          assert_match(/\APASS .*\r\n\z/, s.gets); s.print "230 logged in\r\n"
          assert_equal("TYPE I\r\n", s.gets); s.print "200 type set to I\r\n"
          assert_equal("CWD foo\r\n", s.gets); s.print "250 CWD successful\r\n"
          assert_equal("PASV\r\n", s.gets)
          TCPServer.open("127.0.0.1", 0) {|data_serv|
            _, data_serv_port, _, data_serv_host = data_serv.addr
            hi = data_serv_port >> 8
            lo = data_serv_port & 0xff
            s.print "227 Entering Passive Mode (127,0,0,1,#{hi},#{lo}).\r\n"
            assert_equal("RETR bar\r\n", s.gets); s.print "150 file okay\r\n"
            data_sock = data_serv.accept
            begin
              data_sock << "content"
            ensure
              data_sock.close
            end
            s.print "226 transfer complete\r\n"
            assert_nil(s.gets)
          }
        ensure
          s.close if s
        end
      }
      begin
        content = URI("ftp://#{host}:#{port}/foo/bar").read
        assert_equal("content", content)
      ensure
        Thread.kill(th)
        th.join
      end
    }
  end

  def test_ftp_active
    TCPServer.open("127.0.0.1", 0) {|serv|
      _, port, _, host = serv.addr
      th = Thread.new {
        s = serv.accept
        begin
          content = "content"
          s.print "220 Test FTP Server\r\n"
          assert_equal("USER anonymous\r\n", s.gets); s.print "331 name ok\r\n"
          assert_match(/\APASS .*\r\n\z/, s.gets); s.print "230 logged in\r\n"
          assert_equal("TYPE I\r\n", s.gets); s.print "200 type set to I\r\n"
          assert_equal("CWD foo\r\n", s.gets); s.print "250 CWD successful\r\n"
          assert(m = /\APORT 127,0,0,1,(\d+),(\d+)\r\n\z/.match(s.gets))
          active_port = m[1].to_i << 8 | m[2].to_i
          TCPSocket.open("127.0.0.1", active_port) {|data_sock|
            s.print "200 data connection opened\r\n"
            assert_equal("RETR bar\r\n", s.gets); s.print "150 file okay\r\n"
            begin
              data_sock << content
            ensure
              data_sock.close
            end
            s.print "226 transfer complete\r\n"
            assert_nil(s.gets)
          }
        ensure
          s.close if s
        end
      }
      begin
        content = URI("ftp://#{host}:#{port}/foo/bar").read(:ftp_active_mode=>true)
        assert_equal("content", content)
      ensure
        Thread.kill(th)
        th.join
      end
    }
  end

  def test_ftp_ascii
    TCPServer.open("127.0.0.1", 0) {|serv|
      _, port, _, host = serv.addr
      th = Thread.new {
        s = serv.accept
        begin
          content = "content"
          s.print "220 Test FTP Server\r\n"
          assert_equal("USER anonymous\r\n", s.gets); s.print "331 name ok\r\n"
          assert_match(/\APASS .*\r\n\z/, s.gets); s.print "230 logged in\r\n"
          assert_equal("TYPE I\r\n", s.gets); s.print "200 type set to I\r\n"
          assert_equal("CWD /foo\r\n", s.gets); s.print "250 CWD successful\r\n"
          assert_equal("TYPE A\r\n", s.gets); s.print "200 type set to A\r\n"
          assert_equal("SIZE bar\r\n", s.gets); s.print "213 #{content.bytesize}\r\n"
          assert_equal("PASV\r\n", s.gets)
          TCPServer.open("127.0.0.1", 0) {|data_serv|
            _, data_serv_port, _, data_serv_host = data_serv.addr
            hi = data_serv_port >> 8
            lo = data_serv_port & 0xff
            s.print "227 Entering Passive Mode (127,0,0,1,#{hi},#{lo}).\r\n"
            assert_equal("RETR bar\r\n", s.gets); s.print "150 file okay\r\n"
            data_sock = data_serv.accept
            begin
              data_sock << content
            ensure
              data_sock.close
            end
            s.print "226 transfer complete\r\n"
            assert_nil(s.gets)
          }
        ensure
          s.close if s
        end
      }
      begin
        length = []
        progress = []
        content = URI("ftp://#{host}:#{port}/%2Ffoo/b%61r;type=a").read(
         :content_length_proc => lambda {|n| length << n },
         :progress_proc => lambda {|n| progress << n })
        assert_equal("content", content)
        assert_equal([7], length)
        assert_equal(7, progress.inject(&:+))
      ensure
        Thread.kill(th)
        th.join
      end
    }
  end

  def test_ftp_over_http_proxy
    TCPServer.open("127.0.0.1", 0) {|proxy_serv|
      proxy_port = proxy_serv.addr[1]
      th = Thread.new {
        proxy_sock = proxy_serv.accept
        begin
          req = proxy_sock.gets("\r\n\r\n")
          assert_match(%r{\AGET ftp://192.0.2.1/foo/bar }, req)
          proxy_sock.print "HTTP/1.0 200 OK\r\n"
          proxy_sock.print "Content-Length: 4\r\n\r\n"
          proxy_sock.print "ab\r\n"
        ensure
          proxy_sock.close
        end
      }
      begin
        with_env('ftp_proxy'=>"http://127.0.0.1:#{proxy_port}") {
          content = URI("ftp://192.0.2.1/foo/bar").read
          assert_equal("ab\r\n", content)
        }
      ensure
        Thread.kill(th)
        th.join
      end
    }
  end

  def test_ftp_over_http_proxy_auth
    TCPServer.open("127.0.0.1", 0) {|proxy_serv|
      proxy_port = proxy_serv.addr[1]
      th = Thread.new {
        proxy_sock = proxy_serv.accept
        begin
          req = proxy_sock.gets("\r\n\r\n")
          assert_match(%r{\AGET ftp://192.0.2.1/foo/bar }, req)
          assert_match(%r{Proxy-Authorization: Basic #{['proxy-user:proxy-password'].pack('m').chomp}\r\n}, req)
          proxy_sock.print "HTTP/1.0 200 OK\r\n"
          proxy_sock.print "Content-Length: 4\r\n\r\n"
          proxy_sock.print "ab\r\n"
        ensure
          proxy_sock.close
        end
      }
      begin
        content = URI("ftp://192.0.2.1/foo/bar").read(
          :proxy_http_basic_authentication => ["http://127.0.0.1:#{proxy_port}", "proxy-user", "proxy-password"])
        assert_equal("ab\r\n", content)
      ensure
        Thread.kill(th)
        th.join
      end
    }
  end

end

