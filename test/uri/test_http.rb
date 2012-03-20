require 'test/unit'
require 'uri/http'

module URI


class TestHTTP < Test::Unit::TestCase
  def setup
  end

  def teardown
  end

  def uri_to_ary(uri)
    uri.class.component.collect {|c| uri.send(c)}
  end

  def test_parse
    u = URI.parse('http://a')
    assert_kind_of(URI::HTTP, u)
    assert_equal(['http',
		   nil, 'a', URI::HTTP.default_port,
		   '', nil, nil], uri_to_ary(u))
  end

  def test_normalize
    host = 'aBcD'
    u1 = URI.parse('http://' + host          + '/eFg?HiJ')
    u2 = URI.parse('http://' + host.downcase + '/eFg?HiJ')
    assert(u1.normalize.host == 'abcd')
    assert(u1.normalize.path == u1.path)
    assert(u1.normalize == u2.normalize)
    assert(!u1.normalize.host.equal?(u1.host))
    assert( u2.normalize.host.equal?(u2.host))

    assert_equal('http://abc/', URI.parse('http://abc').normalize.to_s)
  end

  def test_equal
    assert(URI.parse('http://abc') == URI.parse('http://ABC'))
    assert(URI.parse('http://abc/def') == URI.parse('http://ABC/def'))
    assert(URI.parse('http://abc/def') != URI.parse('http://ABC/DEF'))
  end

  def test_request_uri
    assert_equal('/',         URI.parse('http://a.b.c/').request_uri)
    assert_equal('/?abc=def', URI.parse('http://a.b.c/?abc=def').request_uri)
    assert_equal('/',         URI.parse('http://a.b.c').request_uri)
    assert_equal('/?abc=def', URI.parse('http://a.b.c?abc=def').request_uri)
    assert_equal(nil,         URI.parse('http:foo').request_uri)
  end

  def test_select
    assert_equal(['http', 'a.b.c', 80], URI.parse('http://a.b.c/').select(:scheme, :host, :port))
    u = URI.parse('http://a.b.c/')
    assert_equal(uri_to_ary(u), u.select(*u.component))
    assert_raise(ArgumentError) do
      u.select(:scheme, :host, :not_exist, :port)
    end
  end
end


end
