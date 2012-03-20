require 'test/unit'
require 'yaml'

module Syck
  class TestYamlProperties < Test::Unit::TestCase
    class Foo
      attr_reader :a, :b, :c
      def initialize
        @a = 1
        @b = 2
        @c = 3
      end

      def to_yaml_properties
        [:@a, :@b]
      end
    end

    def test_object_dump_yaml_properties
      foo = YAML.load(YAML.dump(Foo.new))
      assert_equal 1, foo.a
      assert_equal 2, foo.b
      assert_nil foo.c
    end

    class Bar < Struct.new(:foo, :bar)
      attr_reader :baz
      def initialize *args
        super
        @baz = 'hello'
      end

      def to_yaml_properties
        []
      end
    end

    def test_struct_dump_yaml_properties
      bar = YAML.load(YAML.dump(Bar.new('a', 'b')))
      assert_equal 'a', bar.foo
      assert_equal 'b', bar.bar
      assert_nil bar.baz
    end

    def test_string_dump
      string = "okonomiyaki"
      class << string
        def to_yaml_properties
          [:@tastes]
        end
      end

      string.instance_variable_set(:@tastes, 'delicious')
      v = YAML.load YAML.dump string
      assert_equal 'delicious', v.instance_variable_get(:@tastes)
    end

    def test_string_load
      str = YAML.load("--- !str \nstr: okonomiyaki\n:@tastes: delicious\n")
      assert_equal 'okonomiyaki', str
      assert_equal 'delicious', str.instance_variable_get(:@tastes)
    end
  end
end
