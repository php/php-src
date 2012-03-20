require 'test/unit'
require 'cgi'
require 'time'


class CGIHeaderTest < Test::Unit::TestCase


  def setup
    @environ = {
      'SERVER_PROTOCOL' => 'HTTP/1.1',
      'REQUEST_METHOD'  => 'GET',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
    }
    ENV.update(@environ)
  end


  def teardown
    @environ.each do |key, val| ENV.delete(key) end
  end


  def test_cgi_header_simple
    cgi = CGI.new
    ## default content type
    expected = "Content-Type: text/html\r\n\r\n"
    actual = cgi.header
    assert_equal(expected, actual)
    ## content type specified as string
    expected = "Content-Type: text/xhtml; charset=utf8\r\n\r\n"
    actual = cgi.header('text/xhtml; charset=utf8')
    assert_equal(expected, actual)
    ## content type specified as hash
    expected = "Content-Type: image/png\r\n\r\n"
    actual = cgi.header('type'=>'image/png')
    assert_equal(expected, actual)
    ## charset specified
    expected = "Content-Type: text/html; charset=utf8\r\n\r\n"
    actual = cgi.header('charset'=>'utf8')
    assert_equal(expected, actual)
  end


  def test_cgi_header_complex
    cgi = CGI.new
    options = {
      'type'       => 'text/xhtml',
      'charset'    => 'utf8',
      'status'     => 'REDIRECT',
      'server'     => 'webrick',
      'connection' => 'close',
      'length'     => 123,
      'language'   => 'ja',
      'expires'    => Time.gm(2000, 1, 23, 12, 34, 56),
      'location'   => 'http://www.ruby-lang.org/',
    }
    expected =  "Status: 302 Found\r\n"
    expected << "Server: webrick\r\n"
    expected << "Connection: close\r\n"
    expected << "Content-Type: text/xhtml; charset=utf8\r\n"
    expected << "Content-Length: 123\r\n"
    expected << "Content-Language: ja\r\n"
    expected << "Expires: Sun, 23 Jan 2000 12:34:56 GMT\r\n"
    expected << "location: http://www.ruby-lang.org/\r\n"
    expected << "\r\n"
    actual = cgi.header(options)
    assert_equal(expected, actual)
  end


  def test_cgi_header_argerr
    cgi = CGI.new
    #expected = NoMethodError  # must be ArgumentError
    if RUBY_VERSION>="1.9.0"
      expected = ArgumentError   # for CGIAlt
    else
      expected = NoMethodError   # for Ruby1.8
    end
    ex = assert_raise(expected) do
      cgi.header(nil)
    end
  end


  def test_cgi_header_cookie
    cgi = CGI.new
    cookie1 = CGI::Cookie.new('name1', 'abc', '123')
    cookie2 = CGI::Cookie.new('name'=>'name2', 'value'=>'value2', 'secure'=>true)
    ctype = "Content-Type: text/html\r\n"
    sep   = "\r\n"
    c1    = "Set-Cookie: name1=abc&123; path=\r\n"
    c2    = "Set-Cookie: name2=value2; path=; secure\r\n"
    ## CGI::Cookie object
    actual = cgi.header('cookie'=>cookie1)
    expected = ctype + c1 + sep
    assert_equal(expected, actual)
    ## String
    actual = cgi.header('cookie'=>cookie2.to_s)
    expected = ctype + c2 + sep
    assert_equal(expected, actual)
    ## Array
    actual = cgi.header('cookie'=>[cookie1, cookie2])
    expected = ctype + c1 + c2 + sep
    assert_equal(expected, actual)
    ## Hash
    actual = cgi.header('cookie'=>{'name1'=>cookie1, 'name2'=>cookie2})
    expected = ctype + c1 + c2 + sep
    assert_equal(expected, actual)
  end


  def test_cgi_header_output_cookies
    cgi = CGI.new
    ## output cookies
    cookies = [ CGI::Cookie.new('name1', 'abc', '123'),
                CGI::Cookie.new('name'=>'name2', 'value'=>'value2', 'secure'=>true),
              ]
    cgi.instance_variable_set('@output_cookies', cookies)
    expected =  "Content-Type: text/html; charset=utf8\r\n"
    expected << "Set-Cookie: name1=abc&123; path=\r\n"
    expected << "Set-Cookie: name2=value2; path=; secure\r\n"
    expected << "\r\n"
    ## header when string
    actual = cgi.header('text/html; charset=utf8')
    assert_equal(expected, actual)
    ## _header_for_string
    actual = cgi.header('type'=>'text/html', 'charset'=>'utf8')
    assert_equal(expected, actual)
  end


  def test_cgi_header_nph
    time_start = Time.now.to_i
    cgi = CGI.new
    ## 'nph' is true
    ENV['SERVER_SOFTWARE'] = 'Apache 2.2.0'
    actual1 = cgi.header('nph'=>true)
    ## when old IIS, NPH-mode is forced
    ENV['SERVER_SOFTWARE'] = 'IIS/4.0'
    actual2 = cgi.header
    actual3 = cgi.header('status'=>'REDIRECT', 'location'=>'http://www.example.com/')
    ## newer IIS doesn't require NPH-mode   ## [ruby-dev:30537]
    ENV['SERVER_SOFTWARE'] = 'IIS/5.0'
    actual4 = cgi.header
    actual5 = cgi.header('status'=>'REDIRECT', 'location'=>'http://www.example.com/')
    time_end = Time.now.to_i
    date = /^Date: ([A-Z][a-z]{2}, \d{2} [A-Z][a-z]{2} \d{4} \d\d:\d\d:\d\d GMT)\r\n/
    [actual1, actual2, actual3].each do |actual|
      assert_match(date, actual)
      assert_includes(time_start..time_end, date =~ actual && Time.parse($1).to_i)
      actual.sub!(date, "Date: DATE_IS_REMOVED\r\n")
    end
    ## assertion
    expected =  "HTTP/1.1 200 OK\r\n"
    expected << "Date: DATE_IS_REMOVED\r\n"
    expected << "Server: Apache 2.2.0\r\n"
    expected << "Connection: close\r\n"
    expected << "Content-Type: text/html\r\n"
    expected << "\r\n"
    assert_equal(expected, actual1)
    expected.sub!(/^Server: .*?\r\n/, "Server: IIS/4.0\r\n")
    assert_equal(expected, actual2)
    expected.sub!(/^HTTP\/1.1 200 OK\r\n/, "HTTP/1.1 302 Found\r\n")
    expected.sub!(/\r\n\r\n/, "\r\nlocation: http://www.example.com/\r\n\r\n")
    assert_equal(expected, actual3)
    expected =  "Content-Type: text/html\r\n"
    expected << "\r\n"
    assert_equal(expected, actual4)
    expected =  "Status: 302 Found\r\n"
    expected << "Content-Type: text/html\r\n"
    expected << "location: http://www.example.com/\r\n"
    expected << "\r\n"
    assert_equal(expected, actual5)
  ensure
    ENV.delete('SERVER_SOFTWARE')
  end



  instance_methods.each do |method|
    private method if method =~ /^test_(.*)/ && $1 != ENV['TEST']
  end if ENV['TEST']

end
