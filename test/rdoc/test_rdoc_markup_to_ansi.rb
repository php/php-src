require 'rubygems'
require 'rdoc/markup/text_formatter_test_case'
require 'rdoc/markup/to_ansi'
require 'minitest/autorun'

class TestRDocMarkupToAnsi < RDoc::Markup::TextFormatterTestCase

  add_visitor_tests
  add_text_tests

  def setup
    super

    @to = RDoc::Markup::ToAnsi.new
  end

  def accept_blank_line
    assert_equal "\e[0m\n", @to.res.join
  end

  def accept_document
    assert_equal "\e[0mhello\n", @to.res.join
  end

  def accept_heading
    assert_equal "\e[0mHello\n", @to.res.join
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
    assert_equal "\e[0m", @to.res.join
    assert_equal 0, @to.indent, 'indent'
  end

  def accept_list_item_end_lalpha
    assert_equal 0, @to.indent, 'indent'
    assert_equal 'b', @to.list_index.last
  end

  def accept_list_item_end_note
    assert_equal "\e[0m", @to.res.join
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
    assert_equal %W"\e[0m", @to.res
    assert_equal '* ', @to.prefix
  end

  def accept_list_item_start_label
    assert_equal %W"\e[0m", @to.res
    assert_equal "cat:\n  ", @to.prefix

    assert_equal 2, @to.indent
  end

  def accept_list_item_start_lalpha
    assert_equal %W"\e[0m", @to.res
    assert_equal 'a. ', @to.prefix

    assert_equal 'a', @to.list_index.last
    assert_equal 3, @to.indent
  end

  def accept_list_item_start_note
    assert_equal %W"\e[0m", @to.res
    assert_equal "cat:\n  ", @to.prefix

    assert_equal 2, @to.indent
  end

  def accept_list_item_start_number
    assert_equal %W"\e[0m", @to.res
    assert_equal '1. ', @to.prefix

    assert_equal 1, @to.list_index.last
    assert_equal 3, @to.indent
  end

  def accept_list_item_start_ualpha
    assert_equal %W"\e[0m", @to.res
    assert_equal 'A. ', @to.prefix

    assert_equal 'A', @to.list_index.last
    assert_equal 3, @to.indent
  end

  def accept_list_start_bullet
    assert_equal "\e[0m",   @to.res.join
    assert_equal [nil],     @to.list_index
    assert_equal [:BULLET], @to.list_type
    assert_equal [1],       @to.list_width
  end

  def accept_list_start_label
    assert_equal "\e[0m",  @to.res.join
    assert_equal [nil],    @to.list_index
    assert_equal [:LABEL], @to.list_type
    assert_equal [2],      @to.list_width
  end

  def accept_list_start_lalpha
    assert_equal "\e[0m",   @to.res.join
    assert_equal ['a'],     @to.list_index
    assert_equal [:LALPHA], @to.list_type
    assert_equal [1],       @to.list_width
  end

  def accept_list_start_note
    assert_equal "\e[0m", @to.res.join
    assert_equal [nil],   @to.list_index
    assert_equal [:NOTE], @to.list_type
    assert_equal [2],     @to.list_width
  end

  def accept_list_start_number
    assert_equal "\e[0m",   @to.res.join
    assert_equal [1],       @to.list_index
    assert_equal [:NUMBER], @to.list_type
    assert_equal [1],       @to.list_width
  end

  def accept_list_start_ualpha
    assert_equal "\e[0m",   @to.res.join
    assert_equal ['A'],     @to.list_index
    assert_equal [:UALPHA], @to.list_type
    assert_equal [1],       @to.list_width
  end

  def accept_paragraph
    assert_equal "\e[0mhi\n", @to.res.join
  end

  def accept_raw
    raw = <<-RAW.rstrip
\e[0m<table>
<tr><th>Name<th>Count
<tr><td>a<td>1
<tr><td>b<td>2
</table>
    RAW

    assert_equal raw, @to.res.join
  end

  def accept_rule
    assert_equal "\e[0m#{'-' * 78}\n", @to.res.join
  end

  def accept_verbatim
    assert_equal "\e[0m  hi\n    world\n\n", @to.res.join
  end

  def end_accepting
    assert_equal "\e[0mhi", @to.end_accepting
  end

  def start_accepting
    assert_equal 0, @to.indent
    assert_equal %W"\e[0m", @to.res
    assert_empty @to.list_index
    assert_empty @to.list_type
    assert_empty @to.list_width
  end

  def accept_heading_1
    assert_equal "\e[0m\e[1;32mHello\e[m\n", @to.end_accepting
  end

  def accept_heading_2
    assert_equal "\e[0m\e[4;32mHello\e[m\n", @to.end_accepting
  end

  def accept_heading_3
    assert_equal "\e[0m\e[32mHello\e[m\n", @to.end_accepting
  end

  def accept_heading_4
    assert_equal "\e[0mHello\n", @to.end_accepting
  end

  def accept_heading_indent
    assert_equal "\e[0m   \e[1;32mHello\e[m\n", @to.end_accepting
  end

  def accept_heading_b
    assert_equal "\e[0m\e[1;32m\e[1mHello\e[m\e[m\n", @to.end_accepting
  end

  def accept_heading_suppressed_crossref
    assert_equal "\e[0m\e[1;32mHello\e[m\n", @to.end_accepting
  end

  def accept_list_item_start_note_2
    assert_equal "\e[0m\e[7mteletype\e[m:\n  teletype description\n\n",
                 @to.res.join
  end

  def accept_paragraph_b
    assert_equal "\e[0mreg \e[1mbold words\e[m reg\n", @to.end_accepting
  end

  def accept_paragraph_i
    assert_equal "\e[0mreg \e[4mitalic words\e[m reg\n", @to.end_accepting
  end

  def accept_paragraph_indent
    expected = <<-EXPECTED
\e[0m   words words words words words words words words words words words words
   words words words words words words words words words words words words
   words words words words words words
    EXPECTED

    assert_equal expected, @to.end_accepting
  end

  def accept_paragraph_plus
    assert_equal "\e[0mreg \e[7mteletype\e[m reg\n", @to.end_accepting
  end

  def accept_paragraph_star
    assert_equal "\e[0mreg \e[1mbold\e[m reg\n", @to.end_accepting
  end

  def accept_paragraph_underscore
    assert_equal "\e[0mreg \e[4mitalic\e[m reg\n", @to.end_accepting
  end

  def accept_paragraph_wrap
    expected = <<-EXPECTED
\e[0mwords words words words words words words words words words words words words
words words words words words words words words words words words words words
words words words words
    EXPECTED

    assert_equal expected, @to.end_accepting
  end

  def accept_rule_indent
    assert_equal "\e[0m   #{'-' * 75}\n", @to.end_accepting
  end

  def accept_verbatim_indent
    assert_equal "\e[0m    hi\n     world\n\n", @to.end_accepting
  end

  def accept_verbatim_big_indent
    assert_equal "\e[0m    hi\n    world\n\n", @to.end_accepting
  end

  def list_nested
    expected = <<-EXPECTED
\e[0m* l1
  * l1.1
* l2
    EXPECTED

    assert_equal expected, @to.end_accepting
  end

  def list_verbatim
    expected = <<-EXPECTED # HACK overblown
\e[0m* list stuff

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

