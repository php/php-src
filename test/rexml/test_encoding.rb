# coding: binary

require "rexml_test_utils"

require 'rexml/source'
require 'rexml/document'

class EncodingTester < Test::Unit::TestCase
  include REXMLTestUtils
  include REXML

  def setup
    @encoded = "<?xml version='1.0' encoding='ISO-8859-3'?>"+
    "<a><b>\346</b></a>"
    @not_encoded = "<a><b>ĉ</b></a>"
  end

  # Given an encoded document, try to write out to that encoding
  def test_encoded_in_encoded_out
    doc = Document.new( @encoded )
    doc.write( out="" )
    out.force_encoding(::Encoding::ASCII_8BIT)
    assert_equal( @encoded, out )
  end

  # Given an encoded document, try to change the encoding and write it out
  def test_encoded_in_change_out
    doc = Document.new( @encoded )
    doc.xml_decl.encoding = "UTF-8"
    assert_equal("UTF-8", doc.encoding)
    REXML::Formatters::Default.new.write( doc.root, out="" )
    out.force_encoding(::Encoding::ASCII_8BIT)
    assert_equal( @not_encoded, out )
    char = XPath.first( doc, "/a/b/text()" ).to_s
    char.force_encoding(::Encoding::ASCII_8BIT)
    assert_equal( "ĉ", char )
  end

  # * Given an encoded document, try to write it to a different encoding
  def test_encoded_in_different_out
    doc = Document.new( @encoded )
    REXML::Formatters::Default.new.write( doc.root, Output.new( out="", "UTF-8" ) )
    out.force_encoding(::Encoding::ASCII_8BIT)
    assert_equal( @not_encoded, out )
  end

  # * Given a non-encoded document, change the encoding
  def test_in_change_out
    doc = Document.new( @not_encoded )
    doc.xml_decl.encoding = "ISO-8859-3"
    assert_equal("ISO-8859-3", doc.encoding)
    doc.write( out="" )
    out.force_encoding(::Encoding::ASCII_8BIT)
    assert_equal( @encoded, out )
  end

  # * Given a non-encoded document, write to a different encoding
  def test_in_different_out
    doc = Document.new( @not_encoded )
    doc.write( Output.new( out="", "ISO-8859-3" ) )
    out.force_encoding(::Encoding::ASCII_8BIT)
    assert_equal( @encoded, out )
  end

  # * Given an encoded document, accessing text and attribute nodes
  #   should provide UTF-8 text.
  def test_in_different_access
    doc = Document.new <<-EOL
    <?xml version='1.0' encoding='ISO-8859-1'?>
    <a a="\xFF">\xFF</a>
    EOL
    expect = "\303\277"
    expect.force_encoding(::Encoding::UTF_8)
    assert_equal( expect, doc.elements['a'].attributes['a'] )
    assert_equal( expect, doc.elements['a'].text )
  end


  def test_ticket_89
    doc = Document.new <<-EOL
       <?xml version="1.0" encoding="CP-1252" ?>
       <xml><foo></foo></xml>
       EOL

    REXML::Document.new doc
  end

  def test_ticket_110
    utf16 = REXML::Document.new(File.new(fixture_path("ticket_110_utf16.xml")))
    assert_equal(utf16.encoding, "UTF-16")
    assert( utf16[0].kind_of?(REXML::XMLDecl))
  end
end
