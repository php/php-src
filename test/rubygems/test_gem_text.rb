require 'rubygems/test_case'
require "rubygems/text"

class TestGemText < Gem::TestCase
  include Gem::Text

  def test_format_text
    assert_equal "text to\nwrap",     format_text("text to wrap", 8)
  end

  def test_format_text_indent
    assert_equal "  text to\n  wrap", format_text("text to wrap", 8, 2)
  end

  def test_format_text_none
    assert_equal "text to wrap",      format_text("text to wrap", 40)
  end

  def test_format_text_none_indent
    assert_equal "  text to wrap",    format_text("text to wrap", 40, 2)
  end

  def test_format_text_trailing # for two spaces after .
    text = <<-TEXT
This line is really, really long.  So long, in fact, that it is more than eighty characters long!  The purpose of this line is for testing wrapping behavior because sometimes people don't wrap their text to eighty characters.  Without the wrapping, the text might not look good in the RSS feed.
    TEXT

    expected = <<-EXPECTED
This line is really, really long.  So long, in fact, that it is more than
eighty characters long!  The purpose of this line is for testing wrapping
behavior because sometimes people don't wrap their text to eighty characters.
Without the wrapping, the text might not look good in the RSS feed.
    EXPECTED

    assert_equal expected, format_text(text, 78)
  end

  def test_levenshtein_distance_add
    assert_equal 2, levenshtein_distance("zentest", "zntst")
    assert_equal 2, levenshtein_distance("zntst", "zentest")
  end

  def test_levenshtein_distance_empty
    assert_equal 5, levenshtein_distance("abcde", "")
    assert_equal 5, levenshtein_distance("", "abcde")
  end

  def test_levenshtein_distance_remove
    assert_equal 3, levenshtein_distance("zentest", "zentestxxx")
    assert_equal 3, levenshtein_distance("zentestxxx", "zentest")
  end

  def test_levenshtein_distance_replace
    assert_equal 2, levenshtein_distance("zentest", "ZenTest")
    assert_equal 7, levenshtein_distance("xxxxxxx", "ZenTest")
    assert_equal 7, levenshtein_distance("zentest", "xxxxxxx")
  end
end
