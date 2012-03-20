#! /usr/local/bin/ruby


require 'test/unit'
require 'rexml/document'

class TestDocTypeAccessor < Test::Unit::TestCase

  def setup
    @sysid = "urn:x-test:sysid1"
    @notid1 = "urn:x-test:notation1"
    @notid2 = "urn:x-test:notation2"
    document_string1 = <<-"XMLEND"
    <!DOCTYPE r SYSTEM "#{@sysid}" [
      <!NOTATION n1 SYSTEM "#{@notid1}">
      <!NOTATION n2 SYSTEM "#{@notid2}">
    ]>
    <r/>
    XMLEND
    @doctype1 = REXML::Document.new(document_string1).doctype

    @pubid = "TEST_ID"
    document_string2 = <<-"XMLEND"
    <!DOCTYPE r PUBLIC "#{@pubid}">
    <r/>
    XMLEND
    @doctype2 = REXML::Document.new(document_string2).doctype

    document_string3 = <<-"XMLEND"
    <!DOCTYPE r PUBLIC "#{@pubid}" "#{@sysid}">
    <r/>
    XMLEND
    @doctype3 = REXML::Document.new(document_string3).doctype

  end

  def test_public
    assert_equal(nil, @doctype1.public)
    assert_equal(@pubid, @doctype2.public)
    assert_equal(@pubid, @doctype3.public)
  end

  def test_system
    assert_equal(@sysid, @doctype1.system)
    assert_equal(nil, @doctype2.system)
    assert_equal(@sysid, @doctype3.system)
  end

  def test_notation
    assert_equal(@notid1, @doctype1.notation("n1").system)
    assert_equal(@notid2, @doctype1.notation("n2").system)
  end

  def test_notations
    notations = @doctype1.notations
    assert_equal(2, notations.length)
    assert_equal(@notid1, find_notation(notations, "n1").system)
    assert_equal(@notid2, find_notation(notations, "n2").system)
  end

  def find_notation(notations, name)
    notations.find { |notation|
      name == notation.name
    }
  end

end

class TestNotationDeclPublic < Test::Unit::TestCase
  def setup
    @name = "vrml"
    @id = "VRML 1.0"
    @uri = "http://www.web3d.org/"
  end

  def test_to_s
    assert_equal("<!NOTATION #{@name} PUBLIC \"#{@id}\">",
                 decl(@id, nil).to_s)
  end

  def test_to_s_with_uri
    assert_equal("<!NOTATION #{@name} PUBLIC \"#{@id}\" \"#{@uri}\">",
                 decl(@id, @uri).to_s)
  end

  private
  def decl(id, uri)
    REXML::NotationDecl.new(@name, "PUBLIC", id, uri)
  end
end

class TestNotationDeclSystem < Test::Unit::TestCase
  def setup
    @name = "gif"
    @id = "gif viewer"
  end

  def test_to_s
    assert_equal("<!NOTATION #{@name} SYSTEM \"#{@id}\">",
                 decl(@id).to_s)
  end

  private
  def decl(id)
    REXML::NotationDecl.new(@name, "SYSTEM", id, nil)
  end
end
