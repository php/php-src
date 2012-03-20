require 'test/unit'

class TestCondition < Test::Unit::TestCase

  # [should] first test to see if we can run the tests.

  def test_condition
    $x = '0';

    $x == $x && assert(true)
    $x != $x && assert(false)
    $x == $x || assert(false)
    $x != $x || assert(true)

  end
end
