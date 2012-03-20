require 'minitest/autorun'
require 'xmlrpc/client'
require 'net/http'

module XMLRPC
  class ClientTest < MiniTest::Unit::TestCase
    module Fake
      class HTTP
        attr_accessor :read_timeout, :open_timeout, :use_ssl

        def initialize responses = {}
          @started = false
          @responses = responses
        end

        def started?
          @started
        end
        def start
          @started = true
          if block_given?
            begin
              return yield(self)
            ensure
              @started = false
            end
          end
          self
        end

        def request_post path, request, headers
          @responses[path].shift
        end
      end

      class Client < XMLRPC::Client
        attr_reader :args, :http

        def initialize(*args)
          @args = args
          super
        end

        private
        def net_http host, port, proxy_host, proxy_port
          HTTP.new
        end
      end

      class Response
        def self.new body, fields = [], status = '200'
          klass = Class.new(Net::HTTPResponse::CODE_TO_OBJ[status]) {
            def initialize(*args)
              super
              @read = true
            end
          }

          resp = klass.new '1.1', status, 'OK'
          resp.body = body
          fields.each do |k,v|
            resp.add_field k, v
          end
          resp
        end
      end
    end

    def test_new2_host_path_port
      client = Fake::Client.new2 'http://example.org/foo'
      host, path, port, *rest = client.args

      assert_equal 'example.org', host
      assert_equal '/foo', path
      assert_equal 80, port

      rest.each { |x| refute x }
    end

    def test_new2_custom_port
      client = Fake::Client.new2 'http://example.org:1234/foo'
      host, path, port, *rest = client.args

      assert_equal 'example.org', host
      assert_equal '/foo', path
      assert_equal 1234, port

      rest.each { |x| refute x }
    end

    def test_new2_ssl
      client = Fake::Client.new2 'https://example.org/foo'
      host, path, port, proxy_host, proxy_port, user, password, use_ssl, timeout = client.args

      assert_equal 'example.org', host
      assert_equal '/foo', path
      assert_equal 443, port
      assert use_ssl

      refute proxy_host
      refute proxy_port
      refute user
      refute password
      refute timeout
    end

    def test_new2_ssl_custom_port
      client = Fake::Client.new2 'https://example.org:1234/foo'
      host, path, port, proxy_host, proxy_port, user, password, use_ssl, timeout = client.args

      assert_equal 'example.org', host
      assert_equal '/foo', path
      assert_equal 1234, port

      refute proxy_host
      refute proxy_port
      refute user
      refute password
      refute timeout
    end

    def test_new2_user_password
      client = Fake::Client.new2 'http://aaron:tenderlove@example.org/foo'
      host, path, port, proxy_host, proxy_port, user, password, use_ssl, timeout = client.args

      [ host, path, port ].each { |x| assert x }
      assert_equal 'aaron', user
      assert_equal 'tenderlove', password

      [ proxy_host, proxy_port, use_ssl, timeout ].each { |x| refute x }
    end

    def test_new2_proxy_host
      client = Fake::Client.new2 'http://example.org/foo', 'example.com'
      host, path, port, proxy_host, proxy_port, user, password, use_ssl, timeout = client.args

      [ host, path, port ].each { |x| assert x }

      assert_equal 'example.com', proxy_host

      [ user, password, proxy_port, use_ssl, timeout ].each { |x| refute x }
    end

    def test_new2_proxy_port
      client = Fake::Client.new2 'http://example.org/foo', 'example.com:1234'
      host, path, port, proxy_host, proxy_port, user, password, use_ssl, timeout = client.args

      [ host, path, port ].each { |x| assert x }

      assert_equal 'example.com', proxy_host
      assert_equal 1234, proxy_port

      [ user, password, use_ssl, timeout ].each { |x| refute x }
    end

    def test_new2_no_path
      client = Fake::Client.new2 'http://example.org'
      host, path, port, *rest = client.args

      assert_equal 'example.org', host
      assert_nil path
      assert port

      rest.each { |x| refute x }
    end

    def test_new2_slash_path
      client = Fake::Client.new2 'http://example.org/'
      host, path, port, *rest = client.args

      assert_equal 'example.org', host
      assert_equal '/', path
      assert port

      rest.each { |x| refute x }
    end

    def test_new2_bad_protocol
      assert_raises(ArgumentError) do
        XMLRPC::Client.new2 'ftp://example.org'
      end
    end

    def test_new2_bad_uri
      assert_raises(ArgumentError) do
        XMLRPC::Client.new2 ':::::'
      end
    end

    def test_new2_path_with_query
      client = Fake::Client.new2 'http://example.org/foo?bar=baz'
      host, path, port, *rest = client.args

      assert_equal 'example.org', host
      assert_equal '/foo?bar=baz', path
      assert port

      rest.each { |x| refute x }
    end

    def test_request
      fh = read 'blog.xml'

      responses = {
        '/foo' => [ Fake::Response.new(fh, [['Content-Type', 'text/xml']]) ]
      }

      client = fake_client(responses).new2 'http://example.org/foo'

      resp = client.call('wp.getUsersBlogs', 'tlo', 'omg')

      expected = [{
        "isAdmin"  => true,
        "url"      => "http://tenderlovemaking.com/",
        "blogid"   => "1",
        "blogName" => "Tender Lovemaking",
        "xmlrpc"   => "http://tenderlovemaking.com/xmlrpc.php"
      }]

      assert_equal expected, resp
    end

    def test_async_request
      fh = read 'blog.xml'

      responses = {
        '/foo' => [ Fake::Response.new(fh, [['Content-Type', 'text/xml']]) ]
      }

      client = fake_client(responses).new2 'http://example.org/foo'

      resp = client.call_async('wp.getUsersBlogs', 'tlo', 'omg')

      expected = [{
        "isAdmin"  => true,
        "url"      => "http://tenderlovemaking.com/",
        "blogid"   => "1",
        "blogName" => "Tender Lovemaking",
        "xmlrpc"   => "http://tenderlovemaking.com/xmlrpc.php"
      }]

      assert_equal expected, resp
    end

    # make a request without content-type header
    def test_bad_content_type
      fh = read 'blog.xml'

      responses = {
        '/foo' => [ Fake::Response.new(fh) ]
      }

      client = fake_client(responses).new2 'http://example.org/foo'

      resp = client.call('wp.getUsersBlogs', 'tlo', 'omg')

      expected = [{
        "isAdmin"  => true,
        "url"      => "http://tenderlovemaking.com/",
        "blogid"   => "1",
        "blogName" => "Tender Lovemaking",
        "xmlrpc"   => "http://tenderlovemaking.com/xmlrpc.php"
      }]

      assert_equal expected, resp
    end

    def test_i8_tag
      fh = read('blog.xml').gsub(/string/, 'i8')

      responses = {
        '/foo' => [ Fake::Response.new(fh) ]
      }

      client = fake_client(responses).new2 'http://example.org/foo'

      resp = client.call('wp.getUsersBlogs', 'tlo', 'omg')

      assert_equal 1, resp.first['blogid']
    end

    private
    def read filename
      File.read File.expand_path(File.join(__FILE__, '..', 'data', filename))
    end

    def fake_client responses
      Class.new(Fake::Client) {
        define_method(:net_http) { |*_| Fake::HTTP.new(responses) }
      }
    end
  end
end
