require 'net/http'
require 'test/unit'

class HTTPHeaderTest < Test::Unit::TestCase

  class C
    include Net::HTTPHeader
    def initialize
      initialize_http_header({})
    end
    attr_accessor :body
  end

  def setup
    @c = C.new
  end

  def test_size
    assert_equal 0, @c.size
    @c['a'] = 'a'
    assert_equal 1, @c.size
    @c['b'] = 'b'
    assert_equal 2, @c.size
    @c['b'] = 'b'
    assert_equal 2, @c.size
    @c['c'] = 'c'
    assert_equal 3, @c.size
  end

  def test_ASET
    @c['My-Header'] = 'test string'
    @c['my-Header'] = 'test string'
    @c['My-header'] = 'test string'
    @c['my-header'] = 'test string'
    @c['MY-HEADER'] = 'test string'
    assert_equal 1, @c.size

    @c['AaA'] = 'aaa'
    @c['aaA'] = 'aaa'
    @c['AAa'] = 'aaa'
    assert_equal 2, @c.length
  end

  def test_AREF
    @c['My-Header'] = 'test string'
    assert_equal 'test string', @c['my-header']
    assert_equal 'test string', @c['MY-header']
    assert_equal 'test string', @c['my-HEADER']

    @c['Next-Header'] = 'next string'
    assert_equal 'next string', @c['next-header']
  end

  def test_add_field
    @c.add_field 'My-Header', 'a'
    assert_equal 'a', @c['My-Header']
    assert_equal ['a'], @c.get_fields('My-Header')
    @c.add_field 'My-Header', 'b'
    assert_equal 'a, b', @c['My-Header']
    assert_equal ['a', 'b'], @c.get_fields('My-Header')
    @c.add_field 'My-Header', 'c'
    assert_equal 'a, b, c', @c['My-Header']
    assert_equal ['a', 'b', 'c'], @c.get_fields('My-Header')
    @c.add_field 'My-Header', 'd, d'
    assert_equal 'a, b, c, d, d', @c['My-Header']
    assert_equal ['a', 'b', 'c', 'd, d'], @c.get_fields('My-Header')
  end

  def test_get_fields
    @c['My-Header'] = 'test string'
    assert_equal ['test string'], @c.get_fields('my-header')
    assert_equal ['test string'], @c.get_fields('My-header')
    assert_equal ['test string'], @c.get_fields('my-Header')

    assert_nil @c.get_fields('not-found')
    assert_nil @c.get_fields('Not-Found')

    @c.get_fields('my-header').push 'junk'
    assert_equal ['test string'], @c.get_fields('my-header')
    @c.get_fields('my-header').clear
    assert_equal ['test string'], @c.get_fields('my-header')
  end

  def test_delete
    @c['My-Header'] = 'test'
    assert_equal 'test', @c['My-Header']
    assert_nil @c['not-found']
    @c.delete 'My-Header'
    assert_nil @c['My-Header']
    assert_nil @c['not-found']
    @c.delete 'My-Header'
    @c.delete 'My-Header'
    assert_nil @c['My-Header']
    assert_nil @c['not-found']
  end

  def test_each
    @c['My-Header'] = 'test'
    @c.each do |k, v|
      assert_equal 'my-header', k
      assert_equal 'test', v
    end
    @c.each do |k, v|
      assert_equal 'my-header', k
      assert_equal 'test', v
    end
  end

  def test_each_key
    @c['My-Header'] = 'test'
    @c.each_key do |k|
      assert_equal 'my-header', k
    end
    @c.each_key do |k|
      assert_equal 'my-header', k
    end
  end

  def test_each_value
    @c['My-Header'] = 'test'
    @c.each_value do |v|
      assert_equal 'test', v
    end
    @c.each_value do |v|
      assert_equal 'test', v
    end
  end

  def test_canonical_each
    @c['my-header'] = ['a', 'b']
    @c.canonical_each do |k,v|
      assert_equal 'My-Header', k
      assert_equal 'a, b', v
    end
  end

  def test_each_capitalized
    @c['my-header'] = ['a', 'b']
    @c.each_capitalized do |k,v|
      assert_equal 'My-Header', k
      assert_equal 'a, b', v
    end
  end

  def test_key?
    @c['My-Header'] = 'test'
    assert_equal true, @c.key?('My-Header')
    assert_equal true, @c.key?('my-header')
    assert_equal false, @c.key?('Not-Found')
    assert_equal false, @c.key?('not-found')
    assert_equal false, @c.key?('')
    assert_equal false, @c.key?('x' * 1024)
  end

  def test_to_hash
  end

  def test_range
    try_range(1..5,     '1-5')
    try_range(234..567, '234-567')
    try_range(-5..-1,   '-5')
    try_range(1..-1,    '1-')
  end

  def try_range(r, s)
    @c['range'] = "bytes=#{s}"
    assert_equal r, Array(@c.range)[0]
  end

  def test_range=
    @c.range = 0..499
    assert_equal 'bytes=0-499', @c['range']
    @c.range = 0...500
    assert_equal 'bytes=0-499', @c['range']
    @c.range = 300
    assert_equal 'bytes=0-299', @c['range']
    @c.range = -400
    assert_equal 'bytes=-400', @c['range']
    @c.set_range 0, 500
    assert_equal 'bytes=0-499', @c['range']
  end

  def test_content_range
  end

  def test_range_length
    @c['Content-Range'] = "bytes 0-499/1000"
    assert_equal 500, @c.range_length
    @c['Content-Range'] = "bytes 1-500/1000"
    assert_equal 500, @c.range_length
    @c['Content-Range'] = "bytes 1-1/1000"
    assert_equal 1, @c.range_length
  end

  def test_chunked?
    try_chunked true, 'chunked'
    try_chunked true, '  chunked  '
    try_chunked true, '(OK)chunked'

    try_chunked false, 'not-chunked'
    try_chunked false, 'chunked-but-not-chunked'
  end

  def try_chunked(bool, str)
    @c['transfer-encoding'] = str
    assert_equal bool, @c.chunked?
  end

  def test_content_length
    @c.delete('content-length')
    assert_nil @c['content-length']

    try_content_length 500, '500'
    try_content_length 10000_0000_0000, '1000000000000'
    try_content_length 123, '  123'
    try_content_length 1,   '1 23'
    try_content_length 500, '(OK)500'
    assert_raise(Net::HTTPHeaderSyntaxError, 'here is no digit, but') {
      @c['content-length'] = 'no digit'
      @c.content_length
    }
  end

  def try_content_length(len, str)
    @c['content-length'] = str
    assert_equal len, @c.content_length
  end

  def test_content_length=
    @c.content_length = 0
    assert_equal 0, @c.content_length
    @c.content_length = 1
    assert_equal 1, @c.content_length
    @c.content_length = 999
    assert_equal 999, @c.content_length
    @c.content_length = 10000000000000
    assert_equal 10000000000000, @c.content_length
  end

  def test_content_type
    assert_nil @c.content_type
    @c.content_type = 'text/html'
    assert_equal 'text/html', @c.content_type
    @c.content_type = 'application/pdf'
    assert_equal 'application/pdf', @c.content_type
    @c.set_content_type 'text/html', {'charset' => 'iso-2022-jp'}
    assert_equal 'text/html', @c.content_type
    @c.content_type = 'text'
    assert_equal 'text', @c.content_type
  end

  def test_main_type
    assert_nil @c.main_type
    @c.content_type = 'text/html'
    assert_equal 'text', @c.main_type
    @c.content_type = 'application/pdf'
    assert_equal 'application', @c.main_type
    @c.set_content_type 'text/html', {'charset' => 'iso-2022-jp'}
    assert_equal 'text', @c.main_type
    @c.content_type = 'text'
    assert_equal 'text', @c.main_type
  end

  def test_sub_type
    assert_nil @c.sub_type
    @c.content_type = 'text/html'
    assert_equal 'html', @c.sub_type
    @c.content_type = 'application/pdf'
    assert_equal 'pdf', @c.sub_type
    @c.set_content_type 'text/html', {'charset' => 'iso-2022-jp'}
    assert_equal 'html', @c.sub_type
    @c.content_type = 'text'
    assert_nil @c.sub_type
  end

  def test_type_params
    assert_equal({}, @c.type_params)
    @c.content_type = 'text/html'
    assert_equal({}, @c.type_params)
    @c.content_type = 'application/pdf'
    assert_equal({}, @c.type_params)
    @c.set_content_type 'text/html', {'charset' => 'iso-2022-jp'}
    assert_equal({'charset' => 'iso-2022-jp'}, @c.type_params)
    @c.content_type = 'text'
    assert_equal({}, @c.type_params)
  end

  def test_set_content_type
  end

  def test_form_data=
    @c.form_data = {"cmd"=>"search", "q"=>"ruby", "max"=>"50"}
    assert_equal 'application/x-www-form-urlencoded', @c.content_type
    assert_equal %w( cmd=search max=50 q=ruby ), @c.body.split('&').sort
  end

  def test_set_form_data
    @c.set_form_data "cmd"=>"search", "q"=>"ruby", "max"=>"50"
    assert_equal 'application/x-www-form-urlencoded', @c.content_type
    assert_equal %w( cmd=search max=50 q=ruby ), @c.body.split('&').sort

    @c.set_form_data "cmd"=>"search", "q"=>"ruby", "max"=>50
    assert_equal 'application/x-www-form-urlencoded', @c.content_type
    assert_equal %w( cmd=search max=50 q=ruby ), @c.body.split('&').sort

    @c.set_form_data({"cmd"=>"search", "q"=>"ruby", "max"=>"50"}, ';')
    assert_equal 'application/x-www-form-urlencoded', @c.content_type
    assert_equal %w( cmd=search max=50 q=ruby ), @c.body.split(';').sort
  end

  def test_basic_auth
  end

  def test_proxy_basic_auth
  end

end
