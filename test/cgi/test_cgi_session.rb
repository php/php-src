require 'test/unit'
require 'cgi'
require 'cgi/session'
require 'cgi/session/pstore'
require 'stringio'
require 'tmpdir'

class CGISessionTest < Test::Unit::TestCase
  def setup
    @session_dir = File.join(File.dirname(__FILE__), 'session_dir')
    FileUtils.mkdir_p @session_dir
  end

  def teardown
    @environ.each do |key, val| ENV.delete(key) end
    $stdout = STDOUT
    FileUtils.rm_rf(@session_dir)
  end

  def test_cgi_session_filestore
    @environ = {
      'REQUEST_METHOD'  => 'GET',
  #    'QUERY_STRING'    => 'id=123&id=456&id=&str=%40h+%3D%7E+%2F%5E%24%2F',
  #    'HTTP_COOKIE'     => '_session_id=12345; name1=val1&val2;',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    value1="value1"
    value2="\x8F\xBC\x8D]"
    value2.force_encoding("SJIS") if defined?(::Encoding)
    ENV.update(@environ)
    cgi = CGI.new
    session = CGI::Session.new(cgi,"tmpdir"=>@session_dir)
    session["key1"]=value1
    session["key2"]=value2
    assert_equal(value1,session["key1"])
    assert_equal(value2,session["key2"])
    session.close
    $stdout = StringIO.new
    cgi.out{""}

    @environ = {
      'REQUEST_METHOD'  => 'GET',
      # 'HTTP_COOKIE'     => "_session_id=#{session_id}",
      'QUERY_STRING'    => "_session_id=#{session.session_id}",
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    cgi = CGI.new
    session = CGI::Session.new(cgi,"tmpdir"=>@session_dir)
    $stdout = StringIO.new
    assert_equal(value1,session["key1"])
    assert_equal(value2,session["key2"])
    session.close

  end
  def test_cgi_session_pstore
    @environ = {
      'REQUEST_METHOD'  => 'GET',
  #    'QUERY_STRING'    => 'id=123&id=456&id=&str=%40h+%3D%7E+%2F%5E%24%2F',
  #    'HTTP_COOKIE'     => '_session_id=12345; name1=val1&val2;',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    value1="value1"
    value2="\x8F\xBC\x8D]"
    value2.force_encoding("SJIS") if defined?(::Encoding)
    ENV.update(@environ)
    cgi = CGI.new
    session = CGI::Session.new(cgi,"tmpdir"=>@session_dir,"database_manager"=>CGI::Session::PStore)
    session["key1"]=value1
    session["key2"]=value2
    assert_equal(value1,session["key1"])
    assert_equal(value2,session["key2"])
    session.close
    $stdout = StringIO.new
    cgi.out{""}

    @environ = {
      'REQUEST_METHOD'  => 'GET',
      # 'HTTP_COOKIE'     => "_session_id=#{session_id}",
      'QUERY_STRING'    => "_session_id=#{session.session_id}",
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    cgi = CGI.new
    session = CGI::Session.new(cgi,"tmpdir"=>@session_dir,"database_manager"=>CGI::Session::PStore)
    $stdout = StringIO.new
    assert_equal(value1,session["key1"])
    assert_equal(value2,session["key2"])
    session.close
  end
  def test_cgi_session_specify_session_id
    @environ = {
      'REQUEST_METHOD'  => 'GET',
  #    'QUERY_STRING'    => 'id=123&id=456&id=&str=%40h+%3D%7E+%2F%5E%24%2F',
  #    'HTTP_COOKIE'     => '_session_id=12345; name1=val1&val2;',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    value1="value1"
    value2="\x8F\xBC\x8D]"
    value2.force_encoding("SJIS") if defined?(::Encoding)
    ENV.update(@environ)
    cgi = CGI.new
    session = CGI::Session.new(cgi,"tmpdir"=>@session_dir,"session_id"=>"foo")
    session["key1"]=value1
    session["key2"]=value2
    assert_equal(value1,session["key1"])
    assert_equal(value2,session["key2"])
    assert_equal("foo",session.session_id)
    session_id=session.session_id
    session.close
    $stdout = StringIO.new
    cgi.out{""}

    @environ = {
      'REQUEST_METHOD'  => 'GET',
      # 'HTTP_COOKIE'     => "_session_id=#{session_id}",
      'QUERY_STRING'    => "_session_id=#{session.session_id}",
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    cgi = CGI.new
    session = CGI::Session.new(cgi,"tmpdir"=>@session_dir)
    $stdout = StringIO.new
    assert_equal(value1,session["key1"])
    assert_equal(value2,session["key2"])
    assert_equal("foo",session.session_id)
    session.close
  end
  def test_cgi_session_specify_session_key
    @environ = {
      'REQUEST_METHOD'  => 'GET',
  #    'QUERY_STRING'    => 'id=123&id=456&id=&str=%40h+%3D%7E+%2F%5E%24%2F',
  #    'HTTP_COOKIE'     => '_session_id=12345; name1=val1&val2;',
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    value1="value1"
    value2="\x8F\xBC\x8D]"
    value2.force_encoding("SJIS") if defined?(::Encoding)
    ENV.update(@environ)
    cgi = CGI.new
    session = CGI::Session.new(cgi,"tmpdir"=>@session_dir,"session_key"=>"bar")
    session["key1"]=value1
    session["key2"]=value2
    assert_equal(value1,session["key1"])
    assert_equal(value2,session["key2"])
    session_id=session.session_id
    session.close
    $stdout = StringIO.new
    cgi.out{""}

    @environ = {
      'REQUEST_METHOD'  => 'GET',
      'HTTP_COOKIE'     => "bar=#{session_id}",
      # 'QUERY_STRING'    => "bar=#{session.session_id}",
      'SERVER_SOFTWARE' => 'Apache 2.2.0',
      'SERVER_PROTOCOL' => 'HTTP/1.1',
    }
    ENV.update(@environ)
    cgi = CGI.new
    session = CGI::Session.new(cgi,"tmpdir"=>@session_dir,"session_key"=>"bar")
    $stdout = StringIO.new
    assert_equal(value1,session["key1"])
    assert_equal(value2,session["key2"])
    session.close
  end
end
