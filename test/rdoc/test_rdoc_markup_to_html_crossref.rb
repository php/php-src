require 'rubygems'
require 'minitest/autorun'
require 'rdoc/rdoc'
require 'rdoc/code_objects'
require 'rdoc/markup/to_html_crossref'
require File.expand_path '../xref_test_case', __FILE__

class TestRDocMarkupToHtmlCrossref < XrefTestCase

  def setup
    super

    @to = RDoc::Markup::ToHtmlCrossref.new 'index.html', @c1, true
  end

  def test_convert_CROSSREF
    result = @to.convert 'C1'

    assert_equal "\n<p><a href=\"C1.html\">C1</a></p>\n", result
  end

  def test_convert_HYPERLINK_rdoc_ref
    result = @to.convert 'rdoc-ref:C1'

    assert_equal "\n<p><a href=\"C1.html\">C1</a></p>\n", result
  end

  def test_convert_TIDYLINK_rdoc_ref
    result = @to.convert '{foo}[rdoc-ref:C1]'

    assert_equal "\n<p><a href=\"C1.html\">foo</a></p>\n", result
  end

  def test_gen_url
    assert_equal '<a href="C1.html">Some class</a>',
                 @to.gen_url('rdoc-ref:C1', 'Some class')

    assert_equal '<a href="http://example">HTTP example</a>',
                 @to.gen_url('http://example', 'HTTP example')
  end

  def test_handle_special_CROSSREF
    assert_equal "<a href=\"C2/C3.html\">C2::C3</a>", SPECIAL('C2::C3')
  end

  def test_handle_special_CROSSREF_show_hash_false
    @to.show_hash = false

    assert_equal "<a href=\"C1.html#method-i-m\">m</a>",
                 SPECIAL('#m')
  end

  def test_handle_special_HYPERLINK_rdoc
    RDoc::TopLevel.new 'README.txt'
    @to = RDoc::Markup::ToHtmlCrossref.new 'C2.html', @c2, true

    link = @to.handle_special_HYPERLINK hyper 'C2::C3'

    assert_equal '<a href="C2/C3.html">C2::C3</a>', link

    link = @to.handle_special_HYPERLINK hyper 'C4'

    assert_equal '<a href="C4.html">C4</a>', link

    link = @to.handle_special_HYPERLINK hyper 'README.txt'

    assert_equal '<a href="README_txt.html">README.txt</a>', link
  end

  def test_handle_special_TIDYLINK_rdoc
    RDoc::TopLevel.new 'README.txt'
    @to = RDoc::Markup::ToHtmlCrossref.new 'C2.html', @c2, true

    link = @to.handle_special_TIDYLINK tidy 'C2::C3'

    assert_equal '<a href="C2/C3.html">tidy</a>', link

    link = @to.handle_special_TIDYLINK tidy 'C4'

    assert_equal '<a href="C4.html">tidy</a>', link

    link = @to.handle_special_TIDYLINK tidy 'README.txt'

    assert_equal '<a href="README_txt.html">tidy</a>', link
  end

  def test_link
    assert_equal 'n', @to.link('n', 'n')

    assert_equal '<a href="C1.html#method-c-m">m</a>', @to.link('m', 'm')
  end

  def SPECIAL text
    @to.handle_special_CROSSREF special text
  end

  def hyper reference
    RDoc::Markup::Special.new 0, "rdoc-ref:#{reference}"
  end

  def special text
    RDoc::Markup::Special.new 0, text
  end

  def tidy reference
    RDoc::Markup::Special.new 0, "{tidy}[rdoc-ref:#{reference}]"
  end

end

