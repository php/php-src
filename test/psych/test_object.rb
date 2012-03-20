require 'psych/helper'

module Psych
  class Tagged
    yaml_tag '!foo'

    attr_accessor :baz

    def initialize
      @baz = 'bar'
    end
  end

  class Foo
    attr_accessor :parent

    def initialize parent
      @parent = parent
    end
  end

  class TestObject < TestCase
    def test_dump_with_tag
      tag = Tagged.new
      assert_match('foo', Psych.dump(tag))
    end

    def test_tag_round_trip
      tag   = Tagged.new
      tag2  = Psych.load(Psych.dump(tag))
      assert_equal tag.baz, tag2.baz
      assert_instance_of(Tagged, tag2)
    end

    def test_cyclic_references
      foo = Foo.new(nil)
      foo.parent = foo
      loaded = Psych.load Psych.dump foo

      assert_instance_of(Foo, loaded)
      assert_equal loaded, loaded.parent
    end
  end
end
