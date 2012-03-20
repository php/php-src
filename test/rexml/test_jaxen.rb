require 'rexml_test_utils'

require "rexml/document"
require "rexml/xpath"

# Harness to test REXML's capabilities against the test suite from Jaxen
# ryan.a.cox@gmail.com

class JaxenTester < Test::Unit::TestCase
  include REXMLTestUtils
  include REXML

  def test_axis ; test("axis") ; end
  def test_basic ; test("basic") ; end
  def test_basicupdate ; test("basicupdate") ; end
  def test_contents ; test("contents") ; end
  def test_defaultNamespace ; test("defaultNamespace") ; end
  def test_fibo ; test("fibo") ; end
  def test_id ; test("id") ; end
  def test_jaxen24 ; test("jaxen24") ; end
  def test_lang ; test("lang") ; end
  def test_message ; test("message") ; end
  def test_moreover ; test("moreover") ; end
  def test_much_ado ; test("much_ado") ; end
  def test_namespaces ; test("namespaces") ; end
  def test_nitf ; test("nitf") ; end
  def test_numbers ; test("numbers") ; end
  def test_pi ; test("pi") ; end
  def test_pi2 ; test("pi2") ; end
  def test_simple ; test("simple") ; end
  def test_testNamespaces ; test("testNamespaces") ; end
  def test_text ; test("text") ; end
  def test_underscore ; test("underscore") ; end
  def test_web ; test("web") ; end
  def test_web2 ; test("web2") ; end

  private
  def test( fname )
#    Dir.entries( xml_dir ).each { |fname|
#      if fname =~ /\.xml$/
        file = File.new(fixture_path(fname+".xml"))
        doc = Document.new( file )
        XPath.each( doc, "/tests/document" ) {|e| handleDocument(e)}
#      end
#    }
  end

  # processes a tests/document/context node
  def handleContext( testDoc, ctxElement)
    testCtx = XPath.match( testDoc, ctxElement.attributes["select"] )[0]
    namespaces = {}
    if testCtx.class == Element
      testCtx.prefixes.each { |pre| handleNamespace( testCtx, pre, namespaces ) }
    end
    variables = {}
    XPath.each( ctxElement, "@*[namespace-uri() = 'http://jaxen.org/test-harness/var']") { |attrib| handleVariable(testCtx, variables, attrib) }
    XPath.each( ctxElement, "valueOf") { |e| handleValueOf(testCtx, variables, namespaces, e) }
    XPath.each( ctxElement, "test[not(@exception) or (@exception != 'true') ]") { |e| handleNominalTest(testCtx,variables, namespaces, e) }
    XPath.each( ctxElement, "test[@exception = 'true']") { |e| handleExceptionalTest(testCtx,variables, namespaces, e) }
  end

  # processes a tests/document/context/valueOf or tests/document/context/test/valueOf node
  def handleValueOf(ctx,variables, namespaces, valueOfElement)
    expected = valueOfElement.text
    got = XPath.match( ctx, valueOfElement.attributes["select"], namespaces, variables )[0]
    assert_true( (got.nil? && expected.nil?) || !got.nil? )
    case got.class
    when Element
      assert_equal( got.class, Element )
    when Attribute, Text, Comment, TrueClass, FalseClass
      assert_equal( expected, got.to_s )
    when Instruction
      assert_equal( expected, got.content )
    when Fixnum
      assert_equal( exected.to_f, got )
    when String
      # normalize values for comparison
      got = "" if got == nil or got == ""
      expected = "" if expected == nil or expected == ""
      assert_equal( expected, got )
    else
      assert_fail( "Wassup?" )
    end
  end


  # processes a tests/document/context/test node ( where @exception is false or doesn't exist )
  def handleNominalTest(ctx, variables, namespaces, testElement)
    expected = testElement.attributes["count"]
    got = XPath.match( ctx, testElement.attributes["select"], namespaces, variables )
    # might be a test with no count attribute, but nested valueOf elements
    assert( expected == got.size.to_s ) if !expected.nil?

    XPath.each( testElement, "valueOf") { |e|
      handleValueOf(got, variables, namespaces, e)
    }
  end

  # processes a tests/document/context/test node ( where @exception is true )
  def handleExceptionalTest(ctx, variables, namespaces, testElement)
    assert_raise( Exception ) {
      XPath.match( ctx, testElement.attributes["select"], namespaces, variables )
    }
  end

  # processes a tests/document node
  def handleDocument(docElement)
    puts "- Processing document: #{docElement.attributes['url']}"
    testFile = File.new( docElement.attributes["url"] )
    testDoc = Document.new testFile
    XPath.each( docElement, "context") { |e| handleContext(testDoc, e) }
  end

  # processes a variable definition in a namespace like <test var:foo="bar">
  def handleVariable( ctx, variables, attrib )
    puts "--- Found attribute: #{attrib.name}"
    variables[attrib.name] = attrib.value
  end

  # processes a namespace definition like <test xmlns:foo="fiz:bang:bam">
  def handleNamespace( ctx, prefix, namespaces )
    puts "--- Found namespace: #{prefix}"
    namespaces[prefix] = ctx.namespaces[prefix]
  end

end
