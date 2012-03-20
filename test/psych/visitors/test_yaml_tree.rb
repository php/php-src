require 'psych/helper'

module Psych
  module Visitors
    class TestYAMLTree < TestCase
      def setup
        super
        @v = Visitors::YAMLTree.new
      end

      def test_binary_formatting
        gif = "GIF89a\f\x00\f\x00\x84\x00\x00\xFF\xFF\xF7\xF5\xF5\xEE\xE9\xE9\xE5fff\x00\x00\x00\xE7\xE7\xE7^^^\xF3\xF3\xED\x8E\x8E\x8E\xE0\xE0\xE0\x9F\x9F\x9F\x93\x93\x93\xA7\xA7\xA7\x9E\x9E\x9Eiiiccc\xA3\xA3\xA3\x84\x84\x84\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9!\xFE\x0EMade with GIMP\x00,\x00\x00\x00\x00\f\x00\f\x00\x00\x05,  \x8E\x810\x9E\xE3@\x14\xE8i\x10\xC4\xD1\x8A\b\x1C\xCF\x80M$z\xEF\xFF0\x85p\xB8\xB01f\r\e\xCE\x01\xC3\x01\x1E\x10' \x82\n\x01\x00;"
        @v << gif
        scalar = @v.tree.children.first.children.first
        assert_equal Psych::Nodes::Scalar::LITERAL, scalar.style
      end

      def test_object_has_no_class
        yaml = Psych.dump(Object.new)
        assert(Psych.dump(Object.new) !~ /Object/, yaml)
      end

      def test_struct_const
        foo = Struct.new("Foo", :bar)
        assert_cycle foo.new('bar')
        Struct.instance_eval { remove_const(:Foo) }
      end

      A = Struct.new(:foo)

      def test_struct
        assert_cycle A.new('bar')
      end

      def test_struct_anon
        s = Struct.new(:foo).new('bar')
        obj =  Psych.load(Psych.dump(s))
        assert_equal s.foo, obj.foo
      end

      def test_override_method
        s = Struct.new(:method).new('override')
        obj =  Psych.load(Psych.dump(s))
        assert_equal s.method, obj.method
      end

      def test_exception
        ex = Exception.new 'foo'
        loaded = Psych.load(Psych.dump(ex))

        assert_equal ex.message, loaded.message
        assert_equal ex.class, loaded.class
      end

      def test_regexp
        assert_cycle(/foo/)
        assert_cycle(/foo/i)
        assert_cycle(/foo/mx)
      end

      def test_time
        t = Time.now
        assert_equal t, Psych.load(Psych.dump(t))
      end

      def test_date
        date = Date.strptime('2002-12-14', '%Y-%m-%d')
        assert_cycle date
      end

      def test_rational
        assert_cycle Rational(1,2)
      end

      def test_complex
        assert_cycle Complex(1,2)
      end

      def test_scalar
        assert_cycle 'foo'
        assert_cycle ':foo'
        assert_cycle ''
        assert_cycle ':'
      end

      def test_boolean
        assert_cycle true
        assert_cycle 'true'
        assert_cycle false
        assert_cycle 'false'
      end

      def test_range_inclusive
        assert_cycle 1..2
      end

      def test_range_exclusive
        assert_cycle 1...2
      end

      def test_anon_class
        assert_raises(TypeError) do
          @v.accept Class.new
        end

        assert_raises(TypeError) do
          Psych.dump(Class.new)
        end
      end

      def test_hash
        assert_cycle('a' => 'b')
      end

      def test_list
        assert_cycle(%w{ a b })
        assert_cycle([1, 2.2])
      end

      def test_symbol
        assert_cycle :foo
      end

      def test_int
        assert_cycle 1
        assert_cycle(-1)
        assert_cycle '1'
        assert_cycle '-1'
      end

      def test_float
        assert_cycle 1.2
        assert_cycle '1.2'

        assert Psych.load(Psych.dump(0.0 / 0.0)).nan?
        assert_equal 1, Psych.load(Psych.dump(1 / 0.0)).infinite?
        assert_equal(-1, Psych.load(Psych.dump(-1 / 0.0)).infinite?)
      end

      # http://yaml.org/type/null.html
      def test_nil
        assert_cycle nil
        assert_equal nil, Psych.load('null')
        assert_equal nil, Psych.load('Null')
        assert_equal nil, Psych.load('NULL')
        assert_equal nil, Psych.load('~')
        assert_equal({'foo' => nil}, Psych.load('foo: '))

        assert_cycle 'null'
        assert_cycle 'nUll'
        assert_cycle '~'
      end
    end
  end
end
