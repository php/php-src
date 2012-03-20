require 'test/unit'
require 'cgi'
require 'stringio'


class CGICookieTest < Test::Unit::TestCase


  def setup
    ENV['REQUEST_METHOD'] = 'GET'
    @str1="\xE3\x82\x86\xE3\x82\x93\xE3\x82\x86\xE3\x82\x93"
    @str1.force_encoding("UTF-8") if defined?(::Encoding)
  end

  def teardown
    %W[REQUEST_METHOD SCRIPT_NAME].each do |name|
      ENV.delete(name)
    end
  end


  def test_cgi_cookie_new_simple
    cookie = CGI::Cookie.new('name1', 'val1', '&<>"', @str1)
    assert_equal('name1', cookie.name)
    assert_equal(['val1', '&<>"', @str1], cookie.value)
    assert_nil(cookie.domain)
    assert_nil(cookie.expires)
    assert_equal('', cookie.path)
    assert_equal(false, cookie.secure)
    assert_equal("name1=val1&%26%3C%3E%22&%E3%82%86%E3%82%93%E3%82%86%E3%82%93; path=", cookie.to_s)
  end


  def test_cgi_cookie_new_complex
    t = Time.gm(2030, 12, 31, 23, 59, 59)
    value = ['val1', '&<>"', "\xA5\xE0\xA5\xB9\xA5\xAB"]
    value[2].force_encoding("EUC-JP") if defined?(::Encoding)
    cookie = CGI::Cookie.new('name'=>'name1',
                             'value'=>value,
                             'path'=>'/cgi-bin/myapp/',
                             'domain'=>'www.example.com',
                             'expires'=>t,
                             'secure'=>true
                             )
    assert_equal('name1', cookie.name)
    assert_equal(value, cookie.value)
    assert_equal('www.example.com', cookie.domain)
    assert_equal(t, cookie.expires)
    assert_equal('/cgi-bin/myapp/', cookie.path)
    assert_equal(true, cookie.secure)
    assert_equal('name1=val1&%26%3C%3E%22&%A5%E0%A5%B9%A5%AB; domain=www.example.com; path=/cgi-bin/myapp/; expires=Tue, 31 Dec 2030 23:59:59 GMT; secure', cookie.to_s)
  end


  def test_cgi_cookie_scriptname
    cookie = CGI::Cookie.new('name1', 'value1')
    assert_equal('', cookie.path)
    cookie = CGI::Cookie.new('name'=>'name1', 'value'=>'value1')
    assert_equal('', cookie.path)
    ## when ENV['SCRIPT_NAME'] is set, cookie.path is set automatically
    ENV['SCRIPT_NAME'] = '/cgi-bin/app/example.cgi'
    cookie = CGI::Cookie.new('name1', 'value1')
    assert_equal('/cgi-bin/app/', cookie.path)
    cookie = CGI::Cookie.new('name'=>'name1', 'value'=>'value1')
    assert_equal('/cgi-bin/app/', cookie.path)
  end


  def test_cgi_cookie_parse
    ## ';' separator
    cookie_str = 'name1=val1&val2; name2=val2&%26%3C%3E%22&%E3%82%86%E3%82%93%E3%82%86%E3%82%93;_session_id=12345'
    cookies = CGI::Cookie.parse(cookie_str)
    list = [
      ['name1', ['val1', 'val2']],
      ['name2', ['val2', '&<>"',@str1]],
      ['_session_id', ['12345']],
    ]
    list.each do |name, value|
      cookie = cookies[name]
      assert_equal(name, cookie.name)
      assert_equal(value, cookie.value)
    end
    ## ',' separator
    cookie_str = 'name1=val1&val2, name2=val2&%26%3C%3E%22&%E3%82%86%E3%82%93%E3%82%86%E3%82%93,_session_id=12345'
    cookies = CGI::Cookie.parse(cookie_str)
    list.each do |name, value|
      cookie = cookies[name]
      assert_equal(name, cookie.name)
      assert_equal(value, cookie.value)
    end
  end


  def test_cgi_cookie_arrayinterface
    cookie = CGI::Cookie.new('name1', 'a', 'b', 'c')
    assert_equal('a', cookie[0])
    assert_equal('c', cookie[2])
    assert_nil(cookie[3])
    assert_equal('a', cookie.first)
    assert_equal('c', cookie.last)
    assert_equal(['A', 'B', 'C'], cookie.collect{|e| e.upcase})
  end



  instance_methods.each do |method|
    private method if method =~ /^test_(.*)/ && $1 != ENV['TEST']
  end if ENV['TEST']

end
