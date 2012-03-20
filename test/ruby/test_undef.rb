require 'test/unit'

class TestUndef < Test::Unit::TestCase
  class Undef0
    def foo
      "foo"
    end
    undef foo
  end

  class Undef1
    def bar
      "bar"
    end
  end

  class Undef2 < Undef1
    undef bar
  end

  def test_undef
    x = Undef0.new
    assert_raise(NoMethodError) { x.foo }
    y = Undef1.new
    assert_equal "bar", y.bar
    z = Undef2.new
    assert_raise(NoMethodError) { z.foo }
  end

  def test_special_const_undef
    assert_raise(TypeError) do
      1.instance_eval do
        undef to_s
      end
    end
  end
end
