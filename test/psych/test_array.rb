require 'psych/helper'

module Psych
  class TestArray < TestCase
    class X < Array
    end

    class Y < Array
      attr_accessor :val
    end

    def setup
      super
      @list = [{ :a => 'b' }, 'foo']
    end

    def test_subclass
      yaml = Psych.dump X.new
      assert_match X.name, yaml

      list = X.new
      list << 1
      assert_equal X, list.class
      assert_equal 1, list.first
    end

    def test_subclass_with_attributes
      y = Psych.load Psych.dump Y.new.tap {|y| y.val = 1}
      assert_equal Y, y.class
      assert_equal 1, y.val
    end

    def test_backwards_with_syck
      x = Psych.load "--- !seq:#{X.name} []\n\n"
      assert_equal X, x.class
    end

    def test_self_referential
      @list << @list
      assert_cycle(@list)
    end

    def test_cycle
      assert_cycle(@list)
    end
  end
end
