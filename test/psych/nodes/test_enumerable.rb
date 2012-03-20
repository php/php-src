require 'psych/helper'

module Psych
  module Nodes
    class TestEnumerable < TestCase
      def test_includes_enumerable
        yaml = '--- hello'
        assert_equal 3, Psych.parse_stream(yaml).to_a.length
      end

      def test_returns_enumerator
        yaml = '--- hello'
        assert_equal 3, Psych.parse_stream(yaml).each.map { |x| x }.length
      end

      def test_scalar
        assert_equal 3, calls('--- hello').length
      end

      def test_sequence
        assert_equal 4, calls("---\n- hello").length
      end

      def test_mapping
        assert_equal 5, calls("---\nhello: world").length
      end

      def test_alias
        assert_equal 5, calls("--- &yay\n- foo\n- *yay\n").length
      end

      private

      def calls yaml
        calls = []
        Psych.parse_stream(yaml).each do |node|
          calls << node
        end
        calls
      end
    end
  end
end
