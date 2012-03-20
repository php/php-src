require 'stringio'

module Psych
  module Nodes
    ###
    # The base class for any Node in a YAML parse tree.  This class should
    # never be instantiated.
    class Node
      include Enumerable

      # The children of this node
      attr_reader :children

      # An associated tag
      attr_reader :tag

      # Create a new Psych::Nodes::Node
      def initialize
        @children = []
      end

      ###
      # Iterate over each node in the tree. Yields each node to +block+ depth
      # first.
      def each &block
        return enum_for :each unless block_given?
        Visitors::DepthFirst.new(block).accept self
      end

      ###
      # Convert this node to Ruby.
      #
      # See also Psych::Visitors::ToRuby
      def to_ruby
        Visitors::ToRuby.new.accept self
      end
      alias :transform :to_ruby

      ###
      # Convert this node to YAML.
      #
      # See also Psych::Visitors::Emitter
      def yaml io = nil, options = {}
        real_io = io || StringIO.new(''.encode('utf-8'))

        Visitors::Emitter.new(real_io, options).accept self
        return real_io.string unless io
        io
      end
      alias :to_yaml :yaml
    end
  end
end
