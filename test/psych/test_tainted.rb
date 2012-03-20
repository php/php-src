require 'psych/helper'

module Psych
  class TestStringTainted < TestCase
    class Tainted < Handler
      attr_reader :tc

      def initialize tc
        @tc = tc
      end

      def start_document version, tags, implicit
        tags.flatten.each do |tag|
          assert_taintedness tag
        end
      end

      def alias name
        assert_taintedness name
      end

      def scalar value, anchor, tag, plain, quoted, style
        assert_taintedness value
        assert_taintedness tag if tag
        assert_taintedness anchor if anchor
      end

      def start_sequence anchor, tag, implicit, style
        assert_taintedness tag if tag
        assert_taintedness anchor if anchor
      end

      def start_mapping anchor, tag, implicit, style
        assert_taintedness tag if tag
        assert_taintedness anchor if anchor
      end

      def assert_taintedness thing, message = "'#{thing}' should be tainted"
        tc.assert thing.tainted?, message
      end
    end

    class Untainted < Tainted
      def assert_taintedness thing, message = "'#{thing}' should not be tainted"
        tc.assert !thing.tainted?, message
      end
    end


    def setup
      handler = Tainted.new self
      @parser = Psych::Parser.new handler
    end

    def test_tags_are_tainted
      assert_taintedness "%TAG !yaml! tag:yaml.org,2002:\n---\n!yaml!str \"foo\""
    end

    def test_alias
      assert_taintedness  "--- &ponies\n- foo\n- *ponies"
    end

    def test_scalar
      assert_taintedness "--- ponies"
    end

    def test_anchor
      assert_taintedness "--- &hi ponies"
    end

    def test_scalar_tag
      assert_taintedness "--- !str ponies"
    end

    def test_seq_start_tag
      assert_taintedness "--- !!seq [ a ]"
    end

    def test_seq_start_anchor
      assert_taintedness "--- &zomg [ a ]"
    end

    def test_seq_mapping_tag
      assert_taintedness "--- !!map { a: b }"
    end

    def test_seq_mapping_anchor
      assert_taintedness "--- &himom { a: b }"
    end

    def assert_taintedness string
      @parser.parse string.taint
    end
  end

  class TestStringUntainted < TestStringTainted
    def setup
      handler = Untainted.new self
      @parser = Psych::Parser.new handler
    end

    def assert_taintedness string
      @parser.parse string
    end
  end

  class TestStringIOUntainted < TestStringTainted
    def setup
      handler = Untainted.new self
      @parser = Psych::Parser.new handler
    end

    def assert_taintedness string
      @parser.parse StringIO.new(string)
    end
  end

  class TestIOTainted < TestStringTainted
    def assert_taintedness string
      t = Tempfile.new(['something', 'yml'])
      t.binmode
      t.write string
      t.close
      File.open(t.path, 'r:bom|utf-8') { |f|
        @parser.parse f
      }
      t.close(true)
    end
  end
end
