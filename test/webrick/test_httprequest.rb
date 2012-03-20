require "webrick"
require "stringio"
require "test/unit"

class TestWEBrickHTTPRequest < Test::Unit::TestCase
  def test_simple_request
    msg = <<-_end_of_message_
GET /
    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg))
    assert(req.meta_vars) # fails if @header was not initialized and iteration is attempted on the nil reference
  end

  def test_parse_09
    msg = <<-_end_of_message_
      GET /
      foobar    # HTTP/0.9 request don't have header nor entity body.
    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal("GET", req.request_method)
    assert_equal("/", req.unparsed_uri)
    assert_equal(WEBrick::HTTPVersion.new("0.9"), req.http_version)
    assert_equal(WEBrick::Config::HTTP[:ServerName], req.host)
    assert_equal(80, req.port)
    assert_equal(false, req.keep_alive?)
    assert_equal(nil, req.body)
    assert(req.query.empty?)
  end

  def test_parse_10
    msg = <<-_end_of_message_
      GET / HTTP/1.0

    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal("GET", req.request_method)
    assert_equal("/", req.unparsed_uri)
    assert_equal(WEBrick::HTTPVersion.new("1.0"), req.http_version)
    assert_equal(WEBrick::Config::HTTP[:ServerName], req.host)
    assert_equal(80, req.port)
    assert_equal(false, req.keep_alive?)
    assert_equal(nil, req.body)
    assert(req.query.empty?)
  end

  def test_parse_11
    msg = <<-_end_of_message_
      GET /path HTTP/1.1

    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal("GET", req.request_method)
    assert_equal("/path", req.unparsed_uri)
    assert_equal("", req.script_name)
    assert_equal("/path", req.path_info)
    assert_equal(WEBrick::HTTPVersion.new("1.1"), req.http_version)
    assert_equal(WEBrick::Config::HTTP[:ServerName], req.host)
    assert_equal(80, req.port)
    assert_equal(true, req.keep_alive?)
    assert_equal(nil, req.body)
    assert(req.query.empty?)
  end

  def test_request_uri_too_large
    msg = <<-_end_of_message_
      GET /#{"a"*2084} HTTP/1.1
    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    assert_raise(WEBrick::HTTPStatus::RequestURITooLarge){
      req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    }
  end

  def test_parse_headers
    msg = <<-_end_of_message_
      GET /path HTTP/1.1
      Host: test.ruby-lang.org:8080
      Connection: close
      Accept: text/*;q=0.3, text/html;q=0.7, text/html;level=1,
              text/html;level=2;q=0.4, */*;q=0.5
      Accept-Encoding: compress;q=0.5
      Accept-Encoding: gzip;q=1.0, identity; q=0.4, *;q=0
      Accept-Language: en;q=0.5, *; q=0
      Accept-Language: ja
      Content-Type: text/plain
      Content-Length: 7
      X-Empty-Header:

      foobar
    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal(
      URI.parse("http://test.ruby-lang.org:8080/path"), req.request_uri)
    assert_equal("test.ruby-lang.org", req.host)
    assert_equal(8080, req.port)
    assert_equal(false, req.keep_alive?)
    assert_equal(
      %w(text/html;level=1 text/html */* text/html;level=2 text/*),
      req.accept)
    assert_equal(%w(gzip compress identity *), req.accept_encoding)
    assert_equal(%w(ja en *), req.accept_language)
    assert_equal(7, req.content_length)
    assert_equal("text/plain", req.content_type)
    assert_equal("foobar\n", req.body)
    assert_equal("", req["x-empty-header"])
    assert_equal(nil, req["x-no-header"])
    assert(req.query.empty?)
  end

  def test_parse_header2()
    msg = <<-_end_of_message_
      POST /foo/bar/../baz?q=a HTTP/1.0
      Content-Length: 9
      User-Agent:
        FOO   BAR
        BAZ

      hogehoge
    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal("POST", req.request_method)
    assert_equal("/foo/baz", req.path)
    assert_equal("", req.script_name)
    assert_equal("/foo/baz", req.path_info)
    assert_equal("9", req['content-length'])
    assert_equal("FOO BAR BAZ", req['user-agent'])
    assert_equal("hogehoge\n", req.body)
  end

  def test_parse_headers3
    msg = <<-_end_of_message_
      GET /path HTTP/1.1
      Host: test.ruby-lang.org

    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal(URI.parse("http://test.ruby-lang.org/path"), req.request_uri)
    assert_equal("test.ruby-lang.org", req.host)
    assert_equal(80, req.port)

    msg = <<-_end_of_message_
      GET /path HTTP/1.1
      Host: 192.168.1.1

    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal(URI.parse("http://192.168.1.1/path"), req.request_uri)
    assert_equal("192.168.1.1", req.host)
    assert_equal(80, req.port)

    msg = <<-_end_of_message_
      GET /path HTTP/1.1
      Host: [fe80::208:dff:feef:98c7]

    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal(URI.parse("http://[fe80::208:dff:feef:98c7]/path"),
                 req.request_uri)
    assert_equal("[fe80::208:dff:feef:98c7]", req.host)
    assert_equal(80, req.port)

    msg = <<-_end_of_message_
      GET /path HTTP/1.1
      Host: 192.168.1.1:8080

    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal(URI.parse("http://192.168.1.1:8080/path"), req.request_uri)
    assert_equal("192.168.1.1", req.host)
    assert_equal(8080, req.port)

    msg = <<-_end_of_message_
      GET /path HTTP/1.1
      Host: [fe80::208:dff:feef:98c7]:8080

    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    assert_equal(URI.parse("http://[fe80::208:dff:feef:98c7]:8080/path"),
                 req.request_uri)
    assert_equal("[fe80::208:dff:feef:98c7]", req.host)
    assert_equal(8080, req.port)
  end

  def test_parse_get_params
    param = "foo=1;foo=2;foo=3;bar=x"
    msg = <<-_end_of_message_
      GET /path?#{param} HTTP/1.1
      Host: test.ruby-lang.org:8080

    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    query = req.query
    assert_equal("1", query["foo"])
    assert_equal(["1", "2", "3"], query["foo"].to_ary)
    assert_equal(["1", "2", "3"], query["foo"].list)
    assert_equal("x", query["bar"])
    assert_equal(["x"], query["bar"].list)
  end

  def test_parse_post_params
    param = "foo=1;foo=2;foo=3;bar=x"
    msg = <<-_end_of_message_
      POST /path?foo=x;foo=y;foo=z;bar=1 HTTP/1.1
      Host: test.ruby-lang.org:8080
      Content-Length: #{param.size}
      Content-Type: application/x-www-form-urlencoded

      #{param}
    _end_of_message_
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
    query = req.query
    assert_equal("1", query["foo"])
    assert_equal(["1", "2", "3"], query["foo"].to_ary)
    assert_equal(["1", "2", "3"], query["foo"].list)
    assert_equal("x", query["bar"])
    assert_equal(["x"], query["bar"].list)
  end

  def test_chunked
    crlf = "\x0d\x0a"
    msg = <<-_end_of_message_
      POST /path HTTP/1.1
      Host: test.ruby-lang.org:8080
      Transfer-Encoding: chunked

    _end_of_message_
    msg.gsub!(/^ {6}/, "")
    open(__FILE__){|io|
      while chunk = io.read(100)
        msg << chunk.size.to_s(16) << crlf
        msg << chunk << crlf
      end
    }
    msg << "0" << crlf
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg))
    assert_equal(File.read(__FILE__), req.body)
  end

  def test_forwarded
    msg = <<-_end_of_message_
      GET /foo HTTP/1.1
      Host: localhost:10080
      User-Agent: w3m/0.5.2
      X-Forwarded-For: 123.123.123.123
      X-Forwarded-Host: forward.example.com
      X-Forwarded-Server: server.example.com
      Connection: Keep-Alive

    _end_of_message_
    msg.gsub!(/^ {6}/, "")
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg))
    assert_equal("server.example.com", req.server_name)
    assert_equal("http://forward.example.com/foo", req.request_uri.to_s)
    assert_equal("forward.example.com", req.host)
    assert_equal(80, req.port)
    assert_equal("123.123.123.123", req.remote_ip)
    assert(!req.ssl?)

    msg = <<-_end_of_message_
      GET /foo HTTP/1.1
      Host: localhost:10080
      User-Agent: w3m/0.5.2
      X-Forwarded-For: 192.168.1.10, 172.16.1.1, 123.123.123.123
      X-Forwarded-Host: forward.example.com:8080
      X-Forwarded-Server: server.example.com
      Connection: Keep-Alive

    _end_of_message_
    msg.gsub!(/^ {6}/, "")
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg))
    assert_equal("server.example.com", req.server_name)
    assert_equal("http://forward.example.com:8080/foo", req.request_uri.to_s)
    assert_equal("forward.example.com", req.host)
    assert_equal(8080, req.port)
    assert_equal("123.123.123.123", req.remote_ip)
    assert(!req.ssl?)

    msg = <<-_end_of_message_
      GET /foo HTTP/1.1
      Host: localhost:10080
      Client-IP: 234.234.234.234
      X-Forwarded-Proto: https
      X-Forwarded-For: 192.168.1.10, 10.0.0.1, 123.123.123.123
      X-Forwarded-Host: forward.example.com
      X-Forwarded-Server: server.example.com
      X-Requested-With: XMLHttpRequest
      Connection: Keep-Alive

    _end_of_message_
    msg.gsub!(/^ {6}/, "")
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg))
    assert_equal("server.example.com", req.server_name)
    assert_equal("https://forward.example.com/foo", req.request_uri.to_s)
    assert_equal("forward.example.com", req.host)
    assert_equal(443, req.port)
    assert_equal("234.234.234.234", req.remote_ip)
    assert(req.ssl?)

    msg = <<-_end_of_message_
      GET /foo HTTP/1.1
      Host: localhost:10080
      Client-IP: 234.234.234.234
      X-Forwarded-Proto: https
      X-Forwarded-For: 192.168.1.10
      X-Forwarded-Host: forward1.example.com:1234, forward2.example.com:5678
      X-Forwarded-Server: server1.example.com, server2.example.com
      X-Requested-With: XMLHttpRequest
      Connection: Keep-Alive

    _end_of_message_
    msg.gsub!(/^ {6}/, "")
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg))
    assert_equal("server1.example.com", req.server_name)
    assert_equal("https://forward1.example.com:1234/foo", req.request_uri.to_s)
    assert_equal("forward1.example.com", req.host)
    assert_equal(1234, req.port)
    assert_equal("234.234.234.234", req.remote_ip)
    assert(req.ssl?)
  end

  def test_continue_sent
    msg = <<-_end_of_message_
      POST /path HTTP/1.1
      Expect: 100-continue

    _end_of_message_
    msg.gsub!(/^ {6}/, "")
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg))
    assert req['expect']
    l = msg.size
    req.continue
    assert_not_equal l, msg.size
    assert_match /HTTP\/1.1 100 continue\r\n\r\n\z/, msg
    assert !req['expect']
  end

  def test_continue_not_sent
    msg = <<-_end_of_message_
      POST /path HTTP/1.1

    _end_of_message_
    msg.gsub!(/^ {6}/, "")
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(StringIO.new(msg))
    assert !req['expect']
    l = msg.size
    req.continue
    assert_equal l, msg.size
  end

  def test_bad_messages
    param = "foo=1;foo=2;foo=3;bar=x"
    msg = <<-_end_of_message_
      POST /path?foo=x;foo=y;foo=z;bar=1 HTTP/1.1
      Host: test.ruby-lang.org:8080
      Content-Type: application/x-www-form-urlencoded

      #{param}
    _end_of_message_
    assert_raise(WEBrick::HTTPStatus::LengthRequired){
      req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
      req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
      req.body
    }

    msg = <<-_end_of_message_
      POST /path?foo=x;foo=y;foo=z;bar=1 HTTP/1.1
      Host: test.ruby-lang.org:8080
      Content-Length: 100000

      body is too short.
    _end_of_message_
    assert_raise(WEBrick::HTTPStatus::BadRequest){
      req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
      req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
      req.body
    }

    msg = <<-_end_of_message_
      POST /path?foo=x;foo=y;foo=z;bar=1 HTTP/1.1
      Host: test.ruby-lang.org:8080
      Transfer-Encoding: foobar

      body is too short.
    _end_of_message_
    assert_raise(WEBrick::HTTPStatus::NotImplemented){
      req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
      req.parse(StringIO.new(msg.gsub(/^ {6}/, "")))
      req.body
    }
  end
end
