require 'psych/helper'

module Psych
  module Visitors
    class TestToRuby < TestCase
      def setup
        super
        @visitor = ToRuby.new
      end

      def test_object
        mapping = Nodes::Mapping.new nil, "!ruby/object"
        mapping.children << Nodes::Scalar.new('foo')
        mapping.children << Nodes::Scalar.new('bar')

        o = mapping.to_ruby
        assert_equal 'bar', o.instance_variable_get(:@foo)
      end

      def test_awesome
        Psych.load('1900-01-01T00:00:00+00:00')
      end

      def test_legacy_struct
        foo = Struct.new('AWESOME', :bar)
        assert_equal foo.new('baz'), Psych.load(<<-eoyml)
!ruby/struct:AWESOME
  bar: baz
        eoyml
      end

      def test_binary
        gif = "GIF89a\f\x00\f\x00\x84\x00\x00\xFF\xFF\xF7\xF5\xF5\xEE\xE9\xE9\xE5fff\x00\x00\x00\xE7\xE7\xE7^^^\xF3\xF3\xED\x8E\x8E\x8E\xE0\xE0\xE0\x9F\x9F\x9F\x93\x93\x93\xA7\xA7\xA7\x9E\x9E\x9Eiiiccc\xA3\xA3\xA3\x84\x84\x84\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9\xFF\xFE\xF9!\xFE\x0EMade with GIMP\x00,\x00\x00\x00\x00\f\x00\f\x00\x00\x05,  \x8E\x810\x9E\xE3@\x14\xE8i\x10\xC4\xD1\x8A\b\x1C\xCF\x80M$z\xEF\xFF0\x85p\xB8\xB01f\r\e\xCE\x01\xC3\x01\x1E\x10' \x82\n\x01\x00;"

        hash = Psych.load(<<-'eoyaml')
canonical: !!binary "\
 R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5\
 OTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/+\
 +f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLC\
 AgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs="
generic: !binary |
 R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5
 OTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/+
 +f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLC
 AgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs=
description:
 The binary value above is a tiny arrow encoded as a gif image.
        eoyaml
        assert_equal gif, hash['canonical']
        assert_equal gif, hash['generic']
      end

      A = Struct.new(:foo)

      def test_struct
        s = A.new('bar')

        mapping = Nodes::Mapping.new nil, "!ruby/struct:#{s.class}"
        mapping.children << Nodes::Scalar.new('foo')
        mapping.children << Nodes::Scalar.new('bar')

        ruby = mapping.to_ruby

        assert_equal s.class, ruby.class
        assert_equal s.foo, ruby.foo
        assert_equal s, ruby
      end

      def test_anon_struct_legacy
        s = Struct.new(:foo).new('bar')

        mapping = Nodes::Mapping.new nil, '!ruby/struct:'
        mapping.children << Nodes::Scalar.new('foo')
        mapping.children << Nodes::Scalar.new('bar')

        assert_equal s.foo, mapping.to_ruby.foo
      end

      def test_anon_struct
        s = Struct.new(:foo).new('bar')

        mapping = Nodes::Mapping.new nil, '!ruby/struct'
        mapping.children << Nodes::Scalar.new('foo')
        mapping.children << Nodes::Scalar.new('bar')

        assert_equal s.foo, mapping.to_ruby.foo
      end

      def test_exception
        exc = Exception.new 'hello'

        mapping = Nodes::Mapping.new nil, '!ruby/exception'
        mapping.children << Nodes::Scalar.new('message')
        mapping.children << Nodes::Scalar.new('hello')

        ruby = mapping.to_ruby

        assert_equal exc.class, ruby.class
        assert_equal exc.message, ruby.message
      end

      def test_regexp
        node = Nodes::Scalar.new('/foo/', nil, '!ruby/regexp')
        assert_equal(/foo/, node.to_ruby)

        node = Nodes::Scalar.new('/foo/m', nil, '!ruby/regexp')
        assert_equal(/foo/m, node.to_ruby)

        node = Nodes::Scalar.new('/foo/ix', nil, '!ruby/regexp')
        assert_equal(/foo/ix, node.to_ruby)
      end

      def test_time
        now = Time.now
        zone = now.strftime('%z')
        zone = " #{zone[0,3]}:#{zone[3,5]}"

        formatted = now.strftime("%Y-%m-%d %H:%M:%S.%9N") + zone

        assert_equal now, Nodes::Scalar.new(formatted).to_ruby
      end

      def test_time_utc
        now = Time.now.utc
        formatted = now.strftime("%Y-%m-%d %H:%M:%S") +
          ".%09dZ" % [now.nsec]

        assert_equal now, Nodes::Scalar.new(formatted).to_ruby
      end

      def test_time_utc_no_z
        now = Time.now.utc
        formatted = now.strftime("%Y-%m-%d %H:%M:%S") +
          ".%09d" % [now.nsec]

        assert_equal now, Nodes::Scalar.new(formatted).to_ruby
      end

      def test_date
        d = '1980-12-16'
        actual = Date.strptime(d, '%Y-%m-%d')

        date = Nodes::Scalar.new(d, nil, 'tag:yaml.org,2002:timestamp', false)

        assert_equal actual, date.to_ruby
      end

      def test_rational
        mapping = Nodes::Mapping.new nil, '!ruby/object:Rational'
        mapping.children << Nodes::Scalar.new('denominator')
        mapping.children << Nodes::Scalar.new('2')
        mapping.children << Nodes::Scalar.new('numerator')
        mapping.children << Nodes::Scalar.new('1')

        assert_equal Rational(1,2), mapping.to_ruby
      end

      def test_complex
        mapping = Nodes::Mapping.new nil, '!ruby/object:Complex'
        mapping.children << Nodes::Scalar.new('image')
        mapping.children << Nodes::Scalar.new('2')
        mapping.children << Nodes::Scalar.new('real')
        mapping.children << Nodes::Scalar.new('1')

        assert_equal Complex(1,2), mapping.to_ruby
      end

      if RUBY_VERSION >= '1.9'
        def test_complex_string
          node = Nodes::Scalar.new '3+4i', nil, "!ruby/object:Complex"
          assert_equal Complex(3, 4), node.to_ruby
        end

        def test_rational_string
          node = Nodes::Scalar.new '1/2', nil, "!ruby/object:Rational"
          assert_equal Rational(1, 2), node.to_ruby
        end
      end

      def test_range_string
        node = Nodes::Scalar.new '1..2', nil, "!ruby/range"
        assert_equal 1..2, node.to_ruby
      end

      def test_range_string_triple
        node = Nodes::Scalar.new '1...3', nil, "!ruby/range"
        assert_equal 1...3, node.to_ruby
      end

      def test_integer
        i = Nodes::Scalar.new('1', nil, 'tag:yaml.org,2002:int')
        assert_equal 1, i.to_ruby

        assert_equal 1, Nodes::Scalar.new('1').to_ruby

        i = Nodes::Scalar.new('-1', nil, 'tag:yaml.org,2002:int')
        assert_equal(-1, i.to_ruby)

        assert_equal(-1, Nodes::Scalar.new('-1').to_ruby)
        assert_equal 1, Nodes::Scalar.new('+1').to_ruby
      end

      def test_int_ignore
        ['1,000', '1_000'].each do |num|
          i = Nodes::Scalar.new(num, nil, 'tag:yaml.org,2002:int')
          assert_equal 1000, i.to_ruby

          assert_equal 1000, Nodes::Scalar.new(num).to_ruby
        end
      end

      def test_float_ignore
        ['1,000.3', '1_000.3'].each do |num|
          i = Nodes::Scalar.new(num, nil, 'tag:yaml.org,2002:float')
          assert_equal 1000.3, i.to_ruby

          i = Nodes::Scalar.new(num, nil, '!float')
          assert_equal 1000.3, i.to_ruby

          assert_equal 1000.3, Nodes::Scalar.new(num).to_ruby
        end
      end

      # http://yaml.org/type/bool.html
      def test_boolean_true
        %w{ yes Yes YES true True TRUE on On ON }.each do |t|
          i = Nodes::Scalar.new(t, nil, 'tag:yaml.org,2002:bool')
          assert_equal true, i.to_ruby
          assert_equal true, Nodes::Scalar.new(t).to_ruby
        end
      end

      # http://yaml.org/type/bool.html
      def test_boolean_false
        %w{ no No NO false False FALSE off Off OFF }.each do |t|
          i = Nodes::Scalar.new(t, nil, 'tag:yaml.org,2002:bool')
          assert_equal false, i.to_ruby
          assert_equal false, Nodes::Scalar.new(t).to_ruby
        end
      end

      def test_float
        i = Nodes::Scalar.new('12', nil, 'tag:yaml.org,2002:float')
        assert_equal 12.0, i.to_ruby

        i = Nodes::Scalar.new('1.2', nil, 'tag:yaml.org,2002:float')
        assert_equal 1.2, i.to_ruby

        i = Nodes::Scalar.new('1.2')
        assert_equal 1.2, i.to_ruby

        assert_equal 1, Nodes::Scalar.new('.Inf').to_ruby.infinite?
        assert_equal 1, Nodes::Scalar.new('.inf').to_ruby.infinite?
        assert_equal 1, Nodes::Scalar.new('.Inf', nil, 'tag:yaml.org,2002:float').to_ruby.infinite?

        assert_equal(-1, Nodes::Scalar.new('-.inf').to_ruby.infinite?)
        assert_equal(-1, Nodes::Scalar.new('-.Inf').to_ruby.infinite?)
        assert_equal(-1, Nodes::Scalar.new('-.Inf', nil, 'tag:yaml.org,2002:float').to_ruby.infinite?)

        assert Nodes::Scalar.new('.NaN').to_ruby.nan?
        assert Nodes::Scalar.new('.NaN', nil, 'tag:yaml.org,2002:float').to_ruby.nan?
      end

      def test_exp_float
        exp = 1.2e+30

        i = Nodes::Scalar.new(exp.to_s, nil, 'tag:yaml.org,2002:float')
        assert_equal exp, i.to_ruby

        assert_equal exp, Nodes::Scalar.new(exp.to_s).to_ruby
      end

      def test_scalar
        scalar = Nodes::Scalar.new('foo')
        assert_equal 'foo', @visitor.accept(scalar)
        assert_equal 'foo', scalar.to_ruby
      end

      def test_sequence
        seq = Nodes::Sequence.new
        seq.children << Nodes::Scalar.new('foo')
        seq.children << Nodes::Scalar.new('bar')

        assert_equal %w{ foo bar }, seq.to_ruby
      end

      def test_mapping
        mapping = Nodes::Mapping.new
        mapping.children << Nodes::Scalar.new('foo')
        mapping.children << Nodes::Scalar.new('bar')
        assert_equal({'foo' => 'bar'}, mapping.to_ruby)
      end

      def test_document
        doc = Nodes::Document.new
        doc.children << Nodes::Scalar.new('foo')
        assert_equal 'foo', doc.to_ruby
      end

      def test_stream
        a = Nodes::Document.new
        a.children << Nodes::Scalar.new('foo')

        b = Nodes::Document.new
        b.children << Nodes::Scalar.new('bar')

        stream = Nodes::Stream.new
        stream.children << a
        stream.children << b

        assert_equal %w{ foo bar }, stream.to_ruby
      end

      def test_alias
        seq = Nodes::Sequence.new
        seq.children << Nodes::Scalar.new('foo', 'A')
        seq.children << Nodes::Alias.new('A')

        list = seq.to_ruby
        assert_equal %w{ foo foo }, list
        assert_equal list[0].object_id, list[1].object_id
      end
    end
  end
end
