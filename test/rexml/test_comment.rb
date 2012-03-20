require "test/unit/testcase"

require 'rexml/document'

module REXMLTest
  class CommentTester < Test::Unit::TestCase
    # Bug #5278
    def test_hyphen_end_line_in_doctype
      xml = <<-XML
<?xml version="1.0"?>
<!DOCTYPE root [
<!-- comment end with hyphen -
     here -->
]>
<root/>
      XML
      document = REXML::Document.new(xml)
      comments = document.doctype.children.find_all do |child|
        child.is_a?(REXML::Comment)
      end
      assert_equal([" comment end with hyphen -\n     here "],
                   comments.collect(&:to_s))
    end
  end
end
