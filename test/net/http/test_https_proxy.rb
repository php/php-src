begin
  require 'net/https'
rescue LoadError
end
require 'test/unit'

class HTTPSProxyTest < Test::Unit::TestCase
  def test_https_proxy_authentication
    begin
      OpenSSL
    rescue LoadError
      skip 'autoload problem. see [ruby-dev:45021][Bug #5786]'
    end

    t = nil
    TCPServer.open("127.0.0.1", 0) {|serv|
      _, port, _, _ = serv.addr
      t = Thread.new {
        proxy = Net::HTTP.Proxy("127.0.0.1", port, 'user', 'password')
        http = proxy.new("foo.example.org", 8000)
        http.use_ssl = true
        http.verify_mode = OpenSSL::SSL::VERIFY_NONE
        begin
          http.start
        rescue EOFError
        end
      }
      sock = serv.accept
      proxy_request = sock.gets("\r\n\r\n")
      assert_equal(
        "CONNECT foo.example.org:8000 HTTP/1.1\r\n" +
        "Host: foo.example.org:8000\r\n" +
        "Proxy-Authorization: Basic dXNlcjpwYXNzd29yZA==\r\n" +
        "\r\n",
        proxy_request,
        "[ruby-dev:25673]")
      sock.close
    }
  ensure
    t.join if t
  end
end if defined?(OpenSSL)

