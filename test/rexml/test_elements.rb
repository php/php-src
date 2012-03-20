require 'test/unit/testcase'
require 'rexml/document'

class ElementsTester < Test::Unit::TestCase
  include REXML
  def test_accessor
    doc = Document.new '<a><b/><c id="1"/><c id="2"/><d/></a>'
    assert_equal 'b', doc.root.elements[1].name
    assert_equal '1', doc.root.elements['c'].attributes['id']
    assert_equal '2', doc.root.elements[2,'c'].attributes['id']
  end

  def test_indexing
    doc = Document.new '<a/>'
    doc.root.elements[10] = Element.new('b')
    assert_equal 'b', doc.root.elements[1].name
    doc.root.elements[1] = Element.new('c')
    assert_equal 'c', doc.root.elements[1].name
    doc.root.elements['c'] = Element.new('d')
    assert_equal 'd', doc.root.elements[1].name
  end

  def test_delete
    doc = Document.new '<a><b/><c/><c id="1"/></a>'
    block = proc { |str|
      out = ''
      doc.write out
      assert_equal str, out
    }
    b = doc.root.elements[1]
    doc.root.elements.delete b
    block.call( "<a><c/><c id='1'/></a>" )
    doc.elements.delete("a/c[@id='1']")
    block.call( '<a><c/></a>' )
    doc.root.elements.delete 1
    block.call( '<a/>' )
  end

  def test_delete_all
    doc = Document.new '<a><c/><c/><c/><c/></a>'
    deleted = doc.elements.delete_all 'a/c'
    assert_equal 4, deleted.size
  end

  def test_ticket_36
    doc = Document.new( "<a xmlns:xi='foo'><b><xi:c id='1'/></b><xi:c id='2'/></a>" )

    deleted = doc.root.elements.delete_all( "xi:c" )
    assert_equal( 1, deleted.size )

    doc = Document.new( "<a xmlns:xi='foo'><b><xi:c id='1'/></b><xi:c id='2'/></a>" )
    deleted = doc.root.elements.delete_all( "//xi:c" )
    assert_equal( 2, deleted.size )
  end

  def test_add
    a = Element.new 'a'
    a.elements.add Element.new('b')
    assert_equal 'b', a.elements[1].name
    a.elements.add 'c'
    assert_equal 'c', a.elements[2].name
  end

  def test_size
    doc = Document.new '<a>sean<b/>elliott<b/>russell<b/></a>'
    assert_equal 6, doc.root.size
    assert_equal 3, doc.root.elements.size
  end

  def test_each
    doc = Document.new '<a><b/><c/><d/>sean<b/><c/><d/></a>'
    count = 0
    block = proc {|e| count += 1}
    doc.root.elements.each(&block)
    assert_equal 6, count
    count = 0
    doc.root.elements.each('b', &block)
    assert_equal 2, count
    count = 0
    doc.root.elements.each('child::node()', &block)
    assert_equal 6, count
    count = 0
    XPath.each(doc.root, 'child::node()', &block)
    assert_equal 7, count
  end

  def test_each_with_frozen_condition
    doc = Document.new('<books><book name="Ruby"/><book name="XML"/></books>')
    names = []
    doc.root.elements.each('book'.freeze) do |element|
      names << element.attributes["name"]
    end
    assert_equal(["Ruby", "XML"], names)
  end

  def test_to_a
    doc = Document.new '<a>sean<b/>elliott<c/></a>'
    assert_equal 2, doc.root.elements.to_a.size
    assert_equal 2, doc.root.elements.to_a("child::node()").size
    assert_equal 4, XPath.match(doc.root, "child::node()").size
  end

  def test_collect
    doc = Document.new( "<a><b id='1'/><b id='2'/></a>" )
    r = doc.elements.collect( "/a/b" ) { |e| e.attributes["id"].to_i }
    assert_equal( [1,2], r )
  end

  def test_inject
    doc = Document.new( "<a><b id='1'/><b id='2'/></a>" )
    r = doc.elements.inject( "/a/b", 3 ) { |s, e|
      s + e.attributes["id"].to_i
    }
    assert_equal 6, r
  end
end
