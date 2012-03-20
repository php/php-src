require 'test/unit'
require "-test-/string/string"

class Test_StringEllipsize < Test::Unit::TestCase
  def setup
    @foobar = Bug::String.new("foobar")
  end

  def assert_equal_with_class(expected, result, *rest)
    assert_equal(expected.encoding, result.encoding, *rest)
    assert_equal(expected, result, result.encoding.name)
    assert_instance_of(Bug::String, result, *rest)
  end

  def test_longer
    assert_equal_with_class("", @foobar.ellipsize(0))
    assert_equal_with_class(".", @foobar.ellipsize(1))
    assert_equal_with_class("..", @foobar.ellipsize(2))
    assert_equal_with_class("...", @foobar.ellipsize(3))
    assert_equal_with_class("f...", @foobar.ellipsize(4))
    assert_equal_with_class("fo...", @foobar.ellipsize(5))
  end

  def test_shorter
    assert_same(@foobar, @foobar.ellipsize(6))
    assert_same(@foobar, @foobar.ellipsize(7))
  end

  def test_negative_length
    assert_raise(IndexError) {@foobar.ellipsize(-1)}
  end

  def test_nonascii
    a = "\u3042"
    encs = Encoding.list.each do |enc|
      next if enc.dummy?
      begin
        s = a.encode(enc)
        e = "...".encode(enc)
      rescue
      else
        assert_equal_with_class(s*12+e, Bug::String.new(s*20).ellipsize(15))
      end
    end
  end
end
