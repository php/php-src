# coding: utf-8

require 'psych/helper'

module Psych
  class TestParser < TestCase
    class EventCatcher < Handler
      attr_accessor :parser
      attr_reader :calls, :marks
      def initialize
        @parser = nil
        @calls  = []
        @marks  = []
      end

      (Handler.instance_methods(true) -
       Object.instance_methods).each do |m|
        class_eval %{
          def #{m} *args
            super
            @marks << @parser.mark if @parser
            @calls << [:#{m}, args]
          end
        }
      end
    end

    def setup
      super
      @handler        = EventCatcher.new
      @parser         = Psych::Parser.new @handler
      @handler.parser = @parser
    end

    def test_exception_memory_leak
      yaml = <<-eoyaml
%YAML 1.1
%TAG ! tag:tenderlovemaking.com,2009:
--- &ponies
- first element
- *ponies
- foo: bar
...
      eoyaml

      [:start_stream, :start_document, :end_document, :alias, :scalar,
       :start_sequence, :end_sequence, :start_mapping, :end_mapping,
       :end_stream].each do |method|

        klass = Class.new(Psych::Handler) do
          define_method(method) do |*args|
            raise
          end
        end

        parser = Psych::Parser.new klass.new
        2.times {
          assert_raises(RuntimeError, method.to_s) do
            parser.parse yaml
          end
        }
      end
    end

    def test_multiparse
      3.times do
        @parser.parse '--- foo'
      end
    end

    def test_filename
      ex = assert_raises(Psych::SyntaxError) do
        @parser.parse '--- `', 'omg!'
      end
      assert_match 'omg!', ex.message
    end

    def test_line_numbers
      assert_equal 0, @parser.mark.line
      @parser.parse "---\n- hello\n- world"
      line_calls = @handler.marks.map(&:line).zip(@handler.calls.map(&:first))
      assert_equal [[0, :start_stream],
                    [0, :start_document],
                    [1, :start_sequence],
                    [2, :scalar],
                    [3, :scalar],
                    [3, :end_sequence],
                    [3, :end_document],
                    [3, :end_stream]], line_calls

      assert_equal 3, @parser.mark.line
    end

    def test_column_numbers
      assert_equal 0, @parser.mark.column
      @parser.parse "---\n- hello\n- world"
      col_calls = @handler.marks.map(&:column).zip(@handler.calls.map(&:first))
      assert_equal [[0, :start_stream],
                    [3, :start_document],
                    [1, :start_sequence],
                    [0, :scalar],
                    [0, :scalar],
                    [0, :end_sequence],
                    [0, :end_document],
                    [0, :end_stream]], col_calls

      assert_equal 0, @parser.mark.column
    end

    def test_index_numbers
      assert_equal 0, @parser.mark.index
      @parser.parse "---\n- hello\n- world"
      idx_calls = @handler.marks.map(&:index).zip(@handler.calls.map(&:first))
      assert_equal [[0, :start_stream],
                    [3, :start_document],
                    [5, :start_sequence],
                    [12, :scalar],
                    [19, :scalar],
                    [19, :end_sequence],
                    [19, :end_document],
                    [19, :end_stream]], idx_calls

      assert_equal 19, @parser.mark.index
    end

    def test_bom
      tadpole = 'おたまじゃくし'

      # BOM + text
      yml = "\uFEFF#{tadpole}".encode('UTF-16LE')
      @parser.parse yml
      assert_equal tadpole, @parser.handler.calls[2][1].first
    end

    def test_external_encoding
      tadpole = 'おたまじゃくし'

      @parser.external_encoding = Psych::Parser::UTF16LE
      @parser.parse tadpole.encode 'UTF-16LE'
      assert_equal tadpole, @parser.handler.calls[2][1].first
    end

    def test_bogus_io
      o = Object.new
      def o.external_encoding; nil end
      def o.read len; self end

      assert_raises(TypeError) do
        @parser.parse o
      end
    end

    def test_parse_io
      @parser.parse StringIO.new("--- a")
      assert_called :start_stream
      assert_called :scalar
      assert_called :end_stream
    end

    def test_syntax_error
      assert_raises(Psych::SyntaxError) do
        @parser.parse("---\n\"foo\"\n\"bar\"\n")
      end
    end

    def test_syntax_error_twice
      assert_raises(Psych::SyntaxError) do
        @parser.parse("---\n\"foo\"\n\"bar\"\n")
      end

      assert_raises(Psych::SyntaxError) do
        @parser.parse("---\n\"foo\"\n\"bar\"\n")
      end
    end

    def test_syntax_error_has_path_for_string
      e = assert_raises(Psych::SyntaxError) do
        @parser.parse("---\n\"foo\"\n\"bar\"\n")
      end
      assert_match '(<unknown>):', e.message
    end

    def test_syntax_error_has_path_for_io
      io = StringIO.new "---\n\"foo\"\n\"bar\"\n"
      def io.path; "hello!"; end

      e = assert_raises(Psych::SyntaxError) do
        @parser.parse(io)
      end
      assert_match "(#{io.path}):", e.message
    end

    def test_mapping_end
      @parser.parse("---\n!!map { key: value }")
      assert_called :end_mapping
    end

    def test_mapping_tag
      @parser.parse("---\n!!map { key: value }")
      assert_called :start_mapping, ["tag:yaml.org,2002:map", false, Nodes::Mapping::FLOW]
    end

    def test_mapping_anchor
      @parser.parse("---\n&A { key: value }")
      assert_called :start_mapping, ['A', true, Nodes::Mapping::FLOW]
    end

    def test_mapping_block
      @parser.parse("---\n  key: value")
      assert_called :start_mapping, [true, Nodes::Mapping::BLOCK]
    end

    def test_mapping_start
      @parser.parse("---\n{ key: value }")
      assert_called :start_mapping
      assert_called :start_mapping, [true, Nodes::Mapping::FLOW]
    end

    def test_sequence_end
      @parser.parse("---\n&A [1, 2]")
      assert_called :end_sequence
    end

    def test_sequence_start_anchor
      @parser.parse("---\n&A [1, 2]")
      assert_called :start_sequence, ["A", true, Nodes::Sequence::FLOW]
    end

    def test_sequence_start_tag
      @parser.parse("---\n!!seq [1, 2]")
      assert_called :start_sequence, ["tag:yaml.org,2002:seq", false, Nodes::Sequence::FLOW]
    end

    def test_sequence_start_flow
      @parser.parse("---\n[1, 2]")
      assert_called :start_sequence, [true, Nodes::Sequence::FLOW]
    end

    def test_sequence_start_block
      @parser.parse("---\n  - 1\n  - 2")
      assert_called :start_sequence, [true, Nodes::Sequence::BLOCK]
    end

    def test_literal_scalar
      @parser.parse(<<-eoyml)
%YAML 1.1
---
"literal\n\
        \ttext\n"
      eoyml
      assert_called :scalar, ['literal text ', false, true, Nodes::Scalar::DOUBLE_QUOTED]
    end

    def test_scalar
      @parser.parse("--- foo\n")
      assert_called :scalar, ['foo', true, false, Nodes::Scalar::PLAIN]
    end

    def test_scalar_with_tag
      @parser.parse("---\n!!str foo\n")
      assert_called :scalar, ['foo', 'tag:yaml.org,2002:str', false, false, Nodes::Scalar::PLAIN]
    end

    def test_scalar_with_anchor
      @parser.parse("---\n&A foo\n")
      assert_called :scalar, ['foo', 'A', true, false, Nodes::Scalar::PLAIN]
    end

    def test_scalar_plain_implicit
      @parser.parse("---\n&A foo\n")
      assert_called :scalar, ['foo', 'A', true, false, Nodes::Scalar::PLAIN]
    end

    def test_alias
      @parser.parse(<<-eoyml)
%YAML 1.1
---
!!seq [
  !!str "Without properties",
  &A !!str "Anchored",
  !!str "Tagged",
  *A,
  !!str "",
]
      eoyml
      assert_called :alias, ['A']
    end

    def test_end_stream
      @parser.parse("--- foo\n")
      assert_called :end_stream
    end

    def test_start_stream
      @parser.parse("--- foo\n")
      assert_called :start_stream
    end

    def test_end_document_implicit
      @parser.parse("\"foo\"\n")
      assert_called :end_document, [true]
    end

    def test_end_document_explicit
      @parser.parse("\"foo\"\n...")
      assert_called :end_document, [false]
    end

    def test_start_document_version
      @parser.parse("%YAML 1.1\n---\n\"foo\"\n")
      assert_called :start_document, [[1,1], [], false]
    end

    def test_start_document_tag
      @parser.parse("%TAG !yaml! tag:yaml.org,2002\n---\n!yaml!str \"foo\"\n")
      assert_called :start_document, [[], [['!yaml!', 'tag:yaml.org,2002']], false]
    end

    def assert_called call, with = nil, parser = @parser
      if with
        call = parser.handler.calls.find { |x|
          x.first == call && x.last.compact == with
        }
        assert(call,
          "#{[call,with].inspect} not in #{parser.handler.calls.inspect}"
        )
      else
        assert parser.handler.calls.any? { |x| x.first == call }
      end
    end
  end
end
