#! /usr/local/bin/ruby


require 'test/unit'
require 'rexml/document'

class TestAttributes < Test::Unit::TestCase
  def setup
    @ns_a = "urn:x-test:a"
    @ns_b = "urn:x-test:b"
    element_string = <<-"XMLEND"
    <test xmlns:a="#{@ns_a}"
          xmlns:b="#{@ns_b}"
          a = "1"
          b = '2'
          a:c = "3"
          a:d = '4'
          a:e = "5"
          b:f = "6"/>
    XMLEND
    @attributes = REXML::Document.new(element_string).root.attributes
  end

  def test_get_attribute_ns
    assert_equal("1", @attributes.get_attribute_ns("", "a").value)
    assert_equal("2", @attributes.get_attribute_ns("", "b").value)
    assert_equal("3", @attributes.get_attribute_ns(@ns_a, "c").value)
    assert_equal("4", @attributes.get_attribute_ns(@ns_a, "d").value)
    assert_equal("5", @attributes.get_attribute_ns(@ns_a, "e").value)
    assert_equal("6", @attributes.get_attribute_ns(@ns_b, "f").value)
  end
end
