require 'psych/helper'

module Psych
  class TestClass < TestCase
    module Foo
    end

    def test_cycle_anonymous_class
      assert_raises(::TypeError) do
        assert_cycle(Class.new)
      end
    end

    def test_cycle_anonymous_module
      assert_raises(::TypeError) do
        assert_cycle(Module.new)
      end
    end

    def test_cycle
      assert_cycle(TestClass)
    end

    def test_dump
      Psych.dump TestClass
    end

    def test_cycle_module
      assert_cycle(Foo)
    end

    def test_dump_module
      Psych.dump Foo
    end
  end
end
