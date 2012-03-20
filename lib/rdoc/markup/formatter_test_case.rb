require 'minitest/unit'
require 'rdoc/markup/formatter'

##
# Test case for creating new RDoc::Markup formatters.  See
# test/test_rdoc_markup_to_*.rb for examples.
#
# This test case adds a variety of tests to your subclass when
# #add_visitor_tests is called.  Most tests set up a scenario then call a
# method you will provide to perform the assertion on the output.
#
# Your subclass must instantiate a visitor and assign it to <tt>@to</tt>.
#
# For example, test_accept_blank_line sets up a RDoc::Markup::BlockLine then
# calls accept_blank_line on your visitor.  You are responsible for asserting
# that the output is correct.
#
# Example:
#
#  class TestRDocMarkupToNewFormat < RDoc::Markup::FormatterTestCase
#
#    add_visitor_tests
#
#    def setup
#      super
#
#      @to = RDoc::Markup::ToNewFormat.new
#    end
#
#    def accept_blank_line
#      assert_equal :junk, @to.res.join
#    end
#
#    # ...
#
#  end

class RDoc::Markup::FormatterTestCase < MiniTest::Unit::TestCase

  ##
  # Call #setup when inheriting from this test case.
  #
  # Provides the following instance variables:
  #
  # +@m+::           RDoc::Markup.new
  # +@RM+::          RDoc::Markup # to reduce typing
  # +@bullet_list+:: @RM::List.new :BULLET, # ...
  # +@label_list+::  @RM::List.new :LABEL, # ...
  # +@lalpha_list+:: @RM::List.new :LALPHA, # ...
  # +@note_list+::   @RM::List.new :NOTE, # ...
  # +@number_list+:: @RM::List.new :NUMBER, # ...
  # +@ualpha_list+:: @RM::List.new :UALPHA, # ...

  def setup
    super

    @m = RDoc::Markup.new
    @RM = RDoc::Markup

    @bullet_list = @RM::List.new(:BULLET,
      @RM::ListItem.new(nil, @RM::Paragraph.new('l1')),
      @RM::ListItem.new(nil, @RM::Paragraph.new('l2')))

    @label_list = @RM::List.new(:LABEL,
      @RM::ListItem.new('cat', @RM::Paragraph.new('cats are cool')),
      @RM::ListItem.new('dog', @RM::Paragraph.new('dogs are cool too')))

    @lalpha_list = @RM::List.new(:LALPHA,
      @RM::ListItem.new(nil, @RM::Paragraph.new('l1')),
      @RM::ListItem.new(nil, @RM::Paragraph.new('l2')))

    @note_list = @RM::List.new(:NOTE,
      @RM::ListItem.new('cat', @RM::Paragraph.new('cats are cool')),
      @RM::ListItem.new('dog', @RM::Paragraph.new('dogs are cool too')))

    @number_list = @RM::List.new(:NUMBER,
      @RM::ListItem.new(nil, @RM::Paragraph.new('l1')),
      @RM::ListItem.new(nil, @RM::Paragraph.new('l2')))

    @ualpha_list = @RM::List.new(:UALPHA,
      @RM::ListItem.new(nil, @RM::Paragraph.new('l1')),
      @RM::ListItem.new(nil, @RM::Paragraph.new('l2')))
  end

  ##
  # Call to add the visitor tests to your test case

  def self.add_visitor_tests
    self.class_eval do

      ##
      # Calls start_accepting which needs to verify startup state

      def test_start_accepting
        @to.start_accepting

        start_accepting
      end

      ##
      # Calls end_accepting on your test case which needs to call
      # <tt>@to.end_accepting</tt> and verify document generation

      def test_end_accepting
        @to.start_accepting
        @to.res << 'hi'

        end_accepting
      end

      ##
      # Calls accept_blank_line

      def test_accept_blank_line
        @to.start_accepting

        @to.accept_blank_line @RM::BlankLine.new

        accept_blank_line
      end

      ##
      # Test case that calls <tt>@to.accept_document</tt>

      def test_accept_document
        @to.start_accepting
        @to.accept_document @RM::Document.new @RM::Paragraph.new 'hello'

        accept_document
      end

      ##
      # Calls accept_heading with a level 5 RDoc::Markup::Heading

      def test_accept_heading
        @to.start_accepting

        @to.accept_heading @RM::Heading.new(5, 'Hello')

        accept_heading
      end

      ##
      # Calls accept_heading_1 with a level 1 RDoc::Markup::Heading

      def test_accept_heading_1
        @to.start_accepting

        @to.accept_heading @RM::Heading.new(1, 'Hello')

        accept_heading_1
      end

      ##
      # Calls accept_heading_2 with a level 2 RDoc::Markup::Heading

      def test_accept_heading_2
        @to.start_accepting

        @to.accept_heading @RM::Heading.new(2, 'Hello')

        accept_heading_2
      end

      ##
      # Calls accept_heading_3 with a level 3 RDoc::Markup::Heading

      def test_accept_heading_3
        # HACK this doesn't belong here
        skip "No String#chars, upgrade your ruby" unless ''.respond_to? :chars

        @to.start_accepting

        @to.accept_heading @RM::Heading.new(3, 'Hello')

        accept_heading_3
      end

      ##
      # Calls accept_heading_4 with a level 4 RDoc::Markup::Heading

      def test_accept_heading_4
        @to.start_accepting

        @to.accept_heading @RM::Heading.new(4, 'Hello')

        accept_heading_4
      end

      ##
      # Calls accept_heading_b with a bold level 1 RDoc::Markup::Heading

      def test_accept_heading_b
        @to.start_accepting

        @to.accept_heading @RM::Heading.new(1, '*Hello*')

        accept_heading_b
      end

      ##
      # Calls accept_heading_suppressed_crossref with a level 1
      # RDoc::Markup::Heading containing a suppressed crossref

      def test_accept_heading_suppressed_crossref # HACK to_html_crossref test
        @to.start_accepting

        @to.accept_heading @RM::Heading.new(1, '\\Hello')

        accept_heading_suppressed_crossref
      end

      ##
      # Calls accept_paragraph

      def test_accept_paragraph
        @to.start_accepting

        @to.accept_paragraph @RM::Paragraph.new('hi')

        accept_paragraph
      end

      ##
      # Calls accept_paragraph_b with a RDoc::Markup::Paragraph containing
      # bold words

      def test_accept_paragraph_b
        @to.start_accepting

        @to.accept_paragraph @RM::Paragraph.new('reg <b>bold words</b> reg')

        accept_paragraph_b
      end

      ##
      # Calls accept_paragraph_i with a RDoc::Markup::Paragraph containing
      # emphasized words

      def test_accept_paragraph_i
        @to.start_accepting

        @to.accept_paragraph @RM::Paragraph.new('reg <em>italic words</em> reg')

        accept_paragraph_i
      end

      ##
      # Calls accept_paragraph_plus with a RDoc::Markup::Paragraph containing
      # teletype words

      def test_accept_paragraph_plus
        @to.start_accepting

        @to.accept_paragraph @RM::Paragraph.new('reg +teletype+ reg')

        accept_paragraph_plus
      end

      ##
      # Calls accept_paragraph_star with a RDoc::Markup::Paragraph containing
      # bold words

      def test_accept_paragraph_star
        @to.start_accepting

        @to.accept_paragraph @RM::Paragraph.new('reg *bold* reg')

        accept_paragraph_star
      end

      ##
      # Calls accept_paragraph_underscore with a RDoc::Markup::Paragraph
      # containing emphasized words

      def test_accept_paragraph_underscore
        @to.start_accepting

        @to.accept_paragraph @RM::Paragraph.new('reg _italic_ reg')

        accept_paragraph_underscore
      end

      ##
      # Calls accept_verbatim with a RDoc::Markup::Verbatim

      def test_accept_verbatim
        @to.start_accepting

        @to.accept_verbatim @RM::Verbatim.new("hi\n", "  world\n")

        accept_verbatim
      end

      ##
      # Calls accept_raw with a RDoc::Markup::Raw

      def test_accept_raw
        @to.start_accepting

        @to.accept_raw @RM::Raw.new("<table>",
                                    "<tr><th>Name<th>Count",
                                    "<tr><td>a<td>1",
                                    "<tr><td>b<td>2",
                                    "</table>")

        accept_raw
      end

      ##
      # Calls accept_rule with a RDoc::Markup::Rule

      def test_accept_rule
        @to.start_accepting

        @to.accept_rule @RM::Rule.new(4)

        accept_rule
      end

      ##
      # Calls accept_list_item_start_bullet

      def test_accept_list_item_start_bullet
        @to.start_accepting

        @to.accept_list_start @bullet_list

        @to.accept_list_item_start @bullet_list.items.first

        accept_list_item_start_bullet
      end

      ##
      # Calls accept_list_item_start_label

      def test_accept_list_item_start_label
        @to.start_accepting

        @to.accept_list_start @label_list

        @to.accept_list_item_start @label_list.items.first

        accept_list_item_start_label
      end

      ##
      # Calls accept_list_item_start_lalpha

      def test_accept_list_item_start_lalpha
        @to.start_accepting

        @to.accept_list_start @lalpha_list

        @to.accept_list_item_start @lalpha_list.items.first

        accept_list_item_start_lalpha
      end

      ##
      # Calls accept_list_item_start_note

      def test_accept_list_item_start_note
        @to.start_accepting

        @to.accept_list_start @note_list

        @to.accept_list_item_start @note_list.items.first

        accept_list_item_start_note
      end

      ##
      # Calls accept_list_item_start_note_2

      def test_accept_list_item_start_note_2
        list = @RM::List.new(:NOTE,
                 @RM::ListItem.new('<tt>teletype</tt>',
                   @RM::Paragraph.new('teletype description')))

        @to.start_accepting

        list.accept @to

        @to.end_accepting

        accept_list_item_start_note_2
      end

      ##
      # Calls accept_list_item_start_number

      def test_accept_list_item_start_number
        @to.start_accepting

        @to.accept_list_start @number_list

        @to.accept_list_item_start @number_list.items.first

        accept_list_item_start_number
      end

      ##
      # Calls accept_list_item_start_ualpha

      def test_accept_list_item_start_ualpha
        @to.start_accepting

        @to.accept_list_start @ualpha_list

        @to.accept_list_item_start @ualpha_list.items.first

        accept_list_item_start_ualpha
      end

      ##
      # Calls accept_list_item_end_bullet

      def test_accept_list_item_end_bullet
        @to.start_accepting

        @to.accept_list_start @bullet_list

        @to.accept_list_item_start @bullet_list.items.first

        @to.accept_list_item_end @bullet_list.items.first

        accept_list_item_end_bullet
      end

      ##
      # Calls accept_list_item_end_label

      def test_accept_list_item_end_label
        @to.start_accepting

        @to.accept_list_start @label_list

        @to.accept_list_item_start @label_list.items.first

        @to.accept_list_item_end @label_list.items.first

        accept_list_item_end_label
      end

      ##
      # Calls accept_list_item_end_lalpha

      def test_accept_list_item_end_lalpha
        @to.start_accepting

        @to.accept_list_start @lalpha_list

        @to.accept_list_item_start @lalpha_list.items.first

        @to.accept_list_item_end @lalpha_list.items.first

        accept_list_item_end_lalpha
      end

      ##
      # Calls accept_list_item_end_note

      def test_accept_list_item_end_note
        @to.start_accepting

        @to.accept_list_start @note_list

        @to.accept_list_item_start @note_list.items.first

        @to.accept_list_item_end @note_list.items.first

        accept_list_item_end_note
      end

      ##
      # Calls accept_list_item_end_number

      def test_accept_list_item_end_number
        @to.start_accepting

        @to.accept_list_start @number_list

        @to.accept_list_item_start @number_list.items.first

        @to.accept_list_item_end @number_list.items.first

        accept_list_item_end_number
      end

      ##
      # Calls accept_list_item_end_ualpha

      def test_accept_list_item_end_ualpha
        @to.start_accepting

        @to.accept_list_start @ualpha_list

        @to.accept_list_item_start @ualpha_list.items.first

        @to.accept_list_item_end @ualpha_list.items.first

        accept_list_item_end_ualpha
      end

      ##
      # Calls accept_list_start_bullet

      def test_accept_list_start_bullet
        @to.start_accepting

        @to.accept_list_start @bullet_list

        accept_list_start_bullet
      end

      ##
      # Calls accept_list_start_label

      def test_accept_list_start_label
        @to.start_accepting

        @to.accept_list_start @label_list

        accept_list_start_label
      end

      ##
      # Calls accept_list_start_lalpha

      def test_accept_list_start_lalpha
        @to.start_accepting

        @to.accept_list_start @lalpha_list

        accept_list_start_lalpha
      end

      ##
      # Calls accept_list_start_note

      def test_accept_list_start_note
        @to.start_accepting

        @to.accept_list_start @note_list

        accept_list_start_note
      end

      ##
      # Calls accept_list_start_number

      def test_accept_list_start_number
        @to.start_accepting

        @to.accept_list_start @number_list

        accept_list_start_number
      end

      ##
      # Calls accept_list_start_ualpha

      def test_accept_list_start_ualpha
        @to.start_accepting

        @to.accept_list_start @ualpha_list

        accept_list_start_ualpha
      end

      ##
      # Calls accept_list_end_bullet

      def test_accept_list_end_bullet
        @to.start_accepting

        @to.accept_list_start @bullet_list

        @to.accept_list_end @bullet_list

        accept_list_end_bullet
      end

      ##
      # Calls accept_list_end_label

      def test_accept_list_end_label
        @to.start_accepting

        @to.accept_list_start @label_list

        @to.accept_list_end @label_list

        accept_list_end_label
      end

      ##
      # Calls accept_list_end_lalpha

      def test_accept_list_end_lalpha
        @to.start_accepting

        @to.accept_list_start @lalpha_list

        @to.accept_list_end @lalpha_list

        accept_list_end_lalpha
      end

      ##
      # Calls accept_list_end_number

      def test_accept_list_end_number
        @to.start_accepting

        @to.accept_list_start @number_list

        @to.accept_list_end @number_list

        accept_list_end_number
      end

      ##
      # Calls accept_list_end_note

      def test_accept_list_end_note
        @to.start_accepting

        @to.accept_list_start @note_list

        @to.accept_list_end @note_list

        accept_list_end_note
      end

      ##
      # Calls accept_list_end_ulpha

      def test_accept_list_end_ualpha
        @to.start_accepting

        @to.accept_list_start @ualpha_list

        @to.accept_list_end @ualpha_list

        accept_list_end_ualpha
      end

      ##
      # Calls list_nested with a two-level list

      def test_list_nested
        doc = @RM::Document.new(
                @RM::List.new(:BULLET,
                  @RM::ListItem.new(nil,
                    @RM::Paragraph.new('l1'),
                    @RM::List.new(:BULLET,
                      @RM::ListItem.new(nil,
                        @RM::Paragraph.new('l1.1')))),
                  @RM::ListItem.new(nil,
                    @RM::Paragraph.new('l2'))))

        doc.accept @to

        list_nested
      end

      ##
      # Calls list_verbatim with a list containing a verbatim block

      def test_list_verbatim # HACK overblown
        doc = @RM::Document.new(
                @RM::List.new(:BULLET,
                  @RM::ListItem.new(nil,
                    @RM::Paragraph.new('list', 'stuff'),
                    @RM::BlankLine.new,
                    @RM::Verbatim.new("* list\n",
                                      "  with\n",
                                      "\n",
                                      "  second\n",
                                      "\n",
                                      "  1. indented\n",
                                      "  2. numbered\n",
                                      "\n",
                                      "  third\n",
                                      "\n",
                                      "* second\n"))))

        doc.accept @to

        list_verbatim
      end

    end
  end

end

