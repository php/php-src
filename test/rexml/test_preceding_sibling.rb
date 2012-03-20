# ISSUE 32
require 'test/unit'
require 'rexml/document'

# daz - for report by Dan Kohn in:
#  http://blade.nagaokaut.ac.jp/cgi-bin/scat.rb/ruby/ruby-talk/156328
class XPathTesterDd < Test::Unit::TestCase
  include REXML

  def setup
    @@docDd = Document.new(<<-EOS, :ignore_whitespace_nodes => :all)
      <a>
        <b x='ab01A'>
          <c y='abc01A'>Success</c>
        </b>
        <b x='ab02A' y='ab02B'>
          <c>abc02C</c>
        </b>
      </a>
    EOS
  end

  def test_Dd_preceding_sibling_children
    arr = []
    XPath.each(@@docDd, "//b[@x='ab02A']/preceding-sibling::b/child::*") do |cell|
      arr << cell.texts.join
    end
    assert_equal( 'Success', arr.join )
  end

  def test_Dd_preceding_sibling_all
    arr = []
    XPath.each(@@docDd, "//b[@x='ab02A']/preceding-sibling::*") do |cell|
      arr << cell.to_s
    end
    assert_equal( "<b x='ab01A'><c y='abc01A'>Success</c></b>", arr.join )
  end
end
