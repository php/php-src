require 'tempfile'
require 'rubygems'
require 'minitest/autorun'
require 'rdoc/options'
require 'rdoc/parser'

class TestRDocParserSimple < MiniTest::Unit::TestCase

  def setup
    @tempfile = Tempfile.new self.class.name
    filename = @tempfile.path

    @top_level = RDoc::TopLevel.new filename
    @fn = filename
    @options = RDoc::Options.new
    @stats = RDoc::Stats.new 0

    RDoc::TopLevel.reset
  end

  def teardown
    @tempfile.close
  end

  def test_initialize_metadata
    parser = util_parser ":unhandled: \n"

    assert_includes @top_level.metadata, 'unhandled'

    assert_equal ":unhandled: \n", parser.content
  end

  def test_remove_coding_comment
    parser = util_parser <<-TEXT
# -*- mode: rdoc; coding: utf-8; fill-column: 74; -*-

Regular expressions (<i>regexp</i>s) are patterns which describe the
contents of a string.
    TEXT

    parser.scan

    expected = <<-TEXT.strip
Regular expressions (<i>regexp</i>s) are patterns which describe the
contents of a string.
    TEXT

    assert_equal expected, @top_level.comment
  end

  # RDoc stops processing comments if it finds a comment line CONTAINING
  # '<tt>#--</tt>'. This can be used to separate external from internal
  # comments, or to stop a comment being associated with a method,
  # class, or module. Commenting CAN be turned back on with
  # a line that STARTS '<tt>#++</tt>'.
  #
  # I've seen guys that comment their code like this:
  #   # This method....
  #   #-----------------
  #   def method
  #
  # => either we do it only in ruby code, or we require the leading #
  #    (to avoid conflict with rules).
  #
  #   TODO: require the leading #, to provide the feature in simple text files.
  #   Note: in ruby & C code, we require '#--' & '#++' or '*--' & '*++',
  #   to allow rules:
  #
  #   # this is a comment
  #   #---
  #   # private text
  #   #+++
  #   # this is a rule:
  #   # ---

  def test_remove_private_comments
    parser = util_parser ''
    text = "foo\n\n--\nbar\n++\n\nbaz\n"

    expected = "foo\n\n\n\nbaz\n"

    assert_equal expected, parser.remove_private_comments(text)
  end

  def test_remove_private_comments_star
    parser = util_parser ''

    text = "* foo\n* bar\n"
    expected = text.dup

    assert_equal expected, parser.remove_private_comments(text)
  end

  def util_parser(content)
    RDoc::Parser::Simple.new @top_level, @fn, content, @options, @stats
  end

end

