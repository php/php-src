module Psych
  module Visitors
    class Emitter < Psych::Visitors::Visitor
      def initialize io, options = {}
        @handler = Psych::Emitter.new io
        @handler.indentation = options[:indentation] if options[:indentation]
        @handler.canonical = options[:canonical] if options[:canonical]
        @handler.line_width = options[:line_width] if options[:line_width]
      end

      def visit_Psych_Nodes_Stream o
        @handler.start_stream o.encoding
        o.children.each { |c| accept c }
        @handler.end_stream
      end

      def visit_Psych_Nodes_Document o
        @handler.start_document o.version, o.tag_directives, o.implicit
        o.children.each { |c| accept c }
        @handler.end_document o.implicit_end
      end

      def visit_Psych_Nodes_Scalar o
        @handler.scalar o.value, o.anchor, o.tag, o.plain, o.quoted, o.style
      end

      def visit_Psych_Nodes_Sequence o
        @handler.start_sequence o.anchor, o.tag, o.implicit, o.style
        o.children.each { |c| accept c }
        @handler.end_sequence
      end

      def visit_Psych_Nodes_Mapping o
        @handler.start_mapping o.anchor, o.tag, o.implicit, o.style
        o.children.each { |c| accept c }
        @handler.end_mapping
      end

      def visit_Psych_Nodes_Alias o
        @handler.alias o.anchor
      end
    end
  end
end
