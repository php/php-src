require 'psych/helper'

module Psych
  module Visitors
    class TestDepthFirst < TestCase
      class Collector < Struct.new(:calls)
        def initialize(calls = [])
          super
        end

        def call obj
          calls << obj
        end
      end

      def test_scalar
        collector = Collector.new
        visitor   = Visitors::DepthFirst.new collector
        visitor.accept Psych.parse_stream '--- hello'

        assert_equal 3, collector.calls.length
      end

      def test_sequence
        collector = Collector.new
        visitor   = Visitors::DepthFirst.new collector
        visitor.accept Psych.parse_stream "---\n- hello"

        assert_equal 4, collector.calls.length
      end

      def test_mapping
        collector = Collector.new
        visitor   = Visitors::DepthFirst.new collector
        visitor.accept Psych.parse_stream "---\nhello: world"

        assert_equal 5, collector.calls.length
      end

      def test_alias
        collector = Collector.new
        visitor   = Visitors::DepthFirst.new collector
        visitor.accept Psych.parse_stream "--- &yay\n- foo\n- *yay\n"

        assert_equal 5, collector.calls.length
      end
    end
  end
end
