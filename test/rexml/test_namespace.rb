require "rexml_test_utils"

require "rexml/document"

class TestNamespace < Test::Unit::TestCase
  include REXMLTestUtils
  include REXML

  def setup
    @xsa_source = <<-EOL
      <?xml version="1.0"?>
      <?xsl stylesheet="blah.xsl"?>
      <!-- The first line tests the XMLDecl, the second tests PI.
      The next line tests DocType. This line tests comments. -->
      <!DOCTYPE xsa PUBLIC
        "-//LM Garshol//DTD XML Software Autoupdate 1.0//EN//XML"
        "http://www.garshol.priv.no/download/xsa/xsa.dtd">

      <xsa>
        <vendor id="blah">
          <name>Lars Marius Garshol</name>
          <email>larsga@garshol.priv.no</email>
          <url>http://www.stud.ifi.uio.no/~lmariusg/</url>
        </vendor>
      </xsa>
    EOL
  end

  def test_xml_namespace
    xml = <<-XML
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:xml="http://www.w3.org/XML/1998/namespace" />
XML
    document = Document.new(xml)
    assert_equal("http://www.w3.org/XML/1998/namespace",
                 document.root.namespace("xml"))
  end
end
