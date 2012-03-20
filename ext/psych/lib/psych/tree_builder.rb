require 'psych/handler'

module Psych
  ###
  # This class works in conjunction with Psych::Parser to build an in-memory
  # parse tree that represents a YAML document.
  #
  # == Example
  #
  #   parser = Psych::Parser.new Psych::TreeBuilder.new
  #   parser.parse('--- foo')
  #   tree = parser.handler.root
  #
  # See Psych::Handler for documentation on the event methods used in this
  # class.
  class TreeBuilder < Psych::Handler
    # Returns the root node for the built tree
    attr_reader :root

    # Create a new TreeBuilder instance
    def initialize
      @stack = []
      @last  = nil
      @root  = nil
    end

    %w{
      Sequence
      Mapping
    }.each do |node|
      class_eval %{
        def start_#{node.downcase}(anchor, tag, implicit, style)
          n = Nodes::#{node}.new(anchor, tag, implicit, style)
          @last.children << n
          push n
        end

        def end_#{node.downcase}
          pop
        end
      }
    end

    ###
    # Handles start_document events with +version+, +tag_directives+,
    # and +implicit+ styling.
    #
    # See Psych::Handler#start_document
    def start_document version, tag_directives, implicit
      n = Nodes::Document.new version, tag_directives, implicit
      @last.children << n
      push n
    end

    ###
    # Handles end_document events with +version+, +tag_directives+,
    # and +implicit+ styling.
    #
    # See Psych::Handler#start_document
    def end_document implicit_end = !streaming?
      @last.implicit_end = implicit_end
      pop
    end

    def start_stream encoding
      @root = Nodes::Stream.new(encoding)
      push @root
    end

    def end_stream
      pop
    end

    def scalar value, anchor, tag, plain, quoted, style
      s = Nodes::Scalar.new(value,anchor,tag,plain,quoted,style)
      @last.children << s
      s
    end

    def alias anchor
      @last.children << Nodes::Alias.new(anchor)
    end

    private
    def push value
      @stack.push value
      @last = value
    end

    def pop
      x = @stack.pop
      @last = @stack.last
      x
    end
  end
end
