require 'test/unit'

class TestConst < Test::Unit::TestCase
  TEST1 = 1
  TEST2 = 2

  module Const
    TEST3 = 3
    TEST4 = 4
  end

  module Const2
    TEST3 = 6
    TEST4 = 8
  end

  def test_const
    assert defined?(TEST1)
    assert_equal 1, TEST1
    assert defined?(TEST2)
    assert_equal 2, TEST2

    self.class.class_eval {
      include Const
    }
    assert defined?(TEST1)
    assert_equal 1, TEST1
    assert defined?(TEST2)
    assert_equal 2, TEST2
    assert defined?(TEST3)
    assert_equal 3, TEST3
    assert defined?(TEST4)
    assert_equal 4, TEST4

    self.class.class_eval {
      include Const2
    }
    STDERR.print "intentionally redefines TEST3, TEST4\n" if $VERBOSE
    assert defined?(TEST1)
    assert_equal 1, TEST1
    assert defined?(TEST2)
    assert_equal 2, TEST2
    assert defined?(TEST3)
    assert_equal 6, TEST3
    assert defined?(TEST4)
    assert_equal 8, TEST4
  end

  def test_redefinition
    c = Class.new
    c.const_set(:X, 1)
    assert_output(nil, <<-WARNING) {c.const_set(:X, 2)}
#{__FILE__}:#{__LINE__-1}: warning: already initialized constant X
#{__FILE__}:#{__LINE__-3}: warning: previous definition of X was here
WARNING
  end
end
