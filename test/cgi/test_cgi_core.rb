require 'test/unit'
require 'cgi'
require 'stringio'


class CGICoreTest < Test::Unit::TestCase


  def setup
    #@environ = {
    #  'SERVER_PROTOCOL' => 'HTTP/1.1',
    #  'REQUEST_METHOD'  => 'GET',
    #  'SERVER_SOFTWARE' => 'Apache 2.2.0',
    #}
    #ENV.update(@environ)
  end


  def teardown
    @environ.each do |key, val| ENV.delete(key) end
    $stdout = STDOUT
  end

  def test_cgi_parse_illegal_query
    @environ = {
      'REQUEST_METHOD'  => 'GET',
      'QUERY_STRING'    => 'a=111&&b=222&c&d=',
      'HTTP_COOKIE'     => '_session_id=12345; name1=val1&val2;',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    cgi = CGI.new
    assert_equal(["a","b","c","d"],cgi.keys.sort) if RUBY_VERSION>="1.9"
    assert_equal("",cgi["d"])
  end

  def test_cgi_core_params_GET
    @environ = {
      'REQUEST_METHOD'  => 'GET',
      'QUERY_STRING'    => 'id=123&id=456&id=&str=%40h+%3D%7E+%2F%5E%24%2F',
      'HTTP_COOKIE'     => '_session_id=12345; name1=val1&val2;',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    cgi = CGI.new
    ## cgi[]
    assert_equal('123', cgi['id'])
    assert_equal('@h =~ /^$/', cgi['str'])
    ## cgi[][], cgi[].first, cgi[].to_ary (obsolete 1.9)
    if RUBY_VERSION<"1.9"
      $stderr = StringIO.new
      begin
        assert_equal('123', cgi['id'][0])
        assert_equal('456', cgi['id'][1])
        assert_equal('',    cgi['id'][2])
        assert_nil(cgi['id'][3])
        assert_equal('@h =~ /^$/', cgi['str'][0])
        assert_nil(cgi['str'][1])
        assert_equal('123', cgi['id'].first)
        assert_equal('123', cgi['id'].last)       # should be '' ?
        id1, id2, id3 = cgi['id']
        assert_equal(['123', '456', ''], [id1, id2, id3])
        str1, = cgi['str']
        assert_equal('@h =~ /^$/', str1)
        assert_not_same(str1, cgi['str'])    # necessary?
      ensure
        $stderr = STDERR
      end
    end
    ## cgi.params
    assert_equal(['123', '456', ''], cgi.params['id'])
    assert_equal(['@h =~ /^$/'], cgi.params['str'])
    ## cgi.keys
    assert_equal(['id', 'str'], cgi.keys.sort)
    ## cgi.key?, cgi.has_key?, cgi.include?
    assert_equal(true,  cgi.key?('id'))
    assert_equal(true,  cgi.has_key?('id'))
    assert_equal(true,  cgi.include?('id'))
    assert_equal(false, cgi.key?('foo'))
    assert_equal(false, cgi.has_key?('foo'))
    assert_equal(false, cgi.include?('foo'))
    ## invalid parameter name
    assert_equal('', cgi['*notfound*'])    # [ruby-dev:30740]
    assert_equal([], cgi.params['*notfound*'])
  end


  def test_cgi_core_params_POST
    query_str = 'id=123&id=456&id=&str=%40h+%3D%7E+%2F%5E%24%2F'
    @environ = {
      'REQUEST_METHOD'  => 'POST',
      'CONTENT_LENGTH'  => query_str.length.to_s,
      'HTTP_COOKIE'     => '_session_id=12345; name1=val1&val2;',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    $stdin = StringIO.new
    $stdin << query_str
    $stdin.rewind
    cgi = CGI.new
    ## cgi[]
    assert_equal('123', cgi['id'])
    assert_equal('@h =~ /^$/', cgi['str'])
    ## cgi.params
    assert_equal(['123', '456', ''], cgi.params['id'])
    assert_equal(['@h =~ /^$/'], cgi.params['str'])
    ## invalid parameter name
    assert_equal('', cgi['*notfound*'])
    assert_equal([], cgi.params['*notfound*'])
  end

  def test_cgi_core_params_encoding_check
    query_str = 'str=%BE%BE%B9%BE'
    @environ = {
        'REQUEST_METHOD'  => 'POST',
        'CONTENT_LENGTH'  => query_str.length.to_s,
        'SERVER_SOFTWARE' => 'Apache 2.2.0',
        'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    $stdin = StringIO.new
    $stdin << query_str
    $stdin.rewind
    if defined?(::Encoding)
      hash={}
      cgi = CGI.new(:accept_charset=>"UTF-8"){|key,val|hash[key]=val}
      ## cgi[]
      assert_equal("\xBE\xBE\xB9\xBE".force_encoding("UTF-8"), cgi['str'])
      ## cgi.params
      assert_equal(["\xBE\xBE\xB9\xBE".force_encoding("UTF-8")], cgi.params['str'])
      ## accept-charset error
      assert_equal({"str"=>"\xBE\xBE\xB9\xBE".force_encoding("UTF-8")},hash)

      $stdin.rewind
      assert_raise(CGI::InvalidEncoding) do
        cgi = CGI.new(:accept_charset=>"UTF-8")
      end

      $stdin.rewind
      cgi = CGI.new(:accept_charset=>"EUC-JP")
      ## cgi[]
      assert_equal("\xBE\xBE\xB9\xBE".force_encoding("EUC-JP"), cgi['str'])
      ## cgi.params
      assert_equal(["\xBE\xBE\xB9\xBE".force_encoding("EUC-JP")], cgi.params['str'])
    else
      assert(true)
    end
  end


  def test_cgi_core_cookie
    @environ = {
      'REQUEST_METHOD'  => 'GET',
      'QUERY_STRING'    => 'id=123&id=456&id=&str=%40h+%3D%7E+%2F%5E%24%2F',
      'HTTP_COOKIE'     => '_session_id=12345; name1=val1&val2;',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    cgi = CGI.new
    assert_not_equal(nil,cgi.cookies)
    [ ['_session_id', ['12345'],        ],
      ['name1',       ['val1', 'val2'], ],
    ].each do |key, expected|
      cookie = cgi.cookies[key]
      assert_kind_of(CGI::Cookie, cookie)
      assert_equal(expected, cookie.value)
      assert_equal(false, cookie.secure)
      assert_nil(cookie.expires)
      assert_nil(cookie.domain)
      assert_equal('', cookie.path)
    end
  end


  def test_cgi_core_maxcontentlength
    @environ = {
      'REQUEST_METHOD'  => 'POST',
      'CONTENT_LENGTH'  => (64 * 1024 * 1024).to_s
    }
    ENV.update(@environ)
    ex = assert_raise(StandardError) do
      cgi = CGI.new
    end
    assert_equal("too large post data.", ex.message)
  end if CGI.const_defined?(:MAX_CONTENT_LENGTH)


  def test_cgi_core_out
    @environ = {
      'REQUEST_METHOD'  => 'GET',
      'QUERY_STRING'    => 'id=123&id=456&id=&str=%40h+%3D%7E+%2F%5E%24%2F',
      'HTTP_COOKIE'     => '_session_id=12345; name1=val1&val2;',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    cgi = CGI.new
    ## jis, euc, sjis string
    jis_str = "\e$B8+$m!\"?M$,%4%_$N$h$&$@\e(B"
    euc_str = "\270\253\244\355\241\242\277\315\244\254\245\264\245\337\244\316\244\350\244\246\244\300"
    sjis_str = "\214\251\202\353\201A\220l\202\252\203S\203~\202\314\202\346\202\244\202\276"
    if RUBY_VERSION<"1.9"
      ## iso-2022-jp
      options = { 'charset'=>'iso-2022-jp' }
      $stdout = StringIO.new
      cgi.out(options) { euc_str }
      assert_equal('ja', options['language'])
      actual = $stdout.string
      expected = "Content-Type: text/html; charset=iso-2022-jp\r\n" +
               "Content-Length: 28\r\n" +
               "Content-Language: ja\r\n" +
               "\r\n" +
               jis_str
      assert_equal(expected,actual)
      ## euc-jp
      options = { 'charset'=>'EUC-JP' }
      $stdout = StringIO.new
      cgi.out(options) { euc_str }
      assert_equal('ja', options['language'])
      actual = $stdout.string
      expected = "Content-Type: text/html; charset=EUC-JP\r\n" +
               "Content-Length: 22\r\n" +
               "Content-Language: ja\r\n" +
               "\r\n" +
               euc_str
      assert_equal(expected, actual)
      ## shift_jis
      options = { 'charset'=>'Shift_JIS' }
      $stdout = StringIO.new
      cgi.out(options) { euc_str }
      assert_equal('ja', options['language'])
      actual = $stdout.string
      expected = "Content-Type: text/html; charset=Shift_JIS\r\n" +
               "Content-Length: 22\r\n" +
               "Content-Language: ja\r\n" +
               "\r\n" +
               sjis_str
      assert_equal(expected, actual)
    end
    ## utf8 (not converted)
    options = { 'charset'=>'utf8' }
    $stdout = StringIO.new
    cgi.out(options) { euc_str }
    assert_nil(options['language'])
    actual = $stdout.string
    expected = "Content-Type: text/html; charset=utf8\r\n" +
               "Content-Length: 22\r\n" +
               "\r\n" +
               euc_str
    if defined?(::Encoding)
      actual.force_encoding("ASCII-8BIT")
      expected.force_encoding("ASCII-8BIT")
    end
    assert_equal(expected, actual)
    ## language is keeped
    options = { 'charset'=>'Shift_JIS', 'language'=>'en' }
    $stdout = StringIO.new
    cgi.out(options) { euc_str }
    assert_equal('en', options['language'])
    ## HEAD method
    ENV['REQUEST_METHOD'] = 'HEAD'
    options = { 'charset'=>'utf8' }
    $stdout = StringIO.new
    cgi.out(options) { euc_str }
    actual = $stdout.string
    expected = "Content-Type: text/html; charset=utf8\r\n" +
               "Content-Length: 22\r\n" +
               "\r\n"
    assert_equal(expected, actual)
  end


  def test_cgi_core_print
    @environ = {
      'REQUEST_METHOD'  => 'GET',
    }
    ENV.update(@environ)
    cgi = CGI.new
    $stdout = StringIO.new
    str = "foobar"
    cgi.print(str)
    expected = str
    actual = $stdout.string
    assert_equal(expected, actual)
  end


  def test_cgi_core_environs
    @environ = {
      'REQUEST_METHOD' => 'GET',
    }
    ENV.update(@environ)
    cgi = CGI.new
    ##
    list1 = %w[ AUTH_TYPE CONTENT_TYPE GATEWAY_INTERFACE PATH_INFO
        PATH_TRANSLATED QUERY_STRING REMOTE_ADDR REMOTE_HOST
        REMOTE_IDENT REMOTE_USER REQUEST_METHOD SCRIPT_NAME
        SERVER_NAME SERVER_PROTOCOL SERVER_SOFTWARE
        HTTP_ACCEPT HTTP_ACCEPT_CHARSET HTTP_ACCEPT_ENCODING
        HTTP_ACCEPT_LANGUAGE HTTP_CACHE_CONTROL HTTP_FROM HTTP_HOST
        HTTP_NEGOTIATE HTTP_PRAGMA HTTP_REFERER HTTP_USER_AGENT
    ]
    list2 = %w[ CONTENT_LENGTH SERVER_PORT ]
    ## string expected
    list1.each do |name|
      @environ[name] = "**#{name}**"
    end
    ENV.update(@environ)
    list1.each do |name|
      method = name.sub(/\AHTTP_/, '').downcase
      actual = cgi.__send__ method
      expected = "**#{name}**"
      assert_equal(expected, actual)
    end
    ## integer expected
    ENV['CONTENT_LENGTH'] = '123'
    ENV['SERVER_PORT'] = '8080'
    assert_equal(123, cgi.content_length)
    assert_equal(8080, cgi.server_port)
    ## raw cookie
    ENV['HTTP_COOKIE'] = 'name1=val1'
    ENV['HTTP_COOKIE2'] = 'name2=val2'
    assert_equal('name1=val1', cgi.raw_cookie)
    assert_equal('name2=val2', cgi.raw_cookie2)
  end


  def test_cgi_core_htmltype
    @environ = {
      'REQUEST_METHOD' => 'GET',
    }
    ENV.update(@environ)
    ## no htmltype
    cgi = CGI.new
    assert_raise(NoMethodError) do cgi.doctype end
    ## html3
    cgi = CGI.new('html3')
    expected = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">'
    assert_equal(expected, cgi.doctype)
    ## html4
    cgi = CGI.new('html4')
    expected = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">'
    assert_equal(expected, cgi.doctype)
    ## html4 transitional
    cgi = CGI.new('html4Tr')
    expected = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">'
    assert_equal(expected, cgi.doctype)
    ## html4 frameset
    cgi = CGI.new('html4Fr')
    expected = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">'
    assert_equal(expected, cgi.doctype)
  end


  instance_methods.each do |method|
    private method if method =~ /^test_(.*)/ && $1 != ENV['TEST']
  end if ENV['TEST']

end
