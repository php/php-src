require 'psych/helper'

module Psych
  class TestDocument < TestCase
    def setup
      super
      @stream = Psych.parse_stream(<<-eoyml)
%YAML 1.1
%TAG ! tag:tenderlovemaking.com,2009:
--- !fun
      eoyml
      @doc = @stream.children.first
    end

    def test_parse_tag
      assert_equal([['!', 'tag:tenderlovemaking.com,2009:']],
        @doc.tag_directives)
    end

    def test_emit_tag
      assert_match('%TAG ! tag:tenderlovemaking.com,2009:', @stream.yaml)
    end

    def test_emit_multitag
      @doc.tag_directives << ['!!', 'foo.com,2009:']
      yaml = @stream.yaml
      assert_match('%TAG ! tag:tenderlovemaking.com,2009:', yaml)
      assert_match('%TAG !! foo.com,2009:', yaml)
    end

    def test_emit_bad_tag
      assert_raises(RuntimeError) do
        @doc.tag_directives = [['!']]
        @stream.yaml
      end
    end

    def test_parse_version
      assert_equal([1,1], @doc.version)
    end

    def test_emit_version
      assert_match('%YAML 1.1', @stream.yaml)
    end
  end
end
