require 'rubygems'
require 'minitest/autorun'
require 'rdoc'
require 'rdoc/markup'
require 'rdoc/markup/formatter'

class TestRDocMarkupFormatter < MiniTest::Unit::TestCase

  class ToTest < RDoc::Markup::Formatter

    def initialize markup
      super

      add_tag :TT, '<tt>', '</tt>'
    end

    def accept_paragraph paragraph
      @res << attributes(paragraph.text)
    end

    def attributes text
      convert_flow @am.flow text.dup
    end

    def handle_special_CAPS special
      "handled #{special.text}"
    end

    def start_accepting
      @res = ""
    end

    def end_accepting
      @res
    end

  end

  def setup
    @markup = RDoc::Markup.new
    @markup.add_special(/[A-Z]+/, :CAPS)

    @to = ToTest.new @markup

    @caps    = RDoc::Markup::Attribute.bitmap_for :CAPS
    @special = RDoc::Markup::Attribute.bitmap_for :_SPECIAL_
    @tt      = RDoc::Markup::Attribute.bitmap_for :TT
  end

  def test_convert_tt_special
    converted = @to.convert '<tt>AAA</tt>'

    assert_equal '<tt>AAA</tt>', converted
  end

end

