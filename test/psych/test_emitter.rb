# -*- coding: utf-8 -*-

require 'psych/helper'

module Psych
  class TestEmitter < TestCase
    def setup
      super
      @out = StringIO.new('')
      @emitter = Psych::Emitter.new @out
    end

    def test_line_width
      assert_equal 0, @emitter.line_width
      assert_equal 10, @emitter.line_width = 10
      assert_equal 10, @emitter.line_width
    end

    def test_set_canonical
      @emitter.canonical = true
      assert_equal true, @emitter.canonical

      @emitter.canonical = false
      assert_equal false, @emitter.canonical
    end

    def test_indentation_set
      assert_equal 2, @emitter.indentation
      @emitter.indentation = 5
      assert_equal 5, @emitter.indentation
    end

    def test_emit_utf_8
      @emitter.start_stream Psych::Nodes::Stream::UTF8
      @emitter.start_document [], [], false
      @emitter.scalar '日本語', nil, nil, false, true, 1
      @emitter.end_document true
      @emitter.end_stream
      assert_match('日本語', @out.string)
    end

    def test_start_stream_arg_error
      assert_raises(TypeError) do
        @emitter.start_stream 'asdfasdf'
      end
    end

    def test_start_doc_arg_error
      @emitter.start_stream Psych::Nodes::Stream::UTF8

      [
        [nil, [], false],
        [[nil, nil], [], false],
        [[], 'foo', false],
        [[], ['foo'], false],
        [[], [nil,nil], false],
      ].each do |args|
        assert_raises(TypeError) do
          @emitter.start_document(*args)
        end
      end
    end

    def test_scalar_arg_error
      @emitter.start_stream Psych::Nodes::Stream::UTF8
      @emitter.start_document [], [], false

      [
        [:foo, nil, nil, false, true, 1],
        ['foo', Object.new, nil, false, true, 1],
        ['foo', nil, Object.new, false, true, 1],
        ['foo', nil, nil, false, true, :foo],
        [nil, nil, nil, false, true, 1],
      ].each do |args|
        assert_raises(TypeError) do
          @emitter.scalar(*args)
        end
      end
    end

    def test_start_sequence_arg_error
      @emitter.start_stream Psych::Nodes::Stream::UTF8
      @emitter.start_document [], [], false

      assert_raises(TypeError) do
        @emitter.start_sequence(nil, Object.new, true, 1)
      end

      assert_raises(TypeError) do
        @emitter.start_sequence(nil, nil, true, :foo)
      end
    end
  end
end
