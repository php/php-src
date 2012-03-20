require 'test/unit'
require 'socket'

class TestSocketOption < Test::Unit::TestCase
  def test_new
    data = [1].pack("i")
    sockopt = Socket::Option.new(:INET, :SOCKET, :KEEPALIVE, data)
    assert_equal(Socket::AF_INET, sockopt.family)
    assert_equal(Socket::SOL_SOCKET, sockopt.level)
    assert_equal(Socket::SO_KEEPALIVE, sockopt.optname)
    assert_equal(Socket::SO_KEEPALIVE, sockopt.optname)
    assert_equal(data, sockopt.data)
  end

  def test_bool
    opt = Socket::Option.bool(:INET, :SOCKET, :KEEPALIVE, true)
    assert_equal(1, opt.int)
    opt = Socket::Option.bool(:INET, :SOCKET, :KEEPALIVE, false)
    assert_equal(0, opt.int)
    opt = Socket::Option.int(:INET, :SOCKET, :KEEPALIVE, 0)
    assert_equal(false, opt.bool)
    opt = Socket::Option.int(:INET, :SOCKET, :KEEPALIVE, 1)
    assert_equal(true, opt.bool)
    opt = Socket::Option.int(:INET, :SOCKET, :KEEPALIVE, 2)
    assert_equal(true, opt.bool)
  end

  def test_unpack
    sockopt = Socket::Option.new(:INET, :SOCKET, :KEEPALIVE, [1].pack("i"))
    assert_equal([1], sockopt.unpack("i"))
    assert_equal([1], sockopt.data.unpack("i"))
  end
end
