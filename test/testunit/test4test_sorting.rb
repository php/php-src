require 'test/unit'

class TestForTestHideSkip < Test::Unit::TestCase
  def test_c
    skip "do nothing"
  end

  def test_b
    assert_equal true, false
  end

  def test_a
    raise
  end
end
