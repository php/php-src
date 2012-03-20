require 'net/http'
require 'test/unit'
require 'stringio'

class HTTPResponseTest < Test::Unit::TestCase
  def test_singleline_header
    io = dummy_io(<<EOS.gsub(/\n/, "\r\n"))
HTTP/1.1 200 OK
Content-Length: 5
Connection: close

hello
EOS
    res = Net::HTTPResponse.read_new(io)
    assert_equal('5', res.header['content-length'])
    assert_equal('close', res.header['connection'])
  end

  def test_multiline_header
    io = dummy_io(<<EOS.gsub(/\n/, "\r\n"))
HTTP/1.1 200 OK
X-Foo: XXX
   YYY
X-Bar:
 XXX
\tYYY

hello
EOS
    res = Net::HTTPResponse.read_new(io)
    assert_equal('XXX YYY', res.header['x-foo'])
    assert_equal('XXX YYY', res.header['x-bar'])
  end

private

  def dummy_io(str)
    Net::BufferedIO.new(StringIO.new(str))
  end
end
