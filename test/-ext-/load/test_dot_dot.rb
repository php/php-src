require 'test/unit'

class Test_DotDot < Test::Unit::TestCase
  def test_load_dot_dot
    feature = '[ruby-dev:41774]'
    assert_nothing_raised(LoadError, feature) {
      require '-test-/load/dot.dot/dot.dot'
    }
  end
end
