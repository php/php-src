require 'test/unit'

class TestIfunless < Test::Unit::TestCase
  def test_if_unless
    $x = 'test';
    assert(if $x == $x then true else false end)
    $bad = false
    unless $x == $x
      $bad = true
    end
    assert(!$bad)
    assert(unless $x != $x then true else false end)
  end
end
