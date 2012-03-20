require 'test/unit'
require 'cgi'


class CGIModrubyTest < Test::Unit::TestCase


  def setup
    @environ = {
      'SERVER_PROTOCOL' => 'HTTP/1.1',
      'REQUEST_METHOD'  => 'GET',
      #'QUERY_STRING'    => 'a=foo&b=bar',
    }
    ENV.update(@environ)
    CGI.class_eval { const_set(:MOD_RUBY, true) }
    Apache._reset()
    #@cgi = CGI.new
    #@req = Apache.request
  end


  def teardown
    @environ.each do |key, val| ENV.delete(key) end
    CGI.class_eval { remove_const(:MOD_RUBY) }
  end


  def test_cgi_modruby_simple
    req = Apache.request
    cgi = CGI.new
    assert(req._setup_cgi_env_invoked?)
    assert(! req._send_http_header_invoked?)
    actual = cgi.header
    assert_equal('', actual)
    assert_equal('text/html', req.content_type)
    assert(req._send_http_header_invoked?)
  end


  def test_cgi_modruby_complex
    req = Apache.request
    cgi = CGI.new
    options = {
      'status'   => 'FORBIDDEN',
      'location' => 'http://www.example.com/',
      'type'     => 'image/gif',
      'content-encoding' => 'deflate',
      'cookie'   => [ CGI::Cookie.new('name1', 'abc', '123'),
                      CGI::Cookie.new('name'=>'name2', 'value'=>'value2', 'secure'=>true),
                    ],
    }
    assert(req._setup_cgi_env_invoked?)
    assert(! req._send_http_header_invoked?)
    actual = cgi.header(options)
    assert_equal('', actual)
    assert_equal('image/gif', req.content_type)
    assert_equal('403 Forbidden', req.status_line)
    assert_equal(403, req.status)
    assert_equal('deflate', req.content_encoding)
    assert_equal('http://www.example.com/', req.headers_out['location'])
    assert_equal(["name1=abc&123; path=", "name2=value2; path=; secure"],
                 req.headers_out['Set-Cookie'])
    assert(req._send_http_header_invoked?)
  end


  def test_cgi_modruby_location
    req = Apache.request
    cgi = CGI.new
    options = {
      'status'   => '200 OK',
      'location' => 'http://www.example.com/',
    }
    actual = cgi.header(options)
    assert_equal('200 OK', req.status_line)  # should be '302 Found' ?
    assert_equal(302, req.status)
    assert_equal('http://www.example.com/', req.headers_out['location'])
  end


  def test_cgi_modruby_requestparams
    req = Apache.request
    req.args = 'a=foo&b=bar'
    cgi = CGI.new
    assert_equal('foo', cgi['a'])
    assert_equal('bar', cgi['b'])
  end


  instance_methods.each do |method|
    private method if method =~ /^test_(.*)/ && $1 != ENV['TEST']
  end if ENV['TEST']

end



## dummy class for mod_ruby
class Apache  #:nodoc:

  def self._reset
    @request = Request.new
  end

  def self.request
    return @request
  end

  class Request

    def initialize
      hash = {}
      def hash.add(name, value)
        (self[name] ||= []) << value
      end
      @headers_out  = hash
      @status_line  = nil
      @status       = nil
      @content_type = nil
      @content_encoding = nil
    end
    attr_accessor :headers_out, :status_line, :status, :content_type, :content_encoding

    attr_accessor :args
    #def args
    #  return ENV['QUERY_STRING']
    #end

    def send_http_header
      @http_header = '*invoked*'
    end
    def _send_http_header_invoked?
      @http_header ? true : false
    end

    def setup_cgi_env
      @cgi_env = '*invoked*'
    end
    def _setup_cgi_env_invoked?
      @cgi_env ? true : false
    end

  end

end
