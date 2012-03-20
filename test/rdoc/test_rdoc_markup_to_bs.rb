require 'rubygems'
require 'rdoc/markup/text_formatter_test_case'
require 'rdoc/markup/to_bs'
require 'minitest/autorun'

class TestRDocMarkupToBs < RDoc::Markup::TextFormatterTestCase

  add_visitor_tests
  add_text_tests

  def setup
    super

    @to = RDoc::Markup::ToBs.new
  end

  def accept_blank_line
    assert_equal "\n", @to.res.join
  end

  def accept_document
    assert_equal "hello\n", @to.res.join
  end

  def accept_heading
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "===== H\bHe\bel\bll\blo\bo\n", @to.res.join
  end

  def accept_list_end_bullet
    assert_empty @to.list_index
    assert_empty @to.list_type
    assert_empty @to.list_width
  end

  def accept_list_end_label
    assert_empty @to.list_index
    assert_empty @to.list_type
    assert_empty @to.list_width
  end

  def accept_list_end_lalpha
    assert_empty @to.list_index
    assert_empty @to.list_type
    assert_empty @to.list_width
  end

  def accept_list_end_note
    assert_empty @to.list_index
    assert_empty @to.list_type
    assert_empty @to.list_width
  end

  def accept_list_end_number
    assert_empty @to.list_index
    assert_empty @to.list_type
    assert_empty @to.list_width
  end

  def accept_list_end_ualpha
    assert_empty @to.list_index
    assert_empty @to.list_type
    assert_empty @to.list_width
  end

  def accept_list_item_end_bullet
    assert_equal 0, @to.indent, 'indent'
  end

  def accept_list_item_end_label
    assert_equal "\n", @to.res.join
    assert_equal 0, @to.indent, 'indent'
  end

  def accept_list_item_end_lalpha
    assert_equal 0, @to.indent, 'indent'
    assert_equal 'b', @to.list_index.last
  end

  def accept_list_item_end_note
    assert_equal "\n", @to.res.join
    assert_equal 0, @to.indent, 'indent'
  end

  def accept_list_item_end_number
    assert_equal 0, @to.indent, 'indent'
    assert_equal 2, @to.list_index.last
  end

  def accept_list_item_end_ualpha
    assert_equal 0, @to.indent, 'indent'
    assert_equal 'B', @to.list_index.last
  end

  def accept_list_item_start_bullet
    assert_equal [''], @to.res
    assert_equal '* ', @to.prefix
  end

  def accept_list_item_start_label
    assert_equal [''], @to.res
    assert_equal "cat:\n  ", @to.prefix

    assert_equal 2, @to.indent
  end

  def accept_list_item_start_lalpha
    assert_equal [''], @to.res
    assert_equal 'a. ', @to.prefix

    assert_equal 'a', @to.list_index.last
    assert_equal 3, @to.indent
  end

  def accept_list_item_start_note
    assert_equal [''], @to.res
    assert_equal "cat:\n  ", @to.prefix

    assert_equal 2, @to.indent
  end

  def accept_list_item_start_number
    assert_equal [''], @to.res
    assert_equal '1. ', @to.prefix

    assert_equal 1, @to.list_index.last
    assert_equal 3, @to.indent
  end

  def accept_list_item_start_ualpha
    assert_equal [''], @to.res
    assert_equal 'A. ', @to.prefix

    assert_equal 'A', @to.list_index.last
    assert_equal 3, @to.indent
  end

  def accept_list_start_bullet
    assert_equal "",   @to.res.join
    assert_equal [nil],     @to.list_index
    assert_equal [:BULLET], @to.list_type
    assert_equal [1],       @to.list_width
  end

  def accept_list_start_label
    assert_equal "",  @to.res.join
    assert_equal [nil],    @to.list_index
    assert_equal [:LABEL], @to.list_type
    assert_equal [2],      @to.list_width
  end

  def accept_list_start_lalpha
    assert_equal "",   @to.res.join
    assert_equal ['a'],     @to.list_index
    assert_equal [:LALPHA], @to.list_type
    assert_equal [1],       @to.list_width
  end

  def accept_list_start_note
    assert_equal "", @to.res.join
    assert_equal [nil],   @to.list_index
    assert_equal [:NOTE], @to.list_type
    assert_equal [2],     @to.list_width
  end

  def accept_list_start_number
    assert_equal "",   @to.res.join
    assert_equal [1],       @to.list_index
    assert_equal [:NUMBER], @to.list_type
    assert_equal [1],       @to.list_width
  end

  def accept_list_start_ualpha
    assert_equal "",   @to.res.join
    assert_equal ['A'],     @to.list_index
    assert_equal [:UALPHA], @to.list_type
    assert_equal [1],       @to.list_width
  end

  def accept_paragraph
    assert_equal "hi\n", @to.res.join
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
    assert_equal "#{'-' * 78}\n", @to.res.join
  end

  def accept_verbatim
    assert_equal "  hi\n    world\n\n", @to.res.join
  end

  def end_accepting
    assert_equal "hi", @to.end_accepting
  end

  def start_accepting
    assert_equal 0, @to.indent
    assert_equal [''], @to.res
    assert_empty @to.list_index
    assert_empty @to.list_type
    assert_empty @to.list_width
  end

  def accept_heading_1
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "= H\bHe\bel\bll\blo\bo\n", @to.end_accepting
  end

  def accept_heading_2
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "== H\bHe\bel\bll\blo\bo\n", @to.end_accepting
  end

  def accept_heading_3
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "=== H\bHe\bel\bll\blo\bo\n", @to.end_accepting
  end

  def accept_heading_4
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "==== H\bHe\bel\bll\blo\bo\n", @to.end_accepting
  end

  def accept_heading_indent
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "   = H\bHe\bel\bll\blo\bo\n", @to.end_accepting
  end

  def accept_heading_b
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "= H\bHe\bel\bll\blo\bo\n", @to.end_accepting
  end

  def accept_heading_suppressed_crossref
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "= H\bHe\bel\bll\blo\bo\n", @to.end_accepting
  end

  def accept_list_item_start_note_2
    assert_equal "teletype:\n  teletype description\n\n", @to.res.join
  end

  def accept_paragraph_b
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "reg b\bbo\bol\bld\bd \b w\bwo\bor\brd\bds\bs reg\n",
                 @to.end_accepting
  end

  def accept_paragraph_i
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "reg _\bi_\bt_\ba_\bl_\bi_\bc_\b _\bw_\bo_\br_\bd_\bs reg\n",
                 @to.end_accepting
  end

  def accept_paragraph_indent
    expected = <<-EXPECTED
   words words words words words words words words words words words words
   words words words words words words words words words words words words
   words words words words words words
    EXPECTED

    assert_equal expected, @to.end_accepting
  end

  def accept_paragraph_plus
    assert_equal "reg teletype reg\n", @to.end_accepting
  end

  def accept_paragraph_star
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "reg b\bbo\bol\bld\bd reg\n", @to.end_accepting
  end

  def accept_paragraph_underscore
    skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars
    assert_equal "reg _\bi_\bt_\ba_\bl_\bi_\bc reg\n", @to.end_accepting
  end

  def accept_paragraph_wrap
    expected = <<-EXPECTED
words words words words words words words words words words words words words
words words words words words words words words words words words words words
words words words words
    EXPECTED

    assert_equal expected, @to.end_accepting
  end

  def accept_rule_indent
    assert_equal "   #{'-' * 75}\n", @to.end_accepting
  end

  def accept_verbatim_indent
    assert_equal "    hi\n     world\n\n", @to.end_accepting
  end

  def accept_verbatim_big_indent
    assert_equal "    hi\n    world\n\n", @to.end_accepting
  end

  def list_nested
    expected = <<-EXPECTED
* l1
  * l1.1
* l2
    EXPECTED

    assert_equal expected, @to.end_accepting
  end

  def list_verbatim
    expected = <<-EXPECTED # HACK overblown
* list stuff

    * list
      with

      second

      1. indented
      2. numbered

      third

    * second

    EXPECTED

    assert_equal expected, @to.end_accepting
  end

end

