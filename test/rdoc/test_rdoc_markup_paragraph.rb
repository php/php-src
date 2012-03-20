require 'pp'
require 'rubygems'
require 'minitest/autorun'
require 'rdoc/markup'

class TestRDocMarkupParagraph < MiniTest::Unit::TestCase

  def test_accept
    visitor = Object.new
    def visitor.accept_paragraph(obj) @obj = obj end
    def visitor.obj() @obj end

    paragraph = RDoc::Markup::Paragraph.new

    paragraph.accept visitor

    assert_equal paragraph, visitor.obj
  end

end

