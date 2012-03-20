require 'test/unit'

class TestCall < Test::Unit::TestCase
  def aaa(a, b=100, *rest)
    res = [a, b]
    res += rest if rest
    return res
  end

  def test_call
    assert_raise(ArgumentError) {aaa()}
    assert_raise(ArgumentError) {aaa}

    assert_equal([1, 100], aaa(1))
    assert_equal([1, 2], aaa(1, 2))
    assert_equal([1, 2, 3, 4], aaa(1, 2, 3, 4))
    assert_equal([1, 2, 3, 4], aaa(1, *[2, 3, 4]))
  end
end
