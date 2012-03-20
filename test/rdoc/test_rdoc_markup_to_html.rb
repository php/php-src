require 'rubygems'
require 'rdoc/markup/formatter_test_case'
require 'rdoc/markup/to_html'
require 'minitest/autorun'

class TestRDocMarkupToHtml < RDoc::Markup::FormatterTestCase

  add_visitor_tests

  def setup
    super

    @to = RDoc::Markup::ToHtml.new
  end

  def test_class_gen_relative_url
    def gen(from, to)
      RDoc::Markup::ToHtml.gen_relative_url from, to
    end

    assert_equal 'a.html',    gen('a.html',   'a.html')
    assert_equal 'b.html',    gen('a.html',   'b.html')

    assert_equal 'd.html',    gen('a/c.html', 'a/d.html')
    assert_equal '../a.html', gen('a/c.html', 'a.html')
    assert_equal 'a/c.html',  gen('a.html',   'a/c.html')
  end

  def accept_blank_line
    assert_empty @to.res.join
  end

  def accept_document
    assert_equal "\n<p>hello</p>\n", @to.res.join
  end

  def accept_heading
    assert_equal "\n<h5>Hello</h5>\n", @to.res.join
  end

  def accept_heading_1
    assert_equal "\n<h1>Hello</h1>\n", @to.res.join
  end

  def accept_heading_2
    assert_equal "\n<h2>Hello</h2>\n", @to.res.join
  end

  def accept_heading_3
    assert_equal "\n<h3>Hello</h3>\n", @to.res.join
  end

  def accept_heading_4
    assert_equal "\n<h4>Hello</h4>\n", @to.res.join
  end

  def accept_heading_b
    assert_equal "\n<h1><b>Hello</b></h1>\n", @to.res.join
  end

  def accept_heading_suppressed_crossref
    assert_equal "\n<h1>Hello</h1>\n", @to.res.join
  end

  def accept_list_end_bullet
    assert_equal [], @to.list
    assert_equal [], @to.in_list_entry

    assert_equal "<ul></ul>\n", @to.res.join
  end

  def accept_list_end_label
    assert_equal [], @to.list
    assert_equal [], @to.in_list_entry

    assert_equal "<dl class=\"rdoc-list\"></dl>\n", @to.res.join
  end

  def accept_list_end_lalpha
    assert_equal [], @to.list
    assert_equal [], @to.in_list_entry

    assert_equal "<ol style=\"display: lower-alpha\"></ol>\n", @to.res.join
  end

  def accept_list_end_number
    assert_equal [], @to.list
    assert_equal [], @to.in_list_entry

    assert_equal "<ol></ol>\n", @to.res.join
  end

  def accept_list_end_note
    assert_equal [], @to.list
    assert_equal [], @to.in_list_entry

    assert_equal "<table class=\"rdoc-list\"></table>\n", @to.res.join
  end

  def accept_list_end_ualpha
    assert_equal [], @to.list
    assert_equal [], @to.in_list_entry

    assert_equal "<ol style=\"display: upper-alpha\"></ol>\n", @to.res.join
  end

  def accept_list_item_end_bullet
    assert_equal %w[</li>], @to.in_list_entry
  end

  def accept_list_item_end_label
    assert_equal %w[</dd>], @to.in_list_entry
  end

  def accept_list_item_end_lalpha
    assert_equal %w[</li>], @to.in_list_entry
  end

  def accept_list_item_end_note
    assert_equal %w[</td></tr>], @to.in_list_entry
  end

  def accept_list_item_end_number
    assert_equal %w[</li>], @to.in_list_entry
  end

  def accept_list_item_end_ualpha
    assert_equal %w[</li>], @to.in_list_entry
  end

  def accept_list_item_start_bullet
    assert_equal "<ul><li>", @to.res.join
  end

  def accept_list_item_start_label
    assert_equal "<dl class=\"rdoc-list\"><dt>cat</dt>\n<dd>", @to.res.join
  end

  def accept_list_item_start_lalpha
    assert_equal "<ol style=\"display: lower-alpha\"><li>", @to.res.join
  end

  def accept_list_item_start_note
    assert_equal "<table class=\"rdoc-list\"><tr><td class=\"rdoc-term\"><p>cat</p></td>\n<td>",
                 @to.res.join
  end

  def accept_list_item_start_note_2
    expected = <<-EXPECTED
<table class="rdoc-list"><tr><td class="rdoc-term"><p><tt>teletype</tt></p></td>
<td>
<p>teletype description</p>
</td></tr></table>
    EXPECTED

    assert_equal expected, @to.res.join
  end

  def accept_list_item_start_number
    assert_equal "<ol><li>", @to.res.join
  end

  def accept_list_item_start_ualpha
    assert_equal "<ol style=\"display: upper-alpha\"><li>", @to.res.join
  end

  def accept_list_start_bullet
    assert_equal [:BULLET], @to.list
    assert_equal [false], @to.in_list_entry

    assert_equal "<ul>", @to.res.join
  end

  def accept_list_start_label
    assert_equal [:LABEL], @to.list
    assert_equal [false], @to.in_list_entry

    assert_equal '<dl class="rdoc-list">', @to.res.join
  end

  def accept_list_start_lalpha
    assert_equal [:LALPHA], @to.list
    assert_equal [false], @to.in_list_entry

    assert_equal "<ol style=\"display: lower-alpha\">", @to.res.join
  end

  def accept_list_start_note
    assert_equal [:NOTE], @to.list
    assert_equal [false], @to.in_list_entry

    assert_equal "<table class=\"rdoc-list\">", @to.res.join
  end

  def accept_list_start_number
    assert_equal [:NUMBER], @to.list
    assert_equal [false], @to.in_list_entry

    assert_equal "<ol>", @to.res.join
  end

  def accept_list_start_ualpha
    assert_equal [:UALPHA], @to.list
    assert_equal [false], @to.in_list_entry

    assert_equal "<ol style=\"display: upper-alpha\">", @to.res.join
  end

  def accept_paragraph
    assert_equal "\n<p>hi</p>\n", @to.res.join
  end

  def accept_paragraph_b
    assert_equal "\n<p>reg <b>bold words</b> reg</p>\n", @to.res.join
  end

  def accept_paragraph_i
    assert_equal "\n<p>reg <em>italic words</em> reg</p>\n", @to.res.join
  end

  def accept_paragraph_plus
    assert_equal "\n<p>reg <tt>teletype</tt> reg</p>\n", @to.res.join
  end

  def accept_paragraph_star
    assert_equal "\n<p>reg <b>bold</b> reg</p>\n", @to.res.join
  end

  def accept_paragraph_underscore
    assert_equal "\n<p>reg <em>italic</em> reg</p>\n", @to.res.join
  end

  def accept_raw
    raw = <<-RAW.rstrip
<table>
<tr><th>Name<th>Count
<tr><td>a<td>1
<tr><td>b<td>2
</table>
    RAW

    assert_equal raw, @to.res.join
  end

  def accept_rule
    assert_equal "<hr style=\"height: 4px\">\n", @to.res.join
  end

  def accept_verbatim
    assert_equal "\n<pre>hi\n  world</pre>\n", @to.res.join
  end

  def end_accepting
    assert_equal 'hi', @to.end_accepting
  end

  def start_accepting
    assert_equal [], @to.res
    assert_equal [], @to.in_list_entry
    assert_equal [], @to.list
  end

  def list_nested
    expected = <<-EXPECTED
<ul><li>
<p>l1</p>
<ul><li>
<p>l1.1</p>
</li></ul>
</li><li>
<p>l2</p>
</li></ul>
    EXPECTED

    assert_equal expected, @to.res.join
  end

  def list_verbatim
    expected = <<-EXPECTED
<ul><li>
<p>list stuff</p>

<pre>* list
  with

  second

  1. indented
  2. numbered

  third

* second</pre>
</li></ul>
    EXPECTED

    assert_equal expected, @to.end_accepting
  end

  def test_convert_string
    assert_equal '&lt;&gt;', @to.convert_string('<>')
  end

  def test_gen_url
    assert_equal '<a href="example">example</a>',
                 @to.gen_url('link:example', 'example')
  end

  def test_gem_url_image_url
    assert_equal '<img src="http://example.com/image.png" />', @to.gen_url('http://example.com/image.png', 'ignored')
  end

  def test_gem_url_ssl_image_url
    assert_equal '<img src="https://example.com/image.png" />', @to.gen_url('https://example.com/image.png', 'ignored')
  end

  def test_handle_special_HYPERLINK_link
    special = RDoc::Markup::Special.new 0, 'link:README.txt'

    link = @to.handle_special_HYPERLINK special

    assert_equal '<a href="README.txt">README.txt</a>', link
  end

  def test_list_verbatim_2
    str = "* one\n    verb1\n    verb2\n* two\n"

    expected = <<-EXPECTED
<ul><li>
<p>one</p>

<pre>verb1
verb2</pre>
</li><li>
<p>two</p>
</li></ul>
    EXPECTED

    assert_equal expected, @m.convert(str, @to)
  end

  def test_to_html
    assert_equal "\n<p><tt>--</tt></p>\n", util_format("<tt>--</tt>")
  end

  def util_format text
    paragraph = RDoc::Markup::Paragraph.new text

    @to.start_accepting
    @to.accept_paragraph paragraph
    @to.end_accepting
  end

end

