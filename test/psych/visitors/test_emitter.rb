require 'psych/helper'

module Psych
  module Visitors
    class TestEmitter < TestCase
      def setup
        super
        @io = StringIO.new
        @visitor = Visitors::Emitter.new @io
      end

      def test_options
        io = StringIO.new
        visitor = Visitors::Emitter.new io, :indentation => 3

        s       = Nodes::Stream.new
        doc     = Nodes::Document.new
        mapping = Nodes::Mapping.new
        m2      = Nodes::Mapping.new
        m2.children << Nodes::Scalar.new('a')
        m2.children << Nodes::Scalar.new('b')

        mapping.children << Nodes::Scalar.new('key')
        mapping.children << m2
        doc.children << mapping
        s.children << doc

        visitor.accept s
        assert_match(/^[ ]{3}a/, io.string)
      end

      def test_stream
        s = Nodes::Stream.new
        @visitor.accept s
        assert_equal '', @io.string
      end

      def test_document
        s       = Nodes::Stream.new
        doc     = Nodes::Document.new [1,1]
        scalar  = Nodes::Scalar.new 'hello world'

        doc.children << scalar
        s.children << doc

        @visitor.accept s

        assert_match(/1.1/, @io.string)
        assert_equal @io.string, s.yaml
      end

      def test_document_implicit_end
        s       = Nodes::Stream.new
        doc     = Nodes::Document.new
        mapping = Nodes::Mapping.new
        mapping.children << Nodes::Scalar.new('key')
        mapping.children << Nodes::Scalar.new('value')
        doc.children << mapping
        s.children << doc

        @visitor.accept s

        assert_match(/key: value/, @io.string)
        assert_equal @io.string, s.yaml
        assert(/\.\.\./ !~ s.yaml)
      end

      def test_scalar
        s       = Nodes::Stream.new
        doc     = Nodes::Document.new
        scalar  = Nodes::Scalar.new 'hello world'

        doc.children << scalar
        s.children << doc

        @visitor.accept s

        assert_match(/hello/, @io.string)
        assert_equal @io.string, s.yaml
      end

      def test_scalar_with_tag
        s       = Nodes::Stream.new
        doc     = Nodes::Document.new
        scalar  = Nodes::Scalar.new 'hello world', nil, '!str', false, false, 5

        doc.children << scalar
        s.children << doc

        @visitor.accept s

        assert_match(/str/, @io.string)
        assert_match(/hello/, @io.string)
        assert_equal @io.string, s.yaml
      end

      def test_sequence
        s       = Nodes::Stream.new
        doc     = Nodes::Document.new
        scalar  = Nodes::Scalar.new 'hello world'
        seq     = Nodes::Sequence.new

        seq.children << scalar
        doc.children << seq
        s.children << doc

        @visitor.accept s

        assert_match(/- hello/, @io.string)
        assert_equal @io.string, s.yaml
      end

      def test_mapping
        s       = Nodes::Stream.new
        doc     = Nodes::Document.new
        mapping = Nodes::Mapping.new
        mapping.children << Nodes::Scalar.new('key')
        mapping.children << Nodes::Scalar.new('value')
        doc.children << mapping
        s.children << doc

        @visitor.accept s

        assert_match(/key: value/, @io.string)
        assert_equal @io.string, s.yaml
      end

      def test_alias
        s       = Nodes::Stream.new
        doc     = Nodes::Document.new
        mapping = Nodes::Mapping.new
        mapping.children << Nodes::Scalar.new('key', 'A')
        mapping.children << Nodes::Alias.new('A')
        doc.children << mapping
        s.children << doc

        @visitor.accept s

        assert_match(/&A key: \*A/, @io.string)
        assert_equal @io.string, s.yaml
      end
    end
  end
end
