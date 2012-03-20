require 'test/unit'
require 'rexml/document'
require 'rexml/element'
require 'rexml/xpath'

class XpathTestCase < Test::Unit::TestCase
  def setup
    @doc = REXML::Document.new
  end

  def tear_down
  end

  def test_text_as_element
    node1 = REXML::Element.new('a', @doc)
    node2 = REXML::Element.new('b', node1)
    textnode = REXML::Text.new('test', false, node2)
    assert_equal(1, @doc.elements.size, "doc owns 1 element node1")
    assert_same(node1, @doc.elements[1], "doc owns 1 element node1")
    assert_equal(1, node1.elements.size, "node1 owns 1 element node2")
    assert_same(node2, node1.elements[1], "node1 owns 1 element node2")
    assert_equal(1, node2.size, "node2 owns 1 text element")
  end

  def test_text_in_xpath_query
    node1 = REXML::Element.new('a', @doc)
    node2 = REXML::Element.new('b', node1)
    textnode = REXML::Text.new('test', false, node2)
    textnode.parent = node2   # should be unnecessary
    nodes = @doc.get_elements('//b')
    assert_equal(1, nodes.size, "document has one element")
    # why doesn't this query work right?
    nodes = REXML::XPath.match(@doc, '//text()')
    assert_equal(1, nodes.size, "//text() should yield one Text element")
    assert_equal(REXML::Text, nodes[0].class)
  end

  def test_comment_in_xpath_query
    node1 = REXML::Element.new('a', @doc)
    node2 = REXML::Element.new('b', node1)
    commentnode = REXML::Comment.new('test', node2)
    nodes = REXML::XPath.match(@doc, '//comment()')
    assert_equal(1, nodes.size, "//comment() should yield one Comment element")
    assert_same commentnode, nodes[0]
  end

  def test_parentage
    node1 = REXML::Element.new('a', @doc)
    assert_same(@doc, node1.parent, "node1 parent is document")
    node2 = REXML::Element.new('b', node1)
    assert_same(node1, node2.parent, "node2 parent is node1")
    textnode = REXML::Text.new('test', false, node2)
    # why isn't the text's parent node2?
    # Also look at Comment, etc.
    assert_same(node2, textnode.parent)
    comment = REXML::Comment.new('Test comment', node2)
    assert_same(node2, comment.parent)
  end

  def test_ancestors
    node1 = REXML::Element.new('a', @doc)
    node2 = REXML::Element.new('b', node1)
    textnode = REXML::Text.new('test', false, node2)
    #textnode.parent = node2   # should be unnecessary
    assert_same node2, textnode.parent
    nodes = @doc.get_elements('//b/ancestor::*')
    assert_equal(1, nodes.size, "<b> has one element ancestor")
    nodes = @doc.get_elements('//b/ancestor::node()')
    assert_equal(2, nodes.size, "<b> has two node ancestors")
    assert nodes[1].kind_of?(REXML::Document)
  end
end
