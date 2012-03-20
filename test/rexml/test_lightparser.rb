require 'rexml_test_utils'
require 'rexml/parsers/lightparser'

class LightParserTester < Test::Unit::TestCase
  include REXMLTestUtils
  include REXML
  def test_parsing
    f = File.new(fixture_path("documentation.xml"))
    parser = REXML::Parsers::LightParser.new( f )
    root = parser.parse
  end
end
