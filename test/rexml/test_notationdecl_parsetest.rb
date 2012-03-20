#! /usr/bin/ruby

require 'test/unit'
require 'rexml/document'

class TestNotationDecl < Test::Unit::TestCase
  def setup
    doc_string = <<-'XMLEND'
    <!DOCTYPE r SYSTEM "urn:x-henrikmartensson:test" [
      <!NOTATION n1 PUBLIC "-//HM//NOTATION TEST1//EN" 'urn:x-henrikmartensson.org:test5'>
      <!NOTATION n2 PUBLIC '-//HM//NOTATION TEST2//EN' "urn:x-henrikmartensson.org:test6">
    ]>
    <r/>
    XMLEND
    @doctype = REXML::Document.new(doc_string).doctype
  end

  def test_notation
    assert(@doctype.notation('n1'), "Testing notation n1")
    assert(@doctype.notation('n2'), "Testing notation n2")
  end

end
