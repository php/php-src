require 'rubygems'
require 'rdoc/markup/formatter_test_case'
require 'rdoc/markup/to_tt_only'
require 'minitest/autorun'

class TestRDocMarkupToTtOnly < RDoc::Markup::FormatterTestCase

  add_visitor_tests

  def setup
    super

    @to = RDoc::Markup::ToTtOnly.new
  end

  def accept_blank_line
    assert_empty @to.end_accepting
  end

  def accept_document
    assert_equal [], @to.res
  end

  def accept_heading
    assert_empty @to.end_accepting
  end

  def accept_list_end_bullet
    assert_empty @to.res
  end

  def accept_list_end_label
    assert_empty @to.res
  end

  def accept_list_end_lalpha
    assert_empty @to.res
  end

  def accept_list_end_note
    assert_empty @to.res
  end

  def accept_list_end_number
    assert_empty @to.res
  end

  def accept_list_end_ualpha
    assert_empty @to.res
  end

  def accept_list_item_end_bullet
    assert_empty @to.res
  end

  def accept_list_item_end_label
    assert_empty @to.res
  end

  def accept_list_item_end_lalpha
    assert_empty @to.res
  end

  def accept_list_item_end_note
    assert_empty @to.res
  end

  def accept_list_item_end_number
    assert_empty @to.res
  end

  def accept_list_item_end_ualpha
    assert_empty @to.res
  end

  def accept_list_item_start_bullet
    assert_empty @to.res
  end

  def accept_list_item_start_label
    assert_empty @to.res
  end

  def accept_list_item_start_lalpha
    assert_empty @to.res
  end

  def accept_list_item_start_note
    assert_empty @to.res
  end

  def accept_list_item_start_number
    assert_empty @to.res
  end

  def accept_list_item_start_ualpha
    assert_empty @to.res
  end

  def accept_list_start_bullet
    assert_empty @to.res
  end

  def accept_list_start_label
    assert_empty @to.res
  end

  def accept_list_start_lalpha
    assert_empty @to.res
  end

  def accept_list_start_note
    assert_empty @to.res
  end

  def accept_list_start_number
    assert_empty @to.res
  end

  def accept_list_start_ualpha
    assert_empty @to.res
  end

  def accept_paragraph
    assert_empty @to.end_accepting
  end

  def accept_raw
    assert_empty @to.end_accepting
  end

  def accept_rule
    assert_empty @to.end_accepting
  end

  def accept_verbatim
    assert_empty @to.end_accepting
  end

  def end_accepting
    assert_equal %w[hi], @to.end_accepting
  end

  def start_accepting
    assert_empty @to.end_accepting
  end

  def accept_heading_1
    assert_empty @to.end_accepting
  end

  def accept_heading_2
    assert_empty @to.end_accepting
  end

  def accept_heading_3
    assert_empty @to.end_accepting
  end

  def accept_heading_4
    assert_empty @to.end_accepting
  end

  def accept_heading_indent
    assert_empty @to.end_accepting
  end

  def accept_heading_b
    assert_empty @to.end_accepting
  end

  def accept_heading_suppressed_crossref
    assert_empty @to.end_accepting
  end

  def accept_list_item_start_note_2
    assert_equal [nil, 'teletype', nil], @to.res
  end

  def accept_paragraph_b
    assert_empty @to.end_accepting
  end

  def accept_paragraph_i
    assert_empty @to.end_accepting
  end

  def accept_paragraph_indent
    assert_empty @to.end_accepting
  end

  def accept_paragraph_plus
    assert_equal %w[teletype], @to.end_accepting
  end

  def accept_paragraph_star
    assert_empty @to.end_accepting
  end

  def accept_paragraph_underscore
    assert_empty @to.end_accepting
  end

  def accept_paragraph_wrap
    assert_empty @to.end_accepting
  end

  def accept_rule_indent
    assert_empty @to.end_accepting
  end

  def accept_verbatim_indent
    assert_empty @to.end_accepting
  end

  def accept_verbatim_big_indent
    assert_empty @to.end_accepting
  end

  def list_nested
    assert_empty @to.end_accepting
  end

  def list_verbatim
    assert_empty @to.end_accepting
  end

end

