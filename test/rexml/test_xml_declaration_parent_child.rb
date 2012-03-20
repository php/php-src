#!/usr/bin/env ruby
#
#  Created by Henrik Mårtensson on 2007-02-18.
#  Copyright (c) 2007. All rights reserved.

require "rexml/document"
require "test/unit"

class TestXmlDeclaration < Test::Unit::TestCase
  def setup
    xml = <<-'END_XML'
    <?xml encoding= 'UTF-8' standalone='yes'?>
    <root>
    </root>
    END_XML
    @doc = REXML::Document.new xml
    @root = @doc.root
    @xml_declaration = @doc.children[0]
  end

  def test_xml_declaration_is_first_child
    assert_kind_of(REXML::XMLDecl, @xml_declaration)
  end

  def test_xml_declaration_has_document_as_parent
   assert_kind_of(REXML::Document, @xml_declaration.parent)
  end

  def test_xml_declaration_has_sibling
    assert_kind_of(REXML::XMLDecl, @root.previous_sibling.previous_sibling)
    assert_kind_of(REXML::Element, @xml_declaration.next_sibling.next_sibling)
  end
end
