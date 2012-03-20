require 'psych/helper'

class PsychStructWithIvar < Struct.new(:foo)
  attr_reader :bar
  def initialize *args
    super
    @bar = 'hello'
  end
end

module Psych
  class TestStruct < TestCase
    class StructSubclass < Struct.new(:foo)
      def initialize foo, bar
        super(foo)
        @bar = bar
      end
    end

    def test_self_referential_struct
      ss = StructSubclass.new(nil, 'foo')
      ss.foo = ss

      loaded = Psych.load(Psych.dump(ss))
      assert_instance_of(StructSubclass, loaded.foo)

      assert_equal(ss, loaded)
    end

    def test_roundtrip
      thing = PsychStructWithIvar.new('bar')
      struct = Psych.load(Psych.dump(thing))

      assert_equal 'hello', struct.bar
      assert_equal 'bar', struct.foo
    end

    def test_load
      obj = Psych.load(<<-eoyml)
--- !ruby/struct:PsychStructWithIvar
:foo: bar
:@bar: hello
      eoyml

      assert_equal 'hello', obj.bar
      assert_equal 'bar', obj.foo
    end
  end
end
