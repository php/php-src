require 'test/unit'
require '-test-/iter/break'

class TestIterBreak < Test::Unit::TestCase
  def test_iter_break
    feature5895 = '[ruby-dev:45132]'
    assert_equal(42, 1.times{Bug::Breakable.iter_break(42)}, feature5895)
  end
end
