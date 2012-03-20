require 'psych/helper'

module Psych
  class TestStream < TestCase
    def test_parse_partial
      rb = Psych.parse("--- foo\n...\n--- `").to_ruby
      assert_equal 'foo', rb
    end

    def test_load_partial
      rb = Psych.load("--- foo\n...\n--- `")
      assert_equal 'foo', rb
    end

    def test_parse_stream_yields_documents
      list = []
      Psych.parse_stream("--- foo\n...\n--- bar") do |doc|
        list << doc.to_ruby
      end
      assert_equal %w{ foo bar }, list
    end

    def test_parse_stream_break
      list = []
      Psych.parse_stream("--- foo\n...\n--- `") do |doc|
        list << doc.to_ruby
        break
      end
      assert_equal %w{ foo }, list
    end

    def test_load_stream_yields_documents
      list = []
      Psych.load_stream("--- foo\n...\n--- bar") do |ruby|
        list << ruby
      end
      assert_equal %w{ foo bar }, list
    end

    def test_load_stream_break
      list = []
      Psych.load_stream("--- foo\n...\n--- `") do |ruby|
        list << ruby
        break
      end
      assert_equal %w{ foo }, list
    end

    def test_explicit_documents
      io     = StringIO.new
      stream = Psych::Stream.new(io)
      stream.start
      stream.push({ 'foo' => 'bar' })

      assert !stream.finished?, 'stream not finished'
      stream.finish
      assert stream.finished?, 'stream finished'

      assert_match(/^---/, io.string)
      assert_match(/\.\.\.$/, io.string)
    end

    def test_start_takes_block
      io     = StringIO.new
      stream = Psych::Stream.new(io)
      stream.start do |emitter|
        emitter.push({ 'foo' => 'bar' })
      end

      assert stream.finished?, 'stream finished'
      assert_match(/^---/, io.string)
      assert_match(/\.\.\.$/, io.string)
    end

    def test_no_backreferences
      io     = StringIO.new
      stream = Psych::Stream.new(io)
      stream.start do |emitter|
        x = { 'foo' => 'bar' }
        emitter.push x
        emitter.push x
      end

      assert stream.finished?, 'stream finished'
      assert_match(/^---/, io.string)
      assert_match(/\.\.\.$/, io.string)
      assert_equal 2, io.string.scan('---').length
      assert_equal 2, io.string.scan('...').length
      assert_equal 2, io.string.scan('foo').length
      assert_equal 2, io.string.scan('bar').length
    end
  end
end
