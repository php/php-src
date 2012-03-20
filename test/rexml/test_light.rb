require "rexml_test_utils"
require "rexml/light/node"
require "rexml/parsers/lightparser"

class LightTester < Test::Unit::TestCase
  include REXMLTestUtils
  include REXML::Light

  def test_parse_large
    xml_string = fixture_path("documentation.xml")
    parser = REXML::Parsers::LightParser.new(xml_string)
    tag, content = parser.parse
    assert_equal([:document, :text], [tag, content.first])
  end

  # FIXME INCOMPLETE
  # This is because the light API is not yet ready to be used to produce
  # trees.
=begin
  def test_add_element
    doc = Node.new
    foo = doc.add_element( 'foo' )
    assert_equal( "foo", foo.name )
  end

  def test_add_attribute
    foo = Node.new( "a" )
    foo["attr"] = "bar"
    assert_equal( "bar", foo["attr"] )
  end

  def test_write_document
    r = make_small_document
    assert_equal( "<a><b/><c/></a>", r.to_s )
  end

  def test_add_attribute_under_namespace
    foo = Node.new("a")
    foo["attr", "a"] = "1"
    foo["attr", "b"] = "2"
    foo["attr"] = "3"
    assert_equal( '1', foo['attr', 'a'] )
    assert_equal( '2', foo['attr', 'b'] )
    assert_equal( '3', foo['attr'] )
  end

  def test_change_namespace_of_element
    foo = Node.new
    assert_equal( '', foo.namespace )
    foo.namespace = 'a'
    assert_equal( 'a', foo.namespace )
  end

  def test_access_child_elements
    foo = make_small_document
    assert_equal( 1, foo.size )
    a = foo[0]
    assert_equal( 2, a.size )
    assert_equal( 'b', a[0].name )
    assert_equal( 'c', a[1].name )
  end

  def test_itterate_over_children
    foo = make_small_document
    ctr = 0
    foo[0].each { ctr += 1 }
    assert_equal( 2, ctr )
  end

  def test_add_text
    foo = Node.new( "a" )
    foo.add_text( "Sean" )
    sean = foo[0]
    assert( sean.node_type == :text )
  end

  def test_add_instruction
    foo = Node.new( "a" )
    foo.add_instruction( "target", "value" )
    assert( foo[0].node_type == :processing_instruction )
  end

  def test_add_comment
    foo = Node.new( "a" )
    foo.add_comment( "target", "value" )
    assert( foo[0].node_type == :comment )
  end

  def test_get_root
    foo = Node.new( 'a' )
    10.times { foo = foo.add_element('b') }
    assert_equals( 'b', foo.name )
    assert_equals( 'a', foo.root.name )
  end

  def make_small_document
    r = Node.new
    a = r.add_element( "a" )
    a.add_element( 'b' )
    a.add_element( 'c' )
    r
  end
=end
end
