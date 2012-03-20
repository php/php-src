#------------------------------------------------------------------------------
# file: rexml_test.rb
# desc: test's REXML's XML/XPath implementation
# auth: Philip J Grabner <grabner>at<uberdev>dot<org>
# date: 2006/08/17
# copy: (C) CopyLoose 2006 Bib Development Team <bib-devel>at<uberdev>dot<org>
#------------------------------------------------------------------------------

require 'test/unit'
require 'rexml/document'

class Ticket80 < Test::Unit::TestCase

  @@xmlstr = '<?xml version="1.0"?>
<root xmlns="urn:some-xml-ns" xmlns:other="urn:some-other-xml-ns">
 <l1-foo>
  <l2 value="foo-01"/>
  <l2 value="foo-02"/>
  <l2 value="foo-03"/>
 </l1-foo>
 <other:l1>
  <l2 value="no-show"/>
 </other:l1>
 <l1-bar>
  <l2 value="bar-01"/>
  <l2 value="bar-02"/>
 </l1-bar>
</root>'

  #----------------------------------------------------------------------------
  def test_xpathNamespacedChildWildcard
    # tests the "prefix:*" node test syntax
    out = Array.new
    REXML::XPath.each( REXML::Document.new(@@xmlstr),
                       '/ns:root/ns:*/ns:l2/@value',
                       { 'ns' => 'urn:some-xml-ns' } ) do |node| out.push node.value ; end
    chk = [ 'foo-01', 'foo-02', 'foo-03', 'bar-01', 'bar-02' ]
    assert_equal chk, out
  end

  #----------------------------------------------------------------------------
  def test_xpathNamespacedChildWildcardWorkaround
    # tests a workaround for the "prefix:*" node test syntax
    out = Array.new
    REXML::XPath.each( REXML::Document.new(@@xmlstr),
                       '/ns:root/*[namespace-uri()="urn:some-xml-ns"]/ns:l2/@value',
                       { 'ns' => 'urn:some-xml-ns' } ) do |node| out.push node.value ; end
    chk = [ 'foo-01', 'foo-02', 'foo-03', 'bar-01', 'bar-02' ]
    assert_equal chk, out
  end

end

#------------------------------------------------------------------------------
# end of rexml_test.rb
#------------------------------------------------------------------------------
