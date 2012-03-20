require 'test/unit'

class TestE < Test::Unit::TestCase
  def test_not_fail
    assert_equal(1,1)
  end

  def test_always_skip
    skip "always"
  end

  def test_always_fail
    assert_equal(0,1)
  end
end

