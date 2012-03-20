require 'test/unit'
require 'uri'

class URI::TestParser < Test::Unit::TestCase
  def uri_to_ary(uri)
    uri.class.component.collect {|c| uri.send(c)}
  end

  def test_compare
    url = 'http://a/b/c/d;p?q'
    u0 = URI.parse(url)
    u1 = URI.parse(url)
    p = URI::Parser.new
    u2 = p.parse(url)
    u3 = p.parse(url)

    assert(u0 == u1)
    assert(u0.eql?(u1))
    assert(!u0.equal?(u1))

    assert(u1 == u2)
    assert(!u1.eql?(u2))
    assert(!u1.equal?(u2))

    assert(u2 == u3)
    assert(u2.eql?(u3))
    assert(!u2.equal?(u3))
  end

  def test_parse
    escaped = URI::REGEXP::PATTERN::ESCAPED
    hex = URI::REGEXP::PATTERN::HEX
    p1 = URI::Parser.new(:ESCAPED => "(?:#{escaped}|%u[#{hex}]{4})")
    u1 = p1.parse('http://a/b/%uABCD')
    assert_equal(['http', nil, 'a', URI::HTTP.default_port, '/b/%uABCD', nil, nil],
		 uri_to_ary(u1))
    u1.path = '/%uDCBA'
    assert_equal(['http', nil, 'a', URI::HTTP.default_port, '/%uDCBA', nil, nil],
		 uri_to_ary(u1))
  end
end
