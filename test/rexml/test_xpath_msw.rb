require "test/unit/testcase"
require "rexml/document"

class XPathAxesTester < Test::Unit::TestCase
  include REXML
  SOURCE = <<-EOF
    <a id='1'>
      <e id='2'>
        <f id='3'/>
        <f id='4'/>
        <f id='5'/>
        <f id='6'/>
      </e>
    </a>
    EOF

  def setup
    @@doc = Document.new(SOURCE) unless defined? @@doc
  end

  def test_preceding_sibling_axis
    context = XPath.first(@@doc,"/a/e/f[last()]")
    assert_equal "6", context.attributes["id"]

    prev = XPath.first(context, "preceding-sibling::f")
    assert_equal "5", prev.attributes["id"]

    prev = XPath.first(context, "preceding-sibling::f[1]")
    assert_equal "5", prev.attributes["id"]

    prev = XPath.first(context, "preceding-sibling::f[2]")
    assert_equal "4", prev.attributes["id"]

    prev = XPath.first(context, "preceding-sibling::f[3]")
    assert_equal "3", prev.attributes["id"]
  end
end

