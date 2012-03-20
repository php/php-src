require 'rdoc/markup/formatter_test_case'

##
# Test case for creating new plain-text RDoc::Markup formatters.  See also
# RDoc::Markup::FormatterTestCase
#
# See test_rdoc_markup_to_rdoc.rb for a complete example.
#
# Example:
#
#  class TestRDocMarkupToNewTextFormat < RDoc::Markup::TextFormatterTestCase
#
#    add_visitor_tests
#    add_text_tests
#
#    def setup
#      super
#
#      @to = RDoc::Markup::ToNewTextFormat.new
#    end
#
#    def accept_blank_line
#      assert_equal :junk, @to.res.join
#    end
#
#    # ...
#
#  end

class RDoc::Markup::TextFormatterTestCase < RDoc::Markup::FormatterTestCase

  ##
  # Adds test cases to the calling TestCase.

  def self.add_text_tests
    self.class_eval do

      ##
      # Test case that calls <tt>@to.accept_heading</tt>

      def test_accept_heading_indent
        @to.start_accepting
        @to.indent = 3
        @to.accept_heading @RM::Heading.new(1, 'Hello')

        accept_heading_indent
      end

      ##
      # Test case that calls <tt>@to.accept_rule</tt>

      def test_accept_rule_indent
        @to.start_accepting
        @to.indent = 3
        @to.accept_rule @RM::Rule.new(1)

        accept_rule_indent
      end

      ##
      # Test case that calls <tt>@to.accept_verbatim</tt>

      def test_accept_verbatim_indent
        @to.start_accepting
        @to.indent = 2
        @to.accept_verbatim @RM::Verbatim.new("hi\n", " world\n")

        accept_verbatim_indent
      end

      ##
      # Test case that calls <tt>@to.accept_verbatim</tt> with a big indent

      def test_accept_verbatim_big_indent
        @to.start_accepting
        @to.indent = 2
        @to.accept_verbatim @RM::Verbatim.new("hi\n", "world\n")

        accept_verbatim_big_indent
      end

      ##
      # Test case that calls <tt>@to.accept_paragraph</tt> with an indent

      def test_accept_paragraph_indent
        @to.start_accepting
        @to.indent = 3
        @to.accept_paragraph @RM::Paragraph.new(('words ' * 30).strip)

        accept_paragraph_indent
      end

      ##
      # Test case that calls <tt>@to.accept_paragraph</tt> with a long line

      def test_accept_paragraph_wrap
        @to.start_accepting
        @to.accept_paragraph @RM::Paragraph.new(('words ' * 30).strip)

        accept_paragraph_wrap
      end

      ##
      # Test case that calls <tt>@to.attributes</tt> with an escaped
      # cross-reference.  If this test doesn't pass something may be very
      # wrong.

      def test_attributes
        assert_equal 'Dog', @to.attributes("\\Dog")
      end

    end
  end

end

