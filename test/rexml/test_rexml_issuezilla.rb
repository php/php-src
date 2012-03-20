require 'rexml_test_utils'
require 'rexml/document'

class TestIssuezillaParsing < Test::Unit::TestCase
  include REXMLTestUtils
  def test_rexml
    doc = REXML::Document.new(File.new(fixture_path("ofbiz-issues-full-177.xml")))
    ctr = 1
    doc.root.each_element('//issue') do |issue|
      assert_equal( ctr, issue.elements['issue_id'].text.to_i )
      ctr += 1
    end
  end
end
