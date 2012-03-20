require 'psych/helper'

module Psych
  module JSON
    class TestStream < TestCase
      def setup
        @io     = StringIO.new
        @stream = Psych::JSON::Stream.new(@io)
        @stream.start
      end

      def test_explicit_documents
        @io     = StringIO.new
        @stream = Psych::JSON::Stream.new(@io)
        @stream.start

        @stream.push({ 'foo' => 'bar' })

        assert !@stream.finished?, 'stream not finished'
        @stream.finish
        assert @stream.finished?, 'stream finished'

        assert_match(/^---/, @io.string)
        assert_match(/\.\.\.$/, @io.string)
      end

      def test_null
        @stream.push(nil)
        assert_match(/^--- null/, @io.string)
      end

      def test_string
        @stream.push "foo"
        assert_match(/(["])foo\1/, @io.string)
      end

      def test_symbol
        @stream.push :foo
        assert_match(/(["])foo\1/, @io.string)
      end

      def test_int
        @stream.push 10
        assert_match(/^--- 10/, @io.string)
      end

      def test_float
        @stream.push 1.2
        assert_match(/^--- 1.2/, @io.string)
      end

      def test_hash
        hash = { 'one' => 'two' }
        @stream.push hash

        json = @io.string
        assert_match(/}$/, json)
        assert_match(/^--- \{/, json)
        assert_match(/["]one['"]/, json)
        assert_match(/["]two['"]/, json)
      end

      def test_list_to_json
        list = %w{ one two }
        @stream.push list

        json = @io.string
        assert_match(/]$/, json)
        assert_match(/^--- \[/, json)
        assert_match(/["]one["]/, json)
        assert_match(/["]two["]/, json)
      end

      class Foo; end

      def test_json_dump_exclude_tag
        @stream << Foo.new
        json = @io.string
        refute_match('Foo', json)
      end

      class Bar
        def encode_with coder
          coder.represent_seq 'omg', %w{ a b c }
        end
      end

      def test_json_list_dump_exclude_tag
        @stream << Bar.new
        json = @io.string
        refute_match('omg', json)
      end

      def test_time
        time = Time.utc(2010, 10, 10)
        @stream.push({'a' => time })
        json = @io.string
        assert_match "{\"a\": \"2010-10-10 00:00:00.000000000 Z\"}\n", json
      end

      def test_datetime
        time = Time.new(2010, 10, 10).to_datetime
        @stream.push({'a' => time })
        json = @io.string
        assert_match "{\"a\": \"#{time.strftime("%Y-%m-%d %H:%M:%S.%9N %:z")}\"}\n", json
      end
    end
  end
end
