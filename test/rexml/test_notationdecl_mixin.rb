#! /usr/local/bin/ruby


require 'test/unit'
require 'rexml/document'

class TestNotationDeclMixin < Test::Unit::TestCase
  def setup
    @pubid1 = "TEST1"
    @pubid2 = "TEST2"
    @sysid2 = "urn:x-henrikmartensson.org:test2"
    @pubid3 = "TEST3"
    @pubid4 = "TEST4"
    @sysid4 = "urn:x-henrikmartensson.org:test4"
    @pubid5 = "TEST5"
    @sysid5 = "urn:x-henrikmartensson.org:test5"
    @pubid6 = "TEST6"
    @sysid6 = "urn:x-henrikmartensson.org:test6"
    @sysid7 = "urn:x-henrikmartensson.org:test7"
    doc_string = <<-"XMLEND"
    <!DOCTYPE r SYSTEM "urn:x-henrikmartensson:test" [
      <!NOTATION n1 PUBLIC "#{@pubid1}">
      <!NOTATION n2 PUBLIC "#{@pubid2}" "#{@sysid2}">
      <!NOTATION n3 PUBLIC '#{@pubid3}'>
      <!NOTATION n4 PUBLIC '#{@pubid4}' '#{@sysid4}'>
      <!NOTATION n5 PUBLIC "#{@pubid5}" '#{@sysid5}'>
      <!NOTATION n6 PUBLIC '#{@pubid6}' "#{@sysid6}">
      <!NOTATION n7 SYSTEM "#{@sysid7}">
    ]>
    <r/>
    XMLEND
    @doctype = REXML::Document.new(doc_string).doctype
  end

  def test_name
    assert_equal('n1', @doctype.notation('n1').name)
  end

  def test_public_2
    assert_equal(@pubid1, @doctype.notation('n1').public)
    assert_equal(@pubid2, @doctype.notation('n2').public)
    assert_equal(@pubid3, @doctype.notation('n3').public)
    assert_equal(@pubid4, @doctype.notation('n4').public)
    assert_equal(@pubid5, @doctype.notation('n5').public)
    assert_equal(@pubid6, @doctype.notation('n6').public)
    assert_nil(@doctype.notation('n7').public)
  end

  def test_system_2
    assert_equal(@sysid2, @doctype.notation('n2').system)
    assert_nil(@doctype.notation('n3').system)
    assert_equal(@sysid4, @doctype.notation('n4').system)
    assert_equal(@sysid5, @doctype.notation('n5').system)
    assert_equal(@sysid6, @doctype.notation('n6').system)
    assert_equal(@sysid7, @doctype.notation('n7').system)
  end

end
