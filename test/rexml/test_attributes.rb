require 'test/unit/testcase'
require 'rexml/document'

class AttributesTester < Test::Unit::TestCase
  include REXML
  def test_accessor
    doc = Document.new("<a xmlns:foo='a' xmlns:bar='b' foo:att='1' bar:att='2' att='3'/>")
    assert_equal '3', doc.root.attributes['att']
    assert_equal '2', doc.root.attributes['bar:att']
    doc.root.attributes['att'] = 5
    assert_equal '5', doc.root.attributes['att']
  end

  def test_each_attribute
    doc = Document.new('<a x="1" y="2"/>')
    doc.root.attributes.each_attribute {|attr|
      if attr.expanded_name == 'x'
        assert_equal '1', attr.value
      elsif attr.expanded_name == 'y'
        assert_equal '2', attr.value
      else
        assert_fail "No such attribute!!"
      end
    }
  end

  def test_each
    doc = Document.new('<a x="1" y="2"/>')
    doc.root.attributes.each {|name, value|
      if name == 'x'
        assert_equal '1', value
      elsif name == 'y'
        assert_equal '2', value
      else
        assert_fail "No such attribute!!"
      end
    }
  end

  def test_get_attribute
    doc = Document.new('<a xmlns:x="a" x:foo="1" foo="2" bar="3"/>')
    assert_equal '2', doc.root.attributes.get_attribute("foo").value
    assert_equal '1', doc.root.attributes.get_attribute("x:foo").value
  end

  def test_size
    doc = Document.new("<a xmlns:foo='a' x='1' y='2' foo:x='3'/>")
    assert_equal 4, doc.root.attributes.length
  end

  def test_setter
    doc = Document.new("<a xmlns:x='a' x:foo='1' foo='3'/>")
    doc.root.attributes['y:foo'] = '2'
    assert_equal '2', doc.root.attributes['y:foo']
    doc.root.attributes['foo'] = '4'
    assert_equal '4', doc.root.attributes['foo']
    doc.root.attributes['x:foo'] = nil
    assert_equal 3, doc.root.attributes.size
  end

  def test_delete
    doc = Document.new("<a xmlns:y='a' xmlns:x='b' xmlns:z='c' y:foo='0' x:foo='1' foo='3' z:foo='4'/>")
    doc.root.attributes.delete 'foo'
    assert_equal 6, doc.root.attributes.size
    assert_equal '1', doc.root.attributes['x:foo']

    doc.root.attributes.delete 'x:foo'
    assert_equal 5, doc.root.attributes.size

    attr = doc.root.attributes.get_attribute('y:foo')
    doc.root.attributes.delete attr
    assert_equal 4, doc.root.attributes.size

    assert_equal '4', doc.root.attributes['z:foo']
  end

  def test_prefixes
    doc = Document.new("<a xmlns='foo' xmlns:x='bar' xmlns:y='twee' z='glorp' x:k='gru'/>")
    prefixes = doc.root.attributes.prefixes
    assert_equal 2, prefixes.size
    assert_equal 0, (prefixes - ['x', 'y']).size
  end

  # Contributed by Mike Stok
  def test_values_with_apostrophes
    doc = Document.new(%q#<tag h1="1'2'" h2='1"2'/>#)
    s = doc.to_s
    assert(s =~ /h1='1&apos;2&apos;'/)
    assert(s =~ /h2='1"2'/)
  end

  # Submitted by Kou
  def test_namespace_conflict
    assert_raise( ParseException,
                  "Declaring two attributes with the same namespace should be an error" ) do
      REXML::Document.new <<-XML
      <x xmlns:n1="http://www.w3.org"
         xmlns:n2="http://www.w3.org" >
        <bad n1:a="1"  n2:a="2" />
      </x>
      XML
    end

    REXML::Document.new("<a xmlns:a='a' xmlns:b='a'></a>")
  end

  # Submitted by Kou
  def test_attribute_deletion
    e = REXML::Element.new
    e.add_namespace("a", "http://a/")
    e.add_namespace("b", "http://b/")
    e.add_attributes({"c" => "cc", "a:c" => "cC", "b:c" => "CC"})

    e.attributes.delete("c")
    assert_nil(e.attributes.get_attribute("c"))

    before_size = e.attributes.size
    e.attributes.delete("c")
    assert_nil(e.attributes.get_attribute("c"))
    assert_equal(before_size, e.attributes.size)

    e.attributes.delete(e.attributes.get_attribute("a:c"))
    assert_nil(e.attributes.get_attribute("a:c"))

    e.attributes.delete("b:c")
    assert_nil(e.attributes.get_attribute("b:c"))

    before_size = e.attributes.size
    e.attributes.delete(e.attributes.get_attribute("b:c"))
    assert_nil(e.attributes.get_attribute("b:c"))
    assert_equal(before_size, e.attributes.size)

    before_size = e.attributes.size
    e.attributes.delete("c")
    assert_nil(e.attributes.get_attribute("c"))
    assert_equal(before_size, e.attributes.size)

    e.add_attribute("c", "cc")

    e.attributes.delete(e.attributes.get_attribute("c"))
    assert_nil(e.attributes.get_attribute("c"))
  end

  # Submitted by Kou
  def test_element_usage
    attr = Attribute.new("name", "value")
    elem = Element.new("elem")
    a = Attribute.new(attr, elem)
    assert_equal(elem, a.element)
  end

  def attr_test(attr_name,attr_value)
    a1 = REXML::Attribute.new(attr_name,attr_value)

    s1 = a1.value
    s2 = a1.value

    #p s1
    #p s2
    assert_equal(s1,s2)

    a2 = REXML::Attribute.new(attr_name,attr_value)

    a2.to_s        # NB invocation of to_s
    s1 = a2.value
    s2 = a2.value

    #p s1
    #p s2
    assert_equal(s1,s2)
  end

  def test_amp_attributes
    attr_test('name','value with &amp; ampersand only')
  end

  def test_amp_and_lf_attributes
    attr_test('name','value with LF &#x000a; &amp; ampersand')
  end

  def test_quoting
    d = Document.new(%q{<a x='1' y="2"/>})
    assert_equal( %q{<a x='1' y='2'/>}, d.to_s )
    d.root.context[:attribute_quote] = :quote
    assert_equal( %q{<a x="1" y="2"/>}, d.to_s )

    d = Document.new(%q{<a x='1' y="2"><b z='3'/></a>})
    assert_equal( %q{<a x='1' y='2'><b z='3'/></a>}, d.to_s )
    d.root.context[:attribute_quote] = :quote
    assert_equal( %q{<a x="1" y="2"><b z="3"/></a>}, d.to_s )
  end

  def test_ticket_127
    doc = Document.new
    doc.add_element 'a', { 'v' => 'x & y' }
    assert doc.to_s.index(';')
  end
end
