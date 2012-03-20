# rexml_xpath_attribute_query.rb
# May 16, 2007
#

require 'test/unit'
require 'rexml/document'

class TestRexmlXpathAttributeQuery < Test::Unit::TestCase

  # xmlstr1 and xmlstr2 only differ in the second line - namespaces in the root element
  @@xmlstr1 = '<?xml version="1.0" encoding="UTF-8"?>
<feed xmlns="http://www.w3.org/2005/Atom" xmlns:openSearch="http://a9.com/-/spec/opensearchrss/1.0/" xmlns:gd="http://schemas.google.com/g/2005" xmlns:gCal="http://schemas.google.com/gCal/2005">
  <id>http://www.google.com/calendar/feeds/me%40gmail.com</id>
  <entry>
    <id>http://www.google.com/calendar/feeds/me%40gmail.com/me%40gmail.com</id>
    <published>2007-05-16T13:42:27.942Z</published>
    <updated>2007-05-15T03:29:28.000Z</updated>
    <title type="text">My Calendar</title>
    <link rel="alternate" type="application/atom+xml" href="http://www.google.com/calendar/feeds/me%40gmail.com/private/full"/>
    <link rel="http://schemas.google.com/acl/2007#accessControlList" type="application/atom+xml" href="http://www.google.com/calendar/feeds/me%40gmail.com/acl/full"/>
    <link rel="self" type="application/atom+xml" href="http://www.google.com/calendar/feeds/me%40gmail.com/me%40gmail.com"/>
    <author>
      <name>Me</name>
      <email>me@gmail.com</email>
    </author>
  </entry>
</feed>'


  @@xmlstr2 = '<?xml version="1.0" encoding="UTF-8"?>
<feed>
  <id>http://www.google.com/calendar/feeds/me%40gmail.com</id>
  <entry>
    <id>http://www.google.com/calendar/feeds/me%40gmail.com/me%40gmail.com</id>
    <published>2007-05-16T13:42:27.942Z</published>
    <updated>2007-05-15T03:29:28.000Z</updated>
    <title type="text">My Calendar</title>
    <link rel="alternate" type="application/atom+xml" href="http://www.google.com/calendar/feeds/me%40gmail.com/private/full"/>
    <link rel="http://schemas.google.com/acl/2007#accessControlList" type="application/atom+xml" href="http://www.google.com/calendar/feeds/me%40gmail.com/acl/full"/>
    <link rel="self" type="application/atom+xml" href="http://www.google.com/calendar/feeds/me%40gmail.com/me%40gmail.com"/>
    <author>
      <name>Me</name>
      <email>me@gmail.com</email>
    </author>
  </entry>
</feed>'

  # Fails
  def test_xpath_query
    do_test @@xmlstr1
  end

  # Passes
  def test_xpath_query_no_namespace
    do_test @@xmlstr2
  end

  def do_test(xmlString)
    hrefs = [
      "http://www.google.com/calendar/feeds/me%40gmail.com/private/full",
      "http://www.google.com/calendar/feeds/me%40gmail.com/acl/full",
      "http://www.google.com/calendar/feeds/me%40gmail.com/me%40gmail.com"
    ]
    ctr=0
    REXML::Document.new(xmlString).elements.each("feed/entry") do |element|
      @alternate_link = element.elements["link[@rel='alternate']"]
      assert_not_nil( @alternate_link )
      assert_equal( hrefs[ctr], @alternate_link.attributes['href'])
      ctr += 1
    end
  end


  def test_another_way
    doc = REXML::Document.new(@@xmlstr1)
    hrefs = [
      "http://www.google.com/calendar/feeds/me%40gmail.com/private/full",
      "http://www.google.com/calendar/feeds/me%40gmail.com/acl/full",
      "http://www.google.com/calendar/feeds/me%40gmail.com/me%40gmail.com"
    ]
    ctr=0
    REXML::XPath.each(doc, "//link[@rel='alternate']") do |element|
      @alternate_link = element
      assert_not_nil @alternate_link
      assert_equal( hrefs[ctr], @alternate_link.attributes['href'])
      ctr += 1
    end
  end
end
